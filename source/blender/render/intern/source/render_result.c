/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version. 
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2006 Blender Foundation.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file blender/render/intern/source/render_result.c
 *  \ingroup render
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MEM_guardedalloc.h"

#include "BLI_utildefines.h"
#include "BLI_listbase.h"
#include "BLI_md5.h"
#include "BLI_path_util.h"
#include "BLI_rect.h"
#include "BLI_string.h"
#include "BLI_system.h"
#include "BLI_threads.h"

#include "BKE_image.h"
#include "BKE_global.h"
#include "BKE_main.h"
#include "BKE_report.h"
#include "BKE_camera.h"

#include "IMB_imbuf.h"
#include "IMB_imbuf_types.h"
#include "IMB_colormanagement.h"

#include "intern/openexr/openexr_multi.h"

#include "render_result.h"
#include "render_types.h"

/********************************** Free *************************************/

void render_result_free(RenderResult *res)
{
	if (res == NULL) return;

	while (res->layers.first) {
		RenderLayer *rl = res->layers.first;

		/* acolrect and scolrect are optionally allocated in shade_tile, only free here since it can be used for drawing */
		if (rl->acolrect) MEM_freeN(rl->acolrect);
		if (rl->scolrect) MEM_freeN(rl->scolrect);
		if (rl->display_buffer) MEM_freeN(rl->display_buffer);
		
		while (rl->passes.first) {
			RenderPass *rpass = rl->passes.first;
			if (rpass->rect) MEM_freeN(rpass->rect);
			BLI_remlink(&rl->passes, rpass);
			MEM_freeN(rpass);
		}
		BLI_remlink(&res->layers, rl);
		MEM_freeN(rl);
	}
	
	while (res->views.first) {
		RenderView *rv = res->views.first;
		BLI_remlink(&res->views, rv);

		if (rv->rectf)
			MEM_freeN(rv->rectf);

		if (rv->rectz)
			MEM_freeN(rv->rectz);

		MEM_freeN(rv);
	}

	if (res->rect32)
		MEM_freeN(res->rect32);
	if (res->rectz)
		MEM_freeN(res->rectz);
	if (res->rectf)
		MEM_freeN(res->rectf);
	if (res->text)
		MEM_freeN(res->text);
	
	MEM_freeN(res);
}

/* version that's compatible with fullsample buffers */
void render_result_free_list(ListBase *lb, RenderResult *rr)
{
	RenderResult *rrnext;
	
	for (; rr; rr = rrnext) {
		rrnext = rr->next;
		
		if (lb && lb->first)
			BLI_remlink(lb, rr);
		
		render_result_free(rr);
	}
}

/********************************* multiview *************************************/

static const char *get_view_name(ListBase *views, int view_id)
{
	RenderView *rv;
	int id = 0;
	for (rv= (RenderView *)views->first, id=0; rv; rv=rv->next, id++) {
		if (id == view_id) return rv->name;
	}
	return "";
}

int render_result_get_view_id(Render *re, const char *view)
{
	RenderView *rv;
	int id = 0;

	if (!re || !re->result)
		return 0;

	/* -1 = all views */
	if (view[0] == '\0')
		return -1;

	for (rv= (RenderView *)re->result->views.first; rv; rv=rv->next, id++) {
		if (strcmp(rv->name, view)==0)
			return id;
	}

	return 0;
}

/* create a new views Listbase in rr without duplicating the memory pointers */
void render_result_views_shallowcopy(RenderResult *dst, RenderResult *src)
{
	RenderView *rview;

	if (dst == NULL || src == NULL)
		return;

	for (rview = (RenderView *)src->views.first; rview; rview = rview->next) {
		RenderView *rv;

		rv = MEM_callocN(sizeof(RenderView), "new render view");
		BLI_addtail(&dst->views, rv);

		BLI_strncpy(rv->name, rview->name, sizeof(rv->name));
		rv->camera = rview->camera;
		rv->rectf = rview->rectf;
		rv->rectz = rview->rectz;
		rv->rect32 = rview->rect32;
	}
}

/* free the views created temporarily */
void render_result_views_shallowdelete(RenderResult *rr)
{
	if (rr == NULL)
		return;

	while (rr->views.first) {
		RenderView *rv = rr->views.first;
		BLI_remlink(&rr->views, rv);
		MEM_freeN(rv);
	}
}

static const char *name_from_passtype(int passtype, int channel)
{
	if (passtype == SCE_PASS_COMBINED) {
		if (channel == -1) return "Combined";
		if (channel == 0) return "Combined.R";
		if (channel == 1) return "Combined.G";
		if (channel == 2) return "Combined.B";
		return "Combined.A";
	}
	if (passtype == SCE_PASS_Z) {
		if (channel == -1) return "Depth";
		return "Depth.Z";
	}
	if (passtype == SCE_PASS_VECTOR) {
		if (channel == -1) return "Vector";
		if (channel == 0) return "Vector.X";
		if (channel == 1) return "Vector.Y";
		if (channel == 2) return "Vector.Z";
		return "Vector.W";
	}
	if (passtype == SCE_PASS_NORMAL) {
		if (channel == -1) return "Normal";
		if (channel == 0) return "Normal.X";
		if (channel == 1) return "Normal.Y";
		return "Normal.Z";
	}
	if (passtype == SCE_PASS_UV) {
		if (channel == -1) return "UV";
		if (channel == 0) return "UV.U";
		if (channel == 1) return "UV.V";
		return "UV.A";
	}
	if (passtype == SCE_PASS_RGBA) {
		if (channel == -1) return "Color";
		if (channel == 0) return "Color.R";
		if (channel == 1) return "Color.G";
		if (channel == 2) return "Color.B";
		return "Color.A";
	}
	if (passtype == SCE_PASS_EMIT) {
		if (channel == -1) return "Emit";
		if (channel == 0) return "Emit.R";
		if (channel == 1) return "Emit.G";
		return "Emit.B";
	}
	if (passtype == SCE_PASS_DIFFUSE) {
		if (channel == -1) return "Diffuse";
		if (channel == 0) return "Diffuse.R";
		if (channel == 1) return "Diffuse.G";
		return "Diffuse.B";
	}
	if (passtype == SCE_PASS_SPEC) {
		if (channel == -1) return "Spec";
		if (channel == 0) return "Spec.R";
		if (channel == 1) return "Spec.G";
		return "Spec.B";
	}
	if (passtype == SCE_PASS_SHADOW) {
		if (channel == -1) return "Shadow";
		if (channel == 0) return "Shadow.R";
		if (channel == 1) return "Shadow.G";
		return "Shadow.B";
	}
	if (passtype == SCE_PASS_AO) {
		if (channel == -1) return "AO";
		if (channel == 0) return "AO.R";
		if (channel == 1) return "AO.G";
		return "AO.B";
	}
	if (passtype == SCE_PASS_ENVIRONMENT) {
		if (channel == -1) return "Env";
		if (channel == 0) return "Env.R";
		if (channel == 1) return "Env.G";
		return "Env.B";
	}
	if (passtype == SCE_PASS_INDIRECT) {
		if (channel == -1) return "Indirect";
		if (channel == 0) return "Indirect.R";
		if (channel == 1) return "Indirect.G";
		return "Indirect.B";
	}
	if (passtype == SCE_PASS_REFLECT) {
		if (channel == -1) return "Reflect";
		if (channel == 0) return "Reflect.R";
		if (channel == 1) return "Reflect.G";
		return "Reflect.B";
	}
	if (passtype == SCE_PASS_REFRACT) {
		if (channel == -1) return "Refract";
		if (channel == 0) return "Refract.R";
		if (channel == 1) return "Refract.G";
		return "Refract.B";
	}
	if (passtype == SCE_PASS_INDEXOB) {
		if (channel == -1) return "IndexOB";
		return "IndexOB.X";
	}
	if (passtype == SCE_PASS_INDEXMA) {
		if (channel == -1) return "IndexMA";
		return "IndexMA.X";
	}
	if (passtype == SCE_PASS_MIST) {
		if (channel == -1) return "Mist";
		return "Mist.Z";
	}
	if (passtype == SCE_PASS_RAYHITS) {
		if (channel == -1) return "Rayhits";
		if (channel == 0) return "Rayhits.R";
		if (channel == 1) return "Rayhits.G";
		return "Rayhits.B";
	}
	if (passtype == SCE_PASS_DIFFUSE_DIRECT) {
		if (channel == -1) return "DiffDir";
		if (channel == 0) return "DiffDir.R";
		if (channel == 1) return "DiffDir.G";
		return "DiffDir.B";
	}
	if (passtype == SCE_PASS_DIFFUSE_INDIRECT) {
		if (channel == -1) return "DiffInd";
		if (channel == 0) return "DiffInd.R";
		if (channel == 1) return "DiffInd.G";
		return "DiffInd.B";
	}
	if (passtype == SCE_PASS_DIFFUSE_COLOR) {
		if (channel == -1) return "DiffCol";
		if (channel == 0) return "DiffCol.R";
		if (channel == 1) return "DiffCol.G";
		return "DiffCol.B";
	}
	if (passtype == SCE_PASS_GLOSSY_DIRECT) {
		if (channel == -1) return "GlossDir";
		if (channel == 0) return "GlossDir.R";
		if (channel == 1) return "GlossDir.G";
		return "GlossDir.B";
	}
	if (passtype == SCE_PASS_GLOSSY_INDIRECT) {
		if (channel == -1) return "GlossInd";
		if (channel == 0) return "GlossInd.R";
		if (channel == 1) return "GlossInd.G";
		return "GlossInd.B";
	}
	if (passtype == SCE_PASS_GLOSSY_COLOR) {
		if (channel == -1) return "GlossCol";
		if (channel == 0) return "GlossCol.R";
		if (channel == 1) return "GlossCol.G";
		return "GlossCol.B";
	}
	if (passtype == SCE_PASS_TRANSM_DIRECT) {
		if (channel == -1) return "TransDir";
		if (channel == 0) return "TransDir.R";
		if (channel == 1) return "TransDir.G";
		return "TransDir.B";
	}
	if (passtype == SCE_PASS_TRANSM_INDIRECT) {
		if (channel == -1) return "TransInd";
		if (channel == 0) return "TransInd.R";
		if (channel == 1) return "TransInd.G";
		return "TransInd.B";
	}
	if (passtype == SCE_PASS_TRANSM_COLOR) {
		if (channel == -1) return "TransCol";
		if (channel == 0) return "TransCol.R";
		if (channel == 1) return "TransCol.G";
		return "TransCol.B";
	}
	if (passtype == SCE_PASS_SUBSURFACE_DIRECT) {
		if (channel == -1) return "SubsurfaceDir";
		if (channel == 0) return "SubsurfaceDir.R";
		if (channel == 1) return "SubsurfaceDir.G";
		return "SubsurfaceDir.B";
	}
	if (passtype == SCE_PASS_SUBSURFACE_INDIRECT) {
		if (channel == -1) return "SubsurfaceInd";
		if (channel == 0) return "SubsurfaceInd.R";
		if (channel == 1) return "SubsurfaceInd.G";
		return "SubsurfaceInd.B";
	}
	if (passtype == SCE_PASS_SUBSURFACE_COLOR) {
		if (channel == -1) return "SubsurfaceCol";
		if (channel == 0) return "SubsurfaceCol.R";
		if (channel == 1) return "SubsurfaceCol.G";
		return "SubsurfaceCol.B";
	}
	return "Unknown";
}

static int passtype_from_name(const char *str)
{
	
	if (strstr(str, "Combined") == str)
		return SCE_PASS_COMBINED;

	if (strstr(str, "Depth") == str)
		return SCE_PASS_Z;

	if (strstr(str, "Vector") == str)
		return SCE_PASS_VECTOR;

	if (strstr(str, "Normal") == str)
		return SCE_PASS_NORMAL;

	if (strstr(str, "UV") == str)
		return SCE_PASS_UV;

	if (strstr(str, "Color") == str)
		return SCE_PASS_RGBA;

	if (strstr(str, "Emit") == str)
		return SCE_PASS_EMIT;

	if (strstr(str, "Diffuse") == str)
		return SCE_PASS_DIFFUSE;

	if (strstr(str, "Spec") == str)
		return SCE_PASS_SPEC;

	if (strstr(str, "Shadow") == str)
		return SCE_PASS_SHADOW;
	
	if (strstr(str, "AO") == str)
		return SCE_PASS_AO;

	if (strstr(str, "Env") == str)
		return SCE_PASS_ENVIRONMENT;

	if (strstr(str, "Indirect") == str)
		return SCE_PASS_INDIRECT;

	if (strstr(str, "Reflect") == str)
		return SCE_PASS_REFLECT;

	if (strstr(str, "Refract") == str)
		return SCE_PASS_REFRACT;

	if (strstr(str, "IndexOB") == str)
		return SCE_PASS_INDEXOB;

	if (strstr(str, "IndexMA") == str)
		return SCE_PASS_INDEXMA;

	if (strstr(str, "Mist") == str)
		return SCE_PASS_MIST;
	
	if (strstr(str, "RayHits") == str)
		return SCE_PASS_RAYHITS;

	if (strstr(str, "DiffDir") == str)
		return SCE_PASS_DIFFUSE_DIRECT;

	if (strstr(str, "DiffInd") == str)
		return SCE_PASS_DIFFUSE_INDIRECT;

	if (strstr(str, "DiffCol") == str)
		return SCE_PASS_DIFFUSE_COLOR;

	if (strstr(str, "GlossDir") == str)
		return SCE_PASS_GLOSSY_DIRECT;

	if (strstr(str, "GlossInd") == str)
		return SCE_PASS_GLOSSY_INDIRECT;

	if (strstr(str, "GlossCol") == str)
		return SCE_PASS_GLOSSY_COLOR;

	if (strstr(str, "TransDir") == str)
		return SCE_PASS_TRANSM_DIRECT;

	if (strstr(str, "TransInd") == str)
		return SCE_PASS_TRANSM_INDIRECT;

	if (strstr(str, "TransCol") == str)
		return SCE_PASS_TRANSM_COLOR;
		
	if (strcmp(str, "SubsurfaceDir") == 0)
		return SCE_PASS_SUBSURFACE_DIRECT;

	if (strcmp(str, "SubsurfaceInd") == 0)
		return SCE_PASS_SUBSURFACE_INDIRECT;

	if (strcmp(str, "SubsurfaceCol") == 0)
		return SCE_PASS_SUBSURFACE_COLOR;

	return 0;
}


static void set_pass_name(char *passname, int passtype, int channel, const char *view)
{
	const char *end;
	const char *token;
	int len;

	const char *passtype_name = name_from_passtype(passtype, channel);

	if (view == NULL || view[0] == '\0') {
		BLI_strncpy(passname, passtype_name, EXR_PASS_MAXNAME);
		return;
	}

	end = passtype_name + strlen(passtype_name);
	len = IMB_exr_split_token(passtype_name, end, &token);

	if (len == strlen(passtype_name))
		sprintf(passname, "%s.%s", passtype_name, view);
	else
		sprintf(passname, "%.*s%s.%s", (int)(end-passtype_name) - len, passtype_name, view, token);
}

/********************************** New **************************************/

static void render_layer_add_pass(RenderResult *rr, RenderLayer *rl, int channels, int passtype, int view_id)
{
	const char *view = get_view_name(&rr->views, view_id);
	const char *typestr = name_from_passtype(passtype, -1);
	RenderPass *rpass = MEM_callocN(sizeof(RenderPass), typestr);
	int rectsize = rr->rectx * rr->recty * channels;
	
	BLI_addtail(&rl->passes, rpass);
	rpass->passtype = passtype;
	rpass->channels = channels;
	rpass->rectx = rl->rectx;
	rpass->recty = rl->recty;
	rpass->view_id = view_id;

	set_pass_name(rpass->name, rpass->passtype, -1, view);
	BLI_strncpy(rpass->internal_name, typestr, sizeof(rpass->internal_name));
	BLI_strncpy(rpass->view, view, sizeof(rpass->view));
	
	if (rl->exrhandle) {
		int a;
		for (a = 0; a < channels; a++)
			IMB_exr_add_channel(rl->exrhandle, rl->name, name_from_passtype(passtype, a), view, 0, 0, NULL);
	}
	else {
		float *rect;
		int x;
		
		rpass->rect = MEM_mapallocN(sizeof(float) * rectsize, typestr);
		
		if (passtype == SCE_PASS_VECTOR) {
			/* initialize to max speed */
			rect = rpass->rect;
			for (x = rectsize - 1; x >= 0; x--)
				rect[x] = PASS_VECTOR_MAX;
		}
		else if (passtype == SCE_PASS_Z) {
			rect = rpass->rect;
			for (x = rectsize - 1; x >= 0; x--)
				rect[x] = 10e10;
		}
	}
}

/* called by main render as well for parts */
/* will read info from Render *re to define layers */
/* called in threads */
/* re->winx,winy is coordinate space of entire image, partrct the part within */
RenderResult *render_result_new(Render *re, rcti *partrct, int crop, int savebuffers, const char *layername, int view)
{
	RenderResult *rr;
	RenderLayer *rl;
	RenderView *rv;
	SceneRenderLayer *srl;
	SceneRenderView *srv;
	int rectx, recty;
	int nr, i;
	bool basic_stereo = re->r.views_setup == SCE_VIEWS_SETUP_BASIC;
	
	rectx = BLI_rcti_size_x(partrct);
	recty = BLI_rcti_size_y(partrct);
	
	if (rectx <= 0 || recty <= 0)
		return NULL;
	
	rr = MEM_callocN(sizeof(RenderResult), "new render result");
	rr->rectx = rectx;
	rr->recty = recty;
	rr->renrect.xmin = 0; rr->renrect.xmax = rectx - 2 * crop;
	/* crop is one or two extra pixels rendered for filtering, is used for merging and display too */
	rr->crop = crop;

	/* tilerect is relative coordinates within render disprect. do not subtract crop yet */
	rr->tilerect.xmin = partrct->xmin - re->disprect.xmin;
	rr->tilerect.xmax = partrct->xmax - re->disprect.xmin;
	rr->tilerect.ymin = partrct->ymin - re->disprect.ymin;
	rr->tilerect.ymax = partrct->ymax - re->disprect.ymin;
	
	if (savebuffers) {
		rr->do_exr_tile = true;
	}

	/* check renderdata for amount of views */
	if ((re->r.scemode & R_MULTIVIEW)) {
		for (srv = re->r.views.first; srv; srv = srv->next) {
			bool left, right;

			if (srv->viewflag & SCE_VIEW_DISABLE)
				continue;

			if (basic_stereo) {
				left = (strcmp(srv->name, STEREO_LEFT_NAME) == 0);
				right = (strcmp(srv->name, STEREO_RIGHT_NAME) == 0);

				if ((!left) && (!right))
					continue;
			}

			rv = MEM_callocN(sizeof(RenderView), "new render view");
			BLI_addtail(&rr->views, rv);

			BLI_strncpy(rv->name, srv->name, sizeof(rv->name));

			if (re->r.views_setup == SCE_VIEWS_SETUP_BASIC)
				rv->camera = RE_GetCameraStereo(re, left);
			else
				rv->camera = BKE_camera_multiview_advanced(re->scene, &re->r, RE_GetCamera(re), srv->suffix);
		}
	}

	/* we always need at least one view */
	if (BLI_countlist(&rr->views) == 0) {
		rv = MEM_callocN(sizeof(RenderView), "new render view");
		BLI_addtail(&rr->views, rv);

		rv->camera = RE_GetCamera(re);
	}

	/* check renderdata for amount of layers */
	for (nr = 0, srl = re->r.layers.first; srl; srl = srl->next, nr++) {

		if (layername && layername[0])
			if (strcmp(srl->name, layername) != 0)
				continue;

		if (re->r.scemode & R_SINGLE_LAYER) {
			if (nr != re->r.actlay)
				continue;
		}
		else {
			if (srl->layflag & SCE_LAY_DISABLE)
				continue;
		}
		
		rl = MEM_callocN(sizeof(RenderLayer), "new render layer");
		BLI_addtail(&rr->layers, rl);
		
		BLI_strncpy(rl->name, srl->name, sizeof(rl->name));
		rl->lay = srl->lay;
		rl->lay_zmask = srl->lay_zmask;
		rl->lay_exclude = srl->lay_exclude;
		rl->layflag = srl->layflag;
		rl->passflag = srl->passflag; /* for debugging: srl->passflag | SCE_PASS_RAYHITS; */
		rl->pass_xor = srl->pass_xor;
		rl->light_override = srl->light_override;
		rl->mat_override = srl->mat_override;
		rl->rectx = rectx;
		rl->recty = recty;
		
		if (rr->do_exr_tile) {
			rl->display_buffer = MEM_mapallocN(rectx * recty * sizeof(unsigned int), "Combined display space rgba");
			rl->exrhandle = IMB_exr_get_handle();
		}

		for (nr = 0, rv = (RenderView *)(&rr->views)->first; rv; rv=rv->next, nr++) {

			if (view != -1 && view != nr)
				continue;

			if (rr->do_exr_tile)
				IMB_exr_add_view(rl->exrhandle, rv->name);

			/* a renderlayer should always have a Combined pass*/
			render_layer_add_pass(rr, rl, 4, SCE_PASS_COMBINED, nr);

			if (srl->passflag  & SCE_PASS_Z)
				render_layer_add_pass(rr, rl, 1, SCE_PASS_Z, nr);
			if (srl->passflag  & SCE_PASS_VECTOR)
				render_layer_add_pass(rr, rl, 4, SCE_PASS_VECTOR, nr);
			if (srl->passflag  & SCE_PASS_NORMAL)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_NORMAL, nr);
			if (srl->passflag  & SCE_PASS_UV)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_UV, nr);
			if (srl->passflag  & SCE_PASS_RGBA)
				render_layer_add_pass(rr, rl, 4, SCE_PASS_RGBA, nr);
			if (srl->passflag  & SCE_PASS_EMIT)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_EMIT, nr);
			if (srl->passflag  & SCE_PASS_DIFFUSE)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_DIFFUSE, nr);
			if (srl->passflag  & SCE_PASS_SPEC)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_SPEC, nr);
			if (srl->passflag  & SCE_PASS_AO)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_AO, nr);
			if (srl->passflag  & SCE_PASS_ENVIRONMENT)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_ENVIRONMENT, nr);
			if (srl->passflag  & SCE_PASS_INDIRECT)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_INDIRECT, nr);
			if (srl->passflag  & SCE_PASS_SHADOW)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_SHADOW, nr);
			if (srl->passflag  & SCE_PASS_REFLECT)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_REFLECT, nr);
			if (srl->passflag  & SCE_PASS_REFRACT)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_REFRACT, nr);
			if (srl->passflag  & SCE_PASS_INDEXOB)
				render_layer_add_pass(rr, rl, 1, SCE_PASS_INDEXOB, nr);
			if (srl->passflag  & SCE_PASS_INDEXMA)
				render_layer_add_pass(rr, rl, 1, SCE_PASS_INDEXMA, nr);
			if (srl->passflag  & SCE_PASS_MIST)
				render_layer_add_pass(rr, rl, 1, SCE_PASS_MIST, nr);
			if (rl->passflag & SCE_PASS_RAYHITS)
				render_layer_add_pass(rr, rl, 4, SCE_PASS_RAYHITS, nr);
			if (srl->passflag  & SCE_PASS_DIFFUSE_DIRECT)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_DIFFUSE_DIRECT, nr);
			if (srl->passflag  & SCE_PASS_DIFFUSE_INDIRECT)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_DIFFUSE_INDIRECT, nr);
			if (srl->passflag  & SCE_PASS_DIFFUSE_COLOR)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_DIFFUSE_COLOR, nr);
			if (srl->passflag  & SCE_PASS_GLOSSY_DIRECT)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_GLOSSY_DIRECT, nr);
			if (srl->passflag  & SCE_PASS_GLOSSY_INDIRECT)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_GLOSSY_INDIRECT, nr);
			if (srl->passflag  & SCE_PASS_GLOSSY_COLOR)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_GLOSSY_COLOR, nr);
			if (srl->passflag  & SCE_PASS_TRANSM_DIRECT)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_TRANSM_DIRECT, nr);
			if (srl->passflag  & SCE_PASS_TRANSM_INDIRECT)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_TRANSM_INDIRECT, nr);
			if (srl->passflag  & SCE_PASS_TRANSM_COLOR)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_TRANSM_COLOR, nr);
			if (srl->passflag  & SCE_PASS_SUBSURFACE_DIRECT)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_SUBSURFACE_DIRECT, nr);
			if (srl->passflag  & SCE_PASS_SUBSURFACE_INDIRECT)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_SUBSURFACE_INDIRECT, nr);
			if (srl->passflag  & SCE_PASS_SUBSURFACE_COLOR)
				render_layer_add_pass(rr, rl, 3, SCE_PASS_SUBSURFACE_COLOR, nr);
		}
	}
	/* sss, previewrender and envmap don't do layers, so we make a default one */
	if (BLI_listbase_is_empty(&rr->layers) && !(layername && layername[0])) {
		rl = MEM_callocN(sizeof(RenderLayer), "new render layer");
		BLI_addtail(&rr->layers, rl);
		
		rl->rectx = rectx;
		rl->recty = recty;

		/* duplicate code... */
		if (rr->do_exr_tile) {
			rl->display_buffer = MEM_mapallocN(rectx * recty * sizeof(unsigned int), "Combined display space rgba");
			rl->exrhandle = IMB_exr_get_handle();
		}

		nr = 0;
		for (rv = (RenderView *)(&rr->views)->first; rv; rv=rv->next, nr++) {

			if (view != -1 && view != nr)
				continue;

			if (rr->do_exr_tile) {
				IMB_exr_add_view(rl->exrhandle, rv->name);

				for (i=0; i < 4; i++)
					IMB_exr_add_channel(rl->exrhandle, rl->name, name_from_passtype(SCE_PASS_COMBINED, i), rv->name, 0, 0, NULL);
			}
			else {
				render_layer_add_pass(rr, rl, 4, SCE_PASS_COMBINED, nr);
			}
		}

		/* note, this has to be in sync with scene.c */
		rl->lay = (1 << 20) - 1;
		rl->layflag = 0x7FFF;    /* solid ztra halo strand */
		rl->passflag = SCE_PASS_COMBINED;
		
		re->r.actlay = 0;
	}
	
	/* border render; calculate offset for use in compositor. compo is centralized coords */
	/* XXX obsolete? I now use it for drawing border render offset (ton) */
	rr->xof = re->disprect.xmin + BLI_rcti_cent_x(&re->disprect) - (re->winx / 2);
	rr->yof = re->disprect.ymin + BLI_rcti_cent_y(&re->disprect) - (re->winy / 2);
	
	return rr;
}

/* allocate osa new results for samples */
RenderResult *render_result_new_full_sample(Render *re, ListBase *lb, rcti *partrct, int crop, int savebuffers, int view)
{
	int a;
	
	if (re->osa == 0)
		return render_result_new(re, partrct, crop, savebuffers, RR_ALL_LAYERS, view);
	
	for (a = 0; a < re->osa; a++) {
		RenderResult *rr = render_result_new(re, partrct, crop, savebuffers, RR_ALL_LAYERS, view);
		BLI_addtail(lb, rr);
		rr->sample_nr = a;
	}
	
	return lb->first;
}

/* callbacks for render_result_new_from_exr */
static void *ml_addlayer_cb(void *base, const char *str)
{
	RenderResult *rr = base;
	RenderLayer *rl;
	
	rl = MEM_callocN(sizeof(RenderLayer), "new render layer");
	BLI_addtail(&rr->layers, rl);
	
	BLI_strncpy(rl->name, str, EXR_LAY_MAXNAME);
	return rl;
}

static void ml_addpass_cb(void *UNUSED(base), void *lay, const char *str, float *rect, int totchan, const char *chan_id, const char *view, const int view_id)
{
	RenderLayer *rl = lay;
	RenderPass *rpass = MEM_callocN(sizeof(RenderPass), "loaded pass");
	int a;
	
	BLI_addtail(&rl->passes, rpass);
	rpass->channels = totchan;
	rpass->passtype = passtype_from_name(str);
	if (rpass->passtype == 0) printf("unknown pass %s\n", str);
	rl->passflag |= rpass->passtype;
	
	/* channel id chars */
	for (a = 0; a < totchan; a++)
		rpass->chan_id[a] = chan_id[a];

	rpass->rect = rect;
	if (view[0] != '\0')
		BLI_snprintf(rpass->name, sizeof(rpass->name), "%s.%s", str, view);
	else
		BLI_strncpy(rpass->name,  str, sizeof(rpass->name));

	rpass->view_id = view_id;
	BLI_strncpy(rpass->view, view, sizeof(rpass->view));
	BLI_strncpy(rpass->internal_name, str, sizeof(rpass->internal_name));
}

static void *ml_addview_cb(void *base, const char *str)
{
	RenderResult *rr = base;
	RenderView *rv;

	rv = MEM_callocN(sizeof(RenderView), "new render view");
	BLI_addtail(&rr->views, rv);

	BLI_strncpy(rv->name, str, EXR_VIEW_MAXNAME);
	return rv;
}

static int order_render_passes(void *a, void *b)
{
	// 1 if a is after b
	RenderPass *rpa = (RenderPass *) a;
	RenderPass *rpb = (RenderPass *) b;

	if (rpa->passtype > rpb->passtype)
		return 1;
	else if (rpa->passtype < rpb->passtype)
		return 0;

	/* they have the same type */
	/* left first */
	if (strcmp(rpa->view, STEREO_LEFT_NAME) == 0)
		return 0;
	else if (strcmp(rpb->view, STEREO_LEFT_NAME) == 0)
		return 1;

	/* right second */
	if (strcmp(rpa->view, STEREO_RIGHT_NAME) == 0)
		return 0;
	else if (strcmp(rpb->view, STEREO_RIGHT_NAME) == 0)
		return 1;

	/* remaining in ascending id order */
	return (rpa->view_id < rpb->view_id);
}

/* from imbuf, if a handle was returned we convert this to render result */
RenderResult *render_result_new_from_exr(void *exrhandle, const char *colorspace, bool predivide, int rectx, int recty)
{
	RenderResult *rr = MEM_callocN(sizeof(RenderResult), __func__);
	RenderLayer *rl;
	RenderPass *rpass;
	const char *to_colorspace = IMB_colormanagement_role_colorspace_name_get(COLOR_ROLE_SCENE_LINEAR);
	int i;

	rr->rectx = rectx;
	rr->recty = recty;

	for(i=0; i < BLI_countlist(&rr->views);i++);
	
	IMB_exr_multilayer_convert(exrhandle, rr, ml_addview_cb, ml_addlayer_cb, ml_addpass_cb);

	for (rl = rr->layers.first; rl; rl = rl->next) {
		int c=0;
		rl->rectx = rectx;
		rl->recty = recty;

		BLI_sortlist(&rl->passes, order_render_passes);

		for (rpass = rl->passes.first; rpass; rpass = rpass->next) {
			printf("%d: %s\n", c++, rpass->name);
			rpass->rectx = rectx;
			rpass->recty = recty;

			if (rpass->channels >= 3) {
				IMB_colormanagement_transform(rpass->rect, rpass->rectx, rpass->recty, rpass->channels,
				                              colorspace, to_colorspace, predivide);
			}
		}
	}
	
	return rr;
}

/*********************************** Merge ***********************************/

static void do_merge_tile(RenderResult *rr, RenderResult *rrpart, float *target, float *tile, int pixsize)
{
	int y, ofs, copylen, tilex, tiley;
	
	copylen = tilex = rrpart->rectx;
	tiley = rrpart->recty;
	
	if (rrpart->crop) { /* filters add pixel extra */
		tile += pixsize * (rrpart->crop + rrpart->crop * tilex);
		
		copylen = tilex - 2 * rrpart->crop;
		tiley -= 2 * rrpart->crop;
		
		ofs = (rrpart->tilerect.ymin + rrpart->crop) * rr->rectx + (rrpart->tilerect.xmin + rrpart->crop);
		target += pixsize * ofs;
	}
	else {
		ofs = (rrpart->tilerect.ymin * rr->rectx + rrpart->tilerect.xmin);
		target += pixsize * ofs;
	}

	copylen *= sizeof(float) * pixsize;
	tilex *= pixsize;
	ofs = pixsize * rr->rectx;

	for (y = 0; y < tiley; y++) {
		memcpy(target, tile, copylen);
		target += ofs;
		tile += tilex;
	}
}

/* used when rendering to a full buffer, or when reading the exr part-layer-pass file */
/* no test happens here if it fits... we also assume layers are in sync */
/* is used within threads */
void render_result_merge(RenderResult *rr, RenderResult *rrpart)
{
	RenderLayer *rl, *rlp;
	RenderPass *rpass, *rpassp;
	
	for (rl = rr->layers.first; rl; rl = rl->next) {
		rlp = RE_GetRenderLayer(rrpart, rl->name);
		if (rlp) {
			/* passes are allocated in sync */
			for (rpass = rl->passes.first, rpassp = rlp->passes.first;
			     rpass && rpassp;
			     rpass = rpass->next)
			{
				/* renderresult have all passes, renderpart only the active view's passes */
				if (strcmp(rpassp->name, rpass->name) != 0)
					continue;

				do_merge_tile(rr, rrpart, rpass->rect, rpassp->rect, rpass->channels);

				/* manually get next render pass */
				rpassp = rpassp->next;
			}
		}
	}
}

/* for passes read from files, these have names stored */
static char *make_pass_name(RenderPass *rpass, int chan)
{
	static char name[EXR_PASS_MAXNAME];
	int len;
	
	BLI_strncpy(name, rpass->name, EXR_PASS_MAXNAME);
	len = strlen(name);
	name[len] = '.';
	name[len + 1] = rpass->chan_id[chan];
	name[len + 2] = 0;

	return name;
}

static RenderPass *render_pass_find(ListBase *passes, const int passtype, const int view_id)
{
	RenderPass *rp;

	for (rp = passes->first; rp; rp = rp->next) {
		if (rp->passtype == passtype &&
			rp->view_id == view_id)
		{
			return rp;
		}
	}
	return NULL;
}

/* returns the number of stereo (left and right) passes in a Render Result */
static size_t render_pass_stereo_count(RenderResult *rr)
{
	size_t totpasses = 0;

	RenderLayer *rl;
	RenderPass *rpass;

	const char *names[2] = {STEREO_LEFT_NAME, STEREO_RIGHT_NAME};
	int view_id[2]= {
		BLI_findstringindex(&rr->views, names[0], offsetof(RenderView, name)),
		BLI_findstringindex(&rr->views, names[1], offsetof(RenderView, name))
	};

	for (rl = rr->layers.first; rl; rl = rl->next) {
		for (rpass = rl->passes.first; rpass; rpass = rpass->next) {
			if ((rpass->view_id == view_id[0]) ||
				(rpass->view_id == view_id[1]))
			{
				totpasses++;
			}
		}
	}

	return totpasses;
}

/* called from within UI and render pipeline, saves both rendered result as a file-read result
 * if multiview is true saves all views in a multiview exr
 * else if view is not NULL saves single view
 * else saves stereo3d
 */
bool RE_WriteRenderResult(ReportList *reports, RenderResult *rr, const char *filename, ImageFormatData *imf, const bool multiview, const char *view)
{
	RenderLayer *rl;
	RenderPass *rpass;
	RenderView *rview;
	void *exrhandle = IMB_exr_get_handle();
	bool success = false;
	int a, nr, i;
	const char *chan_view = NULL;
	int compress = (imf ? imf->exr_codec : 0);
	size_t width, height;

	float **rectf_stereo = NULL;
	size_t rectf_stereo_count = 0;
	float *rectf = NULL;

	const bool is_mono = view && !multiview;
	const bool is_stereo3d = (!view) && (!multiview);
	const char *names[2] = {STEREO_LEFT_NAME, STEREO_RIGHT_NAME};

	width = rr->rectx;
	height = rr->recty;

	if (is_stereo3d) {
		RenderView *rv[2];
		int view_id[2];
		size_t totpasses;

		for (i = 0; i < 2; i++) {
			view_id[i] = BLI_findstringindex(&rr->views, names[i], offsetof(RenderView, name));

			if (view_id[i] == -1) {
				printf("Stereo 3D output is only supported in images that contain left and right views.\n");
				goto cleanup;
			}
		}

		totpasses = render_pass_stereo_count(rr);
		/* one per stereo pair plus one for the combined pass */
		rectf_stereo = MEM_mallocN(sizeof(float *) * ((totpasses / 2) + 1), "RenderResult stereo pairs");

		IMB_exr_add_view(exrhandle, "");
		IMB_stereo_dimensions(imf->stereo_output.display_mode, rr->rectx, rr->recty, &width, &height);

		for (i = 0; i < 2; i++) {
			rv[i] = BLI_findstring(&rr->views, names[i], offsetof(RenderView, name));
		}

		if (rv[0]->rectf && rv[1]->rectf) {
			rectf = IMB_stereo_from_rectf(imf, rr->rectx, rr->recty, 4,
			                              rv[0]->rectf, rv[1]->rectf);

			for (a = 0; a < 4; a++)
				IMB_exr_add_channel(exrhandle, "Composite", name_from_passtype(SCE_PASS_COMBINED, a),
				                    "", 4, 4 * width, rectf + a);

			rectf_stereo[rectf_stereo_count++] = rectf;
		}

		/* add layers/passes and assign channels */
		for (rl = rr->layers.first; rl; rl = rl->next) {
			int passflag = 0;

			/* passes are allocated in sync */
			for (rpass = rl->passes.first; rpass; rpass = rpass->next) {
				int xstride = rpass->channels;
				RenderPass *rp[2];

				/* do this once per type */
				if ((passflag & rpass->passtype))
					continue;

				passflag |= rpass->passtype;

				for (i = 0; i < 2; i++)
					rp[i] = render_pass_find(&rl->passes, rpass->passtype, view_id[i]);

				if ((rp[0] == NULL) || (rp[0]->rect == NULL) ||
				    (rp[1] == NULL) || (rp[1]->rect == NULL))
				{
					/* pass with incomplete stereo 3d data */
					continue;
				}

				rectf = IMB_stereo_from_rectf(imf, rr->rectx, rr->recty, xstride,
				                              rp[0]->rect, rp[1]->rect);

				for (a = 0; a < xstride; a++) {
					if (rpass->passtype) {
						IMB_exr_add_channel(exrhandle, rl->name, name_from_passtype(rpass->passtype, a), chan_view,
						                    xstride, xstride * width, rectf + a);
					}
					else {
						IMB_exr_add_channel(exrhandle, rl->name, make_pass_name(rpass, a), chan_view,
						                    xstride, xstride * width, rectf + a);
					}
				}

				rectf_stereo[rectf_stereo_count++] = rectf;
			}
		}
	}
	else {
		for (nr = 0, rview = (RenderView *) rr->views.first; rview; rview = rview->next, nr++) {
			if (is_mono) {
				if (strcmp (view, rview->name) != 0)
					continue;
				else
					chan_view = "";
			}
			else {
				/* if rendered only one view, we treat as a a non-view render */
				chan_view = rview->name;
			}

			IMB_exr_add_view(exrhandle, rview->name);

			if (rview->rectf) {
				for (a = 0; a < 4; a++)
					IMB_exr_add_channel(exrhandle, "Composite", name_from_passtype(SCE_PASS_COMBINED, a),
					                    chan_view, 4, 4 * width, rview->rectf + a);
			}
		}

		/* add layers/passes and assign channels */
		for (rl = rr->layers.first; rl; rl = rl->next) {

			/* passes are allocated in sync */
			for (rpass = rl->passes.first; rpass; rpass = rpass->next) {
				int xstride = rpass->channels;

				if (is_mono) {
					if (strcmp (view, rpass->view) != 0)
						continue;
					else
						chan_view = "";
				}
				else {
					/* if rendered only one view, we treat as a a non-view render */
					chan_view = (nr > 1 ? get_view_name(&rr->views, rpass->view_id):"");
				}

				for (a = 0; a < xstride; a++) {

					if (rpass->passtype) {
						IMB_exr_add_channel(exrhandle, rl->name, name_from_passtype(rpass->passtype, a), chan_view,
						                    xstride, xstride * width, rpass->rect + a);
					}
					else {
						IMB_exr_add_channel(exrhandle, rl->name, make_pass_name(rpass, a), chan_view,
						                    xstride, xstride * width, rpass->rect + a);
					}
				}
			}
		}
	}

	BLI_make_existing_file(filename);

	/* when the filename has no permissions, this can fail */
	if (multiview) {
		if (IMB_exrmultiview_begin_write(exrhandle, filename, width, height, compress, false)) {
			IMB_exrmultiview_write_channels(exrhandle, -1);
			success = true;
		}
	} else {
		if (IMB_exr_begin_write(exrhandle, filename, width, height, compress)) {
			IMB_exr_write_channels(exrhandle);
			success = true;
		}
	}

	if (success == false) {
		/* TODO, get the error from openexr's exception */
		BKE_report(reports, RPT_ERROR, "Error writing render result (see console)");
		success = false;
	}

cleanup:
	IMB_exr_close(exrhandle);

	for (i = 0; i < rectf_stereo_count; i++)
		MEM_freeN(rectf_stereo[i]);

	return success;
}

/**************************** Single Layer Rendering *************************/

void render_result_single_layer_begin(Render *re)
{
	/* all layers except the active one get temporally pushed away */

	/* officially pushed result should be NULL... error can happen with do_seq */
	RE_FreeRenderResult(re->pushedresult);
	
	re->pushedresult = re->result;
	re->result = NULL;
}

/* if scemode is R_SINGLE_LAYER, at end of rendering, merge the both render results */
void render_result_single_layer_end(Render *re)
{
	SceneRenderLayer *srl;
	RenderLayer *rlpush;
	RenderLayer *rl;
	int nr;

	if (re->result == NULL) {
		printf("pop render result error; no current result!\n");
		return;
	}

	if (!re->pushedresult)
		return;

	if (re->pushedresult->rectx == re->result->rectx && re->pushedresult->recty == re->result->recty) {
		/* find which layer in re->pushedresult should be replaced */
		rl = re->result->layers.first;
		
		/* render result should be empty after this */
		BLI_remlink(&re->result->layers, rl);
		
		/* reconstruct render result layers */
		for (nr = 0, srl = re->r.layers.first; srl; srl = srl->next, nr++) {
			if (nr == re->r.actlay) {
				BLI_addtail(&re->result->layers, rl);
			}
			else {
				rlpush = RE_GetRenderLayer(re->pushedresult, srl->name);
				if (rlpush) {
					BLI_remlink(&re->pushedresult->layers, rlpush);
					BLI_addtail(&re->result->layers, rlpush);
				}
			}
		}
	}

	RE_FreeRenderResult(re->pushedresult);
	re->pushedresult = NULL;
}

/************************* EXR Tile File Rendering ***************************/

static void save_render_result_tile(RenderResult *rr, RenderResult *rrpart, int view_id)
{
	RenderLayer *rlp, *rl;
	RenderPass *rpassp;
	int offs, partx, party;
	
	BLI_lock_thread(LOCK_IMAGE);
	
	for (rlp = rrpart->layers.first; rlp; rlp = rlp->next) {
		rl = RE_GetRenderLayer(rr, rlp->name);

		/* should never happen but prevents crash if it does */
		BLI_assert(rl);
		if (UNLIKELY(rl == NULL)) {
			continue;
		}

		if (rrpart->crop) { /* filters add pixel extra */
			offs = (rrpart->crop + rrpart->crop * rrpart->rectx);
		}
		else {
			offs = 0;
		}

		/* passes are allocated in sync */
		for (rpassp = rlp->passes.first; rpassp; rpassp = rpassp->next) {
			int a, xstride = rpassp->channels;
			const char *viewname = get_view_name(&rr->views, rpassp->view_id);
			char passname[EXR_PASS_MAXNAME];

			for (a = 0; a < xstride; a++) {
				set_pass_name(passname, rpassp->passtype, a, viewname);

				IMB_exr_set_channel(rl->exrhandle, rlp->name, passname,
				                    xstride, xstride * rrpart->rectx, rpassp->rect + a + xstride * offs);
			}
		}
		
	}

	party = rrpart->tilerect.ymin + rrpart->crop;
	partx = rrpart->tilerect.xmin + rrpart->crop;

	for (rlp = rrpart->layers.first; rlp; rlp = rlp->next) {
		rl = RE_GetRenderLayer(rr, rlp->name);

		/* should never happen but prevents crash if it does */
		BLI_assert(rl);
		if (UNLIKELY(rl == NULL)) {
			continue;
		}

		IMB_exrtile_write_channels(rl->exrhandle, partx, party, 0, view_id);
	}

	BLI_unlock_thread(LOCK_IMAGE);
}

static void save_empty_result_tiles(Render *re)
{
	RenderPart *pa;
	RenderResult *rr;
	RenderLayer *rl;
	
	for (rr = re->result; rr; rr = rr->next) {
		for (rl = rr->layers.first; rl; rl = rl->next) {
			IMB_exr_clear_channels(rl->exrhandle);
		
			for (pa = re->parts.first; pa; pa = pa->next) {
				if (pa->status != PART_STATUS_READY) {
					int party = pa->disprect.ymin - re->disprect.ymin + pa->crop;
					int partx = pa->disprect.xmin - re->disprect.xmin + pa->crop;
					IMB_exrtile_write_channels(rl->exrhandle, partx, party, 0, re->actview);
				}
			}
		}
	}
}

/* begin write of exr tile file */
void render_result_exr_file_begin(Render *re)
{
	RenderResult *rr;
	RenderLayer *rl;
	char str[FILE_MAX];

	for (rr = re->result; rr; rr = rr->next) {
		for (rl = rr->layers.first; rl; rl = rl->next) {
			render_result_exr_file_path(re->scene, rl->name, rr->sample_nr, str);
			printf("write exr tmp file, %dx%d, %s\n", rr->rectx, rr->recty, str);
			IMB_exrtile_begin_write(rl->exrhandle, str, 0, rr->rectx, rr->recty, re->partx, re->party);
		}
	}
}

/* end write of exr tile file, read back first sample */
void render_result_exr_file_end(Render *re)
{
	RenderResult *rr;
	RenderLayer *rl;

	save_empty_result_tiles(re);
	
	for (rr = re->result; rr; rr = rr->next) {
		for (rl = rr->layers.first; rl; rl = rl->next) {
			IMB_exr_close(rl->exrhandle);
			rl->exrhandle = NULL;
		}

		rr->do_exr_tile = false;
	}
	
	render_result_free_list(&re->fullresult, re->result);
	re->result = NULL;

	render_result_exr_file_read_sample(re, 0);
}

/* save part into exr file */
void render_result_exr_file_merge(RenderResult *rr, RenderResult *rrpart, int view)
{
	for (; rr && rrpart; rr = rr->next, rrpart = rrpart->next)
		save_render_result_tile(rr, rrpart, view);
}

/* path to temporary exr file */
void render_result_exr_file_path(Scene *scene, const char *layname, int sample, char *filepath)
{
	char name[FILE_MAXFILE + MAX_ID_NAME + MAX_ID_NAME + 100], fi[FILE_MAXFILE];
	
	BLI_split_file_part(G.main->name, fi, sizeof(fi));
	if (sample == 0) {
		BLI_snprintf(name, sizeof(name), "%s_%s_%s.exr", fi, scene->id.name + 2, layname);
	}
	else {
		BLI_snprintf(name, sizeof(name), "%s_%s_%s%d.exr", fi, scene->id.name + 2, layname, sample);
	}

	BLI_make_file_string("/", filepath, BLI_temp_dir_session(), name);
}

/* only for temp buffer, makes exact copy of render result */
int render_result_exr_file_read_sample(Render *re, int sample)
{
	RenderLayer *rl;
	char str[FILE_MAXFILE + MAX_ID_NAME + MAX_ID_NAME + 100] = "";
	bool success = true;

	RE_FreeRenderResult(re->result);
	re->result = render_result_new(re, &re->disprect, 0, RR_USE_MEM, RR_ALL_LAYERS, -1);

	for (rl = re->result->layers.first; rl; rl = rl->next) {
		render_result_exr_file_path(re->scene, rl->name, sample, str);
		printf("read exr tmp file: %s\n", str);

		if (!render_result_exr_file_read_path(re->result, rl, str)) {
			printf("cannot read: %s\n", str);
			success = false;
		}
	}

	return success;
}

/* called for reading temp files, and for external engines */
int render_result_exr_file_read_path(RenderResult *rr, RenderLayer *rl_single, const char *filepath)
{
	RenderLayer *rl;
	RenderPass *rpass;
	void *exrhandle = IMB_exr_get_handle();
	int rectx, recty;

	if (IMB_exr_begin_read(exrhandle, filepath, &rectx, &recty) == 0) {
		printf("failed being read %s\n", filepath);
		IMB_exr_close(exrhandle);
		return 0;
	}

	if (rr == NULL || rectx != rr->rectx || recty != rr->recty) {
		if (rr)
			printf("error in reading render result: dimensions don't match\n");
		else
			printf("error in reading render result: NULL result pointer\n");
		IMB_exr_close(exrhandle);
		return 0;
	}

	for (rl = rr->layers.first; rl; rl = rl->next) {
		if (rl_single && rl_single != rl)
			continue;
		
		/* passes are allocated in sync */
		for (rpass = rl->passes.first; rpass; rpass = rpass->next) {
			int a, xstride = rpass->channels;
			const char *viewname = get_view_name(&rr->views, rpass->view_id);
			char passname[EXR_PASS_MAXNAME];

			for (a = 0; a < xstride; a++) {
				set_pass_name(passname, rpass->passtype, a, viewname);
				IMB_exr_set_channel(exrhandle, rl->name, passname,
				                    xstride, xstride * rectx, rpass->rect + a);
			}

			set_pass_name(rpass->name, rpass->passtype, -1, viewname);
		}
	}

	IMB_exr_read_channels(exrhandle);
	IMB_exr_close(exrhandle);

	return 1;
}

static void render_result_exr_file_cache_path(Scene *sce, const char *root, char *r_path)
{
	char filename_full[FILE_MAX + MAX_ID_NAME + 100], filename[FILE_MAXFILE], dirname[FILE_MAXDIR];
	char path_digest[16] = {0};
	char path_hexdigest[33];

	/* If root is relative, use either current .blend file dir, or temp one if not saved. */
	if (G.main->name[0]) {
		BLI_split_dirfile(G.main->name, dirname, filename, sizeof(dirname), sizeof(filename));
		BLI_replace_extension(filename, sizeof(filename), "");  /* strip '.blend' */
		md5_buffer(G.main->name, strlen(G.main->name), path_digest);
	}
	else {
		BLI_strncpy(dirname, BLI_temp_dir_base(), sizeof(dirname));
		BLI_strncpy(filename, "UNSAVED", sizeof(filename));
	}
	md5_to_hexdigest(path_digest, path_hexdigest);

	/* Default to *non-volatile* tmp dir. */
	if (*root == '\0') {
		root = BLI_temp_dir_base();
	}

	BLI_snprintf(filename_full, sizeof(filename_full), "cached_RR_%s_%s_%s.exr",
	             filename, sce->id.name + 2, path_hexdigest);
	BLI_make_file_string(dirname, r_path, root, filename_full);
}

void render_result_exr_file_cache_write(Render *re)
{
	RenderResult *rr = re->result;
	char str[FILE_MAXFILE + FILE_MAXFILE + MAX_ID_NAME + 100];
	char *root = U.render_cachedir;

	render_result_exr_file_cache_path(re->scene, root, str);
	printf("Caching exr file, %dx%d, %s\n", rr->rectx, rr->recty, str);

	RE_WriteRenderResult(NULL, rr, str, NULL, true, NULL);
}

/* For cache, makes exact copy of render result */
bool render_result_exr_file_cache_read(Render *re)
{
	char str[FILE_MAXFILE + MAX_ID_NAME + MAX_ID_NAME + 100] = "";
	char *root = U.render_cachedir;

	RE_FreeRenderResult(re->result);
	re->result = render_result_new(re, &re->disprect, 0, RR_USE_MEM, RR_ALL_LAYERS, -1);

	/* First try cache. */
	render_result_exr_file_cache_path(re->scene, root, str);

	printf("read exr cache file: %s\n", str);
	if (!render_result_exr_file_read_path(re->result, NULL, str)) {
		printf("cannot read: %s\n", str);
		return false;
	}
	return true;
}

/*************************** Combined Pixel Rect *****************************/

ImBuf *render_result_rect_to_ibuf(RenderResult *rr, RenderData *rd, const int view_id)
{
	ImBuf *ibuf = IMB_allocImBuf(rr->rectx, rr->recty, rd->im_format.planes, 0);
	
	/* if not exists, BKE_imbuf_write makes one */
	ibuf->rect = (unsigned int *) RE_RenderViewGetRect32(rr, view_id);
	ibuf->rect_float = RE_RenderViewGetRectf(rr, view_id);
	ibuf->zbuf_float = RE_RenderViewGetRectz(rr, view_id);
	
	/* float factor for random dither, imbuf takes care of it */
	ibuf->dither = rd->dither_intensity;
	
	/* prepare to gamma correct to sRGB color space
	 * note that sequence editor can generate 8bpc render buffers
	 */
	if (ibuf->rect) {
		if (BKE_imtype_valid_depths(rd->im_format.imtype) & (R_IMF_CHAN_DEPTH_12 | R_IMF_CHAN_DEPTH_16 | R_IMF_CHAN_DEPTH_24 | R_IMF_CHAN_DEPTH_32)) {
			if (rd->im_format.depth == R_IMF_CHAN_DEPTH_8) {
				/* Higher depth bits are supported but not needed for current file output. */
				ibuf->rect_float = NULL;
			}
			else {
				IMB_float_from_rect(ibuf);
			}
		}
		else {
			/* ensure no float buffer remained from previous frame */
			ibuf->rect_float = NULL;
		}
	}

	/* color -> grayscale */
	/* editing directly would alter the render view */
	if (rd->im_format.planes == R_IMF_PLANES_BW) {
		ImBuf *ibuf_bw = IMB_dupImBuf(ibuf);
		IMB_color_to_bw(ibuf_bw);
		IMB_freeImBuf(ibuf);
		ibuf = ibuf_bw;
	}

	return ibuf;
}

void render_result_rect_from_ibuf(RenderResult *rr, RenderData *UNUSED(rd), ImBuf *ibuf)
{
	if (ibuf->rect_float) {
		if (!rr->rectf)
			rr->rectf = MEM_mallocN(4 * sizeof(float) * rr->rectx * rr->recty, "render_seq rectf");
		
		memcpy(rr->rectf, ibuf->rect_float, 4 * sizeof(float) * rr->rectx * rr->recty);

		/* TSK! Since sequence render doesn't free the *rr render result, the old rect32
		 * can hang around when sequence render has rendered a 32 bits one before */
		if (rr->rect32) {
			MEM_freeN(rr->rect32);
			rr->rect32 = NULL;
		}
	}
	else if (ibuf->rect) {
		if (!rr->rect32)
			rr->rect32 = MEM_mallocN(sizeof(int) * rr->rectx * rr->recty, "render_seq rect");

		memcpy(rr->rect32, ibuf->rect, 4 * rr->rectx * rr->recty);

		/* Same things as above, old rectf can hang around from previous render. */
		if (rr->rectf) {
			MEM_freeN(rr->rectf);
			rr->rectf = NULL;
		}
	}
}

void render_result_rect_fill_zero(RenderResult *rr)
{
	if (rr->rectf)
		memset(rr->rectf, 0, 4 * sizeof(float) * rr->rectx * rr->recty);
	else if (rr->rect32)
		memset(rr->rect32, 0, 4 * rr->rectx * rr->recty);
	else
		rr->rect32 = MEM_callocN(sizeof(int) * rr->rectx * rr->recty, "render_seq rect");
}

void render_result_rect_get_pixels(RenderResult *rr, unsigned int *rect, int rectx, int recty,
                                   const ColorManagedViewSettings *view_settings, const ColorManagedDisplaySettings *display_settings,
                                   const int view_id)
{
	if (rr->rect32) {
		int *rect32 = RE_RenderViewGetRect32(rr, view_id);
		memcpy(rect, (rect32 ? rect32 : rr->rect32), sizeof(int) * rr->rectx * rr->recty);
	}
	else if (rr->rectf) {
		float *rectf = RE_RenderViewGetRectf(rr, view_id);
		IMB_display_buffer_transform_apply((unsigned char *) rect, (rectf ? rectf : rr->rectf), rr->rectx, rr->recty, 4,
		                                   view_settings, display_settings, true);
	}
	else
		/* else fill with black */
		memset(rect, 0, sizeof(int) * rectx * recty);
}


/*************************** multiview functions *****************************/

bool RE_HasFakeLayer(RenderResult *res)
{
	RenderView *rv;

	if (res == NULL)
		return false;

	rv = (RenderView *)res->views.first;
	if (rv == NULL)
		return false;

	return (rv->rect32 || rv->rectf);
}

bool RE_RenderResult_is_stereo(RenderResult *res)
{
	if (! BLI_findstring(&res->views, STEREO_LEFT_NAME, offsetof(RenderView, name)))
		return false;

	if (! BLI_findstring(&res->views, STEREO_RIGHT_NAME, offsetof(RenderView, name)))
		return false;

	return true;
}

void RE_RenderViewSetRectf(RenderResult *res, const int view_id, float *rect)
{
	RenderView *rv;
	size_t nr = 0;

	for (nr = 0, rv = res->views.first; rv; rv = rv->next, nr++) {
		if (nr == view_id) {
			rv->rectf = rect;
			return;
		}
	}
}

void RE_RenderViewSetRectz(RenderResult *res, const int view_id, float *rect)
{
	RenderView *rv;
	size_t nr = 0;

	for (nr = 0, rv = res->views.first; rv; rv = rv->next, nr++) {
		if (nr == view_id) {
			rv->rectz = rect;
			return;
		}
	}
}

float *RE_RenderViewGetRectz(RenderResult *res, const int view_id)
{
	RenderView *rv;
	size_t nr = 0;

	for (nr = 0, rv = res->views.first; rv; rv = rv->next, nr++)
		if (nr == view_id)
			return rv->rectz;

	return res->rectz;
}

float *RE_RenderViewGetRectf(RenderResult *res, const int view_id)
{
	RenderView *rv;
	size_t nr = 0;

	for (nr = 0, rv = res->views.first; rv; rv = rv->next, nr++)
		if (nr == view_id)
			return rv->rectf;

	return res->rectf;
}

int *RE_RenderViewGetRect32(RenderResult *res, const int view_id)
{
	RenderView *rv;
	size_t nr;

	for (nr = 0, rv = res->views.first; rv; rv = rv->next, nr++)
		if (nr == view_id)
			return rv->rect32;

	return res->rect32;
}



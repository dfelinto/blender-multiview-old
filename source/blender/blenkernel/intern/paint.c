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
 * along with this program; if not, write to the Free Software  Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2009 by Nicholas Bishop
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file blender/blenkernel/intern/paint.c
 *  \ingroup bke
 */

#include <stdlib.h>
#include <string.h>

#include "DNA_object_types.h"
#include "DNA_mesh_types.h"
#include "DNA_meshdata_types.h"
#include "DNA_scene_types.h"
#include "DNA_brush_types.h"
#include "DNA_space_types.h"

#include "BLI_bitmap.h"
#include "BLI_utildefines.h"
#include "BLI_math_vector.h"

#include "BKE_brush.h"
#include "BKE_context.h"
#include "BKE_global.h"
#include "BKE_library.h"
#include "BKE_paint.h"
#include "BKE_subsurf.h"

#include "bmesh.h"

const char PAINT_CURSOR_SCULPT[3] = {255, 100, 100};
const char PAINT_CURSOR_VERTEX_PAINT[3] = {255, 255, 255};
const char PAINT_CURSOR_WEIGHT_PAINT[3] = {200, 200, 255};
const char PAINT_CURSOR_TEXTURE_PAINT[3] = {255, 255, 255};

static int overlay_flags = 0;

void BKE_paint_invalidate_overlay_tex (Scene *scene, const Tex *tex)
{
	Paint *p = BKE_paint_get_active(scene);
	Brush *br = p->brush;

	if (br->mtex.tex == tex)
		overlay_flags |= PAINT_INVALID_OVERLAY_TEXTURE_PRIMARY;
	if (br->mask_mtex.tex == tex)
		overlay_flags |= PAINT_INVALID_OVERLAY_TEXTURE_SECONDARY;
}

void BKE_paint_invalidate_cursor_overlay (Scene *scene, CurveMapping *curve)
{
	Paint *p = BKE_paint_get_active(scene);
	Brush *br = p->brush;

	if (br->curve == curve)
		overlay_flags |= PAINT_INVALID_OVERLAY_CURVE;
}

void BKE_paint_invalidate_overlay_all(void)
{
	overlay_flags |= (PAINT_INVALID_OVERLAY_TEXTURE_SECONDARY |
	                  PAINT_INVALID_OVERLAY_TEXTURE_PRIMARY |
	                  PAINT_INVALID_OVERLAY_CURVE);
}

int BKE_paint_get_overlay_flags(void)
{
	return overlay_flags;
}

void BKE_paint_reset_overlay_invalid(void)
{
	overlay_flags &= ~(PAINT_INVALID_OVERLAY_TEXTURE_PRIMARY |
	                   PAINT_INVALID_OVERLAY_TEXTURE_SECONDARY |
	                   PAINT_INVALID_OVERLAY_CURVE);
}


Paint *BKE_paint_get_active(Scene *sce)
{
	if (sce) {
		ToolSettings *ts = sce->toolsettings;
		
		if (sce->basact && sce->basact->object) {
			switch (sce->basact->object->mode) {
				case OB_MODE_SCULPT:
					return &ts->sculpt->paint;
				case OB_MODE_VERTEX_PAINT:
					return &ts->vpaint->paint;
				case OB_MODE_WEIGHT_PAINT:
					return &ts->wpaint->paint;
				case OB_MODE_TEXTURE_PAINT:
					return &ts->imapaint.paint;
				case OB_MODE_EDIT:
					if (ts->use_uv_sculpt)
						return &ts->uvsculpt->paint;
					else
						return &ts->imapaint.paint;
			}
		}

		/* default to image paint */
		return &ts->imapaint.paint;
	}

	return NULL;
}

Paint *BKE_paint_get_active_from_context(const bContext *C)
{
	Scene *sce = CTX_data_scene(C);
	SpaceImage *sima;

	if (sce) {
		ToolSettings *ts = sce->toolsettings;
		Object *obact = NULL;

		if (sce->basact && sce->basact->object)
			obact = sce->basact->object;

		if ((sima = CTX_wm_space_image(C)) != NULL) {
			if (obact && obact->mode == OB_MODE_EDIT) {
				if (sima->mode == SI_MODE_PAINT)
					return &ts->imapaint.paint;
				else if (ts->use_uv_sculpt)
					return &ts->uvsculpt->paint;
			}
			else {
				return &ts->imapaint.paint;
			}
		}
		else if (obact) {
			switch (obact->mode) {
				case OB_MODE_SCULPT:
					return &ts->sculpt->paint;
				case OB_MODE_VERTEX_PAINT:
					return &ts->vpaint->paint;
				case OB_MODE_WEIGHT_PAINT:
					return &ts->wpaint->paint;
				case OB_MODE_TEXTURE_PAINT:
					return &ts->imapaint.paint;
				case OB_MODE_EDIT:
					if (ts->use_uv_sculpt)
						return &ts->uvsculpt->paint;
					else
						return &ts->imapaint.paint;
			}
		}
		else {
			/* default to image paint */
			return &ts->imapaint.paint;
		}
	}

	return NULL;
}

PaintMode BKE_paintmode_get_active_from_context(const bContext *C)
{
	Scene *sce = CTX_data_scene(C);
	SpaceImage *sima;

	if (sce) {
		ToolSettings *ts = sce->toolsettings;
		Object *obact = NULL;

		if (sce->basact && sce->basact->object)
			obact = sce->basact->object;

		if ((sima = CTX_wm_space_image(C)) != NULL) {
			if (obact && obact->mode == OB_MODE_EDIT) {
				if (sima->mode == SI_MODE_PAINT)
					return PAINT_TEXTURE_2D;
				else if (ts->use_uv_sculpt)
					return PAINT_SCULPT_UV;
			}
			else {
				return PAINT_TEXTURE_2D;
			}
		}
		else if (obact) {
			switch (obact->mode) {
				case OB_MODE_SCULPT:
					return PAINT_SCULPT;
				case OB_MODE_VERTEX_PAINT:
					return PAINT_VERTEX;
				case OB_MODE_WEIGHT_PAINT:
					return PAINT_WEIGHT;
				case OB_MODE_TEXTURE_PAINT:
					return PAINT_TEXTURE_PROJECTIVE;
				case OB_MODE_EDIT:
					if (ts->use_uv_sculpt)
						return PAINT_SCULPT_UV;
					else
						return PAINT_TEXTURE_2D;
			}
		}
		else {
			/* default to image paint */
			return PAINT_TEXTURE_2D;
		}
	}

	return PAINT_INVALID;
}

Brush *BKE_paint_brush(Paint *p)
{
	return p ? p->brush : NULL;
}

void BKE_paint_brush_set(Paint *p, Brush *br)
{
	if (p) {
		id_us_min((ID *)p->brush);
		id_us_plus((ID *)br);
		p->brush = br;
	}
}

/* are we in vertex paint or weight pain face select mode? */
int paint_facesel_test(Object *ob)
{
	return ( (ob != NULL) &&
	         (ob->type == OB_MESH) &&
	         (ob->data != NULL) &&
	         (((Mesh *)ob->data)->editflag & ME_EDIT_PAINT_FACE_SEL) &&
	         (ob->mode & (OB_MODE_VERTEX_PAINT | OB_MODE_WEIGHT_PAINT | OB_MODE_TEXTURE_PAINT))
	         );
}

/* are we in weight paint vertex select mode? */
int paint_vertsel_test(Object *ob)
{
	return ( (ob != NULL) &&
	         (ob->type == OB_MESH) &&
	         (ob->data != NULL) &&
	         (((Mesh *)ob->data)->editflag & ME_EDIT_PAINT_VERT_SEL) &&
	         (ob->mode & OB_MODE_WEIGHT_PAINT)
	         );
}

void BKE_paint_init(Paint *p, const char col[3])
{
	Brush *brush;

	/* If there's no brush, create one */
	brush = BKE_paint_brush(p);
	if (brush == NULL)
		brush = BKE_brush_add(G.main, "Brush");
	BKE_paint_brush_set(p, brush);

	memcpy(p->paint_cursor_col, col, 3);
	p->paint_cursor_col[3] = 128;

	p->flags |= PAINT_SHOW_BRUSH;
}

void BKE_paint_free(Paint *paint)
{
	id_us_min((ID *)paint->brush);
}

/* called when copying scene settings, so even if 'src' and 'tar' are the same
 * still do a id_us_plus(), rather then if we were copying between 2 existing
 * scenes where a matching value should decrease the existing user count as
 * with paint_brush_set() */
void BKE_paint_copy(Paint *src, Paint *tar)
{
	tar->brush = src->brush;
	id_us_plus((ID *)tar->brush);
}

/* returns non-zero if any of the face's vertices
 * are hidden, zero otherwise */
int paint_is_face_hidden(const MFace *f, const MVert *mvert)
{
	return ((mvert[f->v1].flag & ME_HIDE) ||
	        (mvert[f->v2].flag & ME_HIDE) ||
	        (mvert[f->v3].flag & ME_HIDE) ||
	        (f->v4 && (mvert[f->v4].flag & ME_HIDE)));
}

/* returns non-zero if any of the corners of the grid
 * face whose inner corner is at (x, y) are hidden,
 * zero otherwise */
int paint_is_grid_face_hidden(const unsigned int *grid_hidden,
                              int gridsize, int x, int y)
{
	/* skip face if any of its corners are hidden */
	return (BLI_BITMAP_GET(grid_hidden, y * gridsize + x) ||
	        BLI_BITMAP_GET(grid_hidden, y * gridsize + x + 1) ||
	        BLI_BITMAP_GET(grid_hidden, (y + 1) * gridsize + x + 1) ||
	        BLI_BITMAP_GET(grid_hidden, (y + 1) * gridsize + x));
}

/* Return TRUE if all vertices in the face are visible, FALSE otherwise */
int paint_is_bmesh_face_hidden(BMFace *f)
{
	BMLoop *l_iter;
	BMLoop *l_first;

	l_iter = l_first = BM_FACE_FIRST_LOOP(f);
	do {
		if (BM_elem_flag_test(l_iter->v, BM_ELEM_HIDDEN)) {
			return true;
		}
	} while ((l_iter = l_iter->next) != l_first);

	return false;
}

float paint_grid_paint_mask(const GridPaintMask *gpm, unsigned level,
                            unsigned x, unsigned y)
{
	int factor = ccg_factor(level, gpm->level);
	int gridsize = ccg_gridsize(gpm->level);
	
	return gpm->data[(y * factor) * gridsize + (x * factor)];
}

/* threshhold to move before updating the brush rotation */
#define RAKE_THRESHHOLD 20

void paint_calculate_rake_rotation(UnifiedPaintSettings *ups, const float mouse_pos[2])
{
	const float u = 0.5f;
	const float r = RAKE_THRESHHOLD;

	float dpos[2];
	sub_v2_v2v2(dpos, ups->last_rake, mouse_pos);

	if (len_squared_v2(dpos) >= r * r) {
		ups->brush_rotation = atan2(dpos[0], dpos[1]);

		interp_v2_v2v2(ups->last_rake, ups->last_rake,
		               mouse_pos, u);
	}
}
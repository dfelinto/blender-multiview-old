
/*  world.c        MIX MODEL
 * 
 *  april 95
 * 
 * $Id$
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * The contents of this file may be used under the terms of either the GNU
 * General Public License Version 2 or later (the "GPL", see
 * http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
 * later (the "BL", see http://www.blender.org/BL/ ) which has to be
 * bought from the Blender Foundation to become active, in which case the
 * above mentioned GPL option does not apply.
 *
 * The Original Code is Copyright (C) 2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include <string.h>
#include <math.h>
#include "MEM_guardedalloc.h"

#include "DNA_world_types.h"
#include "DNA_texture_types.h"
#include "DNA_scriptlink_types.h"
#include "DNA_scene_types.h"
#include "DNA_object_types.h"
#include "DNA_camera_types.h"


#include "BLI_blenlib.h"
#include "BLI_arithb.h"

#include "BKE_bad_level_calls.h"
#include "BKE_utildefines.h"

#include "BKE_library.h"
#include "BKE_world.h"
#include "BKE_global.h"
#include "BPY_extern.h"
#include "BKE_main.h"


void free_world(World *wrld)
{
	MTex *mtex;
	int a;
	
	BPY_free_scriptlink(&wrld->scriptlink);
	
	for(a=0; a<8; a++) {
		mtex= wrld->mtex[a];
		if(mtex && mtex->tex) mtex->tex->id.us--;
		if(mtex) MEM_freeN(mtex);
	}
	wrld->ipo= 0;
}


World *add_world(char *name)
{
	World *wrld;

	wrld= alloc_libblock(&G.main->world, ID_WO, name);
	
	wrld->horb= 0.6f;
	wrld->skytype= WO_SKYBLEND;
	wrld->exposure= 1.0f;
	wrld->stardist= 15.0f;
	wrld->starsize= 2.0f;
	wrld->gravity= 9.8f;
		
	return wrld;
}

World *copy_world(World *wrld)
{
	World *wrldn;
	int a;
	
	wrldn= copy_libblock(wrld);
	
	for(a=0; a<8; a++) {
		if(wrld->mtex[a]) {
			wrldn->mtex[a]= MEM_mallocN(sizeof(MTex), "copymaterial");
			memcpy(wrldn->mtex[a], wrld->mtex[a], sizeof(MTex));
			id_us_plus((ID *)wrldn->mtex[a]->tex);
		}
	}
	
	BPY_copy_scriptlink(&wrld->scriptlink);

	id_us_plus((ID *)wrldn->ipo);
	
	return wrldn;
}

void make_local_world(World *wrld)
{
	Scene *sce;
	World *wrldn;
	int local=0, lib=0;
	
	/* - zijn er alleen lib users: niet doen
	 * - zijn er alleen locale users: flag zetten
	 * - mixed: copy
	 */
	
	if(wrld->id.lib==0) return;
	if(wrld->id.us==1) {
		wrld->id.lib= 0;
		wrld->id.flag= LIB_LOCAL;
		new_id(0, (ID *)wrld, 0);
		return;
	}
	
	sce= G.main->scene.first;
	while(sce) {
		if(sce->world==wrld) {
			if(sce->id.lib) lib= 1;
			else local= 1;
		}
		sce= sce->id.next;
	}
	
	if(local && lib==0) {
		wrld->id.lib= 0;
		wrld->id.flag= LIB_LOCAL;
		new_id(0, (ID *)wrld, 0);
	}
	else if(local && lib) {
		wrldn= copy_world(wrld);
		wrldn->id.us= 0;
		
		sce= G.main->scene.first;
		while(sce) {
			if(sce->world==wrld) {
				if(sce->id.lib==0) {
					sce->world= wrldn;
					wrldn->id.us++;
					wrld->id.us--;
				}
			}
			sce= sce->id.next;
		}
	}
}


void init_render_world()
{
	int a;
	char *cp;
	
	if(G.scene->world) {
		R.wrld= *(G.scene->world);
		
		cp= (char *)&R.wrld.fastcol;
		
		cp[0]= 255.0*R.wrld.horr;
		cp[1]= 255.0*R.wrld.horg;
		cp[2]= 255.0*R.wrld.horb;
		cp[3]= 1;
		
		VECCOPY(R.grvec, R.viewmat[2]);
		Normalise(R.grvec);
		Mat3CpyMat4(R.imat, R.viewinv);
		
		for(a=0; a<6; a++) if(R.wrld.mtex[a] && R.wrld.mtex[a]->tex) R.wrld.skytype |= WO_SKYTEX;
		
		if(G.scene->camera && G.scene->camera->type==OB_CAMERA) {
			Camera *cam= G.scene->camera->data;
			if(cam->type==CAM_ORTHO) {
				/* dit is maar ongeveer */
				R.wrld.miststa+= (float)fabs(R.viewmat[3][2]);
			}
		}
	}
	else {
		memset(&R.wrld, 0, sizeof(World));
		R.wrld.exposure= 1.0;
	}
}

/**
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
 * drawing graphics and editing
 */
	
#include <math.h>

#include "BLI_blenlib.h"
#include "BLI_arithb.h"
#include "BLI_editVert.h"

#include "DNA_scene_types.h"
#include "DNA_space_types.h"
#include "DNA_screen_types.h"
#include "DNA_userdef_types.h"
#include "DNA_view3d_types.h"

#include "BKE_global.h"
#include "BKE_main.h"
#include "BKE_scene.h"

#include "BDR_editobject.h"

#include "BIF_space.h"
#include "BIF_drawscene.h"

#include "BSE_view.h"

#include "blendef.h" /* DIE ! */
#include "mydevice.h"


void set_scene(Scene *sce)		/* zie ook scene.c: set_scene_bg() */
{
	bScreen *sc;
	
	G.scene= sce;

	sc= G.main->screen.first;
	while(sc) {
		if((U.flag & SCENEGLOBAL) || sc==G.curscreen) {
		
			if(sce != sc->scene) {
				/* alle area's endlocalview */
				ScrArea *sa= sc->areabase.first;
				while(sa) {
					endlocalview(sa);
					sa= sa->next;
				}		
				sc->scene= sce;
			}
			
		}
		sc= sc->id.next;
	}
	
	copy_view3d_lock(0);	/* space.c */

	/* zijn er camera's in de views die niet in de scene zitten? */
	sc= G.main->screen.first;
	while(sc) {
		if( (U.flag & SCENEGLOBAL) || sc==G.curscreen) {
			ScrArea *sa= sc->areabase.first;
			while(sa) {
				SpaceLink *sl= sa->spacedata.first;
				while(sl) {
					if(sl->spacetype==SPACE_VIEW3D) {
						View3D *v3d= (View3D*) sl;
						if (!v3d->camera || !object_in_scene(v3d->camera, sce)) {
							v3d->camera= scene_find_camera(sc->scene);
							if (sc==G.curscreen) handle_view3d_lock();
							if (!v3d->camera && v3d->persp>1) v3d->persp= 1;
						}
					}
					sl= sl->next;
				}
				sa= sa->next;
			}
		}
		sc= sc->id.next;
	}

	set_scene_bg(G.scene);	
	
	/* volledige redraw */
	allqueue(REDRAWALL, 0);
	allqueue(REDRAWDATASELECT, 0);	/* doet remake */
}



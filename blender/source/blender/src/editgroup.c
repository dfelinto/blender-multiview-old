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
 */

#include <string.h>

#include "MEM_guardedalloc.h"

#include "BLI_blenlib.h"
#include "BLI_arithb.h"
#include "BLI_editVert.h"

#include "DNA_group_types.h"
#include "DNA_object_types.h"
#include "DNA_scene_types.h"
#include "DNA_view3d_types.h"

#include "BKE_group.h"
#include "BKE_global.h"
#include "BKE_main.h"

#include "BIF_space.h"
#include "BIF_interface.h"
#include "BIF_toolbox.h"
#include "BIF_editgroup.h"

#include "interface.h"
#include "blendef.h"
#include "mydevice.h"

void set_active_group(void)
{
	/* with active object, find active group */
	Group *group;
	GroupObject *go;
	
	G.scene->group= NULL;
	
	if(BASACT) {
		group= G.main->group.first;
		while(group) {
			go= group->gobject.first;
			while(go) {
				if(go->ob == OBACT) {
					G.scene->group= group;
					return;
				}
				go= go->next;
			}
			group= group->id.next;
		}
	}
}


void add_selected_to_group(void)
{
	Base *base= FIRSTBASE;
	Group *group;
	
	if(BASACT==NULL) {
		error("No active object");
		return;
	}
	
	if(okee("Add selected to group")==0) return;
	
	if(G.scene->group==NULL) G.scene->group= add_group();
	
	while(base) {
		if TESTBASE(base) {
			
			/* each object only in one group */
			group= find_group(base->object);
			if(group==G.scene->group);
			else {
				if(group) {
					rem_from_group(group, base->object);
				}
				add_to_group(G.scene->group, base->object);
				base->object->flag |= OB_FROMGROUP;
				base->flag |= OB_FROMGROUP;
			}
		}
		base= base->next;
	}
	
	allqueue(REDRAWVIEW3D, 0);
	allqueue(REDRAWBUTSANIM, 0);
}

void rem_selected_from_group(void)
{
	Base *base=FIRSTBASE;
	Group *group;
	
	if(okee("Remove selected from group")==0) return;

	while(base) {
		if TESTBASE(base) {

			group= find_group(base->object);
			if(group) {
				rem_from_group(group, base->object);
			
				base->object->flag &= ~OB_FROMGROUP;
				base->flag &= ~OB_FROMGROUP;
			}
		}
		base= base->next;
	}
	
	allqueue(REDRAWVIEW3D, 0);
	allqueue(REDRAWBUTSANIM, 0);
}

void group_menu(void)
{
	Base *base;
	GroupObject *go;
	GroupKey *gk;
	short nr, len;
	char *str, tstr[40];
	
	if(G.scene->group==NULL) return;
	
	/* make menu string */
	len= 60;
	gk= G.scene->group->gkey.first;
	while(gk) {
		len+= 36;
		gk= gk->next;
	}
	
	str= MEM_mallocN(len, "groupmenu");
	strcpy(str, "Group options%t|Select members %x1");
	
	if(G.scene->group->active) 
		strcat(str, "|Overwrite active key %x2|%l");
	
	nr= 3;
	gk= G.scene->group->gkey.first;
	while(gk) {
		sprintf(tstr, "|Load: %s %%x%d", gk->name, nr++);
		strcat(str, tstr);
		gk= gk->next;
	}

	/* here we go */
	
	nr= pupmenu(str);
	MEM_freeN(str);

	if(nr==1) {
		go= G.scene->group->gobject.first;
		while(go) {
			go->ob->flag |= SELECT;
			go= go->next;
		}
		
		/* nasty thing... that should be solved someday */
		base= FIRSTBASE;
		while(base) {
			base->flag= base->object->flag;
			base= base->next;
		}

	}
	else if(nr==2) {
		go= G.scene->group->gobject.first;
		while(go) {
			add_object_key(go, G.scene->group->active);
			go= go->next;
		}
		
	}
	else if(nr>2) {
		nr-= 2;
		gk= G.scene->group->gkey.first;
		while(gk) {
			nr--;
			if(nr==0) break;
			gk= gk->next;
		}
		
		G.scene->group->active= gk;
		set_group_key(G.scene->group);
		
	}

	allqueue(REDRAWVIEW3D, 0);
	allqueue(REDRAWBUTSANIM, 0);
	allspace(REMAKEIPO, 0);
	allqueue(REDRAWIPO, 0);
}


void prev_group_key(Group *group)
{
	GroupKey *gk= group->active;
	
	if(gk) gk= gk->prev;
	
	if(gk==NULL) group->active= group->gkey.last;
	else group->active= gk;
	
	set_group_key(group);
}

void next_group_key(Group *group)
{
	GroupKey *gk= group->active;
	
	if(gk) gk= gk->next;
	
	if(gk==NULL) group->active= group->gkey.first;
	else group->active= gk;
	
	set_group_key(group);
	
}



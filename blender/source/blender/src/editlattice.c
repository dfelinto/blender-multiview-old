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
 * i.t.t. wat de naam doet vermoeden: ook algemene lattice (calc) functies
 */

#include <stdlib.h>
#include <math.h>
#ifdef WIN32
#include "BLI_winstuff.h"
#endif
#include "MEM_guardedalloc.h"

#include "BLI_blenlib.h"
#include "BLI_arithb.h"
#include "BLI_editVert.h"

#include "DNA_object_types.h"
#include "DNA_scene_types.h"
#include "DNA_lattice_types.h"
#include "DNA_curve_types.h"
#include "DNA_key_types.h"
#include "DNA_view3d_types.h"

#include "BKE_key.h"
#include "BKE_displist.h"
#include "BKE_lattice.h"
#include "BKE_global.h"

#include "BIF_toolbox.h"
#include "BIF_space.h"
#include "BIF_screen.h"
#include "BIF_editlattice.h"
#include "BIF_editkey.h"

#include "BSE_edit.h"

#include "BDR_editobject.h"
#include "BDR_drawobject.h"

#include "blendef.h"
#include "mydevice.h"

#include "render.h"

#include "BKE_armature.h"

void apply_lattice(void)
{
	Base *base;
	Object *par;
	
	if(okee("Apply Lattice Deform")==0) return;
	
	base= FIRSTBASE;
	while(base) {
		if TESTBASELIB(base) {
			if( (par= base->object->parent) ) {
				if(par->type==OB_LATTICE) {
					
					lt_applyflag= 1;
					object_deform(base->object);
					lt_applyflag= 0;
					
					base->object->parent= 0;
				}
			}
		}
		base= base->next;
	}
	
	allqueue(REDRAWVIEW3D, 0);
}

/* ***************************** */




void free_editLatt(void)
{
	if(editLatt) {
		if(editLatt->def) MEM_freeN(editLatt->def);
		MEM_freeN(editLatt);
		editLatt= 0;
	}
}


static void setflagsLatt(int flag)
{
	BPoint *bp;
	int a;
	
	bp= editLatt->def;
	
	a= editLatt->pntsu*editLatt->pntsv*editLatt->pntsw;
	
	while(a--) {
		if(bp->hide==0) {
			bp->f1= flag;
		}
		bp++;
	}
}



void make_editLatt(void)
{
	Lattice *lt;
	KeyBlock *actkey=0;
	
	free_editLatt();
	
	lt= G.obedit->data;

	/* keys? */
	if(lt->key) {
		actkey= lt->key->block.first;
		while(actkey) {
			if(actkey->flag & SELECT) break;
			actkey= actkey->next;
		}
	}

	if(actkey) {
		key_to_latt(actkey, lt);
	}

	editLatt= MEM_dupallocN(lt);
	
	editLatt->def= MEM_dupallocN(lt->def);
	
	setflagsLatt(0);
}


void load_editLatt(void)
{
	Lattice *lt;
	KeyBlock *actkey=0;
	BPoint *bp;
	float *fp;
	int tot;
	
	lt= G.obedit->data;
	
	/* zijn er keys? */
	if(lt->key) {
		actkey= lt->key->block.first;
		while(actkey) {
			if(actkey->flag & SELECT) break;
			actkey= actkey->next;
		}
	}

	if(actkey) {
		/* aktieve key: vertices */
		tot= editLatt->pntsu*editLatt->pntsv*editLatt->pntsw;
		
		if(actkey->data) MEM_freeN(actkey->data);
		
		fp=actkey->data= MEM_callocN(lt->key->elemsize*tot, "actkey->data");
		actkey->totelem= tot;
	
		bp= editLatt->def;
		while(tot--) {
			VECCOPY(fp, bp->vec);
			fp+= 3;
			bp++;
		}
		
		if(actkey) do_spec_key(lt->key);
	}
	else {

		MEM_freeN(lt->def);
	
		lt->def= MEM_dupallocN(editLatt->def);

		lt->flag= editLatt->flag;

		lt->pntsu= editLatt->pntsu;
		lt->pntsv= editLatt->pntsv;
		lt->pntsw= editLatt->pntsw;
		
		lt->typeu= editLatt->typeu;
		lt->typev= editLatt->typev;
		lt->typew= editLatt->typew;
	}
}

void remake_editLatt(void)
{
	if(okee("Reload Original data")==0) return;
	
	make_editLatt();
	allqueue(REDRAWVIEW3D, 0);
	allqueue(REDRAWBUTSEDIT, 0);
}


void deselectall_Latt(void)
{
	BPoint *bp;
	int a;
	
	bp= editLatt->def;
	
	a= editLatt->pntsu*editLatt->pntsv*editLatt->pntsw;
	
	allqueue(REDRAWVIEW3D, 0);
	
	while(a--) {
		if(bp->hide==0) {
			if(bp->f1) {
				setflagsLatt(0);
				return;
			}
		}
		bp++;
	}
	setflagsLatt(1);
}


static BPoint *findnearestLattvert(int sel)
{
	/* sel==1: selected krijgen een nadeel */
	/* in bp wordt nearest weggeschreven */
	BPoint *bp1, *bp;
	short dist= 100, temp, mval[2], a;

	bp= 0;

	/* projektie doen */
	calc_lattverts_ext();	/* drawobject.c */
	
	getmouseco_areawin(mval);

			
	bp1= editLatt->def;
	
	a= editLatt->pntsu*editLatt->pntsv*editLatt->pntsw;
	
	while(a--) {
		if(bp1->hide==0) {
			temp= abs(mval[0]- bp1->s[0])+ abs(mval[1]- bp1->s[1]);
			if( (bp1->f1 & 1)==sel) temp+=5;
			if(temp<dist) { 
				bp= bp1; 
				dist= temp; 
			}
		}
		bp1++;
	}

	return bp;
}


void mouse_lattice(void)
{
	BPoint *bp=0;

	bp= findnearestLattvert(1);

	if(bp) {
		if((G.qual & LR_SHIFTKEY)==0) {
		
			setflagsLatt(0);
			bp->f1 |= 1;

			allqueue(REDRAWVIEW3D, 0);
		}
		else {
			
			if(bp->f1 & 1) bp->f1 &= ~1;
			else bp->f1 |= 1;

			allqueue(REDRAWVIEW3D, 0);

		}

		countall();
	}

	rightmouse_transform();
	
}

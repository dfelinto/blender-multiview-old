/**
 * blenlib/DNA_ika_types.h (mar-2001 nzc)
 *
 * Old ika types. These will be superceded by Reevan's stuff, soon (I
 * hope).
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
#ifndef DNA_IKA_TYPES_H
#define DNA_IKA_TYPES_H

#include "DNA_listBase.h"
#include "DNA_ID.h"

struct Object;
struct Ipo;

typedef struct Deform {
	struct Object *ob;
	short flag, partype;
	int par1, par2, par3;	/* kunnen vertexnrs zijn */
	float imat[4][4], premat[4][4], postmat[4][4];
	float vec[3];	/* als partype==LIMB, voor distfunc */
	float fac, dist, pad;
	
} Deform;

typedef struct Limb {
	struct Limb *next, *prev;
	
	float len, leno, fac, alpha, alphao, pad;
	float eff[2];
	
} Limb;

typedef struct Ika {
	ID id;
	
	short partype, flag, iter, lastfra;
	
	ListBase limbbase;
	float eff[3], effg[3], effn[3];	/* current, global en (local)wanted */
	float mem, slow, toty, totx, xyconstraint;
	
	struct Ipo *ipo;
	struct Object *parent;
	int par1, par2, par3;	/* kunnen vertexnrs zijn */

	int totdef;
	Deform *def;
	
	int def_scroll;
	int limb_scroll;
} Ika;

/* these defines are used for working with ikas*/

/* ika.flag: */
#define IK_GRABEFF		1
#define IK_XYCONSTRAINT	2


#endif

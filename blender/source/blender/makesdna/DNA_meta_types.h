/**
 * blenlib/DNA_meta_types.h (mar-2001 nzc)
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
#ifndef DNA_META_TYPES_H
#define DNA_META_TYPES_H

#include "DNA_listBase.h"
#include "DNA_ID.h"

struct BoundBox;
struct Ipo;
struct Material;


typedef struct MetaElem {
	struct MetaElem *next, *prev;
	
	short type, lay, flag, selcol;
	float x, y, z;
	float expx, expy, expz;
	float rad, rad2, s, len, maxrad2;
	int pad;
	
	float *mat, *imat;
	
} MetaElem;

typedef struct MetaBall {
	ID id;
	
	struct BoundBox *bb;

	ListBase elems;
	ListBase disp;
	struct Ipo *ipo;

	struct Material **mat;

	short flag, totcol;
	int texflag;
	float loc[3];
	float size[3];
	float rot[3];
	float wiresize, rendersize, thresh;
	
} MetaBall;

/* **************** METABALL ********************* */

#define MB_MAXELEM		1024

/* mb->flag */
#define MB_UPDATE_ALWAYS	0
#define MB_UPDATE_HALFRES	1
#define MB_UPDATE_FAST		2

/* ml->type */
#define MB_BALL		0
#define MB_TUBEX	1
#define MB_TUBEY	2
#define MB_TUBEZ	3
#define MB_CIRCLE	4

/* ml->flag */
#define MB_NEGATIVE	2




#endif

/**
 * blenlib/DNA_group_types.h (mar-2001 nzc)
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
#ifndef DNA_GROUP_TYPES_H
#define DNA_GROUP_TYPES_H

#include "DNA_listBase.h"
#include "DNA_ID.h"

struct Object;
struct Ipo;

typedef struct GroupKey {
	struct GroupKey *next, *prev;
	short sfra, efra;
	float cfra;
	char name[32];
} GroupKey;

typedef struct ObjectKey {
	struct ObjectKey *next, *prev;
	GroupKey *gkey;		/* for reference */

	/* copy of relevant data */
	short partype, pad;
	int par1, par2, par3;
	
	struct Object *parent, *track;
	struct Ipo *ipo;

	/* this block identical to object */	
	float loc[3], dloc[3], orig[3];
	float size[3], dsize[3];
	float rot[3], drot[3];
	float quat[4], dquat[4];
	float obmat[4][4];
	float parentinv[4][4];
	float imat[4][4];	/* voor bij render, tijdens simulate, tijdelijk: ipokeys van transform  */
	
	unsigned int lay;				/* kopie van Base */
	
	char transflag, ipoflag;
	char trackflag, upflag;
	
	float sf, ctime, padf;
		

} ObjectKey;

typedef struct GroupObject {
	struct GroupObject *next, *prev;
	struct Object *ob;
	ListBase okey;		/* ObjectKey */
	
} GroupObject;


typedef struct Group {
	ID id;
	
	ListBase gobject;	/* GroupObject */
	ListBase gkey;		/* GroupKey */
	
	GroupKey *active;
	
} Group;



#endif

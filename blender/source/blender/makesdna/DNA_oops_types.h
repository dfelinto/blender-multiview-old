/**
 * blenlib/DNA_oops_types.h (mar-2001 nzc)
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
#ifndef DNA_OOPS_TYPES_H
#define DNA_OOPS_TYPES_H

#define OOPSX	5.0
#define OOPSY	1.8

#include "DNA_listBase.h"

struct ID;

typedef struct Oops {
	struct Oops *next, *prev;
	short type, flag, dt, hide;
	float x, y;		/* linksonder */
	float dx, dy;	/* shuffle */
	struct ID *id;
	ListBase link;
} Oops;

#
#
typedef struct OopsLink {
	struct OopsLink *next, *prev;
	short type, flag;
	ID **idfrom;
	Oops *to, *from;	/* from is voor temp */
	float xof, yof;
	char name[12];
} OopsLink;

/* oops->flag  (1==SELECT) */
#define OOPS_DOSELECT	2
#define OOPS_REFER		4

#endif

/**
 * blenlib/DNA_key_types.h (mar-2001 nzc)
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
#ifndef DNA_KEY_TYPES_H
#define DNA_KEY_TYPES_H

#include "DNA_listBase.h"
#include "DNA_ID.h"

struct Ipo;

typedef struct KeyBlock {
	struct KeyBlock *next, *prev;
	
	float pos;
	short flag, totelem;
	short type, rt;
	int pad;
	
	void *data;
	
} KeyBlock;


typedef struct Key {
	ID id;
	
	KeyBlock *refkey;
	char elemstr[32];
	int elemsize;
	float curval;
	
	ListBase block;
	struct Ipo *ipo;
	
	ID *from;

	short type, totkey;
	short slurph, actkey;
	
} Key;

/* **************** KEY ********************* */

/* type */
#define KEY_NORMAL      0
#define KEY_RELATIVE    1

/* keyblock->type */
#define KEY_LINEAR      0
#define KEY_CARDINAL    1
#define KEY_BSPLINE     2

#endif

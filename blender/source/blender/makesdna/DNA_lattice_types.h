/**
 * blenlib/DNA_mesh_types.h (mar-2001 nzc)
 *
 * Mesh stuff.
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
#ifndef DNA_LATTICE_TYPES_H
#define DNA_LATTICE_TYPES_H

#include "DNA_ID.h"

struct BPoint;
struct Ipo;
struct Key;

typedef struct Lattice {
	ID id;
	
	short pntsu, pntsv, pntsw, flag;
	char typeu, typev, typew, type;
	int pad;
	
	struct BPoint *def;
	
	struct Ipo *ipo;
	struct Key *key;
	
} Lattice;

/* ***************** LATTICE ********************* */

/* flag */
#define LT_GRID		1
#define LT_OUTSIDE	2

#endif

/**
 * blenlib/DNA_property_types.h (mar-2001 nzc)
 *
 * Renderrecipe and scene decription. The fact that there is a
 * hierarchy here is a bit strange, and not desirable.
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
#ifndef DNA_PROPERTY_TYPES_H
#define DNA_PROPERTY_TYPES_H

/* ********************* PROPERTY ************************ */

typedef struct bProperty {
	struct bProperty *next, *prev;
	char name[32];
	short type, otype;		/* otype is for buttons, when a property type changes */
	int data;				/* data should be 4 bytes to store int,float stuff */
	int old;				/* old is for simul */
	short flag, pad;
	void *poin;
	void *oldpoin;			/* oldpoin is for simul */
	
} bProperty;

/* property->type */
#define PROP_BOOL		0
#define PROP_INT		1
#define PROP_FLOAT		2
#define PROP_STRING		3
#define PROP_VECTOR		4
#define PROP_TIME		5

/* property->flag */
#define PROP_DEBUG		1

#define MAX_PROPSTRING	128

#endif

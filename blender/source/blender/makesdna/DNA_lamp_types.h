/**
 * blenlib/DNA_lamp_types.h (mar-2001 nzc)
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
#ifndef DNA_LAMP_TYPES_H
#define DNA_LAMP_TYPES_H

#include "DNA_ID.h"
#include "DNA_scriptlink_types.h"

struct MTex;
struct Ipo;

typedef struct Lamp {
	ID id;
	
	short type, mode;
	
	short colormodel, totex;
	float r, g, b, k;
	
	float energy, dist, spotsize, spotblend;
	float haint;
	float att1, att2;
	
	short bufsize, samp;
	float clipsta, clipend, shadspotsize;
	float bias, soft;
	
	/* texact is voor buttons */
	short texact, shadhalostep;
	
	struct MTex *mtex[8];
	struct Ipo *ipo;
	
	ScriptLink scriptlink;
} Lamp;

/* **************** LAMP ********************* */

/* type */
#define LA_LOCAL		0
#define LA_SUN			1
#define LA_SPOT			2
#define LA_HEMI			3

/* mode */
#define LA_SHAD			1
#define LA_HALO			2
#define LA_LAYER		4
#define LA_QUAD			8
#define LA_NEG			16
#define LA_ONLYSHADOW	32
#define LA_SPHERE		64
#define LA_SQUARE		128
#define LA_TEXTURE		256
#define LA_OSATEX		512
/* use bit 11 for shadow tests... temp only -nzc- */
#define LA_DEEP_SHADOW  1024

/* mapto */
#define LAMAP_COL		1

/* bit isolated... */
#define MAXLAMP		256

#endif

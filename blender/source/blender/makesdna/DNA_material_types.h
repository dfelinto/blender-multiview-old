/**
 * blenlib/DNA_material_types.h (mar-2001 nzc)
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
#ifndef DNA_MATERIAL_TYPES_H
#define DNA_MATERIAL_TYPES_H

/*  #include "BLI_listBase.h" */

#include "DNA_ID.h"
#include "DNA_scriptlink_types.h"

struct MTex;
struct Ipo;
struct Material;

/* LET OP: type veranderen? ook in ipo.h doen */

typedef struct Material {
	ID id;
	
	short colormodel, lay;		/* lay: voor dynamics */
	float r, g, b;
	float specr, specg, specb;
	float mirr, mirg, mirb;
	float ambr, ambb, ambg;
	
	float amb, emit, ang, spectra;
	float alpha, ref, spec, zoffs, add;
	float kfac; /* for transparent solids */
	short har;
	char seed1, seed2;
	
	int mode; 
	int mode2; /* even more material settings :) */
	short flarec, starc, linec, ringc;
	float hasize, flaresize, subsize, flareboost;
	
	/* onderstaand is voor buttons en render*/
	char rgbsel, texact, pr_type, septex;
	short pr_back, pr_lamp;

	int pad1;
	short texco, mapto;
	
	struct MTex *mtex[8];
	struct Ipo *ipo;
	struct Material *ren;
	
	/* dynamic properties */
	float friction, fh, reflect;
	float fhdist, xyfrict;
	short dynamode, pad;
	
	ScriptLink scriptlink;
} Material;

/* **************** MATERIAL ********************* */

	/* maximum number of materials per material array
	 * (on object, mesh, lamp, etc.)
	 */
#define MAXMAT			16

/* colormodel */
#define MA_RGB			0
#define MA_CMYK			1
#define MA_YUV			2
#define MA_HSV			3

/* mode (is int) */
#define MA_TRACEBLE		1
#define MA_SHADOW		2
#define MA_SHLESS		4
#define MA_WIRE			8
#define MA_VERTEXCOL	16
#define MA_HALO			32
#define MA_ZTRA			64
#define MA_VERTEXCOLP	128
#define MA_ZINV			256
#define MA_HALO_RINGS	256
#define MA_ENV			512
#define MA_HALO_LINES	512
#define MA_ONLYSHADOW	1024
#define MA_HALO_XALPHA	1024
#define MA_STAR			0x800
#define MA_FACETEXTURE	0x800
#define MA_HALOTEX		0x1000
#define MA_HALOPUNO		0x2000
#define MA_NOMIST		0x4000
#define MA_HALO_SHADE	0x4000
#define MA_HALO_FLARE	0x8000

/* dynamode */
#define MA_DRAW_DYNABUTS    1
#define MA_FH_NOR	        2

/* texco */
#define TEXCO_ORCO		1
#define TEXCO_REFL		2
#define TEXCO_NORM		4
#define TEXCO_GLOB		8
#define TEXCO_UV		16
#define TEXCO_OBJECT	32
#define TEXCO_LAVECTOR	64
#define TEXCO_VIEW		128
#define TEXCO_STICKY	256
#define TEXCO_OSA		512
#define TEXCO_WINDOW	1024
#define NEED_UV			2048

/* mapto */
#define MAP_COL			1
#define MAP_NORM		2
#define MAP_COLSPEC		4
#define MAP_COLMIR		8
#define MAP_VARS		(0xFFF0)
#define MAP_REF			16
#define MAP_SPEC		32
#define MAP_EMIT		64
#define MAP_ALPHA		128
#define MAP_HAR			256
#define MAP_XTRA		512

/* pr_type */
#define MA_FLAT			0
#define MA_SPHERE		1
#define MA_CUBE			2

/* pr_back */
#define MA_DARK			1

#endif

/**
 * blenlib/DNA_curve_types.h (mar-2001 nzc)
 *
 * Curve stuff.
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
#ifndef DNA_CURVE_TYPES_H
#define DNA_CURVE_TYPES_H

#include "DNA_listBase.h"
#include "DNA_vec_types.h"
#include "DNA_ID.h"

struct BoundBox;
struct Object;
struct Ipo;
struct Key;
struct Material;
struct VFont;

#
#
typedef struct Path {
	int len;
	float *data;
	float totdist;
} Path;

#
#
typedef struct BevList {
	struct BevList *next, *prev;
	short nr, flag;
	short poly, gat;
} BevList;

#
#
typedef struct BevPoint {
	float x, y, z, alfa, sina, cosa, mat[3][3];
	short f1, f2;
} BevPoint;


typedef struct BezTriple {
	float vec[3][3];
	float alfa;
	short s[3][2];
	short h1, h2;
	char f1, f2, f3, hide;
} BezTriple;

typedef struct BPoint {
	float vec[4];
	float alfa;
	short s[2];
	short f1, hide;
} BPoint;

typedef struct Nurb {
	struct Nurb *next, *prev;
	short type, mat_nr;
	short hide, flag;
	short pntsu, pntsv;
	short resolu, resolv;
	short orderu, orderv;
	short flagu, flagv;
	
	float *knotsu, *knotsv;
	BPoint *bp;
	BezTriple *bezt;
	
} Nurb;


typedef struct Curve {
	ID id;
	
	struct BoundBox *bb;
	
	ListBase nurb;
	ListBase disp;
	struct Object *bevobj, *textoncurve;
	struct Ipo *ipo;
	Path *path;
	struct Key *key;
	struct Material **mat;
	
	ListBase bev;
	float *orco;

	float loc[3];
	float size[3];
	float rot[3];

	int texflag;

	short pathlen, totcol;
	short flag, bevresol;
	float width, ext1, ext2;
	
	/* default */
	short resolu, resolv;
	
	/* font stuk */
	short len, lines, pos, spacemode;
	float spacing, linedist, shear, fsize;
	float xof, yof;
	
	char *str, family[24];
	struct VFont *vfont;

} Curve;

typedef struct IpoCurve {
	struct IpoCurve *next,  *prev;
	
	struct BPoint *bp;
	struct BezTriple *bezt;

	rctf maxrct, totrct;

	short blocktype, adrcode, vartype;
	short totvert;
	short ipo, extrap;
	short flag, rt;
	float ymin, ymax;
	unsigned int bitmask;
	
	float curval;
	
} IpoCurve;

/* **************** CURVE ********************* */

/* flag */
#define CU_3D			1
#define CU_FRONT		2
#define CU_BACK			4
#define CU_PATH			8
#define CU_FOLLOW		16
#define CU_UV_ORCO		32
#define CU_NOPUNOFLIP	64

/* spacemode */
#define CU_LEFT			0
#define CU_MIDDLE		1
#define CU_RIGHT		2
#define CU_FLUSH		3

/* flag (nurb) */
#define CU_SMOOTH		ME_SMOOTH

/* type (nurb) */
#define CU_POLY			0
#define CU_BEZIER		1
#define CU_BSPLINE		2
#define CU_CARDINAL		3
#define CU_NURBS		4
#define CU_2D			8

/* flagu flagv (nurb) */
#define CU_CYCLIC		1

/* h1 h2 (beztriple) */
#define HD_FREE		0
#define HD_AUTO		1
#define HD_VECT		2
#define HD_ALIGN	3



#endif

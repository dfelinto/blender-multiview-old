/**
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

#ifndef BSE_TRANS_TYPES_H
#define BSE_TRANS_TYPES_H

struct Object;
/*  struct EditVlak; */
/*  struct EditEdge; */
/*  struct EditVert; */
/*  struct ListBase; */

typedef struct TransOb {
	float *loc;
	float oldloc[9];
	float *eff;
	float oldeff[3];
	float *rot;
	float oldrot[12];
	float olddrot[3];
	float *quat;
	float oldquat[16];
	float olddquat[4];
	float *size;
	float oldsize[12];
	float olddsize[3];
	float obmat[3][3];
	float obinv[3][3];
	float parmat[3][3];
	float parinv[3][3];
	float obvec[3];
	int flag; /* keys */
	float *locx, *locy, *locz;
	float *rotx, *roty, *rotz;
	float *quatx, *quaty, *quatz, *quatw;
	float *sizex, *sizey, *sizez;
	/* __NLA */
	float axismat[3][3];	/* Restmat of object (for localspace transforms) */
	void *data;	/* Arbitrary data */
	/* end __NLA */
	struct Object *ob;
} TransOb;

typedef struct TransVert {
	float *loc;
	float oldloc[3], fac;
	float *val, oldval;
	int flag;
	float *nor;
} TransVert;

typedef struct VPaint {
	float r, g, b, a;
	float size;		/* van brush */
	float gamma, mul;
	short mode, flag;
} VPaint;

#endif /* BSE_TRANS_TYPES_H */

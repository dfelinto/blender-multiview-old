/**
 * blenlib/DNA_view3d_types.h (mar-2001 nzc)
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
#ifndef DNA_VIEW3D_TYPES_H
#define DNA_VIEW3D_TYPES_H

struct Object;
struct Image;
struct Tex;
struct SpaceLink;

/* This is needed to not let VC choke on near and far... old
 * proprietary MS extensions... */
#ifdef WIN32
#undef near
#undef far
#define near clipsta
#define far clipend
#endif

/* The near/far thing is a Win EXCEPTION. Thus, leave near/far in the
 * code, and patch for windows. */

typedef struct BGpic {
    struct Image *ima;
	struct Tex *tex;
    float xof, yof, size, zoom, blend;
    short xim, yim;
	unsigned int *rect;
} BGpic;

typedef struct View3D {
	struct SpaceLink *next, *prev;
	int spacetype, pad;
	struct ScrArea *area;
	
	float viewmat[4][4];
	float viewinv[4][4];
	float persmat[4][4];
	float persinv[4][4];
	float viewquat[4], dist;

	/**
	 * 0 - ortho
	 * 1 - do 3d perspective
	 * 2 - use the camera
	 */
	short persp;
	short view;

	struct Object *camera;
	struct BGpic *bgpic;
	struct View3D *localvd;
	
	/**
	 * The drawing mode for the 3d display. Set to OB_WIRE, OB_SOLID,
	 * OB_SHADED or OB_TEXTURED */
	short drawtype;
	short localview;
	int lay, layact;
	short scenelock, around, camzoom, flag;
	
	float lens, grid, near, far;
	float ofs[3], cursor[3];
	
	short mx, my;	/* moeten achter elkaar blijven staan ivm als pointer doorgeven */
	short mxo, myo;

	short gridlines, viewbut;
	int	pad2, pad3;
} View3D;

/* View3D->flag */
#define V3D_MODE			(16+32+64+128+256+512)
#define V3D_DISPIMAGE		1
#define V3D_DISPBGPIC		2
#define V3D_SETUPBUTS		4
#define V3D_NEEDBACKBUFDRAW	8
#define V3D_EDITMODE		16
#define V3D_VERTEXPAINT		32
#define V3D_FACESELECT		64
#define V3D_POSEMODE		128
#define V3D_TEXTUREPAINT	256
#define V3D_WEIGHTPAINT		512

/* View3D->around */
#define V3D_CENTRE		0
#define V3D_CENTROID	3
#define V3D_CURSOR		1
#define V3D_LOCAL		2

/* View3d->persp */
#define V3D_PERSP_ORTHO          0
#define V3D_PERSP_DO_3D_PERSP    1
#define V3D_PERSP_USE_THE_CAMERA 2

#endif

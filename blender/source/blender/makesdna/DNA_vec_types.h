/**
 * vec_types.h    dec 2000 Nzc
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
 * 
 */

#ifndef DNA_VEC_TYPES_H
#define DNA_VEC_TYPES_H

/* types */

typedef struct vec2s {
	short x, y;
} vec2s;

typedef struct vec2i {
	int x, y;
} vec2i;

typedef struct vec2f {
	float x, y;
} vec2f;

typedef struct vec2d {
	double x, y;
} vec2d;

typedef struct vec3i {
	int x, y, z;
} vec3i;

typedef struct vec3f {
	float x, y, z;
} vec3f;

typedef struct vec3d {
	double x, y, z;
} vec3d;

typedef struct vec4i {
	int x, y, z, w;
} vec4i;

typedef struct vec4f {
	float x, y, z, w;
} vec4f;

typedef struct vec4d {
	double x, y, z, w;
} vec4d;

typedef struct rcti {
    int xmin, xmax;
    int ymin, ymax;
} rcti;

typedef struct rctf {
    float xmin, xmax;
    float ymin, ymax;
} rctf;

#endif

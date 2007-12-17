/* $Id: vector.h 11161 2007-07-03 19:13:26Z campbellbarton $
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version. The Blender
 * Foundation also sells licenses for use in proprietary software under
 * the Blender License.  See http://www.blender.org/BL/ for information
 * about this.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): Willian P. Germano & Joseph Gilbert
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 *
 */

#ifndef EXPP_color_h
#define EXPP_color_h

#include <Python.h>

extern PyTypeObject BPyColor_Type;

#define BPyColor_Check(v) PyObject_TypeCheck(v, &BPyColor_Type)

typedef struct {
	PyObject_VAR_HEAD
	float color[4];			/* R/G/B/A */
	PyObject *source;		/* the data we come from */
	int index;				/* index for mesh-face, colorband or MTex slot */
	char type;				/* when wrapping blender data, this spesify's the type spec/diffuse/ssscol */
	char size;				/* number of channels, 3 for rgb and 4 is rgba */
	
} BPyColorObject;

enum color_type_consts {
	/* 'implicit' kind of a dumb const that just means the PyType defines
	 * the colortype, for instance textures, and lamps only have 1 color
	 * per data */
	BPY_COLOR_IMPLICIT = 0, 
	BPY_COLOR_MAT_DIFF,
	BPY_COLOR_MAT_SPEC,
	BPY_COLOR_MAT_MIR,
	BPY_COLOR_MAT_SSS,
	/*BPY_COLOR_TEX, - use BPY_COLOR_IMPLICIT*/
	BPY_COLOR_MTEX,
	BPY_COLOR_BAND,			/*	for material diffuse colorband or texture colorband */	
	BPY_COLOR_BAND_SPEC,	/*	material spec colorband */
	/*BPY_COLOR_LAMP - use BPY_COLOR_IMPLICIT, we dont need this because a lamp can only have 1 color */
	BPY_COLOR_WORLD_AMB,
	BPY_COLOR_WORLD_HOR,
	BPY_COLOR_WORLD_ZEN,
	
	/* these should always be last to make checking easy
	 * (... >= BPY_COLOR_MESH_FACE_V1) */
	BPY_COLOR_MESH_FACE_V1, 
	BPY_COLOR_MESH_FACE_V2,
	BPY_COLOR_MESH_FACE_V3,
	BPY_COLOR_MESH_FACE_V4,
};

PyObject *Color_CreatePyObject(float *color, char size, char type, int index, PyObject * source);
int Color_CheckPyObject(BPyColorObject * self);
BPyColorObject * Color_GetUpdated(PyObject * obj);
void Color_Init(void);
PyObject *ColorType_Init(void);
#endif

/* $Id: vector.h 12898 2007-12-15 21:44:40Z campbellbarton $
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

#ifndef V24_EXPP_vector_h
#define V24_EXPP_vector_h

#include <Python.h>

extern PyTypeObject V24_vector_Type;

#define V24_VectorObject_Check(v) ((v)->ob_type == &V24_vector_Type)

typedef struct {
	PyObject_VAR_HEAD 
	float *vec;				/*1D array of data (alias), wrapped status depends on wrapped status */
	short size;				/* vec size 2,3 or 4 */
	short wrapped;			/* is wrapped data? */
} V24_VectorObject;

/*prototypes*/
PyObject *V24_Vector_Zero( V24_VectorObject * self );
PyObject *V24_Vector_Normalize( V24_VectorObject * self );
PyObject *V24_Vector_Negate( V24_VectorObject * self );
PyObject *V24_Vector_Resize2D( V24_VectorObject * self );
PyObject *V24_Vector_Resize3D( V24_VectorObject * self );
PyObject *V24_Vector_Resize4D( V24_VectorObject * self );
PyObject *V24_Vector_toPoint( V24_VectorObject * self );
PyObject *V24_Vector_ToTrackQuat( V24_VectorObject * self, PyObject * args );
PyObject *V24_Vector_copy( V24_VectorObject * self );
PyObject *V24_newVectorObject(float *vec, int size, int type);

#endif				/* EXPP_vector_h */

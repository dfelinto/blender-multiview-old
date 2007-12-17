/* $Id: vector.h 11398 2007-07-28 06:28:33Z campbellbarton $
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

#ifndef EXPP_vector_h
#define EXPP_vector_h

#include <Python.h>

extern PyTypeObject BPyVector_Type;

#define BPyVector_Check(v)	PyObject_TypeCheck(v, &BPyVector_Type)

typedef struct {
	PyObject_VAR_HEAD 
	float *vec;				/* 1D array of data (alias), wrapped status depends on wrapped status */
	PyObject *source;		/* see notes below on genlib */ 
	char size;				/* vec size 2,3 or 4 */
	char sub_index;			/* use for face-UV (which 0,1,2,3?) and matrix row */
} BPyVectorObject;

/* VectorObject->genlib
 * 
 * If this is NULL the data is not wrapped.
 * 
 * If its a PyObject, it IS wrapped. valid PyObjects are
 * Py_None -Just that the data is wrapped, dont check the pointer is correct or anything.
 * 			This will make a truth test return true and incref correctly.
 * 
 * Object -			This is from a Object, for now the very fact we exist means that it does.
 * 					Valid values are loc dloc scale.. maybe more.
 * 
 * MatrixObject -	This is from a matrix, check its pointer is a valid pointer from the matrix.
 * 					if its not return an error.
 * 
 * BPyMVertObject -	This is a vertex coordinate, re-assign the pointer whenever used
 * 				incase the verts were re-allocated.
 * 
 * 
 * BPyLPointObject - Lattice coordss, similar to MVert
 * 
 * BPyGenericLibObject -	If it is none of the above, this is assumed
 * 					if BPyGenericLibObject->id == NULL this vector is invalid.
 * */

#define CHECK_VEC_ERROR_INT(_self) if (!Vector_CheckPyObject(_self)) \
		return EXPP_ReturnIntError( PyExc_RuntimeError, "the data this vector references has been removed.")

#define CHECK_VEC_ERROR_PY(_self) if (!Vector_CheckPyObject(_self)) \
		return EXPP_ReturnPyObjError( PyExc_RuntimeError, "the data this vector references has been removed.")

#define VEC_SUBTYPE_OB_LOC 0
#define VEC_SUBTYPE_OB_DLOC 1
#define VEC_SUBTYPE_OB_SCALE 2
#define VEC_SUBTYPE_OB_DSCALE 3


/*prototypes*/
PyObject *Vector_Zero( BPyVectorObject * self );
PyObject *Vector_Normalize( BPyVectorObject * self );
PyObject *Vector_Negate( BPyVectorObject * self );
PyObject *Vector_Resize2D( BPyVectorObject * self );
PyObject *Vector_Resize3D( BPyVectorObject * self );
PyObject *Vector_Resize4D( BPyVectorObject * self );
PyObject *Vector_ToTrackQuat( BPyVectorObject * self, PyObject * args );
PyObject *Vector_copy( BPyVectorObject * self );
PyObject *Vector_CreatePyObject(float *vec, int size, PyObject * source);
void unwrapVector(BPyVectorObject * self);
int Vector_CheckPyObject(BPyVectorObject * self);

PyObject * VectorType_Init();
#endif				/* EXPP_vector_h */

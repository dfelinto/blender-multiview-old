/* 
 * $Id: matrix.h 11936 2007-09-04 05:22:42Z campbellbarton $
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
 * Contributor(s): Joseph Gilbert
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 *
 */

#ifndef EXPP_matrix_h
#define EXPP_matrix_h

#include <Python.h>
#include "gen_library.h"

extern PyTypeObject BPyMatrix_Type;

#define BPyMatrix_Check(v) PyObject_TypeCheck(v, &BPyMatrix_Type)

typedef struct {
	PyObject_VAR_HEAD 
	float *matrix[4];			/* use the first member of this is you need to access a 1D array */
	char rowSize, colSize;
	PyObject *source;
} BPyMatrixObject;

#define CHECK_MAT_ERROR_INT(_self) if (!Matrix_CheckPyObject(_self)) \
		return EXPP_ReturnIntError( PyExc_RuntimeError, "the data this matrix references has been removed.")

#define CHECK_MAT_ERROR_PY(_self) if (!Matrix_CheckPyObject(_self)) \
		return EXPP_ReturnPyObjError( PyExc_RuntimeError, "the data this matrix references has been removed.")

/*prototypes*/
PyObject *Matrix_CreatePyObject(float *mat, int rowSize, int colSize, PyObject * genlib);
int Matrix_CheckPyObject(BPyMatrixObject * self);

PyObject * MatrixType_Init();

#endif				/* EXPP_matrix_H */

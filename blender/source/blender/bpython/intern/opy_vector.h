

/* Matrix and vector objects in Python */

/* $Id$
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

/*****************************/
/*    Matrix Python Object   */
/*****************************/
/* temporar hack for typecasts */

typedef float (*Matrix4Ptr)[4];
typedef struct {
	PyObject_VAR_HEAD
	float *vec;
	int size;
} VectorObject;

typedef struct {
	PyObject_VAR_HEAD
	PyObject *rows[4];
	Matrix4Ptr mat;
} MatrixObject;


/* PROTOS */

PyObject *newVectorObject(float *vec, int size);
PyObject *newMatrixObject(Matrix4Ptr mat);
void init_py_matrix(void);
void init_py_vector(void);



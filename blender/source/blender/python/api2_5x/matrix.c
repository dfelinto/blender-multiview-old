/*
 * $Id: matrix.c 11960 2007-09-07 08:09:41Z campbellbarton $
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
 * Contributor(s): Michel Selten & Joseph Gilbert
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "matrix.h"
#include "Mathutils.h"
#include "BKE_utildefines.h"
#include "BLI_arithb.h"
#include "BLI_blenlib.h"
#include "gen_utils.h"
#include "gen_library.h"

/*-------------------------DOC STRINGS ---------------------------*/
static char Matrix_Zero_doc[] = "() - set all values in the matrix to 0";
static char Matrix_Identity_doc[] = "() - set the square matrix to it's identity matrix";
static char Matrix_Transpose_doc[] = "() - set the matrix to it's transpose";
static char Matrix_Determinant_doc[] = "() - return the determinant of the matrix";
static char Matrix_Invert_doc[] =  "() - set the matrix to it's inverse if an inverse is possible";
static char Matrix_TranslationPart_doc[] = "() - return a vector encompassing the translation of the matrix";
static char Matrix_RotationPart_doc[] = "() - return a vector encompassing the rotation of the matrix";
static char Matrix_scalePart_doc[] = "() - convert matrix to a 3D vector";
static char Matrix_Resize4x4_doc[] = "() - resize the matrix to a 4x4 square matrix";
static char Matrix_toEuler_doc[] = "() - convert matrix to a euler angle rotation";
static char Matrix_toQuat_doc[] = "() - convert matrix to a quaternion rotation";
static char Matrix_copy_doc[] = "() - return a copy of the matrix";

/*-----------------------METHOD DEFINITIONS ----------------------*/
PyObject *Matrix_Zero( BPyMatrixObject * self );
PyObject *Matrix_Identity( BPyMatrixObject * self );
PyObject *Matrix_Transpose( BPyMatrixObject * self );
PyObject *Matrix_Determinant( BPyMatrixObject * self );
PyObject *Matrix_Invert( BPyMatrixObject * self );
PyObject *Matrix_TranslationPart( BPyMatrixObject * self );
PyObject *Matrix_RotationPart( BPyMatrixObject * self );
PyObject *Matrix_scalePart( BPyMatrixObject * self );
PyObject *Matrix_Resize4x4( BPyMatrixObject * self );
PyObject *Matrix_toEuler( BPyMatrixObject * self );
PyObject *Matrix_toQuat( BPyMatrixObject * self );
PyObject *Matrix_copy( BPyMatrixObject * self );

struct PyMethodDef Matrix_methods[] = {
	{"zero", (PyCFunction) Matrix_Zero, METH_NOARGS, Matrix_Zero_doc},
	{"identity", (PyCFunction) Matrix_Identity, METH_NOARGS, Matrix_Identity_doc},
	{"transpose", (PyCFunction) Matrix_Transpose, METH_NOARGS, Matrix_Transpose_doc},
	{"determinant", (PyCFunction) Matrix_Determinant, METH_NOARGS, Matrix_Determinant_doc},
	{"invert", (PyCFunction) Matrix_Invert, METH_NOARGS, Matrix_Invert_doc},
	{"translationPart", (PyCFunction) Matrix_TranslationPart, METH_NOARGS, Matrix_TranslationPart_doc},
	{"rotationPart", (PyCFunction) Matrix_RotationPart, METH_NOARGS, Matrix_RotationPart_doc},
	{"scalePart", (PyCFunction) Matrix_scalePart, METH_NOARGS, Matrix_scalePart_doc},
	{"resize4x4", (PyCFunction) Matrix_Resize4x4, METH_NOARGS, Matrix_Resize4x4_doc},
	{"toEuler", (PyCFunction) Matrix_toEuler, METH_NOARGS, Matrix_toEuler_doc},
	{"toQuat", (PyCFunction) Matrix_toQuat, METH_NOARGS, Matrix_toQuat_doc},
	{"copy", (PyCFunction) Matrix_copy, METH_NOARGS, Matrix_copy_doc},
	{"__copy__", (PyCFunction) Matrix_copy, METH_NOARGS, Matrix_copy_doc},
	{NULL, NULL, 0, NULL}
};

/* factory constructors */

PyObject *Matrix_RotationMatrix(PyObject * self, PyObject * args);
PyObject *Matrix_TranslationMatrix(PyObject * self, PyObject * args);
PyObject *Matrix_ScaleMatrix(PyObject * self, PyObject * args);
PyObject *Matrix_OrthoProjectionMatrix(PyObject * self, PyObject * args);
PyObject *Matrix_ShearMatrix(PyObject * self, PyObject * args);

static char Matrix_RotationMatrix_doc[] = "() - construct a rotation matrix from an angle and axis of rotation";
static char Matrix_ScaleMatrix_doc[] =	"() - construct a scaling matrix from a scaling factor";
static char Matrix_TranslationMatrix_doc[] = "() - create a translation matrix from a vector";
static char Matrix_OrthoProjectionMatrix_doc[] = "() - construct a orthographic projection matrix from a selected plane";
static char Matrix_ShearMatrix_doc[] = "() - construct a shearing matrix from a plane of shear and a shear factor";

struct PyMethodDef MatrixType_methods[] = {
	{"Rotation", (PyCFunction) Matrix_RotationMatrix, METH_VARARGS, Matrix_RotationMatrix_doc},
	{"Scale", (PyCFunction) Matrix_ScaleMatrix, METH_VARARGS, Matrix_ScaleMatrix_doc},
	{"Shear", (PyCFunction) Matrix_ShearMatrix, METH_VARARGS, Matrix_ShearMatrix_doc},
	{"Translation", (PyCFunction) Matrix_TranslationMatrix, METH_VARARGS, Matrix_TranslationMatrix_doc},
	{"OrthoProjection", (PyCFunction) Matrix_OrthoProjectionMatrix,  METH_VARARGS, Matrix_OrthoProjectionMatrix_doc},
	{NULL, NULL, 0, NULL}
};

/*-----------------------------METHODS----------------------------*/
/*---------------------------Matrix.toQuat() ---------------------*/
PyObject *Matrix_toQuat(BPyMatrixObject * self)
{
	float quat[4];
	CHECK_MAT_ERROR_PY(self);
	
	/*must be 3-4 cols, 3-4 rows, square matrix*/
	if(self->colSize < 3 || self->rowSize < 3 || (self->colSize != self->rowSize)) {
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Matrix.toQuat(): inappropriate matrix size - expects 3x3 or 4x4 matrix\n");
	} 
	if(self->colSize == 3){
        Mat3ToQuat((float (*)[3])*self->matrix, quat);
	}else{
		Mat4ToQuat((float (*)[4])*self->matrix, quat);
	}
	
	return Quat_CreatePyObject(quat, (PyObject *)NULL);
}
/*---------------------------Matrix.toEuler() --------------------*/
PyObject *Matrix_toEuler(BPyMatrixObject * self)
{
	float eul[3];

	CHECK_MAT_ERROR_PY(self);
	
	/*must be 3-4 cols, 3-4 rows, square matrix*/
	if(self->colSize ==3 && self->rowSize ==3) {
		Mat3ToEul((float (*)[3])*self->matrix, eul);
	}else if (self->colSize ==4 && self->rowSize ==4) {
		float tempmat3[3][3];
		Mat3CpyMat4(tempmat3, (float (*)[4])*self->matrix);
		Mat3ToEul(tempmat3, eul);
	}else
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Matrix.toEuler(): inappropriate matrix size - expects 3x3 or 4x4 matrix\n");
	
	/* old api used deg's, not anymore */
	/*for(x = 0; x < 3; x++) {
		eul[x] *= (float) (180 / Py_PI);
	}*/
	return Euler_CreatePyObject(eul, (PyObject *)NULL);
}
/*---------------------------Matrix.resize4x4() ------------------*/
PyObject *Matrix_Resize4x4(BPyMatrixObject * self)
{
	int x, first_row_elem, curr_pos, new_pos, blank_columns, blank_rows, index;

	CHECK_MAT_ERROR_PY(self);
	
	if(self->source){
		return EXPP_ReturnPyObjError(PyExc_TypeError,
			"cannot resize wrapped data - only python matrices\n");
	}

	self->matrix[0] = PyMem_Realloc(self->matrix[0], (sizeof(float) * 16));
	if(self->matrix[0] == NULL) {
		return EXPP_ReturnPyObjError(PyExc_MemoryError,
			"matrix.resize4x4(): problem allocating pointer space\n\n");
	}
	
	/*set row pointers*/
	for(x = 1; x < 4; x++) {
		self->matrix[x] = self->matrix[0] + (x * 4);
	}
	/*move data to new spot in array + clean*/
	for(blank_rows = (4 - self->rowSize); blank_rows > 0; blank_rows--){
		for(x = 0; x < 4; x++){
			index = (4 * (self->rowSize + (blank_rows - 1))) + x;
			if (index == 10 || index == 15){
				self->matrix[0][index] = 1.0f;
			}else{
				self->matrix[0][index] = 0.0f;
			}
		}
	}
	for(x = 1; x <= self->rowSize; x++){
		first_row_elem = (self->colSize * (self->rowSize - x));
		curr_pos = (first_row_elem + (self->colSize -1));
		new_pos = (4 * (self->rowSize - x )) + (curr_pos - first_row_elem);
		for(blank_columns = (4 - self->colSize); blank_columns > 0; blank_columns--){
			self->matrix[0][new_pos + blank_columns] = 0.0f;
		}
		for(curr_pos = curr_pos; curr_pos >= first_row_elem; curr_pos--){
			self->matrix[0][new_pos] = self->matrix[0][curr_pos];
			new_pos--;
		}
	}
	self->rowSize = self->colSize = 4;
	return EXPP_incr_ret((PyObject*)self);
}
/*---------------------------Matrix.translationPart() ------------*/
PyObject *Matrix_TranslationPart(BPyMatrixObject * self)
{
	float vec[4];

	CHECK_MAT_ERROR_PY(self);
	
	if(self->colSize < 3 || self->rowSize < 4){
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Matrix.translationPart: inappropriate matrix size\n");
	}

	vec[0] = self->matrix[3][0];
	vec[1] = self->matrix[3][1];
	vec[2] = self->matrix[3][2];

	return Vector_CreatePyObject(vec, 3, (PyObject *)NULL);
}
/*---------------------------Matrix.rotationPart() ---------------*/
PyObject *Matrix_RotationPart(BPyMatrixObject * self)
{
	float mat[16] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	CHECK_MAT_ERROR_PY(self);
	
	if(self->colSize < 3 || self->rowSize < 3){
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Matrix.rotationPart: inappropriate matrix size\n");
	}

	mat[0] = self->matrix[0][0];
	mat[1] = self->matrix[0][1];
	mat[2] = self->matrix[0][2];
	mat[3] = self->matrix[1][0];
	mat[4] = self->matrix[1][1];
	mat[5] = self->matrix[1][2];
	mat[6] = self->matrix[2][0];
	mat[7] = self->matrix[2][1];
	mat[8] = self->matrix[2][2];

	return Matrix_CreatePyObject(mat, 3, 3, (PyObject *)NULL);
}
/*---------------------------Matrix.scalePart() --------------------*/
PyObject *Matrix_scalePart(BPyMatrixObject * self)
{
	float scale[3], rot[3];
	float mat[3][3], imat[3][3], tmat[3][3];
	
	CHECK_MAT_ERROR_PY(self);
	
	/*must be 3-4 cols, 3-4 rows, square matrix*/
	if(self->colSize == 4 && self->rowSize == 4)
		Mat3CpyMat4(mat, (float (*)[4])*self->matrix);
	else if(self->colSize == 3 && self->rowSize == 3)
		Mat3CpyMat3(mat, (float (*)[3])*self->matrix);
	else
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Matrix.scalePart(): inappropriate matrix size - expects 3x3 or 4x4 matrix\n");
	
	
	/* functionality copied from editobject.c apply_obmat */
	Mat3ToEul(mat, rot);
	EulToMat3(rot, tmat);
	Mat3Inv(imat, tmat);
	Mat3MulMat3(tmat, imat, mat);
	
	scale[0]= tmat[0][0];
	scale[1]= tmat[1][1];
	scale[2]= tmat[2][2];
	return Vector_CreatePyObject(scale, 3, (PyObject *)NULL);
}

/* this is also called by __invert__ from the the number
 * protocol so make sure it stays compatible */

/*---------------------------Matrix.invert() ---------------------*/
PyObject *Matrix_Invert(BPyMatrixObject * self)
{
	
	int x, y, z = 0;
	float det = 0.0f;
	PyObject *f = NULL;
	float mat[16] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	CHECK_MAT_ERROR_PY(self);
	
	if(self->rowSize != self->colSize){
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Matrix.invert(ed): only square matrices are supported\n");
	}

	/*calculate the determinant*/
	f = Matrix_Determinant(self);
	det = (float)PyFloat_AS_DOUBLE(f); /*Increfs, so we need to decref*/
	Py_DECREF(f);

	if(det != 0) {
		/*calculate the classical adjoint*/
		if(self->rowSize == 2) {
			mat[0] = self->matrix[1][1];
			mat[1] = -self->matrix[1][0];
			mat[2] = -self->matrix[0][1];
			mat[3] = self->matrix[0][0];
		} else if(self->rowSize == 3) {
			Mat3Adj((float (*)[3]) mat,(float (*)[3]) *self->matrix);
		} else if(self->rowSize == 4) {
			Mat4Adj((float (*)[4]) mat, (float (*)[4]) *self->matrix);
		}
		/*divide by determinate*/
		for(x = 0; x < (self->rowSize * self->colSize); x++) {
			mat[x] /= det;
		}
		/*set values*/
		for(x = 0; x < self->rowSize; x++) {
			for(y = 0; y < self->colSize; y++) {
				self->matrix[x][y] = mat[z];
				z++;
			}
		}
		/*transpose
		Matrix_Transpose(self);*/
	} else {
		return EXPP_ReturnPyObjError(PyExc_ValueError,
				"matrix does not have an inverse");
	}
	return EXPP_incr_ret((PyObject*)self);
}


/*---------------------------Matrix.determinant() ----------------*/
PyObject *Matrix_Determinant(BPyMatrixObject * self)
{
	float det = 0.0f;

	CHECK_MAT_ERROR_PY(self);
	
	if(self->rowSize != self->colSize){
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Matrix.determinant: only square matrices are supported\n");
	}

	if(self->rowSize == 2) {
		det = Det2x2(self->matrix[0][0], self->matrix[0][1],
					 self->matrix[1][0], self->matrix[1][1]);
	} else if(self->rowSize == 3) {
		det = Det3x3(self->matrix[0][0], self->matrix[0][1],
					 self->matrix[0][2], self->matrix[1][0],
					 self->matrix[1][1], self->matrix[1][2],
					 self->matrix[2][0], self->matrix[2][1],
					 self->matrix[2][2]);
	} else {
		det = Det4x4((float (*)[4]) *self->matrix);
	}

	return PyFloat_FromDouble( (double) det );
}
/*---------------------------Matrix.transpose() ------------------*/
PyObject *Matrix_Transpose(BPyMatrixObject * self)
{
	float t = 0.0f;

	CHECK_MAT_ERROR_PY(self);
	
	if(self->rowSize != self->colSize){
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Matrix.transpose(d): only square matrices are supported\n");
	}

	if(self->rowSize == 2) {
		t = self->matrix[1][0];
		self->matrix[1][0] = self->matrix[0][1];
		self->matrix[0][1] = t;
	} else if(self->rowSize == 3) {
		Mat3Transp((float (*)[3])*self->matrix);
	} else {
		Mat4Transp((float (*)[4])*self->matrix);
	}

	return EXPP_incr_ret((PyObject*)self);
}


/*---------------------------Matrix.zero() -----------------------*/
PyObject *Matrix_Zero(BPyMatrixObject * self)
{
	int row, col;

	CHECK_MAT_ERROR_PY(self);
	
	for(row = 0; row < self->rowSize; row++) {
		for(col = 0; col < self->colSize; col++) {
			self->matrix[row][col] = 0.0f;
		}
	}
	return EXPP_incr_ret((PyObject*)self);
}
/*---------------------------Matrix.identity(() ------------------*/
PyObject *Matrix_Identity(BPyMatrixObject * self)
{
	CHECK_MAT_ERROR_PY(self);
	
	if(self->rowSize != self->colSize){
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Matrix.identity: only square matrices are supported\n");
	}

	if(self->rowSize == 2) {
		self->matrix[0][0] = 1.0f;
		self->matrix[0][1] = 0.0f;
		self->matrix[1][0] = 0.0f;
		self->matrix[1][1] = 1.0f;
	} else if(self->rowSize == 3) {
		Mat3One((float (*)[3]) *self->matrix);
	} else {
		Mat4One((float (*)[4]) *self->matrix);
	}

	return EXPP_incr_ret((PyObject*)self);
}

/*---------------------------Matrix.inverted() ------------------*/
PyObject *Matrix_copy(BPyMatrixObject * self)
{
	CHECK_MAT_ERROR_PY(self);
	return Matrix_CreatePyObject((float (*))*self->matrix, self->rowSize, self->colSize, (PyObject *)NULL);
}

/*----------------------------dealloc()(internal) ----------------*/
/*free the py_object*/
static void Matrix_dealloc(BPyMatrixObject * self)
{
	if (self->source) {
		Py_DECREF(self->source);
	} else {
		PyMem_Free(self->matrix[0]);
	}
	PyObject_DEL(self);
}

static PyObject *Matrix_getRowSize(BPyMatrixObject * self)
{
	CHECK_MAT_ERROR_PY(self);
	return PyInt_FromLong((long) self->rowSize);
}

static PyObject *Matrix_getColSize(BPyMatrixObject * self)
{
	CHECK_MAT_ERROR_PY(self);
	return PyInt_FromLong((long) self->colSize);
}

static PyObject *Matrix_getWrapped(BPyMatrixObject * self)
{
	CHECK_MAT_ERROR_PY(self);
	
	if (self->source)
		Py_RETURN_TRUE;
	
	Py_RETURN_FALSE;
}

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef Matrix_getset[] = {
	{"rowSize",
	 (getter)Matrix_getRowSize, (setter)NULL,
	 "matrix row size", NULL},
	{"colSize",
	 (getter)Matrix_getColSize, (setter)NULL,
	 "matrix col size", NULL},
	{"wrapped",
	 (getter)Matrix_getWrapped, (setter)NULL,
	 "true if the matrix is wrapped", NULL},
	{NULL}  /* Sentinel */
};


/*----------------------------print object (internal)-------------*/
/*print the object to screen*/
static PyObject *Matrix_repr(BPyMatrixObject * self)
{
	int x, y;
	char buffer[48], str[1024];

	CHECK_MAT_ERROR_PY(self);
	
	BLI_strncpy(str,"",1024);
	for(x = 0; x < self->rowSize; x++){
		sprintf(buffer, "[");
		strcat(str,buffer);
		for(y = 0; y < (self->colSize - 1); y++) {
			sprintf(buffer, "%.6f, ", self->matrix[x][y]);
			strcat(str,buffer);
		}
		if(x < (self->rowSize-1)){
			sprintf(buffer, "%.6f](matrix [row %d])\n", self->matrix[x][y], x);
			strcat(str,buffer);
		}else{
			sprintf(buffer, "%.6f](matrix [row %d])", self->matrix[x][y], x);
			strcat(str,buffer);
		}
	}

	return PyString_FromString(str);
}
/*------------------------tp_richcmpr*/
/*returns -1 execption, 0 false, 1 true*/
static PyObject* Matrix_richcmpr(PyObject *objectA, PyObject *objectB, int comparison_type)
{
	BPyMatrixObject *matA = NULL, *matB = NULL;
	int result = 0;
	
	if (!BPyMatrix_Check(objectA) || !BPyMatrix_Check(objectB)){
		if (comparison_type == Py_NE){
			Py_RETURN_TRUE;
		}else{
			Py_RETURN_FALSE;
		}
	}
	matA = (BPyMatrixObject*)objectA;
	matB = (BPyMatrixObject*)objectB;

	CHECK_MAT_ERROR_PY(matA);
	CHECK_MAT_ERROR_PY(matB);
	
	if (matA->colSize != matB->colSize || matA->rowSize != matB->rowSize){
		if (comparison_type == Py_NE){
			Py_RETURN_TRUE;
		}else{
			Py_RETURN_FALSE;
		}
	}

	switch (comparison_type){
		case Py_EQ:
			result = EXPP_VectorsAreEqual(matA->matrix[0], matB->matrix[0],
				(matA->rowSize * matA->colSize), 1);
			break;
		case Py_NE:
			result = EXPP_VectorsAreEqual(matA->matrix[0], matB->matrix[0],
				(matA->rowSize * matA->colSize), 1);
			if (result == 0){
				result = 1;
			}else{
				result = 0;
			}
			break;
		default:
			printf("The result of the comparison could not be evaluated");
			break;
	}
	if (result == 1){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}
/*------------------------tp_doc*/
static char BPyMatrix_doc[] = "This is a wrapper for matrix objects.";
/*---------------------SEQUENCE PROTOCOLS------------------------
  ----------------------------len(object)------------------------
  sequence length*/
static int Matrix_len(BPyMatrixObject * self)
{
	return (self->colSize * self->rowSize);
}
/*----------------------------object[]---------------------------
  sequence accessor (get)
  the wrapped vector gives direct access to the matrix data*/
static PyObject *Matrix_item(BPyMatrixObject * self, int i)
{
	BPyVectorObject *vec;
	CHECK_MAT_ERROR_PY(self);
	
	
	if(i < 0 || i >= self->rowSize)
		return EXPP_ReturnPyObjError(PyExc_IndexError,
		"matrix[attribute]: array index out of range\n");

	vec = (BPyVectorObject *)Vector_CreatePyObject(self->matrix[i], self->colSize, (PyObject *)self);
	vec->sub_index = (char)i;
	return (PyObject *)vec; 
}
/*----------------------------object[]-------------------------
  sequence accessor (set)*/
static int Matrix_ass_item(BPyMatrixObject * self, int i, PyObject * ob)
{
	int y, x, size = 0;
	float vec[4];
	PyObject *m, *f;

	CHECK_MAT_ERROR_INT(self);
	
	if(i >= self->rowSize || i < 0){
		return EXPP_ReturnIntError(PyExc_TypeError,
			"matrix[attribute] = x: bad row\n");
	}

	if(PySequence_Check(ob)){
		size = PySequence_Length(ob);
		if(size != self->colSize){
			return EXPP_ReturnIntError(PyExc_TypeError,
				"matrix[attribute] = x: bad sequence size\n");
		}
		for (x = 0; x < size; x++) {
			m = PySequence_GetItem(ob, x);
			if (m == NULL) { /*Failed to read sequence*/
				return EXPP_ReturnIntError(PyExc_RuntimeError, 
					"matrix[attribute] = x: unable to read sequence\n");
			}

			f = PyNumber_Float(m);
			if(f == NULL) { /*parsed item not a number*/
				Py_DECREF(m);
				return EXPP_ReturnIntError(PyExc_TypeError, 
					"matrix[attribute] = x: sequence argument not a number\n");
			}

			vec[x] = (float)PyFloat_AS_DOUBLE(f);
			EXPP_decr2(m, f);
		}
		/*parsed well - now set in matrix*/
		for(y = 0; y < size; y++){
			self->matrix[i][y] = vec[y];
		}
		return 0;
	}else{
		return EXPP_ReturnIntError(PyExc_TypeError,
			"matrix[attribute] = x: expects a sequence of column size\n");
	}
}
/*----------------------------object[z:y]------------------------
  sequence slice (get)*/
static PyObject *Matrix_slice(BPyMatrixObject * self, int begin, int end)
{

	PyObject *list = NULL;
	int count;
	BPyVectorObject *vec;
	
	CHECK_MAT_ERROR_PY(self);
	
	CLAMP(begin, 0, self->rowSize);
	CLAMP(end, 0, self->rowSize);
	begin = MIN2(begin,end);

	list = PyList_New(end - begin);
	for(count = begin; count < end; count++) {
		vec = ( BPyVectorObject*)Vector_CreatePyObject(self->matrix[count], self->colSize, (PyObject *)self);
		vec->sub_index = (char)count;
		PyList_SetItem(list, count - begin,	(PyObject *)vec);
	}

	return list;
}
/*----------------------------object[z:y]------------------------
  sequence slice (set)*/
static int Matrix_ass_slice(BPyMatrixObject * self, int begin, int end,
			     PyObject * seq)
{
	int i, x, y, size, sub_size = 0;
	float mat[16];
	PyObject *subseq;
	PyObject *m, *f;

	CHECK_MAT_ERROR_INT(self);
	
	CLAMP(begin, 0, self->rowSize);
	CLAMP(end, 0, self->rowSize);
	begin = MIN2(begin,end);

	if(PySequence_Check(seq)){
		size = PySequence_Length(seq);
		if(size != (end - begin)){
			return EXPP_ReturnIntError(PyExc_TypeError,
				"matrix[begin:end] = []: size mismatch in slice assignment\n");
		}
		/*parse sub items*/
		for (i = 0; i < size; i++) {
			/*parse each sub sequence*/
			subseq = PySequence_GetItem(seq, i);
			if (subseq == NULL) { /*Failed to read sequence*/
				return EXPP_ReturnIntError(PyExc_RuntimeError, 
					"matrix[begin:end] = []: unable to read sequence\n");
			}

			if(PySequence_Check(subseq)){
				/*subsequence is also a sequence*/
				sub_size = PySequence_Length(subseq);
				if(sub_size != self->colSize){
					Py_DECREF(subseq);
					return EXPP_ReturnIntError(PyExc_TypeError,
						"matrix[begin:end] = []: size mismatch in slice assignment\n");
				}
				for (y = 0; y < sub_size; y++) {
					m = PySequence_GetItem(subseq, y);
					if (m == NULL) { /*Failed to read sequence*/
						Py_DECREF(subseq);
						return EXPP_ReturnIntError(PyExc_RuntimeError, 
							"matrix[begin:end] = []: unable to read sequence\n");
					}

					f = PyNumber_Float(m);
					if(f == NULL) { /*parsed item not a number*/
						EXPP_decr2(m, subseq);
						return EXPP_ReturnIntError(PyExc_TypeError, 
							"matrix[begin:end] = []: sequence argument not a number\n");
					}

					mat[(i * self->colSize) + y] = (float)PyFloat_AS_DOUBLE(f);
					EXPP_decr2(f, m);
				}
			}else{
				Py_DECREF(subseq);
				return EXPP_ReturnIntError(PyExc_TypeError,
					"matrix[begin:end] = []: illegal argument type for built-in operation\n");
			}
			Py_DECREF(subseq);
		}
		/*parsed well - now set in matrix*/
		for(x = 0; x < (size * sub_size); x++){
			self->matrix[begin + (int)floor(x / self->colSize)][x % self->colSize] = mat[x];
		}
		return 0;
	}else{
		return EXPP_ReturnIntError(PyExc_TypeError,
			"matrix[begin:end] = []: illegal argument type for built-in operation\n");
	}
}
/*------------------------NUMERIC PROTOCOLS----------------------
  ------------------------obj + obj------------------------------*/
static PyObject *Matrix_add(PyObject * m1, PyObject * m2)
{
	int x, y;
	float mat[16] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	BPyMatrixObject *mat1 = NULL, *mat2 = NULL;
	
	mat1 = (BPyMatrixObject*)m1;
	mat2 = (BPyMatrixObject*)m2;
	
	if (!BPyMatrix_Check(mat1) || !BPyMatrix_Check(mat2))
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Matrix addition: arguments not valid for this operation....\n");
	
	CHECK_MAT_ERROR_PY(mat1);
	CHECK_MAT_ERROR_PY(mat2);
	
	if(mat1->rowSize != mat2->rowSize || mat1->colSize != mat2->colSize){
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Matrix addition: matrices must have the same dimensions for this operation\n");
	}

	for(x = 0; x < mat1->rowSize; x++) {
		for(y = 0; y < mat1->colSize; y++) {
			mat[((x * mat1->colSize) + y)] = mat1->matrix[x][y] + mat2->matrix[x][y];
		}
	}

	return Matrix_CreatePyObject(mat, mat1->rowSize, mat1->colSize, (PyObject *)NULL);
}
/*------------------------obj - obj------------------------------
  subtraction*/
static PyObject *Matrix_sub(PyObject * m1, PyObject * m2)
{
	int x, y;
	float mat[16] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	BPyMatrixObject *mat1 = NULL, *mat2 = NULL;

	mat1 = (BPyMatrixObject*)m1;
	mat2 = (BPyMatrixObject*)m2;

	if (!BPyMatrix_Check(mat1) || !BPyMatrix_Check(mat2))
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Matrix addition: arguments not valid for this operation....\n");
	
	CHECK_MAT_ERROR_PY(mat1);
	CHECK_MAT_ERROR_PY(mat2);
	
	if(mat1->rowSize != mat2->rowSize || mat1->colSize != mat2->colSize){
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Matrix addition: matrices must have the same dimensions for this operation\n");
	}

	for(x = 0; x < mat1->rowSize; x++) {
		for(y = 0; y < mat1->colSize; y++) {
			mat[((x * mat1->colSize) + y)] = mat1->matrix[x][y] - mat2->matrix[x][y];
		}
	}

	return Matrix_CreatePyObject(mat, mat1->rowSize, mat1->colSize, (PyObject *)NULL);
}
/*------------------------obj * obj------------------------------
  mulplication*/
static PyObject *Matrix_mul(PyObject * m1, PyObject * m2)
{
	int x, y, z;
	float scalar;
	float mat[16] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	double dot = 0.0f;
	BPyMatrixObject *mat1 = NULL, *mat2 = NULL;
	PyObject *f = NULL;

	if (BPyMatrix_Check(m1)) {
		mat1 = (BPyMatrixObject*)m1;
		CHECK_MAT_ERROR_PY(mat1);
	}
	if (BPyMatrix_Check(m2))	{
		mat2 = (BPyMatrixObject*)m2;
		CHECK_MAT_ERROR_PY(mat2);
	}
	
	if (mat1 && mat2) {
		if(mat1->colSize != mat2->rowSize) {
			return EXPP_ReturnPyObjError(PyExc_AttributeError,
				"Matrix multiplication: matrix A rowsize must equal matrix B colsize\n");
		}
		for(x = 0; x < mat1->rowSize; x++) {
			for(y = 0; y < mat2->colSize; y++) {
				for(z = 0; z < mat1->colSize; z++) {
					dot += (mat1->matrix[x][z] * mat2->matrix[z][y]);
				}
				mat[((x * mat1->rowSize) + y)] = (float)dot;
				dot = 0.0f;
			}
		}
		return Matrix_CreatePyObject(mat, mat1->rowSize, mat2->colSize, (PyObject *)NULL);
		
	/* (vec*matrix) is done by vector.c we dont need to worry about that */
	} else if (BPyVector_Check(m2)) {
		CHECK_VEC_ERROR_PY((BPyVectorObject *)m2);
		return column_vector_multiplication(mat1, (BPyVectorObject *)m2);
	} else {
		/* make mat1 the matrix */
		if (!mat1){
			f = PyNumber_Float(m1);
			mat1 = mat2;
		} else {
			f = PyNumber_Float(m2);
			/* mat1 is the matrix */
		}
		if (f == NULL) { /*parsed item not a number*/
			return EXPP_ReturnPyObjError(PyExc_TypeError, 
				"Matrix multiplication: arguments not acceptable for this operation\n");
		}
		
		scalar = (float)PyFloat_AS_DOUBLE(f);
		Py_DECREF(f);
		
		for(x = 0; x < mat1->rowSize; x++) {
			for(y = 0; y < mat1->colSize; y++) {
				mat[((x * mat1->colSize) + y)] = scalar * mat1->matrix[x][y];
			}
		}
		return Matrix_CreatePyObject(mat, mat1->rowSize, mat1->colSize, (PyObject *)NULL);
	}
	
	return EXPP_ReturnPyObjError(PyExc_TypeError, 
		"Matrix multiplication: arguments not acceptable for this operation\n");
}

/*-----------------PROTOCOL DECLARATIONS--------------------------*/
static PySequenceMethods Matrix_SeqMethods = {
	(inquiry) Matrix_len,					/* sq_length */
	(binaryfunc) 0,							/* sq_concat */
	(intargfunc) 0,							/* sq_repeat */
	(intargfunc) Matrix_item,				/* sq_item */
	(intintargfunc) Matrix_slice,			/* sq_slice */
	(intobjargproc) Matrix_ass_item,		/* sq_ass_item */
	(intintobjargproc) Matrix_ass_slice,	/* sq_ass_slice */
};
static PyNumberMethods Matrix_NumMethods = {
	(binaryfunc) Matrix_add,				/* __add__ */
	(binaryfunc) Matrix_sub,				/* __sub__ */
	(binaryfunc) Matrix_mul,				/* __mul__ */
	(binaryfunc) 0,							/* __div__ */
	(binaryfunc) 0,							/* __mod__ */
	(binaryfunc) 0,							/* __divmod__ */
	(ternaryfunc) 0,						/* __pow__ */
	(unaryfunc) 0,							/* __neg__ */
	(unaryfunc) 0,							/* __pos__ */
	(unaryfunc) 0,							/* __abs__ */
	(inquiry) 0,							/* __nonzero__ */
	(unaryfunc) Matrix_Invert,				/* __invert__ */
	(binaryfunc) 0,							/* __lshift__ */
	(binaryfunc) 0,							/* __rshift__ */
	(binaryfunc) 0,							/* __and__ */
	(binaryfunc) 0,							/* __xor__ */
	(binaryfunc) 0,							/* __or__ */
	(coercion) 0,							/* __coerce__ */
	(unaryfunc) 0,							/* __int__ */
	(unaryfunc) 0,							/* __long__ */
	(unaryfunc) 0,							/* __float__ */
	(unaryfunc) 0,							/* __oct__ */
	(unaryfunc) 0,							/* __hex__ */
};

/* this types constructor - apply any fixes to vec, eul, quat and col,  */
static PyObject *Matrix_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	PyObject *listObject = NULL;
	PyObject *argObject, *m, *s, *f;
	BPyMatrixObject *mat;
	int argSize, seqSize = 0, i, j;
	float matrix[16] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	argSize = PySequence_Length(args);
	if(argSize > 4){	//bad arg nums
		return EXPP_ReturnPyObjError(PyExc_AttributeError, 
			"bpy.types.Matrix(): expects 0-4 numeric sequences of the same size\n");
	} else if (argSize == 0) { //return empty 4D matrix
		return Matrix_CreatePyObject(NULL, 4, 4, (PyObject *)NULL);
	} else if (argSize == 1){
		//copy constructor for matrix objects
		argObject = PySequence_GetItem(args, 0);
		if(BPyMatrix_Check(argObject)){
			mat = (BPyMatrixObject *)argObject;

			argSize = mat->rowSize; //rows
			seqSize = mat->colSize; //col
			for(i = 0; i < (seqSize * argSize); i++){
				matrix[i] = mat->matrix[0][i];
			}
		}
		Py_DECREF(argObject);
	}else{ //2-4 arguments (all seqs? all same size?)
		for(i =0; i < argSize; i++){
			argObject = PySequence_GetItem(args, i);
			if (PySequence_Check(argObject)) { //seq?
				if(seqSize){ //0 at first
					if(PySequence_Length(argObject) != seqSize){ //seq size not same
						Py_DECREF(argObject);
						return EXPP_ReturnPyObjError(PyExc_AttributeError, 
						"bpy.types.Matrix(): expects 0-4 numeric sequences of the same size\n");
					}
				}
				seqSize = PySequence_Length(argObject);
			}else{ //arg not a sequence
				Py_XDECREF(argObject);
				return EXPP_ReturnPyObjError(PyExc_TypeError, 
					"bpy.types.Matrix(): expects 0-4 numeric sequences of the same size\n");
			}
			Py_DECREF(argObject);
		}
		//all is well... let's continue parsing
		listObject = args;
		for (i = 0; i < argSize; i++){
			m = PySequence_GetItem(listObject, i);
			if (m == NULL) { // Failed to read sequence
				return EXPP_ReturnPyObjError(PyExc_RuntimeError, 
					"bpy.types.Matrix(): failed to parse arguments...\n");
			}

			for (j = 0; j < seqSize; j++) {
				s = PySequence_GetItem(m, j);
					if (s == NULL) { // Failed to read sequence
					Py_DECREF(m);
					return EXPP_ReturnPyObjError(PyExc_RuntimeError, 
						"bpy.types.Matrix(): failed to parse arguments...\n");
				}

				f = PyNumber_Float(s);
				if(f == NULL) { // parsed item is not a number
					EXPP_decr2(m,s);
					return EXPP_ReturnPyObjError(PyExc_AttributeError, 
						"bpy.types.Matrix(): expects 0-4 numeric sequences of the same size\n");
				}

				matrix[(seqSize*i)+j]=(float)PyFloat_AS_DOUBLE(f);
				EXPP_decr2(f,s);
			}
			Py_DECREF(m);
		}
	}
	return Matrix_CreatePyObject(matrix, argSize, seqSize, (PyObject *)NULL);
}




/*------------------Constructors For Matrix Types ---------------*/

//----------------------------------Matrix.Rotation() ----------
//mat is a 1D array of floats - row[0][0],row[0][1], row[1][0], etc.
//creates a rotation matrix
PyObject *Matrix_RotationMatrix(PyObject * self, PyObject * args)
{
	BPyVectorObject *vec = NULL;
	char *axis = NULL;
	int matSize;
	float angle = 0.0f, norm = 0.0f, cosAngle = 0.0f, sinAngle = 0.0f;
	float mat[16] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	if(!PyArg_ParseTuple
	    (args, "fi|sO!", &angle, &matSize, &axis, &BPyVector_Type, &vec)) {
		return EXPP_ReturnPyObjError (PyExc_TypeError, 
			"bpy.types.Matrix.Rotation(): expected float int and optional string and vector\n");
	}
	
	/* Clamp to -360:360 */
	while (angle<-360.0f)
		angle+=360.0;
	while (angle>360.0f)
		angle-=360.0;
	
	if(matSize != 2 && matSize != 3 && matSize != 4)
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"bpy.types.Matrix.Rotation(): can only return a 2x2 3x3 or 4x4 matrix\n");
	if(matSize == 2 && (axis != NULL || vec != NULL))
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"bpy.types.Matrix.Rotation(): cannot create a 2x2 rotation matrix around arbitrary axis\n");
	if((matSize == 3 || matSize == 4) && axis == NULL)
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"bpy.types.Matrix.Rotation(): please choose an axis of rotation for 3d and 4d matrices\n");
	if(axis) {
		if(((strcmp(axis, "r") == 0) ||
		      (strcmp(axis, "R") == 0)) && vec == NULL)
			return EXPP_ReturnPyObjError(PyExc_AttributeError,
				"bpy.types.Matrix.Rotation(): please define the arbitrary axis of rotation\n");
	}
	if(vec) {
		if(vec->size != 3)
			return EXPP_ReturnPyObjError(PyExc_AttributeError,
						      "bpy.types.Matrix.Rotation(): the arbitrary axis must be a 3D vector\n");
	}
	//convert to radians
	angle = angle * (float) (Py_PI / 180);
	if(axis == NULL && matSize == 2) {
		//2D rotation matrix
		mat[0] = (float) cos (angle);
		mat[1] = (float) sin (angle);
		mat[2] = -((float) sin(angle));
		mat[3] = (float) cos(angle);
	} else if((strcmp(axis, "x") == 0) || (strcmp(axis, "X") == 0)) {
		//rotation around X
		mat[0] = 1.0f;
		mat[4] = (float) cos(angle);
		mat[5] = (float) sin(angle);
		mat[7] = -((float) sin(angle));
		mat[8] = (float) cos(angle);
	} else if((strcmp(axis, "y") == 0) || (strcmp(axis, "Y") == 0)) {
		//rotation around Y
		mat[0] = (float) cos(angle);
		mat[2] = -((float) sin(angle));
		mat[4] = 1.0f;
		mat[6] = (float) sin(angle);
		mat[8] = (float) cos(angle);
	} else if((strcmp(axis, "z") == 0) || (strcmp(axis, "Z") == 0)) {
		//rotation around Z
		mat[0] = (float) cos(angle);
		mat[1] = (float) sin(angle);
		mat[3] = -((float) sin(angle));
		mat[4] = (float) cos(angle);
		mat[8] = 1.0f;
	} else if((strcmp(axis, "r") == 0) || (strcmp(axis, "R") == 0)) {
		//arbitrary rotation
		//normalize arbitrary axis
		norm = (float) sqrt(vec->vec[0] * vec->vec[0] +
				       vec->vec[1] * vec->vec[1] +
				       vec->vec[2] * vec->vec[2]);
		vec->vec[0] /= norm;
		vec->vec[1] /= norm;
		vec->vec[2] /= norm;

		//create matrix
		cosAngle = (float) cos(angle);
		sinAngle = (float) sin(angle);
		mat[0] = ((vec->vec[0] * vec->vec[0]) * (1 - cosAngle)) +
			cosAngle;
		mat[1] = ((vec->vec[0] * vec->vec[1]) * (1 - cosAngle)) +
			(vec->vec[2] * sinAngle);
		mat[2] = ((vec->vec[0] * vec->vec[2]) * (1 - cosAngle)) -
			(vec->vec[1] * sinAngle);
		mat[3] = ((vec->vec[0] * vec->vec[1]) * (1 - cosAngle)) -
			(vec->vec[2] * sinAngle);
		mat[4] = ((vec->vec[1] * vec->vec[1]) * (1 - cosAngle)) +
			cosAngle;
		mat[5] = ((vec->vec[1] * vec->vec[2]) * (1 - cosAngle)) +
			(vec->vec[0] * sinAngle);
		mat[6] = ((vec->vec[0] * vec->vec[2]) * (1 - cosAngle)) +
			(vec->vec[1] * sinAngle);
		mat[7] = ((vec->vec[1] * vec->vec[2]) * (1 - cosAngle)) -
			(vec->vec[0] * sinAngle);
		mat[8] = ((vec->vec[2] * vec->vec[2]) * (1 - cosAngle)) +
			cosAngle;
	} else {
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"bpy.types.Matrix.Rotation(): unrecognizable axis of rotation type - expected x,y,z or r\n");
	}
	if(matSize == 4) {
		//resize matrix
		mat[10] = mat[8];
		mat[9] = mat[7];
		mat[8] = mat[6];
		mat[7] = 0.0f;
		mat[6] = mat[5];
		mat[5] = mat[4];
		mat[4] = mat[3];
		mat[3] = 0.0f;
	}
	//pass to matrix creation
	return Matrix_CreatePyObject(mat, matSize, matSize, (PyObject *)NULL);
}
//----------------------------------Mathutils.TranslationMatrix() -------
//creates a translation matrix
PyObject *Matrix_TranslationMatrix(PyObject * self, PyObject * args)
{
	BPyVectorObject *vec = NULL;
	float mat[16] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	if(!PyArg_ParseTuple(args, "O!", &BPyVector_Type, &vec)) {
		return EXPP_ReturnPyObjError(PyExc_TypeError,
						"bpy.types.Matrix.Translation(): expected vector\n");
	}
	if(vec->size != 3 && vec->size != 4) {
		return EXPP_ReturnPyObjError(PyExc_TypeError,
					      "bpy.types.Matrix.Translation(): vector must be 3D or 4D\n");
	}
	//create a identity matrix and add translation
	Mat4One((float(*)[4]) mat);
	mat[12] = vec->vec[0];
	mat[13] = vec->vec[1];
	mat[14] = vec->vec[2];

	return Matrix_CreatePyObject(mat, 4, 4, (PyObject *)NULL);
}
//----------------------------------Mathutils.ScaleMatrix() -------------
//mat is a 1D array of floats - row[0][0],row[0][1], row[1][0], etc.
//creates a scaling matrix
PyObject *Matrix_ScaleMatrix(PyObject * self, PyObject * args)
{
	BPyVectorObject *vec = NULL;
	float norm = 0.0f, factor;
	int matSize, x;
	float mat[16] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	if(!PyArg_ParseTuple
	    (args, "fi|O!", &factor, &matSize, &BPyVector_Type, &vec)) {
		return EXPP_ReturnPyObjError(PyExc_TypeError,
			"bpy.types.Matrix.Scale(): expected float int and optional vector\n");
	}
	if(matSize != 2 && matSize != 3 && matSize != 4)
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"bpy.types.Matrix.Scale(): can only return a 2x2 3x3 or 4x4 matrix\n");
	if(vec) {
		if(vec->size > 2 && matSize == 2)
			return EXPP_ReturnPyObjError(PyExc_AttributeError,
				"bpy.types.Matrix.Scale(): please use 2D vectors when scaling in 2D\n");
	}
	if(vec == NULL) {	//scaling along axis
		if(matSize == 2) {
			mat[0] = factor;
			mat[3] = factor;
		} else {
			mat[0] = factor;
			mat[4] = factor;
			mat[8] = factor;
		}
	} else { //scaling in arbitrary direction
		//normalize arbitrary axis
		for(x = 0; x < vec->size; x++) {
			norm += vec->vec[x] * vec->vec[x];
		}
		norm = (float) sqrt(norm);
		for(x = 0; x < vec->size; x++) {
			vec->vec[x] /= norm;
		}
		if(matSize == 2) {
			mat[0] = 1 +((factor - 1) *(vec->vec[0] * vec->vec[0]));
			mat[1] =((factor - 1) *(vec->vec[0] * vec->vec[1]));
			mat[2] =((factor - 1) *(vec->vec[0] * vec->vec[1]));
			mat[3] = 1 + ((factor - 1) *(vec->vec[1] * vec->vec[1]));
		} else {
			mat[0] = 1 + ((factor - 1) *(vec->vec[0] * vec->vec[0]));
			mat[1] =((factor - 1) *(vec->vec[0] * vec->vec[1]));
			mat[2] =((factor - 1) *(vec->vec[0] * vec->vec[2]));
			mat[3] =((factor - 1) *(vec->vec[0] * vec->vec[1]));
			mat[4] = 1 + ((factor - 1) *(vec->vec[1] * vec->vec[1]));
			mat[5] =((factor - 1) *(vec->vec[1] * vec->vec[2]));
			mat[6] =((factor - 1) *(vec->vec[0] * vec->vec[2]));
			mat[7] =((factor - 1) *(vec->vec[1] * vec->vec[2]));
			mat[8] = 1 + ((factor - 1) *(vec->vec[2] * vec->vec[2]));
		}
	}
	if(matSize == 4) {
		//resize matrix
		mat[10] = mat[8];
		mat[9] = mat[7];
		mat[8] = mat[6];
		mat[7] = 0.0f;
		mat[6] = mat[5];
		mat[5] = mat[4];
		mat[4] = mat[3];
		mat[3] = 0.0f;
	}
	//pass to matrix creation
	return Matrix_CreatePyObject(mat, matSize, matSize, (PyObject *)NULL);
}
//----------------------------------Mathutils.OrthoProjectionMatrix() ---
//mat is a 1D array of floats - row[0][0],row[0][1], row[1][0], etc.
//creates an ortho projection matrix
PyObject *Matrix_OrthoProjectionMatrix(PyObject * self, PyObject * args)
{
	BPyVectorObject *vec = NULL;
	char *plane;
	int matSize, x;
	float norm = 0.0f;
	float mat[16] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	
	if(!PyArg_ParseTuple
	    (args, "si|O!", &plane, &matSize, &BPyVector_Type, &vec)) {
		return EXPP_ReturnPyObjError(PyExc_TypeError,
			"bpy.types.Matrix.OrthoProjection(): expected string and int and optional vector\n");
	}
	if(matSize != 2 && matSize != 3 && matSize != 4)
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"bpy.types.Matrix.OrthoProjection(): can only return a 2x2 3x3 or 4x4 matrix\n");
	if(vec) {
		if(vec->size > 2 && matSize == 2)
			return EXPP_ReturnPyObjError(PyExc_AttributeError,
				"bpy.types.Matrix.OrthoProjection(): please use 2D vectors when scaling in 2D\n");
	}
	if(vec == NULL) {	//ortho projection onto cardinal plane
		if(((strcmp(plane, "x") == 0)
		      || (strcmp(plane, "X") == 0)) && matSize == 2) {
			mat[0] = 1.0f;
		} else if(((strcmp(plane, "y") == 0) 
			|| (strcmp(plane, "Y") == 0))
			   && matSize == 2) {
			mat[3] = 1.0f;
		} else if(((strcmp(plane, "xy") == 0)
			     || (strcmp(plane, "XY") == 0))
			   && matSize > 2) {
			mat[0] = 1.0f;
			mat[4] = 1.0f;
		} else if(((strcmp(plane, "xz") == 0)
			     || (strcmp(plane, "XZ") == 0))
			   && matSize > 2) {
			mat[0] = 1.0f;
			mat[8] = 1.0f;
		} else if(((strcmp(plane, "yz") == 0)
			     || (strcmp(plane, "YZ") == 0))
			   && matSize > 2) {
			mat[4] = 1.0f;
			mat[8] = 1.0f;
		} else {
			return EXPP_ReturnPyObjError(PyExc_AttributeError,
				"bpy.types.Matrix.OrthoProjection(): unknown plane - expected: x, y, xy, xz, yz\n");
		}
	} else { //arbitrary plane
		//normalize arbitrary axis
		for(x = 0; x < vec->size; x++) {
			norm += vec->vec[x] * vec->vec[x];
		}
		norm = (float) sqrt(norm);
		for(x = 0; x < vec->size; x++) {
			vec->vec[x] /= norm;
		}
		if(((strcmp(plane, "r") == 0)
		      || (strcmp(plane, "R") == 0)) && matSize == 2) {
			mat[0] = 1 - (vec->vec[0] * vec->vec[0]);
			mat[1] = -(vec->vec[0] * vec->vec[1]);
			mat[2] = -(vec->vec[0] * vec->vec[1]);
			mat[3] = 1 - (vec->vec[1] * vec->vec[1]);
		} else if(((strcmp(plane, "r") == 0)
			     || (strcmp(plane, "R") == 0))
			   && matSize > 2) {
			mat[0] = 1 - (vec->vec[0] * vec->vec[0]);
			mat[1] = -(vec->vec[0] * vec->vec[1]);
			mat[2] = -(vec->vec[0] * vec->vec[2]);
			mat[3] = -(vec->vec[0] * vec->vec[1]);
			mat[4] = 1 - (vec->vec[1] * vec->vec[1]);
			mat[5] = -(vec->vec[1] * vec->vec[2]);
			mat[6] = -(vec->vec[0] * vec->vec[2]);
			mat[7] = -(vec->vec[1] * vec->vec[2]);
			mat[8] = 1 - (vec->vec[2] * vec->vec[2]);
		} else {
			return EXPP_ReturnPyObjError(PyExc_AttributeError,
				"bpy.types.Matrix.OrthoProjection(): unknown plane - expected: 'r' expected for axis designation\n");
		}
	}
	if(matSize == 4) {
		//resize matrix
		mat[10] = mat[8];
		mat[9] = mat[7];
		mat[8] = mat[6];
		mat[7] = 0.0f;
		mat[6] = mat[5];
		mat[5] = mat[4];
		mat[4] = mat[3];
		mat[3] = 0.0f;
	}
	//pass to matrix creation
	return Matrix_CreatePyObject(mat, matSize, matSize, (PyObject *)NULL);
}
//----------------------------------Mathutils.ShearMatrix() -------------
//creates a shear matrix
PyObject *Matrix_ShearMatrix(PyObject * self, PyObject * args)
{
	int matSize;
	char *plane;
	float factor;
	float mat[16] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	if(!PyArg_ParseTuple(args, "sfi", &plane, &factor, &matSize)) {
		return EXPP_ReturnPyObjError(PyExc_TypeError,
			"bpy.types.Matrix.Shear(): expected string float and int\n");
	}
	if(matSize != 2 && matSize != 3 && matSize != 4)
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"bpy.types.Matrix.Shear(): can only return a 2x2 3x3 or 4x4 matrix\n");

	if(((strcmp(plane, "x") == 0) || (strcmp(plane, "X") == 0))
	    && matSize == 2) {
		mat[0] = 1.0f;
		mat[2] = factor;
		mat[3] = 1.0f;
	} else if(((strcmp(plane, "y") == 0)
		     || (strcmp(plane, "Y") == 0)) && matSize == 2) {
		mat[0] = 1.0f;
		mat[1] = factor;
		mat[3] = 1.0f;
	} else if(((strcmp(plane, "xy") == 0)
		     || (strcmp(plane, "XY") == 0)) && matSize > 2) {
		mat[0] = 1.0f;
		mat[4] = 1.0f;
		mat[6] = factor;
		mat[7] = factor;
	} else if(((strcmp(plane, "xz") == 0)
		     || (strcmp(plane, "XZ") == 0)) && matSize > 2) {
		mat[0] = 1.0f;
		mat[3] = factor;
		mat[4] = 1.0f;
		mat[5] = factor;
		mat[8] = 1.0f;
	} else if(((strcmp(plane, "yz") == 0)
		     || (strcmp(plane, "YZ") == 0)) && matSize > 2) {
		mat[0] = 1.0f;
		mat[1] = factor;
		mat[2] = factor;
		mat[4] = 1.0f;
		mat[8] = 1.0f;
	} else {
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"bpy.types.Matrix.Shear(): expected: x, y, xy, xz, yz or wrong matrix size for shearing plane\n");
	}
	if(matSize == 4) {
		//resize matrix
		mat[10] = mat[8];
		mat[9] = mat[7];
		mat[8] = mat[6];
		mat[7] = 0.0f;
		mat[6] = mat[5];
		mat[5] = mat[4];
		mat[4] = mat[3];
		mat[3] = 0.0f;
	}
	//pass to matrix creation
	return Matrix_CreatePyObject(mat, matSize, matSize, (PyObject *)NULL);
}


/*------------------PY_OBECT DEFINITION--------------------------*/
PyTypeObject BPyMatrix_Type = {
	PyObject_HEAD_INIT(NULL)					/*tp_head*/
	0,											/*tp_internal*/
	"matrix",									/*tp_name*/
	sizeof(BPyMatrixObject),						/*tp_basicsize*/
	0,											/*tp_itemsize*/
	(destructor)Matrix_dealloc,					/*tp_dealloc*/
	0,											/*tp_print*/
	0,											/*tp_getattr*/
	0,											/*tp_setattr*/
	0,											/*tp_compare*/
	(reprfunc) Matrix_repr,						/*tp_repr*/
	&Matrix_NumMethods,							/*tp_as_number*/
	&Matrix_SeqMethods,							/*tp_as_sequence*/
	0,											/*tp_as_mapping*/
	0,											/*tp_hash*/
	0,											/*tp_call*/
	0,											/*tp_str*/
	0,											/*tp_getattro*/
	0,											/*tp_setattro*/
	0,											/*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,	/*tp_flags*/
	BPyMatrix_doc,							/*tp_doc*/
	0,											/*tp_traverse*/
	0,											/*tp_clear*/
	(richcmpfunc)Matrix_richcmpr,				/*tp_richcompare*/
	0,											/*tp_weaklistoffset*/
	0,											/*tp_iter*/
	0,											/*tp_iternext*/
	0,											/*tp_methods*/
	0,											/*tp_members*/
	Matrix_getset,								/*tp_getset*/
	0,											/*tp_base*/
	0,											/*tp_dict*/
	0,											/*tp_descr_get*/
	0,											/*tp_descr_set*/
	0,											/*tp_dictoffset*/
	0,											/*tp_init*/
	0,											/*tp_alloc*/
	( newfunc )Matrix_new,						/*tp_new*/
	0,											/*tp_free*/
	0,											/*tp_is_gc*/
	0,											/*tp_bases*/
	0,											/*tp_mro*/
	0,											/*tp_cache*/
	0,											/*tp_subclasses*/
	0,											/*tp_weaklist*/
	0											/*tp_del*/
};

/*------------------------Matrix_CreatePyObject (internal)-------------
creates a new matrix object
self->matrix     self->contiguous_ptr (reference to data.xxx)
       [0]------------->[0]
                        [1]
                        [2]
       [1]------------->[3]
                        [4]
                        [5]
	             ....
self->matrix[1][1] = self->contiguous_ptr[4] = self->data.xxx_data[4]*/

/*pass Py_WRAP - if vector is a WRAPPER for data allocated by BLENDER
 (i.e. it was allocated elsewhere by MEM_mallocN())
  pass Py_NEW - if vector is not a WRAPPER and managed by PYTHON
 (i.e. it must be created here with PyMEM_malloc())*/
PyObject *Matrix_CreatePyObject(float *mat, int rowSize, int colSize, PyObject * source)
{
	BPyMatrixObject *self;
	int x, row, col;

	/*matrix objects can be any 2-4row x 2-4col matrix*/
	if(rowSize < 2 || rowSize > 4 || colSize < 2 || colSize > 4){
		return EXPP_ReturnPyObjError(PyExc_RuntimeError,
			"matrix(): row and column sizes must be between 2 and 4\n");
	}

	self = PyObject_NEW(BPyMatrixObject, &BPyMatrix_Type);
	
	
	self->rowSize = rowSize;
	self->colSize = colSize;
	self->source = source;
	
	if(source){

		self->matrix[0] = mat;
		
		/*pointer array points to contigous memory*/
		for(x = 1; x < rowSize; x++) {
			self->matrix[x] = self->matrix[0] + (x * colSize);
		}
		Py_INCREF(source);
		
	} else {		
		self->matrix[0] = PyMem_Malloc(rowSize * colSize * sizeof(float));
		if(self->matrix[0] == NULL) { /*allocation failure*/
			return EXPP_ReturnPyObjError( PyExc_MemoryError,
				"matrix(): problem allocating pointer space\n");
		}
		
		/*pointer array points to contigous memory*/
		for(x = 1; x < rowSize; x++) {
			self->matrix[x] = self->matrix[0] + (x * colSize);
		}
		/*parse*/
		if(mat) {	/*if a float array passed*/
			for(row = 0; row < rowSize; row++) {
				for(col = 0; col < colSize; col++) {
					self->matrix[row][col] = mat[(row * colSize) + col];
				}
			}
		} else if (rowSize == colSize ) { /*or if no arguments are passed return identity matrix for square matrices */
			Matrix_Identity(self);
			Py_DECREF(self);
		}
	}
	return (PyObject *) self;
}

int Matrix_CheckPyObject(BPyMatrixObject * self) {
	if (self->source) {
		if (!(((BPyGenericLibObject *)self->source)->id)) {
			return 0;
		} else {
			return 1;
		}
	}
	return 1;
}


PyObject * MatrixType_Init()
{
	if( BPyMatrix_Type.tp_dict == NULL ) {
		BPyMatrix_Type.tp_dict = PyDict_New();
		EXPP_PyMethodsToDict(BPyMatrix_Type.tp_dict, MatrixType_methods);
		PyType_Ready( &BPyMatrix_Type );
	}
	return (PyObject *)&BPyMatrix_Type;
}

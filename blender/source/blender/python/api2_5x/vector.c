/*
 * $Id: vector.c 12802 2007-12-06 00:01:46Z campbellbarton $
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
 * 
 * Contributor(s): Willian P. Germano & Joseph Gilbert, Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "Mathutils.h"

#include "BLI_blenlib.h"
#include "BKE_utildefines.h"
#include "BLI_arithb.h"
#include "gen_utils.h"

/* so we can update the vector pointer */
#include "Object.h"
#include "Mesh.h"
#include "Lattice.h"
#include "DNA_curve_types.h"

/*-------------------------DOC STRINGS ---------------------------*/
char Vector_Zero_doc[] = "() - set all values in the vector to 0";
char Vector_Normalize_doc[] = "() - normalize the vector";
char Vector_Negate_doc[] = "() - changes vector to it's additive inverse";
char Vector_Resize2D_doc[] = "() - resize a vector to [x,y]";
char Vector_Resize3D_doc[] = "() - resize a vector to [x,y,z]";
char Vector_Resize4D_doc[] = "() - resize a vector to [x,y,z,w]";
char Vector_ToTrackQuat_doc[] = "(track, up) - extract a quaternion from the vector and the track and up axis";
char Vector_copy_doc[] = "() - return a copy of the vector";
/*-----------------------METHOD DEFINITIONS ----------------------*/
struct PyMethodDef Vector_methods[] = {
	{"zero", (PyCFunction) Vector_Zero, METH_NOARGS, Vector_Zero_doc},
	{"normalize", (PyCFunction) Vector_Normalize, METH_NOARGS, Vector_Normalize_doc},
	{"negate", (PyCFunction) Vector_Negate, METH_NOARGS, Vector_Negate_doc},
	{"resize2D", (PyCFunction) Vector_Resize2D, METH_NOARGS, Vector_Resize2D_doc},
	{"resize3D", (PyCFunction) Vector_Resize3D, METH_NOARGS, Vector_Resize2D_doc},
	{"resize4D", (PyCFunction) Vector_Resize4D, METH_NOARGS, Vector_Resize2D_doc},
	{"toTrackQuat", ( PyCFunction ) Vector_ToTrackQuat, METH_VARARGS, Vector_ToTrackQuat_doc},
	{"copy", (PyCFunction) Vector_copy, METH_NOARGS, Vector_copy_doc},
	{"__copy__", (PyCFunction) Vector_copy, METH_NOARGS, Vector_copy_doc},
	{NULL, NULL, 0, NULL}
};

/*-----------------------------METHODS----------------------------*/
/*----------------------------Vector.zero() ----------------------
  set the vector data to 0,0,0 */
PyObject *Vector_Zero(BPyVectorObject * self)
{
	int i;
	
	CHECK_VEC_ERROR_PY(self);
	
	for(i = 0; i < self->size; i++) {
		self->vec[i] = 0.0f;
	}
	return EXPP_incr_ret((PyObject*)self);
}
/*----------------------------Vector.normalize() -----------------
  normalize the vector data to a unit vector */
PyObject *Vector_Normalize(BPyVectorObject * self)
{
	int i;
	float norm = 0.0f;

	CHECK_VEC_ERROR_PY(self);
	
	for(i = 0; i < self->size; i++) {
		norm += self->vec[i] * self->vec[i];
	}
	norm = (float) sqrt(norm);
	for(i = 0; i < self->size; i++) {
		self->vec[i] /= norm;
	}
	return EXPP_incr_ret((PyObject*)self);
}

/*---------------------------Vector.negate() --------------------
  set the vector to it's negative -x, -y, -z */
PyObject *Vector_Negate(BPyVectorObject * self)
{
	int i;
	
	CHECK_VEC_ERROR_PY(self);
	for(i = 0; i < self->size; i++) {
		self->vec[i] = -(self->vec[i]);
	}
	/*printf("Vector.negate(): Deprecated: use -vector instead\n");*/
	return EXPP_incr_ret((PyObject*)self);
}

/*----------------------------Vector.resize2D() ------------------
  resize the vector to x,y */
PyObject *Vector_Resize2D(BPyVectorObject * self)
{
	if(self->source) /* wrapped */
		return EXPP_ReturnPyObjError(PyExc_TypeError,
			"vector.resize2d(): cannot resize wrapped data - only python vectors\n");

	self->vec = PyMem_Realloc(self->vec, (sizeof(float) * 2));
	if(self->vec == NULL)
		return EXPP_ReturnPyObjError(PyExc_MemoryError,
			"vector.resize2d(): problem allocating pointer space\n\n");
	
	self->size = 2;
	return EXPP_incr_ret((PyObject*)self);
}
/*----------------------------Vector.resize3D() ------------------
  resize the vector to x,y,z */
PyObject *Vector_Resize3D(BPyVectorObject * self)
{
	if (self->source)
		return EXPP_ReturnPyObjError(PyExc_TypeError,
			"vector.resize3d(): cannot resize wrapped data - only python vectors\n");

	self->vec = PyMem_Realloc(self->vec, (sizeof(float) * 3));
	if(self->vec == NULL)
		return EXPP_ReturnPyObjError(PyExc_MemoryError,
			"vector.resize3d(): problem allocating pointer space\n\n");
	
	if(self->size == 2)
		self->vec[2] = 0.0f;
	
	self->size = 3;
	return EXPP_incr_ret((PyObject*)self);
}
/*----------------------------Vector.resize4D() ------------------
  resize the vector to x,y,z,w */
PyObject *Vector_Resize4D(BPyVectorObject * self)
{
	if(self->source)
		return EXPP_ReturnPyObjError(PyExc_TypeError,
			"vector.resize4d(): cannot resize wrapped data - only python vectors\n");

	self->vec = PyMem_Realloc(self->vec, (sizeof(float) * 4));
	if(self->vec == NULL)
		return EXPP_ReturnPyObjError(PyExc_MemoryError,
			"vector.resize4d(): problem allocating pointer space\n\n");
	
	if(self->size == 2){
		self->vec[2] = 0.0f;
		self->vec[3] = 1.0f;
	}else if(self->size == 3){
		self->vec[3] = 1.0f;
	}
	self->size = 4;
	return EXPP_incr_ret((PyObject*)self);
}
/*----------------------------Vector.toTrackQuat(track, up) ----------------------
  extract a quaternion from the vector and the track and up axis */
PyObject *Vector_ToTrackQuat( BPyVectorObject * self, PyObject * args )
{
	float vec[3];
	char *strack, *sup;
	short track = 2, up = 1;
	
	CHECK_VEC_ERROR_PY(self);
	
	if( !PyArg_ParseTuple ( args, "|ss", &strack, &sup ) ) {
		return EXPP_ReturnPyObjError( PyExc_TypeError, 
			"expected optional two strings\n" );
	}
	if (self->size != 3) {
		return EXPP_ReturnPyObjError( PyExc_TypeError, "only for 3D vectors\n" );
	}

	if (strack) {
		if (strlen(strack) == 2) {
			if (strack[0] == '-') {
				switch(strack[1]) {
					case 'X':
					case 'x':
						track = 3;
						break;
					case 'Y':
					case 'y':
						track = 4;
						break;
					case 'z':
					case 'Z':
						track = 5;
						break;
					default:
						return EXPP_ReturnPyObjError( PyExc_ValueError,
										  "only X, -X, Y, -Y, Z or -Z for track axis\n" );
				}
			}
			else {
				return EXPP_ReturnPyObjError( PyExc_ValueError,
								  "only X, -X, Y, -Y, Z or -Z for track axis\n" );
			}
		}
		else if (strlen(strack) == 1) {
			switch(strack[0]) {
			case '-':
			case 'X':
			case 'x':
				track = 0;
				break;
			case 'Y':
			case 'y':
				track = 1;
				break;
			case 'z':
			case 'Z':
				track = 2;
				break;
			default:
				return EXPP_ReturnPyObjError( PyExc_ValueError,
								  "only X, -X, Y, -Y, Z or -Z for track axis\n" );
			}
		}
		else {
			return EXPP_ReturnPyObjError( PyExc_ValueError,
							  "only X, -X, Y, -Y, Z or -Z for track axis\n" );
		}
	}

	if (sup) {
		if (strlen(sup) == 1) {
			switch(*sup) {
			case 'X':
			case 'x':
				up = 0;
				break;
			case 'Y':
			case 'y':
				up = 1;
				break;
			case 'z':
			case 'Z':
				up = 2;
				break;
			default:
				return EXPP_ReturnPyObjError( PyExc_ValueError,
								  "only X, Y or Z for up axis\n" );
			}
		}
		else {
			return EXPP_ReturnPyObjError( PyExc_ValueError,
							  "only X, Y or Z for up axis\n" );
		}
	}

	if (track == up) {
			return EXPP_ReturnPyObjError( PyExc_ValueError,
						      "Can't have the same axis for track and up\n" );
	}

	/*
		flip vector around, since vectoquat expect a vector from target to tracking object 
		and the python function expects the inverse (a vector to the target).
	*/
	vec[0] = -self->vec[0];
	vec[1] = -self->vec[1];
	vec[2] = -self->vec[2];

	return Quat_CreatePyObject(vectoquat(vec, track, up), (PyObject *)NULL);
}



/*----------------------------Vector.copy() --------------------------------------
  return a copy of the vector */
PyObject *Vector_copy(BPyVectorObject * self)
{
	CHECK_VEC_ERROR_PY(self);
	return Vector_CreatePyObject(self->vec, self->size, (PyObject *)NULL );
}

/*----------------------------dealloc()(internal) ----------------
  free the py_object */
static void Vector_dealloc(BPyVectorObject * self)
{
	/* only free non wrapped */
	if(self->source) {
		Py_DECREF(self->source); /* may be none, thats ok */
	} else {
		PyMem_Free(self->vec);
	}
	PyObject_DEL(self);
}

/*----------------------------print object (internal)-------------
  print the object to screen */
static PyObject *Vector_repr(BPyVectorObject * self)
{
	if (BPyVector_Check(self)) { 
		int i;
		char buffer[48], str[1024];
		BLI_strncpy(str,"[",1024);
		for(i = 0; i < self->size; i++){
			if(i < (self->size - 1)){
				sprintf(buffer, "%.6f, ", self->vec[i]);
				strcat(str,buffer);
			}else{
				sprintf(buffer, "%.6f", self->vec[i]);
				strcat(str,buffer);
			}
		}
		strcat(str, "](vector)");
	
		return PyString_FromString(str);
	} else {
		return PyString_FromString("[<deleted>](vector)");
	}
}
/*---------------------SEQUENCE PROTOCOLS------------------------
  ----------------------------len(object)------------------------
  sequence length*/
static int Vector_len(BPyVectorObject * self)
{
	return self->size;
}
/*----------------------------object[]---------------------------
  sequence accessor (get)*/
static PyObject *Vector_item(BPyVectorObject * self, int i)
{
	CHECK_VEC_ERROR_PY(self);
	
	if(i < 0 || i >= self->size)
		return EXPP_ReturnPyObjError(PyExc_IndexError,
		"vector[index]: out of range\n");

	return PyFloat_FromDouble(self->vec[i]);

}
/*----------------------------object[]-------------------------
  sequence accessor (set)*/
static int Vector_ass_item(BPyVectorObject * self, int i, PyObject * ob)
{
	float f = ( float )PyFloat_AsDouble( ob );
	if (f==-1 && PyErr_Occurred())
		return EXPP_ReturnIntError(PyExc_TypeError, 
			"vector[index] = x: index argument not a number\n");
	
	CHECK_VEC_ERROR_INT(self);
	
	if(i < 0 || i >= self->size){
		return EXPP_ReturnIntError(PyExc_IndexError,
			"vector[index] = x: assignment index out of range\n");
	}
	self->vec[i] = f;
	return 0;
}

/*----------------------------object[z:y]------------------------
  sequence slice (get) */
static PyObject *Vector_slice(BPyVectorObject * self, int begin, int end)
{
	PyObject *list = NULL;
	int count;
	
	CHECK_VEC_ERROR_PY(self);

	CLAMP(begin, 0, self->size);
	if (end<0) end= self->size+end+1;
	CLAMP(end, 0, self->size);
	begin = MIN2(begin,end);

	list = PyList_New(end - begin);
	for(count = begin; count < end; count++) {
		PyList_SetItem(list, count - begin,
				PyFloat_FromDouble(self->vec[count]));
	}

	return list;
}
/*----------------------------object[z:y]------------------------
  sequence slice (set) */
static int Vector_ass_slice(BPyVectorObject * self, int begin, int end,
			     PyObject * seq)
{
	int i, y, size = 0;
	float vec[4];
	PyObject *v;

	CHECK_VEC_ERROR_INT(self);
	
	CLAMP(begin, 0, self->size);
	if (end<0) end= self->size+end+1;
	CLAMP(end, 0, self->size);
	begin = MIN2(begin,end);

	size = PySequence_Length(seq);
	if(size != (end - begin)){
		return EXPP_ReturnIntError(PyExc_TypeError,
			"vector[begin:end] = []: size mismatch in slice assignment\n");
	}

	for (i = 0; i < size; i++) {
		v = PySequence_GetItem(seq, i);
		if (v == NULL) /* Failed to read sequence */
			return EXPP_ReturnIntError(PyExc_RuntimeError, 
				"vector[begin:end] = []: unable to read sequence\n");
		
		vec[i] = ( float )PyFloat_AsDouble( v ); /* catch error below */
		Py_DECREF(v);
	}
	
	if (PyErr_Occurred())
		return EXPP_ReturnIntError(PyExc_TypeError,
				"vector[begin:end] = []: sequence argument not a number\n");

	/*parsed well - now set in vector*/
	for(y = 0; y < size; y++){
		self->vec[begin + y] = vec[y];
	}
	return 0;
}
/*------------------------NUMERIC PROTOCOLS----------------------
  ------------------------obj + obj------------------------------
  addition*/
static PyObject *Vector_add(BPyVectorObject * vec1, BPyVectorObject * vec2)
{
	int i;
	float vec[4];

	if (!BPyVector_Check(vec1) || !BPyVector_Check(vec2))
		return EXPP_ReturnPyObjError(PyExc_ValueError,
				"Vector addition: arguments not valid for this operation....\n");
	
	if(vec1->size != vec2->size)
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
		"Vector addition: vectors must have the same dimensions for this operation\n");
	
	for(i = 0; i < vec1->size; i++) {
		vec[i] = vec1->vec[i] +	vec2->vec[i];
	}
	return Vector_CreatePyObject(vec, vec1->size, (PyObject *)NULL);
}

/*  ------------------------obj += obj------------------------------
  addition in place */
static PyObject *Vector_iadd(BPyVectorObject * vec1, BPyVectorObject * vec2)
{
	int i;
	
	if (!BPyVector_Check(vec1) || !BPyVector_Check(vec2)) {
		return EXPP_ReturnPyObjError(PyExc_ValueError,
				"Vector addition: arguments not valid for this operation....\n");		
	}
	
	if(vec1->size != vec2->size) {
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
				"Vector addition: vectors must have the same dimensions for this operation\n");
	}

	for(i = 0; i < vec1->size; i++) {
		vec1->vec[i] +=	vec2->vec[i];
	}
	Py_INCREF( (PyObject *)vec1 );
	return (PyObject *)vec1;
}

/*------------------------obj - obj------------------------------
  subtraction*/
static PyObject *Vector_sub(BPyVectorObject * vec1, BPyVectorObject * vec2)
{
	int i;
	float vec[4];

	if (!BPyVector_Check(vec1) || !BPyVector_Check(vec2))
		return EXPP_ReturnPyObjError(PyExc_ValueError,
			"Vector subtraction: arguments not valid for this operation....\n");
	
	CHECK_VEC_ERROR_PY(vec1);
	CHECK_VEC_ERROR_PY(vec2);
	
	if(vec1->size != vec2->size)
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
		"Vector subtraction: vectors must have the same dimensions for this operation\n");
	
	for(i = 0; i < vec1->size; i++) {
		vec[i] = vec1->vec[i] -	vec2->vec[i];
	}

	return Vector_CreatePyObject(vec, vec1->size, (PyObject *)NULL);
}

/*------------------------obj -= obj------------------------------
  subtraction*/
static PyObject *Vector_isub(BPyVectorObject * vec1, BPyVectorObject * vec2)
{
	int i, size;

	if (!BPyVector_Check(vec1) || !BPyVector_Check(vec2))
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Vector subtraction: arguments not valid for this operation....\n");

	if(vec1->size != vec2->size)
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
		"Vector subtraction: vectors must have the same dimensions for this operation\n");

	CHECK_VEC_ERROR_PY(vec1);
	CHECK_VEC_ERROR_PY(vec2);
	
	size = vec1->size;
	for(i = 0; i < vec1->size; i++) {
		vec1->vec[i] = vec1->vec[i] -	vec2->vec[i];
	}

	Py_INCREF( (PyObject *)vec1 );
	return (PyObject *)vec1;
}

/*------------------------obj * obj------------------------------
  mulplication*/
static PyObject *Vector_mul(PyObject * v1, PyObject * v2)
{
	BPyVectorObject *vec1 = NULL, *vec2 = NULL;
	
	if BPyVector_Check(v1) {
		vec1= (BPyVectorObject *)v1;
		CHECK_VEC_ERROR_PY(vec1);
	}
	
	if BPyVector_Check(v2) {
		vec2= (BPyVectorObject *)v2;
		CHECK_VEC_ERROR_PY(vec2);
	}
	
	/* make sure v1 is always the vector */
	if (vec1 && vec2 ) {
		int i;
		double dot = 0.0f;
		
		if(vec1->size != vec2->size)
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Vector multiplication: vectors must have the same dimensions for this operation\n");
		
		/*dot product*/
		for(i = 0; i < vec1->size; i++) {
			dot += vec1->vec[i] * vec2->vec[i];
		}
		return PyFloat_FromDouble(dot);
	}
	
	/*swap so vec1 is always the vector */
	if (vec2) {
		vec1= vec2;
		v2= v1;
	}
	
	if (PyNumber_Check(v2)) {
		/* VEC * NUM */
		int i;
		float vec[4];
		float scalar = (float)PyFloat_AsDouble( v2 );
		
		for(i = 0; i < vec1->size; i++) {
			vec[i] = vec1->vec[i] *	scalar;
		}
		return Vector_CreatePyObject(vec, vec1->size, (PyObject *)NULL);
		
	} else if (BPyMatrix_Check(v2)) {
		CHECK_MAT_ERROR_PY((BPyMatrixObject*)v2);
		
		/* for matrix*vec see matrix.c */
		
		/* VEC * MATRIX */
		//if (v1==v2) { /* mat*vec, we have swapped the order */
		//	printf("T2\n");
		//	return column_vector_multiplication((BPyMatrixObject*)v2, vec1);
		//} else { /* vec*mat */
			return row_vector_multiplication(vec1, (BPyMatrixObject*)v2);
		//}
	} else if (BPyQuat_Check(v2)) {
		BPyQuatObject *quat = (BPyQuatObject *)v2;
		if(vec1->size != 3)
			return EXPP_ReturnPyObjError(PyExc_TypeError, 
				"Vector multiplication: only 3D vector rotations (with quats) currently supported\n");
		
		return quat_rotation((PyObject*)vec1, (PyObject*)quat);
	}
	
	return EXPP_ReturnPyObjError(PyExc_TypeError,
		"Vector multiplication: arguments not acceptable for this operation\n");
}

/*------------------------obj *= obj------------------------------
  in place mulplication */
static PyObject *Vector_imul(PyObject * v1, PyObject * v2)
{
	BPyVectorObject *vec = (BPyVectorObject *)v1;
	int i;
	
	CHECK_VEC_ERROR_PY(vec);
	
	/* only support vec*=float and vec*=mat
	   vec*=vec result is a float so that wont work */
	if (PyNumber_Check(v2)) {
		/* VEC * NUM */
		float scalar = (float)PyFloat_AsDouble( v2 );
		
		for(i = 0; i < vec->size; i++) {
			vec->vec[i] *=	scalar;
		}
		
		Py_INCREF( v1 );
		return v1;
		
	} else if (BPyMatrix_Check(v2)) {
		float vecCopy[4];
		int x,y, size = vec->size;
		BPyMatrixObject *mat= (BPyMatrixObject*)v2;
		
		CHECK_MAT_ERROR_PY((BPyMatrixObject*)v2);
		
		if(mat->colSize != size){
			if(mat->rowSize == 4 && vec->size != 3){
				return EXPP_ReturnPyObjError(PyExc_AttributeError, 
					"vector * matrix: matrix column size and the vector size must be the same");
			} else {
				vecCopy[3] = 1.0f;
			}
		}
		
		for(i = 0; i < size; i++){
			vecCopy[i] = vec->vec[i];
		}
		
		size = MIN2(size, mat->colSize);
		
		/*muliplication*/
		for(x = 0, i = 0; x < size; x++, i++) {
			double dot = 0.0f;
			for(y = 0; y < mat->rowSize; y++) {
				dot += mat->matrix[y][x] * vecCopy[y];
			}
			vec->vec[i] = (float)dot;
		}
		Py_INCREF( v1 );
		return v1;
	}
	return EXPP_ReturnPyObjError(PyExc_TypeError,
		"Vector multiplication: arguments not acceptable for this operation\n");
}

/*------------------------obj / obj------------------------------
  divide*/
static PyObject *Vector_div(BPyVectorObject * v1, PyObject * v2)
{
	int i, size;
	float vec[4], scalar = ( float )PyFloat_AsDouble( v2 );
	BPyVectorObject *vec1 = NULL;
	
	if(!BPyVector_Check(v1) || (scalar==-1 && PyErr_Occurred())) /* not a vector */
		return EXPP_ReturnPyObjError(PyExc_TypeError, 
			"Vector division: Vector must be divided by a float\n");
	
	if(scalar==0.0) /* not a vector */
		return EXPP_ReturnPyObjError(PyExc_ZeroDivisionError, 
			"Vector division: divide by zero error.\n");
	
	CHECK_VEC_ERROR_PY(v1);
	
	size = vec1->size;
	for(i = 0; i < size; i++) {
		vec[i] = vec1->vec[i] /	scalar;
	}
	return Vector_CreatePyObject(vec, size, (PyObject *)NULL);
}

/*------------------------obj / obj------------------------------
  divide*/
static PyObject *Vector_idiv(BPyVectorObject * v1, PyObject * v2)
{
	int i, size;
	float scalar = ( float )PyFloat_AsDouble( v2 );
	
	/*if(!BPyVector_Check(v1))
		return EXPP_ReturnIntError(PyExc_TypeError, 
			"Vector division: Vector must be divided by a float\n");*/
	
	CHECK_VEC_ERROR_PY(v1);
	
	if(scalar==-1 && PyErr_Occurred()) /* parsed item not a number */
		return EXPP_ReturnPyObjError(PyExc_TypeError, 
			"Vector division: Vector must be divided by a float\n");
	
	if(scalar==0.0) /* not a vector */
		return EXPP_ReturnPyObjError(PyExc_ZeroDivisionError, 
			"Vector division: divide by zero error.\n");
	
	size = v1->size;
	for(i = 0; i < size; i++) {
		v1->vec[i] /=	scalar;
	}
	Py_INCREF( (PyObject *)v1 );
	return (PyObject *)v1;
}

/*-------------------------- -obj -------------------------------
  returns the negative of this object*/
static PyObject *Vector_neg(BPyVectorObject *self)
{
	int i;
	float vec[4];
	
	CHECK_VEC_ERROR_PY(self);
	
	for(i = 0; i < self->size; i++){
		vec[i] = -self->vec[i];
	}
	return Vector_CreatePyObject(vec, self->size, (PyObject *)NULL);
}

/*------------------------tp_doc*/
static char BPyVector_doc[] = "This is a wrapper for vector objects.";
/*------------------------vec_magnitude_nosqrt (internal) - for comparing only */
static double vec_magnitude_nosqrt(float *data, int size)
{
	double dot = 0.0f;
	int i;

	for(i=0; i<size; i++){
		dot += data[i];
	}
	/*return (double)sqrt(dot);*/
	/* warning, line above removed because we are not using the length,
	   rather the comparing the sizes and for this we do not need the sqrt
	   for the actual length, the dot must be sqrt'd */
	return (double)dot;
}


/*------------------------tp_richcmpr
  returns -1 execption, 0 false, 1 true */
PyObject* Vector_richcmpr(BPyVectorObject *vecA, BPyVectorObject *vecB, int comparison_type)
{
	int result = 0;
	float epsilon = .000001f;
	double lenA,lenB;

	if (!BPyVector_Check(vecA) || !BPyVector_Check(vecB)){
		if (comparison_type == Py_NE){
			Py_RETURN_TRUE;
		}else{
			Py_RETURN_FALSE;
		}
	}

	CHECK_VEC_ERROR_PY(vecA);
	CHECK_VEC_ERROR_PY(vecB);
	
	if (vecA->size != vecB->size){
		if (comparison_type == Py_NE){
			Py_RETURN_TRUE;
		}else{
			Py_RETURN_FALSE;
		}
	}

	switch (comparison_type){
		case Py_LT:
			lenA = vec_magnitude_nosqrt(vecA->vec, vecA->size);
			lenB = vec_magnitude_nosqrt(vecB->vec, vecB->size);
			if( lenA < lenB ){
				result = 1;
			}
			break;
		case Py_LE:
			lenA = vec_magnitude_nosqrt(vecA->vec, vecA->size);
			lenB = vec_magnitude_nosqrt(vecB->vec, vecB->size);
			if( lenA < lenB ){
				result = 1;
			}else{
				result = (((lenA + epsilon) > lenB) && ((lenA - epsilon) < lenB));
			}
			break;
		case Py_EQ:
			result = EXPP_VectorsAreEqual(vecA->vec, vecB->vec, vecA->size, 1);
			break;
		case Py_NE:
			result = EXPP_VectorsAreEqual(vecA->vec, vecB->vec, vecA->size, 1);
			if (result == 0){
				result = 1;
			}else{
				result = 0;
			}
			break;
		case Py_GT:
			lenA = vec_magnitude_nosqrt(vecA->vec, vecA->size);
			lenB = vec_magnitude_nosqrt(vecB->vec, vecB->size);
			if( lenA > lenB ){
				result = 1;
			}
			break;
		case Py_GE:
			lenA = vec_magnitude_nosqrt(vecA->vec, vecA->size);
			lenB = vec_magnitude_nosqrt(vecB->vec, vecB->size);
			if( lenA > lenB ){
				result = 1;
			}else{
				result = (((lenA + epsilon) > lenB) && ((lenA - epsilon) < lenB));
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
/*-----------------PROTCOL DECLARATIONS--------------------------*/
static PySequenceMethods Vector_SeqMethods = {
	(inquiry) Vector_len,						/* sq_length */
	(binaryfunc) 0,								/* sq_concat */
	(intargfunc) 0,								/* sq_repeat */
	(intargfunc) Vector_item,					/* sq_item */
	(intintargfunc) Vector_slice,				/* sq_slice */
	(intobjargproc) Vector_ass_item,			/* sq_ass_item */
	(intintobjargproc) Vector_ass_slice,		/* sq_ass_slice */
};


/* For numbers without flag bit Py_TPFLAGS_CHECKTYPES set, all
   arguments are guaranteed to be of the object's type (modulo
   coercion hacks -- i.e. if the type's coercion function
   returns other types, then these are allowed as well).  Numbers that
   have the Py_TPFLAGS_CHECKTYPES flag bit set should check *both*
   arguments for proper type and implement the necessary conversions
   in the slot functions themselves. */
 
static PyNumberMethods Vector_NumMethods = {
	(binaryfunc) Vector_add,					/* __add__ */
	(binaryfunc) Vector_sub,					/* __sub__ */
	(binaryfunc) Vector_mul,					/* __mul__ */
	(binaryfunc) Vector_div,					/* __div__ */
	(binaryfunc) NULL,							/* __mod__ */
	(binaryfunc) NULL,							/* __divmod__ */
	(ternaryfunc) NULL,							/* __pow__ */
	(unaryfunc) Vector_neg,						/* __neg__ */
	(unaryfunc) NULL,							/* __pos__ */
	(unaryfunc) NULL,							/* __abs__ */
	(inquiry) NULL,								/* __nonzero__ */
	(unaryfunc) NULL,							/* __invert__ */
	(binaryfunc) NULL,							/* __lshift__ */
	(binaryfunc) NULL,							/* __rshift__ */
	(binaryfunc) NULL,							/* __and__ */
	(binaryfunc) NULL,							/* __xor__ */
	(binaryfunc) NULL,							/* __or__ */
	(coercion)  NULL,							/* __coerce__ */
	(unaryfunc) NULL,							/* __int__ */
	(unaryfunc) NULL,							/* __long__ */
	(unaryfunc) NULL,							/* __float__ */
	(unaryfunc) NULL,							/* __oct__ */
	(unaryfunc) NULL,							/* __hex__ */
	
	/* Added in release 2.0 */
	(binaryfunc) Vector_iadd,					/*__iadd__*/
	(binaryfunc) Vector_isub,					/*__isub__*/
	(binaryfunc) Vector_imul,					/*__imul__*/
	(binaryfunc) Vector_idiv,					/*__idiv__*/
	(binaryfunc) NULL,							/*__imod__*/
	(ternaryfunc) NULL,							/*__ipow__*/
	(binaryfunc) NULL,							/*__ilshift__*/
	(binaryfunc) NULL,							/*__irshift__*/
	(binaryfunc) NULL,							/*__iand__*/
	(binaryfunc) NULL,							/*__ixor__*/
	(binaryfunc) NULL,							/*__ior__*/
 
	/* Added in release 2.2 */
	/* The following require the Py_TPFLAGS_HAVE_CLASS flag */
	(binaryfunc) NULL,							/*__floordiv__  __rfloordiv__*/
	(binaryfunc) NULL,							/*__truediv__ __rfloordiv__*/
	(binaryfunc) NULL,							/*__ifloordiv__*/
	(binaryfunc) NULL,							/*__itruediv__*/
};
/*------------------PY_OBECT DEFINITION--------------------------*/

/*
 * vector axis, vector.x/y/z/w
 */
	
static PyObject *Vector_getAxis( BPyVectorObject * self, void *type )
{
	
	CHECK_VEC_ERROR_PY(self);
	
	switch( (long)type ) {
    case 'X':	/* these are backwards, but that how it works */
		return PyFloat_FromDouble(self->vec[0]);
    case 'Y':
		return PyFloat_FromDouble(self->vec[1]);
    case 'Z':	/* these are backwards, but that how it works */
		if(self->size < 3)
			return EXPP_ReturnPyObjError(PyExc_AttributeError,
				"vector.z: error, cannot get this axis for a 2D vector\n");
		else
			return PyFloat_FromDouble(self->vec[2]);
    case 'W':
		if(self->size < 4)
			return EXPP_ReturnPyObjError(PyExc_AttributeError,
				"vector.w: error, cannot get this axis for a 3D vector\n");
	
		return PyFloat_FromDouble(self->vec[3]);
	default:
		{
			char errstr[1024];
			sprintf( errstr, "undefined type '%d' in Vector_getAxis",
					(int)((long)type & 0xff));
			return EXPP_ReturnPyObjError( PyExc_RuntimeError, errstr );
		}
	}
}

static int Vector_setAxis( BPyVectorObject * self, PyObject * value, void * type )
{
	float param = ( float )PyFloat_AsDouble( value );
	
	CHECK_VEC_ERROR_INT(self);
	if (param==-1 && PyErr_Occurred())
		return EXPP_ReturnIntError( PyExc_TypeError,
			"expected a number for the vector axis" );
	
	switch( (long)type ) {
    case 'X':	/* these are backwards, but that how it works */
		self->vec[0]= param;
		break;
    case 'Y':
		self->vec[1]= param;
		break;
    case 'Z':	/* these are backwards, but that how it works */
		if(self->size < 3)
			return EXPP_ReturnIntError(PyExc_AttributeError,
				"vector.z: error, cannot get this axis for a 2D vector\n");
		self->vec[2]= param;
		break;
    case 'W':
		if(self->size < 4)
			return EXPP_ReturnIntError(PyExc_AttributeError,
				"vector.w: error, cannot get this axis for a 3D vector\n");
	
		self->vec[3]= param;
		break;
	default:
		{
			char errstr[1024];
			sprintf( errstr, "undefined type '%d' in Vector_setAxis",
					(int)((long)type & 0xff));
			return EXPP_ReturnIntError( PyExc_RuntimeError, errstr );
		}
	}

	return 0;
}

/* vector.length */
static PyObject *Vector_getLength( BPyVectorObject * self, void *type )
{
	double dot = 0.0f;
	int i;
	
	CHECK_VEC_ERROR_PY(self);
	
	for(i = 0; i < self->size; i++){
		dot += (self->vec[i] * self->vec[i]);
	}
	return PyFloat_FromDouble(sqrt(dot));
}

static int Vector_setLength( BPyVectorObject * self, PyObject * value )
{
	double dot = 0.0f, param = ( float )PyFloat_AsDouble( value );
	int i;
	
	if (param==-1 && PyErr_Occurred())
		return EXPP_ReturnIntError( PyExc_TypeError,
			"expected a number for the vector axis" );
	
	CHECK_VEC_ERROR_INT(self);
	
	if (param < 0)
		return EXPP_ReturnIntError( PyExc_TypeError,
			"cannot set a vectors length to a negative value" );
	
	if (param==0.0f) {
		for(i = 0; i < self->size; i++){
			self->vec[i]= 0;
		}
		return 0;
	}
	
	for(i = 0; i < self->size; i++){
		dot += (self->vec[i] * self->vec[i]);
	}

	if (!dot) /* cant sqrt zero */
		return 0;
	
	dot = sqrt(dot);
	
	if (dot==param)
		return 0;
	
	dot= dot/param;
	
	for(i = 0; i < self->size; i++){
		self->vec[i]= self->vec[i] / (float)dot;
	}
	
	return 0;
}

static PyObject *Vector_getWrapped( BPyVectorObject * self, void *type )
{
	if (self->source)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}


/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef Vector_getset[] = {
	{"x",
	 (getter)Vector_getAxis, (setter)Vector_setAxis,
	 "Vector X axis",
	 (void *)'X'},
	{"y",
	 (getter)Vector_getAxis, (setter)Vector_setAxis,
	 "Vector Y axis",
	 (void *)'Y'},
	{"z",
	 (getter)Vector_getAxis, (setter)Vector_setAxis,
	 "Vector Z axis",
	 (void *)'Z'},
	{"w",
	 (getter)Vector_getAxis, (setter)Vector_setAxis,
	 "Vector Z axis",
	 (void *)'W'},
	{"length",
	 (getter)Vector_getLength, (setter)Vector_setLength,
	 "Vector Length",
	 NULL},
	{"magnitude",
	 (getter)Vector_getLength, (setter)Vector_setLength,
	 "Vector Length",
	 NULL},
	{"wrapped",
	 (getter)Vector_getWrapped, (setter)NULL,
	 "Vector Length",
	 NULL},
	{NULL}  /* Sentinel */
};


/* this types constructor - apply any fixes to vec, eul, quat and col,  */
static PyObject *Vector_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	float fvec[4];
	switch (PyTuple_Size(args)) {
	case 0:
		return Vector_CreatePyObject(NULL, 3, (PyObject *)NULL);
	case 1: /* single argument */
		args = PyTuple_GET_ITEM(args, 0);
		if (EXPP_setVec3(args, &fvec[0], &fvec[1], &fvec[2])!=-1) {
			return Vector_CreatePyObject(fvec, 3, (PyObject *)NULL);
		} else if (EXPP_setVec4(args, &fvec[0], &fvec[1], &fvec[2], &fvec[3])!=-1) {
			PyErr_Clear(); /* from the failed EXPP_setVec3 */
			return Vector_CreatePyObject(fvec, 4, (PyObject *)NULL);
		} else {
			return NULL;
		}
	case 3:
		if (EXPP_setVec3(args, &fvec[0], &fvec[1], &fvec[2])==-1)
			return NULL;
		return Vector_CreatePyObject(fvec, 3, (PyObject *)NULL);
	case 4:
		if (EXPP_setVec4(args, &fvec[0], &fvec[1], &fvec[2], &fvec[3])==-1)
			return NULL;
		return Vector_CreatePyObject(fvec, 4, (PyObject *)NULL);
	}
	
	return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, expected 0, 1, 3 or 4 args for the new vector." );
}

/* Note
 Py_TPFLAGS_CHECKTYPES allows us to avoid casting all types to Vector when coercing
 but this means for eg that 
 vec*mat and mat*vec both get sent to Vector_mul and it neesd to sort out the order
*/

PyTypeObject BPyVector_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Vector",             /* char *tp_name; */
	sizeof( BPyVectorObject ),         /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) Vector_dealloc,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,   /* cmpfunc tp_compare; */
	( reprfunc ) Vector_repr,     /* reprfunc tp_repr; */

	/* Method suites for standard classes */

	&Vector_NumMethods,                       /* PyNumberMethods *tp_as_number; */
	&Vector_SeqMethods,                       /* PySequenceMethods *tp_as_sequence; */
	NULL,                       /* PyMappingMethods *tp_as_mapping; */

	/* More standard operations (here for binary compatibility) */

	NULL,                       /* hashfunc tp_hash; */
	NULL,                       /* ternaryfunc tp_call; */
	NULL,                       /* reprfunc tp_str; */
	NULL,                       /* getattrofunc tp_getattro; */
	NULL,                       /* setattrofunc tp_setattro; */

	/* Functions to access object as input/output buffer */
	NULL,                       /* PyBufferProcs *tp_as_buffer; */

  /*** Flags to define presence of optional/expanded features ***/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,         /* long tp_flags; */

	BPyVector_doc,                       /*  char *tp_doc;  Documentation string */
  /*** Assigned meaning in release 2.0 ***/
	/* call function for all accessible objects */
	NULL,                       /* traverseproc tp_traverse; */

	/* delete references to contained objects */
	NULL,                       /* inquiry tp_clear; */

  /***  Assigned meaning in release 2.1 ***/
  /*** rich comparisons ***/
	(richcmpfunc)Vector_richcmpr,                       /* richcmpfunc tp_richcompare; */

  /***  weak reference enabler ***/
	0,                          /* long tp_weaklistoffset; */

  /*** Added in release 2.2 ***/
	/*   Iterators */
	NULL,                       /* getiterfunc tp_iter; */
	NULL,                       /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	Vector_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	Vector_getset,         /* struct PyGetSetDef *tp_getset; */
	NULL,                       /* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc )Vector_new,		/* newfunc tp_new; */
	/*  Low-level free-memory routine */
	NULL,                       /* freefunc tp_free;  */
	/* For PyObject_IS_GC */
	NULL,                       /* inquiry tp_is_gc;  */
	NULL,                       /* PyObject *tp_bases; */
	/* method resolution order */
	NULL,                       /* PyObject *tp_mro;  */
	NULL,                       /* PyObject *tp_cache; */
	NULL,                       /* PyObject *tp_subclasses; */
	NULL,                       /* PyObject *tp_weaklist; */
	NULL
};


/*------------------------Vector_CreatePyObject (internal)-------------
  creates a new vector object
  if source is NULL, this is not a wrapped.
  Py_None - wrapped but its pointer wont be updated  
  Matrix - an item of a pointer, use vec->sub_index to set the row
  Vertex - location
  Face - UV, vec->sub_index for the point in the face (0-3)
  PVert - ignored, like None
  
  checkBPyVectorObject to see how different data is updated from its source.
  */
PyObject *Vector_CreatePyObject(float *vec, int size, PyObject * source)
{
	int i;
	BPyVectorObject *self = PyObject_NEW(BPyVectorObject, &BPyVector_Type);
	
	self->size = size; /* MUST be between 2 and 4 */
	self->source = source;
	
	if(source) {
		self->vec = vec;
		Py_INCREF(source);
	} else { /* New */
		self->vec = PyMem_Malloc(size * sizeof(float));
		if(!vec) { /*new empty*/
			for(i = 0; i < size; i++){
				self->vec[i] = 0.0f;
			}
			if(size == 4)  /* do the homogenous thing */
				self->vec[3] = 1.0f;
		}else{
			for(i = 0; i < size; i++){
				self->vec[i] = vec[i];
			}
		}
	}
	return (PyObject *) self;
}

int Vector_CheckPyObject(BPyVectorObject * self) {
	/* A vert can PVert can also */
	if (self->source) {
		if (self->source == Py_None) {
			/* wrapped but dont lookup the pointer */
			return 1;
		} else if (BPyObject_Check(self->source)) {
			/* Object, sub_index for the type of object vector
			 * 
			 * WARNING - for now we know the pointer wont move but this might not always be true
			 * */
			/*
			switch(self->sub_type) {
			case VEC_SUBTYPE_OB_LOC:
				break;
			case VEC_SUBTYPE_OB_DLOC:
				break;
			case VEC_SUBTYPE_OB_SCALE:
				break;
			case VEC_SUBTYPE_OB_DSCALE:
				break;
			}
			*/
			
		} else if (BPyMatrix_Check(self->source)) {
			if (!BPyMatrix_Check((BPyMatrixObject *)self->source)) {
				return 0;
			} else {
				BPyMatrixObject *mat = (BPyMatrixObject *)self->source;
				
				/* the matrix was resized to remove this vector slice */
				if (mat->rowSize < self->sub_index)
					return 0;
				
				self->size = mat->colSize;
				self->vec = mat->matrix[self->sub_index];
				return 1;
			}
		} else if (BPyMVert_Check(self->source)) {
			/* make sure the MVert is valid and update this pointer
			 * 
			 * The way things work at the moment,
			 * - the vector is a user of the vertex,
			 * - the vertex is a user of the bpymesh
			 * - the bpymesh will keep the mesh until its deallocated. but this wont happen because we exist here ;) 
			 * 
			 * So since we exist, we dont need to worry about the mesh not existing 
			 * (((MVert *)self->genlib)->bpymesh->id == NULL)
			 * 
			 * But we do need to check the vertex index is valid and the pointer is up to date.
			 */
			BPyMVertObject *bpymvert = (BPyMVertObject *)self->source;
			
			/* stolen from Mesh.c - MVert_get_pointer */
			
			/* vert has been removed */
			if (bpymvert->index >= ((Mesh *)(bpymvert->bpymesh->mesh))->totvert)
				return 0;
			
			/* update pointer */
			/* the mvert is
			 * &(bpymvert->bpymesh->mesh)->mvert[bpymvert->index] */
			self->vec = (&(bpymvert->bpymesh->mesh)->mvert[bpymvert->index])->co;
			return 1;
		} else if (BPyMFace_Check(self->source)) {
			/* This is a faces UV
			 * 
			 * Warning- updates teh vector with the current UV layer. not much we can do about that
			 * */

			BPyMFaceObject *bpymface = (BPyMFaceObject *)self->source;
			
			
			if(	/* no UV's anymore?? */
				(!((Mesh *)(bpymface->bpymesh->mesh))->mtface) ||
				
				/* faces has been removed */
				(bpymface->index >= ((Mesh *)(bpymface->bpymesh->mesh))->totface) ||
				
				/* was a quad, now is a tri? */
				(self->sub_index == 3 && bpymface->bpymesh->mesh->mface[bpymface->index].v4==0)
			) {
				return 0;
			}
			
			self->vec= (&bpymface->bpymesh->mesh->mtface[bpymface->index])->uv[self->sub_index];
			return 1;
			
		} else if (BPyPVert_Check(self->source)) {
			/* do nothing since a PVert manages its own MVert structure
			 * just important we dont use the BPyPVert as a BPyGenlib
			 * 
			 * also important the PVert is not dealloc'd before the
			 * vector is. removeing the data it points to */
			return 1;
			
		} else if (BPyLPoint_Check(self->source)) {
			/* stolen from Lattice.c - LPoint_get_pointer */
			BPyLPointObject *bpylpoint =	(BPyLPointObject *)self->source;
			Lattice *lattice =		(Lattice *)bpylpoint->bpylat->lattice;
			
			/* point has been removed */
			if (bpylpoint->index >= LATTICE_TOTPOINTS(lattice))
				return 0;
			
			/* update pointer */
			self->vec = (&(lattice)->def[bpylpoint->index])->vec;
			return 1;
		} else {
			if (!((BPyGenericLibObject *)self->source)->id) {
				return 0;
			} else {
				return 1;
			}
		}
	}
	return 1;
}

/* make this vector use its own memory */
void unwrapVector(BPyVectorObject * self)
{
	float *vec;
	int i;
	if (!self->source)
		return;
	
	vec = PyMem_Malloc(self->size * sizeof(float));
	
	for(i = 0; i < self->size; i++) {
		vec[i] = self->vec[i];
	}
	
	self->vec = vec;
	
	/* Not a genlib user anymore */
	if (self->source) {
		Py_DECREF(self->source);
		self->source = NULL;
	}
}

PyObject * VectorType_Init()
{
	PyType_Ready( &BPyVector_Type );
	return (PyObject *)&BPyVector_Type;
}

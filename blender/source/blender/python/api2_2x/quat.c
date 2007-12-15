/*
 * $Id$
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
 * 
 * Contributor(s): Joseph Gilbert
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "Mathutils.h"

#include "BLI_arithb.h"
#include "BKE_utildefines.h"
#include "BLI_blenlib.h"
#include "gen_utils.h"


//-------------------------DOC STRINGS ---------------------------
char V24_Quaternion_Identity_doc[] = "() - set the quaternion to it's identity (1, vector)";
char V24_Quaternion_Negate_doc[] = "() - set all values in the quaternion to their negative";
char V24_Quaternion_Conjugate_doc[] = "() - set the quaternion to it's conjugate";
char V24_Quaternion_Inverse_doc[] = "() - set the quaternion to it's inverse";
char V24_Quaternion_Normalize_doc[] = "() - normalize the vector portion of the quaternion";
char V24_Quaternion_ToEuler_doc[] = "() - return a euler rotation representing the quaternion";
char V24_Quaternion_ToMatrix_doc[] = "() - return a rotation matrix representing the quaternion";
char V24_Quaternion_copy_doc[] = "() - return a copy of the quat";
//-----------------------METHOD DEFINITIONS ----------------------
struct PyMethodDef Quaternion_methods[] = {
	{"identity", (PyCFunction) V24_Quaternion_Identity, METH_NOARGS, V24_Quaternion_Identity_doc},
	{"negate", (PyCFunction) V24_Quaternion_Negate, METH_NOARGS, V24_Quaternion_Negate_doc},
	{"conjugate", (PyCFunction) V24_Quaternion_Conjugate, METH_NOARGS, V24_Quaternion_Conjugate_doc},
	{"inverse", (PyCFunction) V24_Quaternion_Inverse, METH_NOARGS, V24_Quaternion_Inverse_doc},
	{"normalize", (PyCFunction) V24_Quaternion_Normalize, METH_NOARGS, V24_Quaternion_Normalize_doc},
	{"toEuler", (PyCFunction) V24_Quaternion_ToEuler, METH_NOARGS, V24_Quaternion_ToEuler_doc},
	{"toMatrix", (PyCFunction) V24_Quaternion_ToMatrix, METH_NOARGS, V24_Quaternion_ToMatrix_doc},
	{"__copy__", (PyCFunction) V24_Quaternion_copy, METH_NOARGS, V24_Quaternion_copy_doc},
	{"copy", (PyCFunction) V24_Quaternion_copy, METH_NOARGS, V24_Quaternion_copy_doc},
	{NULL, NULL, 0, NULL}
};
//-----------------------------METHODS------------------------------
//----------------------------Quaternion.toEuler()------------------
//return the quat as a euler
PyObject *V24_Quaternion_ToEuler(V24_QuaternionObject * self)
{
	float eul[3];
	int x;

	QuatToEul(self->quat, eul);
	for(x = 0; x < 3; x++) {
		eul[x] *= (180 / (float)Py_PI);
	}
	return V24_newEulerObject(eul, Py_NEW);
}
//----------------------------Quaternion.toMatrix()------------------
//return the quat as a matrix
PyObject *V24_Quaternion_ToMatrix(V24_QuaternionObject * self)
{
	float mat[9] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	QuatToMat3(self->quat, (float (*)[3]) mat);

	return V24_newMatrixObject(mat, 3, 3, Py_NEW);
}
//----------------------------Quaternion.normalize()----------------
//normalize the axis of rotation of [theta,vector]
PyObject *V24_Quaternion_Normalize(V24_QuaternionObject * self)
{
	NormalQuat(self->quat);
	return V24_EXPP_incr_ret((PyObject*)self);
}
//----------------------------Quaternion.inverse()------------------
//invert the quat
PyObject *V24_Quaternion_Inverse(V24_QuaternionObject * self)
{
	double mag = 0.0f;
	int x;

	for(x = 1; x < 4; x++) {
		self->quat[x] = -self->quat[x];
	}
	for(x = 0; x < 4; x++) {
		mag += (self->quat[x] * self->quat[x]);
	}
	mag = sqrt(mag);
	for(x = 0; x < 4; x++) {
		self->quat[x] /= (float)(mag * mag);
	}

	return V24_EXPP_incr_ret((PyObject*)self);
}
//----------------------------Quaternion.identity()-----------------
//generate the identity quaternion
PyObject *V24_Quaternion_Identity(V24_QuaternionObject * self)
{
	self->quat[0] = 1.0;
	self->quat[1] = 0.0;
	self->quat[2] = 0.0;
	self->quat[3] = 0.0;

	return V24_EXPP_incr_ret((PyObject*)self);
}
//----------------------------Quaternion.negate()-------------------
//negate the quat
PyObject *V24_Quaternion_Negate(V24_QuaternionObject * self)
{
	int x;
	for(x = 0; x < 4; x++) {
		self->quat[x] = -self->quat[x];
	}
	return V24_EXPP_incr_ret((PyObject*)self);
}
//----------------------------Quaternion.conjugate()----------------
//negate the vector part
PyObject *V24_Quaternion_Conjugate(V24_QuaternionObject * self)
{
	int x;
	for(x = 1; x < 4; x++) {
		self->quat[x] = -self->quat[x];
	}
	return V24_EXPP_incr_ret((PyObject*)self);
}
//----------------------------Quaternion.copy()----------------
//return a copy of the quat
PyObject *V24_Quaternion_copy(V24_QuaternionObject * self)
{
	return V24_newQuaternionObject(self->quat, Py_NEW);	
}

//----------------------------dealloc()(internal) ------------------
//free the py_object
static void V24_Quaternion_dealloc(V24_QuaternionObject * self)
{
	Py_XDECREF(self->coerced_object);
	//only free py_data
	if(self->data.py_data){
		PyMem_Free(self->data.py_data);
	}
	PyObject_DEL(self);
}
//----------------------------getattr()(internal) ------------------
//object.attribute access (get)
static PyObject *V24_Quaternion_getattr(V24_QuaternionObject * self, char *name)
{
	int x;
	double mag = 0.0f;
	float vec[3];

	if(STREQ(name,"w")){
		return PyFloat_FromDouble(self->quat[0]);
	}else if(STREQ(name, "x")){
		return PyFloat_FromDouble(self->quat[1]);
	}else if(STREQ(name, "y")){
		return PyFloat_FromDouble(self->quat[2]);
	}else if(STREQ(name, "z")){
		return PyFloat_FromDouble(self->quat[3]);
	}
	if(STREQ(name, "magnitude")) {
		for(x = 0; x < 4; x++) {
			mag += self->quat[x] * self->quat[x];
		}
		mag = sqrt(mag);
		return PyFloat_FromDouble(mag);
	}
	if(STREQ(name, "angle")) {
		mag = self->quat[0];
		mag = 2 * (acos(mag));
		mag *= (180 / Py_PI);
		return PyFloat_FromDouble(mag);
	}
	if(STREQ(name, "axis")) {
		mag = self->quat[0] * (Py_PI / 180);
		mag = 2 * (acos(mag));
		mag = sin(mag / 2);
		for(x = 0; x < 3; x++) {
			vec[x] = (float)(self->quat[x + 1] / mag);
		}
		Normalize(vec);
		//If the axis of rotation is 0,0,0 set it to 1,0,0 - for zero-degree rotations
		if( V24_EXPP_FloatsAreEqual(vec[0], 0.0f, 10) &&
			V24_EXPP_FloatsAreEqual(vec[1], 0.0f, 10) &&
			V24_EXPP_FloatsAreEqual(vec[2], 0.0f, 10) ){
			vec[0] = 1.0f;
		}
		return (PyObject *) V24_newVectorObject(vec, 3, Py_NEW);
	}
	if(STREQ(name, "wrapped")){
		if(self->wrapped == Py_WRAP)
			return V24_EXPP_incr_ret((PyObject *)Py_True);
		else 
			return V24_EXPP_incr_ret((PyObject *)Py_False);
	}

	return Py_FindMethod(Quaternion_methods, (PyObject *) self, name);
}
//----------------------------setattr()(internal) ------------------
//object.attribute access (set)
static int V24_Quaternion_setattr(V24_QuaternionObject * self, char *name, PyObject * q)
{
	PyObject *f = NULL;

	f = PyNumber_Float(q);
	if(f == NULL) { // parsed item not a number
		return V24_EXPP_ReturnIntError(PyExc_TypeError, 
			"quaternion.attribute = x: argument not a number\n");
	}

	if(STREQ(name,"w")){
		self->quat[0] = (float)PyFloat_AS_DOUBLE(f);
	}else if(STREQ(name, "x")){
		self->quat[1] = (float)PyFloat_AS_DOUBLE(f);
	}else if(STREQ(name, "y")){
		self->quat[2] = (float)PyFloat_AS_DOUBLE(f);
	}else if(STREQ(name, "z")){
		self->quat[3] = (float)PyFloat_AS_DOUBLE(f);
	}else{
		Py_DECREF(f);
		return V24_EXPP_ReturnIntError(PyExc_AttributeError,
				"quaternion.attribute = x: unknown attribute\n");
	}

	Py_DECREF(f);
	return 0;
}
//----------------------------print object (internal)--------------
//print the object to screen
static PyObject *V24_Quaternion_repr(V24_QuaternionObject * self)
{
	int i;
	char buffer[48], str[1024];

	BLI_strncpy(str,"[",1024);
	for(i = 0; i < 4; i++){
		if(i < (3)){
			sprintf(buffer, "%.6f, ", self->quat[i]);
			strcat(str,buffer);
		}else{
			sprintf(buffer, "%.6f", self->quat[i]);
			strcat(str,buffer);
		}
	}
	strcat(str, "](quaternion)");

	return PyString_FromString(str);
}
//------------------------tp_richcmpr
//returns -1 execption, 0 false, 1 true
static PyObject* V24_Quaternion_richcmpr(PyObject *objectA, PyObject *objectB, int comparison_type)
{
	V24_QuaternionObject *quatA = NULL, *quatB = NULL;
	int result = 0;

	if (!V24_QuaternionObject_Check(objectA) || !V24_QuaternionObject_Check(objectB)){
		if (comparison_type == Py_NE){
			return V24_EXPP_incr_ret(Py_True); 
		}else{
			return V24_EXPP_incr_ret(Py_False);
		}
	}
	quatA = (V24_QuaternionObject*)objectA;
	quatB = (V24_QuaternionObject*)objectB;

	switch (comparison_type){
		case Py_EQ:
			result = V24_EXPP_VectorsAreEqual(quatA->quat, quatB->quat, 4, 1);
			break;
		case Py_NE:
			result = V24_EXPP_VectorsAreEqual(quatA->quat, quatB->quat, 4, 1);
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
		return V24_EXPP_incr_ret(Py_True);
	}else{
		return V24_EXPP_incr_ret(Py_False);
	}
}
//------------------------tp_doc
static char V24_QuaternionObject_doc[] = "This is a wrapper for quaternion objects.";
//---------------------SEQUENCE PROTOCOLS------------------------
//----------------------------len(object)------------------------
//sequence length
static int V24_Quaternion_len(V24_QuaternionObject * self)
{
	return 4;
}
//----------------------------object[]---------------------------
//sequence accessor (get)
static PyObject *V24_Quaternion_item(V24_QuaternionObject * self, int i)
{
	if(i < 0 || i >= 4)
		return V24_EXPP_ReturnPyObjError(PyExc_IndexError,
		"quaternion[attribute]: array index out of range\n");
	return PyFloat_FromDouble(self->quat[i]);

}
//----------------------------object[]-------------------------
//sequence accessor (set)
static int V24_Quaternion_ass_item(V24_QuaternionObject * self, int i, PyObject * ob)
{
	PyObject *f = NULL;

	f = PyNumber_Float(ob);
	if(f == NULL) { // parsed item not a number
		return V24_EXPP_ReturnIntError(PyExc_TypeError, 
			"quaternion[attribute] = x: argument not a number\n");
	}

	if(i < 0 || i >= 4){
		Py_DECREF(f);
		return V24_EXPP_ReturnIntError(PyExc_IndexError,
			"quaternion[attribute] = x: array assignment index out of range\n");
	}
	self->quat[i] = (float)PyFloat_AS_DOUBLE(f);
	Py_DECREF(f);
	return 0;
}
//----------------------------object[z:y]------------------------
//sequence slice (get)
static PyObject *V24_Quaternion_slice(V24_QuaternionObject * self, int begin, int end)
{
	PyObject *list = NULL;
	int count;

	CLAMP(begin, 0, 4);
	if (end<0) end= 5+end;
	CLAMP(end, 0, 4);
	begin = MIN2(begin,end);

	list = PyList_New(end - begin);
	for(count = begin; count < end; count++) {
		PyList_SetItem(list, count - begin,
				PyFloat_FromDouble(self->quat[count]));
	}

	return list;
}
//----------------------------object[z:y]------------------------
//sequence slice (set)
static int V24_Quaternion_ass_slice(V24_QuaternionObject * self, int begin, int end,
			     PyObject * seq)
{
	int i, y, size = 0;
	float quat[4];
	PyObject *q, *f;

	CLAMP(begin, 0, 4);
	if (end<0) end= 5+end;
	CLAMP(end, 0, 4);
	begin = MIN2(begin,end);

	size = PySequence_Length(seq);
	if(size != (end - begin)){
		return V24_EXPP_ReturnIntError(PyExc_TypeError,
			"quaternion[begin:end] = []: size mismatch in slice assignment\n");
	}

	for (i = 0; i < size; i++) {
		q = PySequence_GetItem(seq, i);
		if (q == NULL) { // Failed to read sequence
			return V24_EXPP_ReturnIntError(PyExc_RuntimeError, 
				"quaternion[begin:end] = []: unable to read sequence\n");
		}

		f = PyNumber_Float(q);
		if(f == NULL) { // parsed item not a number
			Py_DECREF(q);
			return V24_EXPP_ReturnIntError(PyExc_TypeError, 
				"quaternion[begin:end] = []: sequence argument not a number\n");
		}

		quat[i] = (float)PyFloat_AS_DOUBLE(f);
		V24_EXPP_decr2(f,q);
	}
	//parsed well - now set in vector
	for(y = 0; y < size; y++){
		self->quat[begin + y] = quat[y];
	}
	return 0;
}
//------------------------NUMERIC PROTOCOLS----------------------
//------------------------obj + obj------------------------------
//addition
static PyObject *V24_Quaternion_add(PyObject * q1, PyObject * q2)
{
	int x;
	float quat[4];
	V24_QuaternionObject *quat1 = NULL, *quat2 = NULL;

	quat1 = (V24_QuaternionObject*)q1;
	quat2 = (V24_QuaternionObject*)q2;

	if(quat1->coerced_object || quat2->coerced_object){
		return V24_EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Quaternion addition: arguments not valid for this operation....\n");
	}
	for(x = 0; x < 4; x++) {
		quat[x] = quat1->quat[x] + quat2->quat[x];
	}

	return V24_newQuaternionObject(quat, Py_NEW);
}
//------------------------obj - obj------------------------------
//subtraction
static PyObject *V24_Quaternion_sub(PyObject * q1, PyObject * q2)
{
	int x;
	float quat[4];
	V24_QuaternionObject *quat1 = NULL, *quat2 = NULL;

	quat1 = (V24_QuaternionObject*)q1;
	quat2 = (V24_QuaternionObject*)q2;

	if(quat1->coerced_object || quat2->coerced_object){
		return V24_EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Quaternion addition: arguments not valid for this operation....\n");
	}
	for(x = 0; x < 4; x++) {
		quat[x] = quat1->quat[x] - quat2->quat[x];
	}

	return V24_newQuaternionObject(quat, Py_NEW);
}
//------------------------obj * obj------------------------------
//mulplication
static PyObject *V24_Quaternion_mul(PyObject * q1, PyObject * q2)
{
	int x;
	float quat[4], scalar;
	double dot = 0.0f;
	V24_QuaternionObject *quat1 = NULL, *quat2 = NULL;
	PyObject *f = NULL;
	V24_VectorObject *vec = NULL;
	V24_PointObject *pt = NULL;

	quat1 = (V24_QuaternionObject*)q1;
	quat2 = (V24_QuaternionObject*)q2;

	if(quat1->coerced_object){
		if (PyFloat_Check(quat1->coerced_object) || 
			PyInt_Check(quat1->coerced_object)){	// FLOAT/INT * QUAT
			f = PyNumber_Float(quat1->coerced_object);
			if(f == NULL) { // parsed item not a number
				return V24_EXPP_ReturnPyObjError(PyExc_TypeError, 
					"Quaternion multiplication: arguments not acceptable for this operation\n");
			}

			scalar = (float)PyFloat_AS_DOUBLE(f);
			Py_DECREF(f);
			for(x = 0; x < 4; x++) {
				quat[x] = quat2->quat[x] * scalar;
			}
			return V24_newQuaternionObject(quat, Py_NEW);
		}
	}else{
		if(quat2->coerced_object){
			if (PyFloat_Check(quat2->coerced_object) || 
				PyInt_Check(quat2->coerced_object)){	// QUAT * FLOAT/INT
				f = PyNumber_Float(quat2->coerced_object);
				if(f == NULL) { // parsed item not a number
					return V24_EXPP_ReturnPyObjError(PyExc_TypeError, 
						"Quaternion multiplication: arguments not acceptable for this operation\n");
				}

				scalar = (float)PyFloat_AS_DOUBLE(f);
				Py_DECREF(f);
				for(x = 0; x < 4; x++) {
					quat[x] = quat1->quat[x] * scalar;
				}
				return V24_newQuaternionObject(quat, Py_NEW);
			}else if(V24_VectorObject_Check(quat2->coerced_object)){  //QUAT * VEC
				vec = (V24_VectorObject*)quat2->coerced_object;
				if(vec->size != 3){
					return V24_EXPP_ReturnPyObjError(PyExc_TypeError, 
						"Quaternion multiplication: only 3D vector rotations currently supported\n");
				}
				return quat_rotation((PyObject*)quat1, (PyObject*)vec);
			}else if(V24_PointObject_Check(quat2->coerced_object)){  //QUAT * POINT
				pt = (V24_PointObject*)quat2->coerced_object;
				if(pt->size != 3){
					return V24_EXPP_ReturnPyObjError(PyExc_TypeError, 
						"Quaternion multiplication: only 3D point rotations currently supported\n");
				}
				return quat_rotation((PyObject*)quat1, (PyObject*)pt);
			}
		}else{  //QUAT * QUAT (dot product)
			for(x = 0; x < 4; x++) {
				dot += quat1->quat[x] * quat1->quat[x];
			}
			return PyFloat_FromDouble(dot);
		}
	}

	return V24_EXPP_ReturnPyObjError(PyExc_TypeError, 
		"Quaternion multiplication: arguments not acceptable for this operation\n");
}
//------------------------coerce(obj, obj)-----------------------
//coercion of unknown types to type V24_QuaternionObject for numeric protocols
/*Coercion() is called whenever a math operation has 2 operands that
 it doesn't understand how to evaluate. 2+Matrix for example. We want to 
 evaluate some of these operations like: (vector * 2), however, for math
 to proceed, the unknown operand must be cast to a type that python math will
 understand. (e.g. in the case above case, 2 must be cast to a vector and 
 then call vector.multiply(vector, scalar_cast_as_vector)*/
static int V24_Quaternion_coerce(PyObject ** q1, PyObject ** q2)
{
	if(V24_VectorObject_Check(*q2) || PyFloat_Check(*q2) || PyInt_Check(*q2) ||
			V24_PointObject_Check(*q2)) {
		PyObject *coerced = V24_EXPP_incr_ret(*q2);
		*q2 = V24_newQuaternionObject(NULL,Py_NEW);
		((V24_QuaternionObject*)*q2)->coerced_object = coerced;
		Py_INCREF (*q1);
		return 0;
	}

	return V24_EXPP_ReturnIntError(PyExc_TypeError, 
		"quaternion.coerce(): unknown operand - can't coerce for numeric protocols");
}
//-----------------PROTOCOL DECLARATIONS--------------------------
static PySequenceMethods V24_Quaternion_SeqMethods = {
	(inquiry) V24_Quaternion_len,					/* sq_length */
	(binaryfunc) 0,								/* sq_concat */
	(intargfunc) 0,								/* sq_repeat */
	(intargfunc) V24_Quaternion_item,				/* sq_item */
	(intintargfunc) V24_Quaternion_slice,			/* sq_slice */
	(intobjargproc) V24_Quaternion_ass_item,		/* sq_ass_item */
	(intintobjargproc) V24_Quaternion_ass_slice,	/* sq_ass_slice */
};
static PyNumberMethods V24_Quaternion_NumMethods = {
	(binaryfunc) V24_Quaternion_add,				/* __add__ */
	(binaryfunc) V24_Quaternion_sub,				/* __sub__ */
	(binaryfunc) V24_Quaternion_mul,				/* __mul__ */
	(binaryfunc) 0,								/* __div__ */
	(binaryfunc) 0,								/* __mod__ */
	(binaryfunc) 0,								/* __divmod__ */
	(ternaryfunc) 0,							/* __pow__ */
	(unaryfunc) 0,								/* __neg__ */
	(unaryfunc) 0,								/* __pos__ */
	(unaryfunc) 0,								/* __abs__ */
	(inquiry) 0,								/* __nonzero__ */
	(unaryfunc) 0,								/* __invert__ */
	(binaryfunc) 0,								/* __lshift__ */
	(binaryfunc) 0,								/* __rshift__ */
	(binaryfunc) 0,								/* __and__ */
	(binaryfunc) 0,								/* __xor__ */
	(binaryfunc) 0,								/* __or__ */
	(coercion)  V24_Quaternion_coerce,				/* __coerce__ */
	(unaryfunc) 0,								/* __int__ */
	(unaryfunc) 0,								/* __long__ */
	(unaryfunc) 0,								/* __float__ */
	(unaryfunc) 0,								/* __oct__ */
	(unaryfunc) 0,								/* __hex__ */

};
//------------------PY_OBECT DEFINITION--------------------------
PyTypeObject V24_quaternion_Type = {
PyObject_HEAD_INIT(NULL)		//tp_head
	0,								//tp_internal
	"quaternion",						//tp_name
	sizeof(V24_QuaternionObject),			//tp_basicsize
	0,								//tp_itemsize
	(destructor)V24_Quaternion_dealloc,		//tp_dealloc
	0,								//tp_print
	(getattrfunc)V24_Quaternion_getattr,	//tp_getattr
	(setattrfunc) V24_Quaternion_setattr,	//tp_setattr
	0,								//tp_compare
	(reprfunc) V24_Quaternion_repr,			//tp_repr
	&V24_Quaternion_NumMethods,				//tp_as_number
	&V24_Quaternion_SeqMethods,				//tp_as_sequence
	0,								//tp_as_mapping
	0,								//tp_hash
	0,								//tp_call
	0,								//tp_str
	0,								//tp_getattro
	0,								//tp_setattro
	0,								//tp_as_buffer
	Py_TPFLAGS_DEFAULT,				//tp_flags
	V24_QuaternionObject_doc,				//tp_doc
	0,								//tp_traverse
	0,								//tp_clear
	(richcmpfunc)V24_Quaternion_richcmpr,	//tp_richcompare
	0,								//tp_weaklistoffset
	0,								//tp_iter
	0,								//tp_iternext
	0,								//tp_methods
	0,								//tp_members
	0,								//tp_getset
	0,								//tp_base
	0,								//tp_dict
	0,								//tp_descr_get
	0,								//tp_descr_set
	0,								//tp_dictoffset
	0,								//tp_init
	0,								//tp_alloc
	0,								//tp_new
	0,								//tp_free
	0,								//tp_is_gc
	0,								//tp_bases
	0,								//tp_mro
	0,								//tp_cache
	0,								//tp_subclasses
	0,								//tp_weaklist
	0								//tp_del
};
//------------------------V24_newQuaternionObject (internal)-------------
//creates a new quaternion object
/*pass Py_WRAP - if vector is a WRAPPER for data allocated by BLENDER
 (i.e. it was allocated elsewhere by MEM_mallocN())
  pass Py_NEW - if vector is not a WRAPPER and managed by PYTHON
 (i.e. it must be created here with PyMEM_malloc())*/
PyObject *V24_newQuaternionObject(float *quat, int type)
{
	V24_QuaternionObject *self;
	int x;
	
	self = PyObject_NEW(V24_QuaternionObject, &V24_quaternion_Type);
	self->data.blend_data = NULL;
	self->data.py_data = NULL;
	self->coerced_object = NULL;

	if(type == Py_WRAP){
		self->data.blend_data = quat;
		self->quat = self->data.blend_data;
		self->wrapped = Py_WRAP;
	}else if (type == Py_NEW){
		self->data.py_data = PyMem_Malloc(4 * sizeof(float));
		self->quat = self->data.py_data;
		if(!quat) { //new empty
			V24_Quaternion_Identity(self);
			Py_DECREF(self);
		}else{
			for(x = 0; x < 4; x++){
				self->quat[x] = quat[x];
			}
		}
		self->wrapped = Py_NEW;
	}else{ //bad type
		return NULL;
	}
	return (PyObject *) self;
}

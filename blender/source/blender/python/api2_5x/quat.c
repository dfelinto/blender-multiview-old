/*
 * $Id: quat.c 12802 2007-12-06 00:01:46Z campbellbarton $
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
char Quat_Identity_doc[] = "() - set the quaternion to it's identity (1, vector)";
char Quat_Negate_doc[] = "() - set all values in the quaternion to their negative";
char Quat_Conjugate_doc[] = "() - set the quaternion to it's conjugate";
char Quat_Inverse_doc[] = "() - set the quaternion to it's inverse";
char Quat_Normalize_doc[] = "() - normalize the vector portion of the quaternion";
char Quat_ToEuler_doc[] = "() - return a euler rotation representing the quaternion";
char Quat_ToMatrix_doc[] = "() - return a rotation matrix representing the quaternion";
char Quat_copy_doc[] = "() - return a copy of the quat";
//-----------------------METHOD DEFINITIONS ----------------------
struct PyMethodDef Quat_methods[] = {
	{"identity", (PyCFunction) Quat_Identity, METH_NOARGS, Quat_Identity_doc},
	{"negate", (PyCFunction) Quat_Negate, METH_NOARGS, Quat_Negate_doc},
	{"conjugate", (PyCFunction) Quat_Conjugate, METH_NOARGS, Quat_Conjugate_doc},
	{"inverse", (PyCFunction) Quat_Inverse, METH_NOARGS, Quat_Inverse_doc},
	{"normalize", (PyCFunction) Quat_Normalize, METH_NOARGS, Quat_Normalize_doc},
	{"toEuler", (PyCFunction) Quat_ToEuler, METH_NOARGS, Quat_ToEuler_doc},
	{"toMatrix", (PyCFunction) Quat_ToMatrix, METH_NOARGS, Quat_ToMatrix_doc},
	{"__copy__", (PyCFunction) Quat_copy, METH_NOARGS, Quat_copy_doc},
	{"copy", (PyCFunction) Quat_copy, METH_NOARGS, Quat_copy_doc},
	{NULL, NULL, 0, NULL}
};
//-----------------------------METHODS------------------------------
//----------------------------Quaternion.toEuler()------------------
//return the quat as a euler
PyObject *Quat_ToEuler(BPyQuatObject * self)
{
	float eul[3];
	int x;

	QuatToEul(self->quat, eul);
	for(x = 0; x < 3; x++) {
		eul[x] *= (180 / (float)Py_PI);
	}
	return Euler_CreatePyObject(eul, (PyObject *)NULL);
}
//----------------------------Quaternion.toMatrix()------------------
//return the quat as a matrix
PyObject *Quat_ToMatrix(BPyQuatObject * self)
{
	float mat[9] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	QuatToMat3(self->quat, (float (*)[3]) mat);

	return Matrix_CreatePyObject(mat, 3, 3, (PyObject *)NULL);
}
//----------------------------Quaternion.normalize()----------------
//normalize the axis of rotation of [theta,vector]
PyObject *Quat_Normalize(BPyQuatObject * self)
{
	NormalQuat(self->quat);
	return EXPP_incr_ret((PyObject*)self);
}
//----------------------------Quaternion.inverse()------------------
//invert the quat
PyObject *Quat_Inverse(BPyQuatObject * self)
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

	return EXPP_incr_ret((PyObject*)self);
}
//----------------------------Quaternion.identity()-----------------
//generate the identity quaternion
PyObject *Quat_Identity(BPyQuatObject * self)
{
	self->quat[0] = 1.0;
	self->quat[1] = 0.0;
	self->quat[2] = 0.0;
	self->quat[3] = 0.0;

	return EXPP_incr_ret((PyObject*)self);
}
//----------------------------Quaternion.negate()-------------------
//negate the quat
PyObject *Quat_Negate(BPyQuatObject * self)
{
	int x;
	for(x = 0; x < 4; x++) {
		self->quat[x] = -self->quat[x];
	}
	return EXPP_incr_ret((PyObject*)self);
}
//----------------------------Quaternion.conjugate()----------------
//negate the vector part
PyObject *Quat_Conjugate(BPyQuatObject * self)
{
	int x;
	for(x = 1; x < 4; x++) {
		self->quat[x] = -self->quat[x];
	}
	return EXPP_incr_ret((PyObject*)self);
}
//----------------------------Quaternion.copy()----------------
//return a copy of the quat
PyObject *Quat_copy(BPyQuatObject * self)
{
	return Quat_CreatePyObject(self->quat, (PyObject *)NULL);	
}

//----------------------------dealloc()(internal) ------------------
//free the py_object
static void Quat_dealloc(BPyQuatObject * self)
{
	if(self->source){
		Py_DECREF(self->source);
		PyMem_Free(self->quat);
	}
	PyObject_DEL(self);
}

static PyObject *Quat_getMagnitude(BPyQuatObject * self)
{
	float mag;
	int x;
	for(x = 0; x < 4; x++) {
		mag += self->quat[x] * self->quat[x];
	}
	return PyFloat_FromDouble(sqrt(mag));
}

static PyObject *Quat_getAxisAngle(BPyQuatObject * self, void * axis)
{
	return PyFloat_FromDouble(2*acos(self->quat[0]));
}

static PyObject *Quat_getAngle(BPyQuatObject * self, void * axis)
{
	float mag;
	int x;
	float vec[3];
	
	mag = self->quat[0] * (Py_PI / 180);
	mag = 2 * (acos(mag));
	mag = sin(mag / 2);
	for(x = 0; x < 3; x++) {
		vec[x] = (float)(self->quat[x + 1] / mag);
	}
	Normalize(vec);
	//If the axis of rotation is 0,0,0 set it to 1,0,0 - for zero-degree rotations
	if( EXPP_FloatsAreEqual(vec[0], 0.0f, 10) &&
		EXPP_FloatsAreEqual(vec[1], 0.0f, 10) &&
		EXPP_FloatsAreEqual(vec[2], 0.0f, 10) ){
		vec[0] = 1.0f;
	}
	return Vector_CreatePyObject(vec, 3, (PyObject *)NULL);
}

static PyObject *Quat_getAxis(BPyQuatObject * self, void * axis)
{
	return PyFloat_FromDouble(self->quat[(long)axis]);
}

static int Quat_setAxis(BPyQuatObject * self, PyObject * value, void * axis)
{
	float f = ( float )PyFloat_AsDouble( value );
	if (f==-1 && PyErr_Occurred())
		return EXPP_ReturnIntError( PyExc_TypeError,
			"expected a number for the quat axis" );

	self->quat[(long)axis] = f;
	return 0;
}

static PyObject *Quat_getWrapped(BPyQuatObject * self)
{	
	if (self->source)
		Py_RETURN_TRUE;
	
	Py_RETURN_FALSE;
}

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef Quat_getset[] = {
	{"w",
	 (getter)Quat_getAxis, (setter)Quat_setAxis,
	 "quat w axis", (void *)0},
	{"x",
	 (getter)Quat_getAxis, (setter)Quat_setAxis,
	 "quat x axis", (void *)1},
	{"y",
	 (getter)Quat_getAxis, (setter)Quat_setAxis,
	 "quat y axis", (void *)2},
	{"z",
	 (getter)Quat_getAxis, (setter)Quat_setAxis,
	 "quat z axis", (void *)3},
	 
	{"magnitude",
	 (getter)Quat_getMagnitude, (setter)NULL,
	 "quat magnitude", NULL},
	{"axis",
	 (getter)Quat_getAxisAngle, (setter)NULL,
	 "true if the matrix is wrapped", NULL},
	{"angle",
	 (getter)Quat_getAngle, (setter)NULL,
	 "true if the matrix is wrapped", NULL},
	{"wrapped",
	 (getter)Quat_getWrapped, (setter)NULL,
	 "true if the matrix is wrapped", NULL},
	 
	{NULL}  /* Sentinel */
};

//----------------------------print object (internal)--------------
//print the object to screen
static PyObject *Quat_repr(BPyQuatObject * self)
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
static PyObject* Quat_richcmpr(PyObject *objectA, PyObject *objectB, int comparison_type)
{
	BPyQuatObject *quatA = NULL, *quatB = NULL;
	int result = 0;

	if (!BPyQuat_Check(objectA) || !BPyQuat_Check(objectB)){
		if (comparison_type == Py_NE){
			Py_RETURN_TRUE;
		}else{
			Py_RETURN_FALSE;
		}
	}
	quatA = (BPyQuatObject*)objectA;
	quatB = (BPyQuatObject*)objectB;

	switch (comparison_type){
		case Py_EQ:
			result = EXPP_VectorsAreEqual(quatA->quat, quatB->quat, 4, 1);
			break;
		case Py_NE:
			result = EXPP_VectorsAreEqual(quatA->quat, quatB->quat, 4, 1);
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
//------------------------tp_doc
static char BPyQuat_doc[] = "This is a wrapper for quaternion objects.";
//---------------------SEQUENCE PROTOCOLS------------------------
//----------------------------len(object)------------------------
//sequence length
static int Quat_len(BPyQuatObject * self)
{
	return 4;
}
//----------------------------object[]---------------------------
//sequence accessor (get)
static PyObject *Quat_item(BPyQuatObject * self, int i)
{
	if(i < 0 || i >= 4)
		return EXPP_ReturnPyObjError(PyExc_IndexError,
		"quaternion[attribute]: array index out of range\n");
	return PyFloat_FromDouble(self->quat[i]);

}
//----------------------------object[]-------------------------
//sequence accessor (set)
static int Quat_ass_item(BPyQuatObject * self, int i, PyObject * ob)
{
	PyObject *f = NULL;

	f = PyNumber_Float(ob);
	if(f == NULL) { // parsed item not a number
		return EXPP_ReturnIntError(PyExc_TypeError, 
			"quaternion[attribute] = x: argument not a number\n");
	}

	if(i < 0 || i >= 4){
		Py_DECREF(f);
		return EXPP_ReturnIntError(PyExc_IndexError,
			"quaternion[attribute] = x: array assignment index out of range\n");
	}
	self->quat[i] = (float)PyFloat_AS_DOUBLE(f);
	Py_DECREF(f);
	return 0;
}
//----------------------------object[z:y]------------------------
//sequence slice (get)
static PyObject *Quat_slice(BPyQuatObject * self, int begin, int end)
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
static int Quat_ass_slice(BPyQuatObject * self, int begin, int end,
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
		return EXPP_ReturnIntError(PyExc_TypeError,
			"quaternion[begin:end] = []: size mismatch in slice assignment\n");
	}

	for (i = 0; i < size; i++) {
		q = PySequence_GetItem(seq, i);
		if (q == NULL) { // Failed to read sequence
			return EXPP_ReturnIntError(PyExc_RuntimeError, 
				"quaternion[begin:end] = []: unable to read sequence\n");
		}

		f = PyNumber_Float(q);
		if(f == NULL) { // parsed item not a number
			Py_DECREF(q);
			return EXPP_ReturnIntError(PyExc_TypeError, 
				"quaternion[begin:end] = []: sequence argument not a number\n");
		}

		quat[i] = (float)PyFloat_AS_DOUBLE(f);
		EXPP_decr2(f,q);
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
static PyObject *Quat_add(BPyQuatObject * quat1, BPyQuatObject * quat2)
{
	int x;
	float quat[4];

	if (!BPyQuat_Check(quat1) || !BPyQuat_Check(quat2))
		return EXPP_ReturnPyObjError(PyExc_ValueError,
			"Quaternion addition: arguments not valid for this operation....\n");
	
	for(x = 0; x < 4; x++) {
		quat[x] = quat1->quat[x] + quat2->quat[x];
	}

	return Quat_CreatePyObject(quat, (PyObject *)NULL);
}
//------------------------obj - obj------------------------------
//subtraction
static PyObject *Quat_sub(BPyQuatObject * quat1, BPyQuatObject * quat2)
{
	int x;
	float quat[4];

	if (!BPyQuat_Check(quat1) || !BPyQuat_Check(quat2))
		return EXPP_ReturnPyObjError(PyExc_ValueError,
			"Quaternion addition: arguments not valid for this operation....\n");
	
	for(x = 0; x < 4; x++) {
		quat[x] = quat1->quat[x] - quat2->quat[x];
	}

	return Quat_CreatePyObject(quat, (PyObject *)NULL);
}
//------------------------obj * obj------------------------------
//mulplication
static PyObject *Quat_mul(BPyQuatObject * quat1, BPyQuatObject * quat2)
{
	int x;
	float quat[4], scalar;
	double dot = 0.0f;
	PyObject *f = NULL;
	BPyVectorObject *vec = NULL;

	if(!BPyQuat_Check(quat1)){
		if (PyNumber_Check((PyObject *)quat1)){	// FLOAT/INT * QUAT
			f = PyNumber_Float((PyObject *)quat1);
			if(f == NULL) { // parsed item not a number
				return EXPP_ReturnPyObjError(PyExc_TypeError, 
					"Quaternion multiplication: arguments not acceptable for this operation\n");
			}

			scalar = (float)PyFloat_AS_DOUBLE(f);
			Py_DECREF(f);
			for(x = 0; x < 4; x++) {
				quat[x] = quat2->quat[x] * scalar;
			}
			return Quat_CreatePyObject(quat, (PyObject *)NULL);
		}
	}else{
		if(!BPyQuat_Check(quat2)){
			if (PyNumber_Check((PyObject *)quat2)){	// QUAT * FLOAT/INT
				f = PyNumber_Float((PyObject *)quat2);
				if(f == NULL) { // parsed item not a number
					return EXPP_ReturnPyObjError(PyExc_TypeError, 
						"Quaternion multiplication: arguments not acceptable for this operation\n");
				}

				scalar = (float)PyFloat_AS_DOUBLE(f);
				Py_DECREF(f);
				for(x = 0; x < 4; x++) {
					quat[x] = quat1->quat[x] * scalar;
				}
				return Quat_CreatePyObject(quat, (PyObject *)NULL);
			}else if(BPyVector_Check((PyObject *)quat2)){  //QUAT * VEC
				vec = (BPyVectorObject *)quat2;
				if(vec->size != 3){
					return EXPP_ReturnPyObjError(PyExc_TypeError, 
						"Quaternion multiplication: only 3D vector rotations currently supported\n");
				}
				return quat_rotation((PyObject*)quat1, (PyObject*)vec);
			}
		}else{  //QUAT * QUAT (dot product)
			for(x = 0; x < 4; x++) {
				dot += quat1->quat[x] * quat1->quat[x];
			}
			return PyFloat_FromDouble(dot);
		}
	}

	return EXPP_ReturnPyObjError(PyExc_TypeError, 
		"Quaternion multiplication: arguments not acceptable for this operation\n");
}

//-----------------PROTOCOL DECLARATIONS--------------------------
static PySequenceMethods Quat_SeqMethods = {
	(inquiry) Quat_len,					/* sq_length */
	(binaryfunc) 0,								/* sq_concat */
	(intargfunc) 0,								/* sq_repeat */
	(intargfunc) Quat_item,				/* sq_item */
	(intintargfunc) Quat_slice,			/* sq_slice */
	(intobjargproc) Quat_ass_item,		/* sq_ass_item */
	(intintobjargproc) Quat_ass_slice,	/* sq_ass_slice */
};
static PyNumberMethods Quat_NumMethods = {
	(binaryfunc) Quat_add,				/* __add__ */
	(binaryfunc) Quat_sub,				/* __sub__ */
	(binaryfunc) Quat_mul,				/* __mul__ */
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
	(coercion)  0,				/* __coerce__ */
	(unaryfunc) 0,								/* __int__ */
	(unaryfunc) 0,								/* __long__ */
	(unaryfunc) 0,								/* __float__ */
	(unaryfunc) 0,								/* __oct__ */
	(unaryfunc) 0,								/* __hex__ */

};

/* this types constructor - apply any fixes to vec, eul, quat and col */
/* TODO - 2.44 accepted 3 args and worked out the 4th, do we want this?  */
static PyObject *Quat_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	float fvec[4];
	switch (PyTuple_Size(args)) {
	case 0:
		return Quat_CreatePyObject(NULL, (PyObject *)NULL);
	case 1: /* single argument */
		args = PyTuple_GET_ITEM(args, 0);
		if (EXPP_setVec4(args, &fvec[0], &fvec[1], &fvec[2], &fvec[3])!=-1) {
			return Quat_CreatePyObject(fvec, (PyObject *)NULL);
		} else {
			return NULL;
		}
	case 4:
		if (EXPP_setVec4(args, &fvec[0], &fvec[1], &fvec[2], &fvec[3])==-1)
			return NULL;
		return Quat_CreatePyObject(fvec, (PyObject *)NULL);
	}
	
	return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, expected 0, 1, or 4 args for the new vector." );
}

//------------------PY_OBECT DEFINITION--------------------------
PyTypeObject BPyQuat_Type = {
PyObject_HEAD_INIT(NULL)		//tp_head
	0,								//tp_internal
	"quaternion",						//tp_name
	sizeof(BPyQuatObject),			//tp_basicsize
	0,								//tp_itemsize
	(destructor)Quat_dealloc,		//tp_dealloc
	0,								//tp_print
	0,								//tp_getattr
	0,								//tp_setattr
	0,								//tp_compare
	(reprfunc) Quat_repr,			//tp_repr
	&Quat_NumMethods,				//tp_as_number
	&Quat_SeqMethods,				//tp_as_sequence
	0,								//tp_as_mapping
	0,								//tp_hash
	0,								//tp_call
	0,								//tp_str
	0,								//tp_getattro
	0,								//tp_setattro
	0,								//tp_as_buffer
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES, //tp_flags
	BPyQuat_doc,					//tp_doc
	0,								//tp_traverse
	0,								//tp_clear
	(richcmpfunc)Quat_richcmpr,		//tp_richcompare
	0,								//tp_weaklistoffset
	0,								//tp_iter
	0,								//tp_iternext
	0,								//tp_methods
	0,								//tp_members
	Quat_getset,					//tp_getset
	0,								//tp_base
	0,								//tp_dict
	0,								//tp_descr_get
	0,								//tp_descr_set
	0,								//tp_dictoffset
	0,								//tp_init
	0,								//tp_alloc
	( newfunc )Quat_new,			//tp_new
	0,								//tp_free
	0,								//tp_is_gc
	0,								//tp_bases
	0,								//tp_mro
	0,								//tp_cache
	0,								//tp_subclasses
	0,								//tp_weaklist
	0								//tp_del
};
//------------------------newBPyQuatObject (internal)-------------
//creates a new quaternion object
/*pass Py_WRAP - if vector is a WRAPPER for data allocated by BLENDER
 (i.e. it was allocated elsewhere by MEM_mallocN())
  pass Py_NEW - if vector is not a WRAPPER and managed by PYTHON
 (i.e. it must be created here with PyMEM_malloc())*/
PyObject *Quat_CreatePyObject(float *quat, PyObject * source)
{
	BPyQuatObject *self;
	int x;
	
	self = PyObject_NEW(BPyQuatObject, &BPyQuat_Type);
	self->source = source;
	if(source){
		Py_INCREF(source);
		self->quat = quat;
	} else {
		self->quat = PyMem_Malloc(4 * sizeof(float));
		if(!quat) { //new empty
			Quat_Identity(self);
			Py_DECREF(self);
		}else{
			for(x = 0; x < 4; x++){
				self->quat[x] = quat[x];
			}
		}
	}
	return (PyObject *) self;
}

int Quat_CheckPyObject(BPyQuatObject * self)
{
	if (self->source) {
		if (!(((BPyGenericLibObject *)self->source)->id)) {
			return 0;
		} else {
			return 1;
		}
	}
	return 1;
}

PyObject * QuatType_Init()
{
	PyType_Ready( &BPyQuat_Type );
	return (PyObject *)&BPyQuat_Type;
}

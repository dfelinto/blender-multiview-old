/*
 * $Id: euler.c 12898 2007-12-15 21:44:40Z campbellbarton $
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
char V24_Euler_Zero_doc[] = "() - set all values in the euler to 0";
char V24_Euler_Unique_doc[] ="() - sets the euler rotation a unique shortest arc rotation - tests for gimbal lock";
char V24_Euler_ToMatrix_doc[] =	"() - returns a rotation matrix representing the euler rotation";
char V24_Euler_ToQuat_doc[] = "() - returns a quaternion representing the euler rotation";
char V24_Euler_Rotate_doc[] = "() - rotate a euler by certain amount around an axis of rotation";
char V24_Euler_copy_doc[] = "() - returns a copy of the euler.";
//-----------------------METHOD DEFINITIONS ----------------------
struct PyMethodDef V24_Euler_methods[] = {
	{"zero", (PyCFunction) V24_Euler_Zero, METH_NOARGS, V24_Euler_Zero_doc},
	{"unique", (PyCFunction) V24_Euler_Unique, METH_NOARGS, V24_Euler_Unique_doc},
	{"toMatrix", (PyCFunction) V24_Euler_ToMatrix, METH_NOARGS, V24_Euler_ToMatrix_doc},
	{"toQuat", (PyCFunction) V24_Euler_ToQuat, METH_NOARGS, V24_Euler_ToQuat_doc},
	{"rotate", (PyCFunction) V24_Euler_Rotate, METH_VARARGS, V24_Euler_Rotate_doc},
	{"__copy__", (PyCFunction) V24_Euler_copy, METH_VARARGS, V24_Euler_copy_doc},
	{"copy", (PyCFunction) V24_Euler_copy, METH_VARARGS, V24_Euler_copy_doc},
	{NULL, NULL, 0, NULL}
};
//-----------------------------METHODS----------------------------
//----------------------------Euler.toQuat()----------------------
//return a quaternion representation of the euler
PyObject *V24_Euler_ToQuat(V24_EulerObject * self)
{
	float eul[3], quat[4];
	int x;

	for(x = 0; x < 3; x++) {
		eul[x] = self->eul[x] * ((float)Py_PI / 180);
	}
	EulToQuat(eul, quat);
	return V24_newQuaternionObject(quat, Py_NEW);
}
//----------------------------Euler.toMatrix()---------------------
//return a matrix representation of the euler
PyObject *V24_Euler_ToMatrix(V24_EulerObject * self)
{
	float eul[3];
	float mat[9] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	int x;

	for(x = 0; x < 3; x++) {
		eul[x] = self->eul[x] * ((float)Py_PI / 180);
	}
	EulToMat3(eul, (float (*)[3]) mat);
	return V24_newMatrixObject(mat, 3, 3 , Py_NEW);
}
//----------------------------Euler.unique()-----------------------
//sets the x,y,z values to a unique euler rotation
PyObject *V24_Euler_Unique(V24_EulerObject * self)
{
	double heading, pitch, bank;
	double pi2 =  Py_PI * 2.0f;
	double piO2 = Py_PI / 2.0f;
	double Opi2 = 1.0f / pi2;

	//radians
	heading = self->eul[0] * (float)Py_PI / 180;
	pitch = self->eul[1] * (float)Py_PI / 180;
	bank = self->eul[2] * (float)Py_PI / 180;

	//wrap heading in +180 / -180
	pitch += Py_PI;
	pitch -= floor(pitch * Opi2) * pi2;
	pitch -= Py_PI;


	if(pitch < -piO2) {
		pitch = -Py_PI - pitch;
		heading += Py_PI;
		bank += Py_PI;
	} else if(pitch > piO2) {
		pitch = Py_PI - pitch;
		heading += Py_PI;
		bank += Py_PI;
	}
	//gimbal lock test
	if(fabs(pitch) > piO2 - 1e-4) {
		heading += bank;
		bank = 0.0f;
	} else {
		bank += Py_PI;
		bank -= (floor(bank * Opi2)) * pi2;
		bank -= Py_PI;
	}

	heading += Py_PI;
	heading -= (floor(heading * Opi2)) * pi2;
	heading -= Py_PI;

	//back to degrees
	self->eul[0] = (float)(heading * 180 / (float)Py_PI);
	self->eul[1] = (float)(pitch * 180 / (float)Py_PI);
	self->eul[2] = (float)(bank * 180 / (float)Py_PI);

	return V24_EXPP_incr_ret((PyObject*)self);
}
//----------------------------Euler.zero()-------------------------
//sets the euler to 0,0,0
PyObject *V24_Euler_Zero(V24_EulerObject * self)
{
	self->eul[0] = 0.0;
	self->eul[1] = 0.0;
	self->eul[2] = 0.0;

	return V24_EXPP_incr_ret((PyObject*)self);
}
//----------------------------Euler.rotate()-----------------------
//rotates a euler a certain amount and returns the result
//should return a unique euler rotation (i.e. no 720 degree pitches :)
PyObject *V24_Euler_Rotate(V24_EulerObject * self, PyObject *args)
{
	float angle = 0.0f;
	char *axis;
	int x;

	if(!PyArg_ParseTuple(args, "fs", &angle, &axis)){
		return V24_EXPP_ReturnPyObjError(PyExc_TypeError,
			"euler.rotate():expected angle (float) and axis (x,y,z)");
	}
	if(!STREQ3(axis,"x","y","z")){
		return V24_EXPP_ReturnPyObjError(PyExc_TypeError,
			"euler.rotate(): expected axis to be 'x', 'y' or 'z'");
	}

	//covert to radians
	angle *= ((float)Py_PI / 180);
	for(x = 0; x < 3; x++) {
		self->eul[x] *= ((float)Py_PI / 180);
	}
	euler_rot(self->eul, angle, *axis);
	//convert back from radians
	for(x = 0; x < 3; x++) {
		self->eul[x] *= (180 / (float)Py_PI);
	}

	return V24_EXPP_incr_ret((PyObject*)self);
}
//----------------------------Euler.rotate()-----------------------
// return a copy of the euler
PyObject *V24_Euler_copy(V24_EulerObject * self, PyObject *args)
{
	return V24_newEulerObject(self->eul, Py_NEW);
}


//----------------------------dealloc()(internal) ------------------
//free the py_object
static void V24_Euler_dealloc(V24_EulerObject * self)
{
	//only free py_data
	if(self->data.py_data){
		PyMem_Free(self->data.py_data);
	}
	PyObject_DEL(self);
}
//----------------------------getattr()(internal) ------------------
//object.attribute access (get)
static PyObject *V24_Euler_getattr(V24_EulerObject * self, char *name)
{
	if(STREQ(name,"x")){
		return PyFloat_FromDouble(self->eul[0]);
	}else if(STREQ(name, "y")){
		return PyFloat_FromDouble(self->eul[1]);
	}else if(STREQ(name, "z")){
		return PyFloat_FromDouble(self->eul[2]);
	}
	if(STREQ(name, "wrapped")){
		if(self->wrapped == Py_WRAP)
			return V24_EXPP_incr_ret((PyObject *)Py_True);
		else 
			return V24_EXPP_incr_ret((PyObject *)Py_False);
	}
	return Py_FindMethod(V24_Euler_methods, (PyObject *) self, name);
}
//----------------------------setattr()(internal) ------------------
//object.attribute access (set)
static int V24_Euler_setattr(V24_EulerObject * self, char *name, PyObject * e)
{
	PyObject *f = NULL;

	f = PyNumber_Float(e);
	if(f == NULL) { // parsed item not a number
		return V24_EXPP_ReturnIntError(PyExc_TypeError, 
			"euler.attribute = x: argument not a number\n");
	}

	if(STREQ(name,"x")){
		self->eul[0] = (float)PyFloat_AS_DOUBLE(f);
	}else if(STREQ(name, "y")){
		self->eul[1] = (float)PyFloat_AS_DOUBLE(f);
	}else if(STREQ(name, "z")){
		self->eul[2] = (float)PyFloat_AS_DOUBLE(f);
	}else{
		Py_DECREF(f);
		return V24_EXPP_ReturnIntError(PyExc_AttributeError,
				"euler.attribute = x: unknown attribute\n");
	}

	Py_DECREF(f);
	return 0;
}
//----------------------------print object (internal)--------------
//print the object to screen
static PyObject *V24_Euler_repr(V24_EulerObject * self)
{
	int i;
	char buffer[48], str[1024];

	BLI_strncpy(str,"[",1024);
	for(i = 0; i < 3; i++){
		if(i < (2)){
			sprintf(buffer, "%.6f, ", self->eul[i]);
			strcat(str,buffer);
		}else{
			sprintf(buffer, "%.6f", self->eul[i]);
			strcat(str,buffer);
		}
	}
	strcat(str, "](euler)");

	return PyString_FromString(str);
}
//------------------------tp_richcmpr
//returns -1 execption, 0 false, 1 true
static PyObject* V24_Euler_richcmpr(PyObject *objectA, PyObject *objectB, int comparison_type)
{
	V24_EulerObject *eulA = NULL, *eulB = NULL;
	int result = 0;

	if (!V24_EulerObject_Check(objectA) || !V24_EulerObject_Check(objectB)){
		if (comparison_type == Py_NE){
			return V24_EXPP_incr_ret(Py_True); 
		}else{
			return V24_EXPP_incr_ret(Py_False);
		}
	}
	eulA = (V24_EulerObject*)objectA;
	eulB = (V24_EulerObject*)objectB;

	switch (comparison_type){
		case Py_EQ:
			result = V24_EXPP_VectorsAreEqual(eulA->eul, eulB->eul, 3, 1);
			break;
		case Py_NE:
			result = V24_EXPP_VectorsAreEqual(eulA->eul, eulB->eul, 3, 1);
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
static char V24_EulerObject_doc[] = "This is a wrapper for euler objects.";
//---------------------SEQUENCE PROTOCOLS------------------------
//----------------------------len(object)------------------------
//sequence length
static int V24_Euler_len(V24_EulerObject * self)
{
	return 3;
}
//----------------------------object[]---------------------------
//sequence accessor (get)
static PyObject *V24_Euler_item(V24_EulerObject * self, int i)
{
	if(i < 0 || i >= 3)
		return V24_EXPP_ReturnPyObjError(PyExc_IndexError,
		"euler[attribute]: array index out of range\n");

	return PyFloat_FromDouble(self->eul[i]);

}
//----------------------------object[]-------------------------
//sequence accessor (set)
static int V24_Euler_ass_item(V24_EulerObject * self, int i, PyObject * ob)
{
	PyObject *f = NULL;

	f = PyNumber_Float(ob);
	if(f == NULL) { // parsed item not a number
		return V24_EXPP_ReturnIntError(PyExc_TypeError, 
			"euler[attribute] = x: argument not a number\n");
	}

	if(i < 0 || i >= 3){
		Py_DECREF(f);
		return V24_EXPP_ReturnIntError(PyExc_IndexError,
			"euler[attribute] = x: array assignment index out of range\n");
	}
	self->eul[i] = (float)PyFloat_AS_DOUBLE(f);
	Py_DECREF(f);
	return 0;
}
//----------------------------object[z:y]------------------------
//sequence slice (get)
static PyObject *V24_Euler_slice(V24_EulerObject * self, int begin, int end)
{
	PyObject *list = NULL;
	int count;

	CLAMP(begin, 0, 3);
	if (end<0) end= 4+end;
	CLAMP(end, 0, 3);
	begin = MIN2(begin,end);

	list = PyList_New(end - begin);
	for(count = begin; count < end; count++) {
		PyList_SetItem(list, count - begin,
				PyFloat_FromDouble(self->eul[count]));
	}

	return list;
}
//----------------------------object[z:y]------------------------
//sequence slice (set)
static int V24_Euler_ass_slice(V24_EulerObject * self, int begin, int end,
			     PyObject * seq)
{
	int i, y, size = 0;
	float eul[3];
	PyObject *e, *f;

	CLAMP(begin, 0, 3);
	if (end<0) end= 4+end;
	CLAMP(end, 0, 3);
	begin = MIN2(begin,end);

	size = PySequence_Length(seq);
	if(size != (end - begin)){
		return V24_EXPP_ReturnIntError(PyExc_TypeError,
			"euler[begin:end] = []: size mismatch in slice assignment\n");
	}

	for (i = 0; i < size; i++) {
		e = PySequence_GetItem(seq, i);
		if (e == NULL) { // Failed to read sequence
			return V24_EXPP_ReturnIntError(PyExc_RuntimeError, 
				"euler[begin:end] = []: unable to read sequence\n");
		}

		f = PyNumber_Float(e);
		if(f == NULL) { // parsed item not a number
			Py_DECREF(e);
			return V24_EXPP_ReturnIntError(PyExc_TypeError, 
				"euler[begin:end] = []: sequence argument not a number\n");
		}

		eul[i] = (float)PyFloat_AS_DOUBLE(f);
		V24_EXPP_decr2(f,e);
	}
	//parsed well - now set in vector
	for(y = 0; y < 3; y++){
		self->eul[begin + y] = eul[y];
	}
	return 0;
}
//-----------------PROTCOL DECLARATIONS--------------------------
static PySequenceMethods V24_Euler_SeqMethods = {
	(inquiry) V24_Euler_len,						/* sq_length */
	(binaryfunc) 0,								/* sq_concat */
	(intargfunc) 0,								/* sq_repeat */
	(intargfunc) V24_Euler_item,					/* sq_item */
	(intintargfunc) V24_Euler_slice,				/* sq_slice */
	(intobjargproc) V24_Euler_ass_item,				/* sq_ass_item */
	(intintobjargproc) V24_Euler_ass_slice,			/* sq_ass_slice */
};
//------------------PY_OBECT DEFINITION--------------------------
PyTypeObject euler_Type = {
	PyObject_HEAD_INIT(NULL)		//tp_head
	0,								//tp_internal
	"euler",						//tp_name
	sizeof(V24_EulerObject),			//tp_basicsize
	0,								//tp_itemsize
	(destructor)V24_Euler_dealloc,		//tp_dealloc
	0,								//tp_print
	(getattrfunc)V24_Euler_getattr,	//tp_getattr
	(setattrfunc) V24_Euler_setattr,	//tp_setattr
	0,								//tp_compare
	(reprfunc) V24_Euler_repr,			//tp_repr
	0,				//tp_as_number
	&V24_Euler_SeqMethods,				//tp_as_sequence
	0,								//tp_as_mapping
	0,								//tp_hash
	0,								//tp_call
	0,								//tp_str
	0,								//tp_getattro
	0,								//tp_setattro
	0,								//tp_as_buffer
	Py_TPFLAGS_DEFAULT,				//tp_flags
	V24_EulerObject_doc,				//tp_doc
	0,								//tp_traverse
	0,								//tp_clear
	(richcmpfunc)V24_Euler_richcmpr,	//tp_richcompare
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
//------------------------V24_newEulerObject (internal)-------------
//creates a new euler object
/*pass Py_WRAP - if vector is a WRAPPER for data allocated by BLENDER
 (i.e. it was allocated elsewhere by MEM_mallocN())
  pass Py_NEW - if vector is not a WRAPPER and managed by PYTHON
 (i.e. it must be created here with PyMEM_malloc())*/
PyObject *V24_newEulerObject(float *eul, int type)
{
	V24_EulerObject *self;
	int x;

	self = PyObject_NEW(V24_EulerObject, &euler_Type);
	self->data.blend_data = NULL;
	self->data.py_data = NULL;

	if(type == Py_WRAP){
		self->data.blend_data = eul;
		self->eul = self->data.blend_data;
		self->wrapped = Py_WRAP;
	}else if (type == Py_NEW){
		self->data.py_data = PyMem_Malloc(3 * sizeof(float));
		self->eul = self->data.py_data;
		if(!eul) { //new empty
			for(x = 0; x < 3; x++) {
				self->eul[x] = 0.0f;
			}
		}else{
			for(x = 0; x < 3; x++){
				self->eul[x] = eul[x];
			}
		}
		self->wrapped = Py_NEW;
	}else{ //bad type
		return NULL;
	}
	return (PyObject *) self;
}


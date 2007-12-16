/* 
 * $Id: Mathutils.c 11936 2007-09-04 05:22:42Z campbellbarton $
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
 * Inc., 59 Temple Place - Suite 330, Boston, MA	02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * This is a new part of Blender.
 *
 * Contributor(s): Joseph Gilbert, Campbell Barton
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "Mathutils.h"

#include "BLI_arithb.h"
#include "PIL_time.h"
#include "BLI_rand.h"
#include "BKE_utildefines.h"

#include "gen_utils.h"

//-------------------------DOC STRINGS ---------------------------
static char M_Mathutils_doc[] = "The Blender Mathutils module\n\n";
static char M_Mathutils_Rand_doc[] = "() - return a random number";
static char M_Mathutils_CrossVecs_doc[] = "() - returns a vector perpedicular to the 2 vectors crossed";
static char M_Mathutils_DotVecs_doc[] = "() - return the dot product of two vectors";
static char M_Mathutils_MidpointVecs_doc[] = "() - return the vector to the midpoint between two vectors";
static char M_Mathutils_ProjectVecs_doc[] =	"() - returns the projection vector from the projection of vecA onto vecB";
static char M_Mathutils_CrossQuats_doc[] = "() - return the mutliplication of two quaternions";
static char M_Mathutils_DotQuats_doc[] = "() - return the dot product of two quaternions";
static char M_Mathutils_Slerp_doc[] = "() - returns the interpolation between two quaternions";
static char M_Mathutils_DifferenceQuats_doc[] = "() - return the angular displacment difference between two quats";
//-----------------------METHOD DEFINITIONS ----------------------
struct PyMethodDef M_Mathutils_methods[] = {
	{"Rand", (PyCFunction) M_Mathutils_Rand, METH_VARARGS, M_Mathutils_Rand_doc},
	{"CrossVecs", (PyCFunction) M_Mathutils_CrossVecs, METH_VARARGS, M_Mathutils_CrossVecs_doc},
	{"DotVecs", (PyCFunction) M_Mathutils_DotVecs, METH_VARARGS, M_Mathutils_DotVecs_doc},
	{"MidpointVecs", (PyCFunction) M_Mathutils_MidpointVecs, METH_VARARGS, M_Mathutils_MidpointVecs_doc},
	{"ProjectVecs", (PyCFunction) M_Mathutils_ProjectVecs, METH_VARARGS, M_Mathutils_ProjectVecs_doc},
	{"CrossQuats", (PyCFunction) M_Mathutils_CrossQuats, METH_VARARGS, M_Mathutils_CrossQuats_doc},
	{"DotQuats", (PyCFunction) M_Mathutils_DotQuats, METH_VARARGS, M_Mathutils_DotQuats_doc},
	{"DifferenceQuats", (PyCFunction) M_Mathutils_DifferenceQuats, METH_VARARGS,M_Mathutils_DifferenceQuats_doc},
	{"Slerp", (PyCFunction) M_Mathutils_Slerp, METH_VARARGS, M_Mathutils_Slerp_doc},
	
	{NULL, NULL, 0, NULL}
};
//----------------------------MODULE INIT-------------------------
PyObject *Mathutils_Init(void)
{
	PyObject *submodule;

	//seed the generator for the rand function
	BLI_srand((unsigned int) (PIL_check_seconds_timer() *
				      0x7FFFFFFF));
	
	/* needed for getset */	
	submodule = Py_InitModule3("bpy.math.utils",
				    M_Mathutils_methods, M_Mathutils_doc);
	return (submodule);
}
//-----------------------------METHODS----------------------------
//----------------column_vector_multiplication (internal)---------
//COLUMN VECTOR Multiplication (Matrix X Vector)
// [1][2][3]   [a]
// [4][5][6] * [b]
// [7][8][9]   [c]
//vector/matrix multiplication IS NOT COMMUTATIVE!!!!
PyObject *column_vector_multiplication(BPyMatrixObject * mat, BPyVectorObject * vec)
{
	float vecNew[4], vecCopy[4];
	double dot = 0.0f;
	int x, y, z = 0;

	if(mat->rowSize != vec->size){
		if(mat->rowSize == 4 && vec->size != 3){
			return EXPP_ReturnPyObjError(PyExc_AttributeError,
				"matrix * vector: matrix row size and vector size must be the same");
		}else{
			vecCopy[3] = 1.0f;
		}          
	}

	for(x = 0; x < vec->size; x++){
		vecCopy[x] = vec->vec[x];
		}

	for(x = 0; x < mat->rowSize; x++) {
		for(y = 0; y < mat->colSize; y++) {
			dot += mat->matrix[x][y] * vecCopy[y];
		}
		vecNew[z++] = (float)dot;
		dot = 0.0f;
	}
	return Vector_CreatePyObject(vecNew, vec->size, (PyObject *)NULL);
}

//-----------------row_vector_multiplication (internal)-----------
//ROW VECTOR Multiplication - Vector X Matrix
//[x][y][z] *  [1][2][3]
//             [4][5][6]
//             [7][8][9]
//vector/matrix multiplication IS NOT COMMUTATIVE!!!!
PyObject *row_vector_multiplication(BPyVectorObject * vec, BPyMatrixObject * mat)
{
	float vecNew[4], vecCopy[4];
	double dot = 0.0f;
	int x, y, z = 0, vec_size = vec->size;

	if(mat->colSize != vec_size){
		if(mat->rowSize == 4 && vec_size != 3){
			return EXPP_ReturnPyObjError(PyExc_AttributeError, 
				"vector * matrix: matrix column size and the vector size must be the same");
		}else{
			vecCopy[3] = 1.0f;
		}
	}
	
	for(x = 0; x < vec_size; x++){
		vecCopy[x] = vec->vec[x];
	}

	//muliplication
	for(x = 0; x < mat->colSize; x++) {
		for(y = 0; y < mat->rowSize; y++) {
			dot += mat->matrix[y][x] * vecCopy[y];
		}
		vecNew[z++] = (float)dot;
		dot = 0.0f;
	}
	return Vector_CreatePyObject(vecNew, vec_size, (PyObject *)NULL);
}

//-----------------quat_rotation (internal)-----------
//This function multiplies a vector/point * quat or vice versa
//to rotate the point/vector by the quaternion
//arguments should all be 3D
PyObject *quat_rotation(PyObject *arg1, PyObject *arg2)
{
	float rot[3];
	BPyQuatObject *quat = NULL;
	BPyVectorObject *vec = NULL;

	if(BPyQuat_Check(arg1)){
		quat = (BPyQuatObject *)arg1;
		if(BPyVector_Check(arg2)){
			vec = (BPyVectorObject *)arg2;
			rot[0] = quat->quat[0]*quat->quat[0]*vec->vec[0] + 2*quat->quat[2]*quat->quat[0]*vec->vec[2] - 
				2*quat->quat[3]*quat->quat[0]*vec->vec[1] + quat->quat[1]*quat->quat[1]*vec->vec[0] + 
				2*quat->quat[2]*quat->quat[1]*vec->vec[1] + 2*quat->quat[3]*quat->quat[1]*vec->vec[2] - 
				quat->quat[3]*quat->quat[3]*vec->vec[0] - quat->quat[2]*quat->quat[2]*vec->vec[0];
			rot[1] = 2*quat->quat[1]*quat->quat[2]*vec->vec[0] + quat->quat[2]*quat->quat[2]*vec->vec[1] + 
				2*quat->quat[3]*quat->quat[2]*vec->vec[2] + 2*quat->quat[0]*quat->quat[3]*vec->vec[0] - 
				quat->quat[3]*quat->quat[3]*vec->vec[1] + quat->quat[0]*quat->quat[0]*vec->vec[1] - 
				2*quat->quat[1]*quat->quat[0]*vec->vec[2] - quat->quat[1]*quat->quat[1]*vec->vec[1];
			rot[2] = 2*quat->quat[1]*quat->quat[3]*vec->vec[0] + 2*quat->quat[2]*quat->quat[3]*vec->vec[1] + 
				quat->quat[3]*quat->quat[3]*vec->vec[2] - 2*quat->quat[0]*quat->quat[2]*vec->vec[0] - 
				quat->quat[2]*quat->quat[2]*vec->vec[2] + 2*quat->quat[0]*quat->quat[1]*vec->vec[1] - 
				quat->quat[1]*quat->quat[1]*vec->vec[2] + quat->quat[0]*quat->quat[0]*vec->vec[2];
			return Vector_CreatePyObject(rot, 3, (PyObject *)NULL);
		}
	}else if(BPyVector_Check(arg1)){
		vec = (BPyVectorObject*)arg1;
		if(BPyQuat_Check(arg2)){
			quat = (BPyQuatObject *)arg2;
			rot[0] = quat->quat[0]*quat->quat[0]*vec->vec[0] + 2*quat->quat[2]*quat->quat[0]*vec->vec[2] - 
				2*quat->quat[3]*quat->quat[0]*vec->vec[1] + quat->quat[1]*quat->quat[1]*vec->vec[0] + 
				2*quat->quat[2]*quat->quat[1]*vec->vec[1] + 2*quat->quat[3]*quat->quat[1]*vec->vec[2] - 
				quat->quat[3]*quat->quat[3]*vec->vec[0] - quat->quat[2]*quat->quat[2]*vec->vec[0];
			rot[1] = 2*quat->quat[1]*quat->quat[2]*vec->vec[0] + quat->quat[2]*quat->quat[2]*vec->vec[1] + 
				2*quat->quat[3]*quat->quat[2]*vec->vec[2] + 2*quat->quat[0]*quat->quat[3]*vec->vec[0] - 
				quat->quat[3]*quat->quat[3]*vec->vec[1] + quat->quat[0]*quat->quat[0]*vec->vec[1] - 
				2*quat->quat[1]*quat->quat[0]*vec->vec[2] - quat->quat[1]*quat->quat[1]*vec->vec[1];
			rot[2] = 2*quat->quat[1]*quat->quat[3]*vec->vec[0] + 2*quat->quat[2]*quat->quat[3]*vec->vec[1] + 
				quat->quat[3]*quat->quat[3]*vec->vec[2] - 2*quat->quat[0]*quat->quat[2]*vec->vec[0] - 
				quat->quat[2]*quat->quat[2]*vec->vec[2] + 2*quat->quat[0]*quat->quat[1]*vec->vec[1] - 
				quat->quat[1]*quat->quat[1]*vec->vec[2] + quat->quat[0]*quat->quat[0]*vec->vec[2];
			return Vector_CreatePyObject(rot, 3, (PyObject *)NULL);
		}
	}

	return (EXPP_ReturnPyObjError(PyExc_RuntimeError,
		"quat_rotation(internal): internal problem rotating vector/point\n"));
}

//----------------------------------Mathutils.Rand() --------------------
//returns a random number between a high and low value
PyObject *M_Mathutils_Rand(PyObject * self, PyObject * args)
{
	float high, low, range;
	double rand;
	//initializers
	high = 1.0;
	low = 0.0;

	if(!PyArg_ParseTuple(args, "|ff", &low, &high))
		return (EXPP_ReturnPyObjError(PyExc_TypeError,
			"Mathutils.Rand(): expected nothing or optional (float, float)\n"));

	if((high < low) || (high < 0 && low > 0))
		return (EXPP_ReturnPyObjError(PyExc_ValueError,
			"Mathutils.Rand(): high value should be larger than low value\n"));

	//get the random number 0 - 1
	rand = BLI_drand();

	//set it to range
	range = high - low;
	rand = rand * range;
	rand = rand + low;

	return PyFloat_FromDouble(rand);
}
//----------------------------------Mathutils.CrossVecs() ---------------
//finds perpendicular vector - only 3D is supported
PyObject *M_Mathutils_CrossVecs(PyObject * self, PyObject * args)
{
	PyObject *vecCross = NULL;
	BPyVectorObject *vec1 = NULL, *vec2 = NULL;

	if(!PyArg_ParseTuple(args, "O!O!", &BPyVector_Type, &vec1, &BPyVector_Type, &vec2))
		return EXPP_ReturnPyObjError(PyExc_TypeError, 
			"Mathutils.CrossVecs(): expects (2) 3D vector objects\n");
	if(vec1->size != 3 || vec2->size != 3)
		return EXPP_ReturnPyObjError(PyExc_AttributeError, 
			"Mathutils.CrossVecs(): expects (2) 3D vector objects\n");

	vecCross = Vector_CreatePyObject(NULL, 3, (PyObject *)NULL);
	Crossf(((BPyVectorObject *)vecCross)->vec, vec1->vec, vec2->vec);
	return vecCross;
}
//----------------------------------Mathutils.DotVec() -------------------
//calculates the dot product of two vectors
PyObject *M_Mathutils_DotVecs(PyObject * self, PyObject * args)
{
	BPyVectorObject *vec1 = NULL, *vec2 = NULL;
	double dot = 0.0f;
	int x;

	if(!PyArg_ParseTuple(args, "O!O!", &BPyVector_Type, &vec1, &BPyVector_Type, &vec2))
		return EXPP_ReturnPyObjError(PyExc_TypeError, 
			"Mathutils.DotVec(): expects (2) vector objects of the same size\n");
	if(vec1->size != vec2->size)
		return EXPP_ReturnPyObjError(PyExc_AttributeError, 
			"Mathutils.DotVec(): expects (2) vector objects of the same size\n");

	for(x = 0; x < vec1->size; x++) {
		dot += vec1->vec[x] * vec2->vec[x];
	}
	return PyFloat_FromDouble(dot);
}

//----------------------------------Mathutils.MidpointVecs() -------------
//calculates the midpoint between 2 vectors
PyObject *M_Mathutils_MidpointVecs(PyObject * self, PyObject * args)
{
	BPyVectorObject *vec1 = NULL, *vec2 = NULL;
	float vec[4];
	int x;
	
	if(!PyArg_ParseTuple(args, "O!O!", &BPyVector_Type, &vec1, &BPyVector_Type, &vec2))
		return EXPP_ReturnPyObjError(PyExc_TypeError, 
			"Mathutils.MidpointVecs(): expects (2) vector objects of the same size\n");
	if(vec1->size != vec2->size)
		return EXPP_ReturnPyObjError(PyExc_AttributeError, 
			"Mathutils.MidpointVecs(): expects (2) vector objects of the same size\n");

	for(x = 0; x < vec1->size; x++) {
		vec[x] = 0.5f * (vec1->vec[x] + vec2->vec[x]);
	}
	return Vector_CreatePyObject(vec, vec1->size, (PyObject *)NULL);
}
//----------------------------------Mathutils.ProjectVecs() -------------
//projects vector 1 onto vector 2
PyObject *M_Mathutils_ProjectVecs(PyObject * self, PyObject * args)
{
	BPyVectorObject *vec1 = NULL, *vec2 = NULL;
	float vec[4]; 
	double dot = 0.0f, dot2 = 0.0f;
	int x, size;

	if(!PyArg_ParseTuple(args, "O!O!", &BPyVector_Type, &vec1, &BPyVector_Type, &vec2))
		return EXPP_ReturnPyObjError(PyExc_TypeError, 
			"Mathutils.ProjectVecs(): expects (2) vector objects of the same size\n");
	if(vec1->size != vec2->size)
		return EXPP_ReturnPyObjError(PyExc_AttributeError, 
			"Mathutils.ProjectVecs(): expects (2) vector objects of the same size\n");

	//since they are the same size...
	size = vec1->size;

	//get dot products
	for(x = 0; x < size; x++) {
		dot += vec1->vec[x] * vec2->vec[x];
		dot2 += vec2->vec[x] * vec2->vec[x];
	}
	//projection
	dot /= dot2;
	for(x = 0; x < size; x++) {
		vec[x] = (float)(dot * vec2->vec[x]);
	}
	return Vector_CreatePyObject(vec, size, (PyObject *)NULL);
}

//----------------------------------Mathutils.CrossQuats() ----------------
//quaternion multiplication - associate not commutative
PyObject *M_Mathutils_CrossQuats(PyObject * self, PyObject * args)
{
	BPyQuatObject *quatU = NULL, *quatV = NULL;
	float quat[4];

	if(!PyArg_ParseTuple(args, "O!O!", &BPyQuat_Type, &quatU, 
		&BPyQuat_Type, &quatV))
		return EXPP_ReturnPyObjError(PyExc_TypeError,"Mathutils.CrossQuats(): expected Quaternion types");
	QuatMul(quat, quatU->quat, quatV->quat);

	return Quat_CreatePyObject(quat, (PyObject *)NULL);
}
//----------------------------------Mathutils.DotQuats() ----------------
//returns the dot product of 2 quaternions
PyObject *M_Mathutils_DotQuats(PyObject * self, PyObject * args)
{
	BPyQuatObject *quatU = NULL, *quatV = NULL;
	double dot = 0.0f;
	int x;

	if(!PyArg_ParseTuple(args, "O!O!", &BPyQuat_Type, &quatU, 
		&BPyQuat_Type, &quatV))
		return EXPP_ReturnPyObjError(PyExc_TypeError, "Mathutils.DotQuats(): expected Quaternion types");

	for(x = 0; x < 4; x++) {
		dot += quatU->quat[x] * quatV->quat[x];
	}
	return PyFloat_FromDouble(dot);
}
//----------------------------------Mathutils.DifferenceQuats() ---------
//returns the difference between 2 quaternions
PyObject *M_Mathutils_DifferenceQuats(PyObject * self, PyObject * args)
{
	BPyQuatObject *quatU = NULL, *quatV = NULL;
	float quat[4], tempQuat[4];
	double dot = 0.0f;
	int x;

	if(!PyArg_ParseTuple(args, "O!O!", &BPyQuat_Type, 
		&quatU, &BPyQuat_Type, &quatV))
		return EXPP_ReturnPyObjError(PyExc_TypeError, "Mathutils.DifferenceQuats(): expected Quaternion types");

	tempQuat[0] = quatU->quat[0];
	tempQuat[1] = -quatU->quat[1];
	tempQuat[2] = -quatU->quat[2];
	tempQuat[3] = -quatU->quat[3];

	dot = sqrt(tempQuat[0] * tempQuat[0] + tempQuat[1] *  tempQuat[1] +
			       tempQuat[2] * tempQuat[2] + tempQuat[3] * tempQuat[3]);

	for(x = 0; x < 4; x++) {
		tempQuat[x] /= (float)(dot * dot);
	}
	QuatMul(quat, tempQuat, quatV->quat);
	return Quat_CreatePyObject(quat, (PyObject *)NULL);
}
//----------------------------------Mathutils.Slerp() ------------------
//attemps to interpolate 2 quaternions and return the result
PyObject *M_Mathutils_Slerp(PyObject * self, PyObject * args)
{
	BPyQuatObject *quatU = NULL, *quatV = NULL;
	float quat[4], quat_u[4], quat_v[4], param;
	double x, y, dot, sinT, angle, IsinT;
	int z;

	if(!PyArg_ParseTuple(args, "O!O!f", &BPyQuat_Type, 
		&quatU, &BPyQuat_Type, &quatV, &param))
		return EXPP_ReturnPyObjError(PyExc_TypeError, 
			"Mathutils.Slerp(): expected Quaternion types and float");

	if(param > 1.0f || param < 0.0f)
		return EXPP_ReturnPyObjError(PyExc_AttributeError, 
					"Mathutils.Slerp(): interpolation factor must be between 0.0 and 1.0");

	//copy quats
	for(z = 0; z < 4; z++){
		quat_u[z] = quatU->quat[z];
		quat_v[z] = quatV->quat[z];
	}

	//dot product
	dot = quat_u[0] * quat_v[0] + quat_u[1] * quat_v[1] +
		quat_u[2] * quat_v[2] + quat_u[3] * quat_v[3];

	//if negative negate a quat (shortest arc)
	if(dot < 0.0f) {
		quat_v[0] = -quat_v[0];
		quat_v[1] = -quat_v[1];
		quat_v[2] = -quat_v[2];
		quat_v[3] = -quat_v[3];
		dot = -dot;
	}
	if(dot > .99999f) { //very close
		x = 1.0f - param;
		y = param;
	} else {
		//calculate sin of angle
		sinT = sqrt(1.0f - (dot * dot));
		//calculate angle
		angle = atan2(sinT, dot);
		//caluculate inverse of sin(theta)
		IsinT = 1.0f / sinT;
		x = sin((1.0f - param) * angle) * IsinT;
		y = sin(param * angle) * IsinT;
	}
	//interpolate
	quat[0] = (float)(quat_u[0] * x + quat_v[0] * y);
	quat[1] = (float)(quat_u[1] * x + quat_v[1] * y);
	quat[2] = (float)(quat_u[2] * x + quat_v[2] * y);
	quat[3] = (float)(quat_u[3] * x + quat_v[3] * y);

	return Quat_CreatePyObject(quat, (PyObject *)NULL);
}

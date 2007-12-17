/* 
 * $Id: gen_utils.h 11936 2007-09-04 05:22:42Z campbellbarton $
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
 * Contributor(s): Michel Selten, Willian P. Germano, Alex Mole, Joseph Gilbert
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_gen_utils_h
#define EXPP_gen_utils_h

#include <Python.h>
#include "DNA_ID.h"
#include "DNA_scriptlink_types.h"
#include "DNA_listBase.h"
#include "vector.h"

/* for anonymous comparisons */
typedef struct {
	PyObject_HEAD
	void * pointer;
} BPyAnonymousObject;

int EXPP_Anonymous_compare( BPyAnonymousObject * a, BPyAnonymousObject * b );


#define Py_PI  3.14159265358979323846
/* 
   Py_RETURN_NONE
   Python 2.4 macro.  
   defined here until we switch to 2.4
*/
#ifndef Py_RETURN_NONE
#define Py_RETURN_NONE	return Py_BuildValue("O", Py_None)
#endif
#ifndef Py_RETURN_FALSE
#define Py_RETURN_FALSE  return PyBool_FromLong(0) 
#endif
#ifndef Py_RETURN_TRUE
#define Py_RETURN_TRUE  return PyBool_FromLong(1)
#endif

/* Py 2.5 adds a new Py_ssize_t type for 64 bit support */
/* #if'ed for your compiling pleasure */
#if 1

/*  for pre Py 2.5 */
#if PY_VERSION_HEX < 0x02050000
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#define lenfunc inquiry
#define ssizeargfunc intargfunc
#else  
/* Py 2.5 and later */
#define  intargfunc  ssizeargfunc
#define intintargfunc  ssizessizeargfunc 
#endif
#endif

/* Py 2.4 dosnt have these */
#ifndef ssizessizeargfunc 
#define ssizessizeargfunc intintargfunc
#endif

#ifndef ssizeobjargproc
#define ssizeobjargproc intintargfunc
#endif

#ifndef ssizessizeobjargproc
#define ssizessizeobjargproc intintobjargproc
#endif




/* name of list of Armature weak refs built into __main__ */
#define ARM_WEAKREF_LIST_NAME "__arm_weakrefs"

int EXPP_FloatsAreEqual(float A, float B, int floatSteps);
int EXPP_VectorsAreEqual(float *vecA, float *vecB, int size, int floatSteps);

PyObject *EXPP_GetModuleConstant(char *module, char *constant);

PyObject *PythonReturnErrorObject( PyObject * type, char *error_msg );
PyObject *PythonIncRef( PyObject * object );

char *event_to_name( short event );

float EXPP_ClampFloat( float value, float min, float max );
int EXPP_ClampInt( int value, int min, int max );

void EXPP_incr2( PyObject * ob1, PyObject * ob2 );
void EXPP_incr3( PyObject * ob1, PyObject * ob2, PyObject * ob3 );
void EXPP_decr2( PyObject * ob1, PyObject * ob2 );
void EXPP_decr3( PyObject * ob1, PyObject * ob2, PyObject * ob3 );
PyObject *EXPP_incr_ret( PyObject * object );
PyObject *EXPP_incr_ret_True(void);
PyObject *EXPP_incr_ret_False(void);
PyObject *EXPP_ReturnPyObjError( PyObject * type, char *error_msg );
int EXPP_ReturnIntError( PyObject * type, char *error_msg );

PyObject *EXPP_objError(PyObject *type, const char *format, ...);
int EXPP_intError(PyObject *type, const char *format, ...);

int EXPP_check_sequence_consistency( PyObject * seq, PyTypeObject * against );

/* clamping and range-checking utilities */

int EXPP_setIValueClamped( PyObject *value, void *param,
		int min, int max, char type );
int EXPP_setFloatClamped ( PyObject *value, float *param,
			float min, float max);
int EXPP_setVec3Clamped ( BPyVectorObject *value, float *param,
			float min, float max);
int EXPP_setIValueRange( PyObject *value, void *param,
		int min, int max, char type );
int EXPP_setFloatRange ( PyObject *value, float *param,
			float min, float max);

/* utilities to get/set bits in bitfields */

PyObject *EXPP_getBitfield( void *param, int setting, char type );
int EXPP_setBitfield( PyObject * value, void *param, int setting, char type );

/*
 * Procedures to handle older setStuff() methods, which now access 
 * a PyType's setter attributes using the tp_getset mechanism.
 */

PyObject *EXPP_setterWrapper ( PyObject * self, PyObject * args,
				setter func);

PyObject *EXPP_setterWrapperTuple ( PyObject * self, PyObject * args,
				setter func);
/* this queues redraws if we're not in background mode: */
void EXPP_allqueue(unsigned short event, short val);

/* helper to keep dictionaries from causing memory leaks */
int EXPP_dict_set_item_str( PyObject *dict, char *key, PyObject *value);

/* for adding 'self' at the start, but of course it can be used in any case */
PyObject *EXPP_PyTuple_New_Prepend( PyObject * tuple, PyObject * value );

/* nifty util functions for assigning a double, trible or quad*/
int EXPP_setVec2(PyObject * value, float * f1, float * f2);
int EXPP_setVec3(PyObject * value, float * f1, float * f2, float * f3);
int EXPP_setVec4(PyObject * value, float * f1, float * f2, float * f3, float * f4);

void EXPP_PyMethodsToDict(PyObject *dict, struct PyMethodDef *meth);

#endif				/* EXPP_gen_utils_h */


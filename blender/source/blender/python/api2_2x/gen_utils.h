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

#include "constant.h"

#define Py_PI  3.14159265358979323846
#define Py_WRAP 1024
#define Py_NEW  2048

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
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#else  
/* Py 2.5 and later */
#define  intargfunc  ssizeargfunc
#define intintargfunc  ssizessizeargfunc 
#endif

#endif

/* name of list of Armature weak refs built into __main__ */
#define ARM_WEAKREF_LIST_NAME "__arm_weakrefs"

int V24_EXPP_FloatsAreEqual(float A, float B, int floatSteps);
int V24_EXPP_VectorsAreEqual(float *vecA, float *vecB, int size, int floatSteps);

PyObject *V24_EXPP_GetModuleConstant(char *module, char *constant);

int V24_StringEqual( const char *string1, const char *string2 );

PyObject *V24_PythonReturnErrorObject( PyObject * type, char *error_msg );
PyObject *PythonIncRef( PyObject * object );

char *event_to_name( short event );

float V24_EXPP_ClampFloat( float value, float min, float max );
int V24_EXPP_ClampInt( int value, int min, int max );

void V24_EXPP_incr2( PyObject * ob1, PyObject * ob2 );
void V24_EXPP_incr3( PyObject * ob1, PyObject * ob2, PyObject * ob3 );
void V24_EXPP_decr2( PyObject * ob1, PyObject * ob2 );
void V24_EXPP_decr3( PyObject * ob1, PyObject * ob2, PyObject * ob3 );
PyObject *V24_EXPP_incr_ret( PyObject * object );
PyObject *V24_EXPP_incr_ret_True(void);
PyObject *V24_EXPP_incr_ret_False(void);
PyObject *V24_EXPP_ReturnPyObjError( PyObject * type, char *error_msg );
int V24_EXPP_ReturnIntError( PyObject * type, char *error_msg );

PyObject *V24_EXPP_objError(PyObject *type, const char *format, ...);
int V24_EXPP_intError(PyObject *type, const char *format, ...);

int V24_EXPP_check_sequence_consistency( PyObject * seq, PyTypeObject * against );
PyObject *V24_EXPP_tuple_repr( PyObject * self, int size );

/* mapping utilities - see Texture.c for an example of how to use these */
typedef struct {
	const char *sval;
	int ival;
} V24_EXPP_map_pair;

/* maps must end with a pair that has NULL as sval */
int V24_EXPP_map_getIntVal( const V24_EXPP_map_pair * map,
			const char *sval, int *ival );
int V24_EXPP_map_case_getIntVal( const V24_EXPP_map_pair * map,
			     const char *sval, int *ival );
int V24_EXPP_map_getShortVal( const V24_EXPP_map_pair * map,
			  const char *sval, short *ival );
int V24_EXPP_map_getStrVal( const V24_EXPP_map_pair * map,
			int ival, const char **sval );

/* clamping and range-checking utilities */

int V24_EXPP_setIValueClamped( PyObject *value, void *param,
		int min, int max, char type );
int V24_EXPP_setFloatClamped ( PyObject *value, float *param,
			float min, float max);
int V24_EXPP_setVec3Clamped ( PyObject *value, float *param,
			float min, float max);
int V24_EXPP_setIValueRange( PyObject *value, void *param,
		int min, int max, char type );
int V24_EXPP_setFloatRange ( PyObject *value, float *param,
			float min, float max);

/* utility routine for PyType attributes setters with module constant */

int V24_EXPP_setModuleConstant ( V24_BPy_constant *constant, void *param,
			char type );

/* utilities to get/set bits in bitfields */

PyObject *V24_EXPP_getBitfield( void *param, int setting, char type );
int V24_EXPP_setBitfield( PyObject * value, void *param, int setting, char type );

/*
 * Procedures to handle older setStuff() methods, which now access 
 * a PyType's setter attributes using the tp_getset mechanism.
 */

PyObject *V24_EXPP_setterWrapper ( PyObject * self, PyObject * args,
				setter func);

PyObject *V24_EXPP_setterWrapperTuple ( PyObject * self, PyObject * args,
				setter func);

/* scriplinks-related: */
PyObject *V24_EXPP_getScriptLinks(ScriptLink *slink, PyObject *value, int is_scene);
PyObject *V24_EXPP_addScriptLink(ScriptLink *slink, PyObject *args, int is_scene);
PyObject *V24_EXPP_clearScriptLinks(ScriptLink *slink, PyObject *args);

/* this queues redraws if we're not in background mode: */
void V24_EXPP_allqueue(unsigned short event, short val);

/* helper to keep dictionaries from causing memory leaks */
int V24_EXPP_dict_set_item_str( PyObject *dict, char *key, PyObject *value);
PyObject * V24_EXPP_PyTuple_New_Prepend(PyObject *tuple, PyObject *value);

#endif				/* EXPP_gen_utils_h */


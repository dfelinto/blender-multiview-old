/* 
 * $Id: gen_utils.c 11960 2007-09-07 08:09:41Z campbellbarton $
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
 * Contributor(s): Michel Selten, Willian P. Germano, Alex Mole, Ken Hughes,
 * Campbell Barton
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include "gen_utils.h" /*This must come first*/

#include "DNA_text_types.h"
#include "MEM_guardedalloc.h"
#include "BLI_blenlib.h"
#include "BIF_space.h"
#include "BKE_global.h"
#include "BKE_main.h"
#include "BKE_library.h"

#include "Mathutils.h"
#include "vector.h"
#include "color.h"

/*
 * This is to compare any 2 types that have a pointer directly after the
 * PyObject in their struct, this covers quite a few cp
 * */

int EXPP_Anonymous_compare( BPyAnonymousObject * a, BPyAnonymousObject * b )
{
	return ( a->pointer == b->pointer) ? 0 : -1;	
}

/*---------------------- EXPP_FloatsAreEqual -------------------------
  Floating point comparisons 
  floatStep = number of representable floats allowable in between
   float A and float B to be considered equal. */
int EXPP_FloatsAreEqual(float A, float B, int floatSteps)
{
	int a, b, delta;
    assert(floatSteps > 0 && floatSteps < (4 * 1024 * 1024));
    a = *(int*)&A;
    if (a < 0)	
		a = 0x80000000 - a;
    b = *(int*)&B;
    if (b < 0)	
		b = 0x80000000 - b;
    delta = abs(a - b);
    if (delta <= floatSteps)	
		return 1;
    return 0;
}
/*---------------------- EXPP_VectorsAreEqual -------------------------
  Builds on EXPP_FloatsAreEqual to test vectors */
int EXPP_VectorsAreEqual(float *vecA, float *vecB, int size, int floatSteps){

	int x;
	for (x=0; x< size; x++){
		if (EXPP_FloatsAreEqual(vecA[x], vecB[x], floatSteps) == 0)
			return 0;
	}
	return 1;
}

/*---------------------- EXPP_GetModuleConstant -------------------------
  Helper function for returning a module constant */
PyObject *EXPP_GetModuleConstant(char *module, char *constant)
{
	PyObject *py_module = NULL, *py_dict = NULL, *py_constant = NULL;

	/*Careful to pass the correct Package.Module string here or
	* else you add a empty module somewhere*/
	py_module = PyImport_AddModule(module);
	if(!py_module){   /*null = error returning module*/
		return ( EXPP_ReturnPyObjError( PyExc_RuntimeError,
			"error encountered with returning module constant..." ) );
	}
	py_dict = PyModule_GetDict(py_module); /*never fails*/

	py_constant = PyDict_GetItemString(py_dict, constant);
	if(!py_constant){   /*null = key not found*/
		return ( EXPP_ReturnPyObjError( PyExc_RuntimeError,
			"error encountered with returning module constant..." ) );
	}

	return EXPP_incr_ret(py_constant);
}

/*****************************************************************************/
/* Description: This function clamps an int to the given interval	  */
/*							[min, max].	   */
/*****************************************************************************/
int EXPP_ClampInt( int value, int min, int max )
{
	if( value < min )
		return min;
	else if( value > max )
		return max;
	return value;
}

/*****************************************************************************/
/* Description: This function clamps a float to the given interval	 */
/*							[min, max].	 */
/*****************************************************************************/
float EXPP_ClampFloat( float value, float min, float max )
{
	if( value < min )
		return min;
	else if( value > max )
		return max;
	return value;
}

/*****************************************************************************/
/* Description: These functions set an internal string with the given type   */
/*		  and error_msg arguments.				     */
/*****************************************************************************/

PyObject *EXPP_ReturnPyObjError( PyObject * type, char *error_msg )
{				/* same as above, just to change its name smoothly */
	PyErr_SetString( type, error_msg );
	return NULL;
}

int EXPP_ReturnIntError( PyObject * type, char *error_msg )
{
	PyErr_SetString( type, error_msg );
	return -1;
}

int EXPP_intError(PyObject *type, const char *format, ...)
{
	PyObject *error;
	va_list vlist;

	va_start(vlist, format);
	error = PyString_FromFormatV(format, vlist);
	va_end(vlist);

	PyErr_SetObject(type, error);
	Py_DECREF(error);
	return -1;
}
/*Like EXPP_ReturnPyObjError but takes a printf format string and multiple arguments*/
PyObject *EXPP_objError(PyObject *type, const char *format, ...)
{
	PyObject *error;
	va_list vlist;

	va_start(vlist, format);
	error = PyString_FromFormatV(format, vlist);
	va_end(vlist);

	PyErr_SetObject(type, error);
	Py_DECREF(error);
	return NULL;
}

/*****************************************************************************/
/* Description: This function increments the reference count of the given   */
/*			 Python object (usually Py_None) and returns it.    */
/*****************************************************************************/

PyObject *EXPP_incr_ret( PyObject * object )
{
	Py_INCREF( object );
	return ( object );
}

void EXPP_incr2( PyObject * ob1, PyObject * ob2 )
{
  	    Py_INCREF( ob1 );
  	    Py_INCREF( ob2 );
}

void EXPP_incr3( PyObject * ob1, PyObject * ob2, PyObject * ob3 )
{
  	    Py_INCREF( ob1 );
  	    Py_INCREF( ob2 );
  	    Py_INCREF( ob3 );
}

void EXPP_decr2( PyObject * ob1, PyObject * ob2 )
{
  	    Py_DECREF( ob1 );
  	    Py_DECREF( ob2 );
}

void EXPP_decr3( PyObject * ob1, PyObject * ob2, PyObject * ob3 )
{
  	    Py_DECREF( ob1 );
  	    Py_DECREF( ob2 );
  	    Py_DECREF( ob3 );
}
/*****************************************************************************/
/* Description: This function maps the event identifier to a string.	  */
/*****************************************************************************/
char *event_to_name( short event )
{
	switch ( event ) {
	case SCRIPT_FRAMECHANGED:
		return "FrameChanged";
	case SCRIPT_ONLOAD:
		return "OnLoad";
	case SCRIPT_ONSAVE:
		return "OnSave";
	case SCRIPT_REDRAW:
		return "Redraw";
	case SCRIPT_RENDER:
		return "Render";
	case SCRIPT_POSTRENDER:
		return "PostRender";
	default:
		return "Unknown";
	}
}

/*****************************************************************************/
/* Description: Checks whether all objects in a PySequence are of a same  */
/*		given type.  Returns 0 if not, 1 on success.		 */
/*****************************************************************************/
int EXPP_check_sequence_consistency( PyObject * seq, PyTypeObject * against )
{
	PyObject *ob;
	int len = PySequence_Length( seq );
	int i, result = 1;

	for( i = 0; i < len; i++ ) {
		ob = PySequence_GetItem( seq, i );
		if( ob == Py_None )
			result = 2;
		else if( ob->ob_type != against ) {
			Py_DECREF( ob );
			return 0;
		}
		Py_DECREF( ob );
	}
	return result;		/* 1 if all of 'against' type, 2 if there are (also) Nones */
}

/* Redraw wrappers */

/* this queues redraws if we're not in background mode: */
void EXPP_allqueue(unsigned short event, short val)
{
	if (!G.background) allqueue(event, val);
}

/*
 * Utility routines to clamp and store various datatypes.  The object type 
 * is checked and a exception is raised if it's not the correct type.  
 *
 * Inputs:
 *    value: PyObject containing the new value
 *    param: pointer to destination variable
 *    max, min: range of values for clamping
 *    type: kind of pointer and data (uses the same characters as
 *       PyArgs_ParseTuple() and Py_BuildValue()
 *
 * Return 0 on success, -1 on error.
 */

int EXPP_setFloatClamped( PyObject *value, float *param,
								float min, float max )
{
	float f = (float)PyFloat_AsDouble(value);
	if (f==-1 && PyErr_Occurred())
		return -1;

	*param = EXPP_ClampFloat( f, min, max );
	return 0;
}

int EXPP_setIValueClamped( PyObject *value, void *param,
								int min, int max, char type )
{
	int number = PyInt_AsLong( value );
	if (number == -1 && PyErr_Occurred())
		return -1;

	switch ( type ) {
	case 'b':
		*(char *)param = (char)EXPP_ClampInt( number, min, max );
		return 0;
	case 'h':
		*(short *)param = (short)EXPP_ClampInt( number, min, max );
		return 0;
	case 'H':
		*(unsigned short *)param = (unsigned short)EXPP_ClampInt( number, min, max );
		return 0;
	case 'i':
		*(int *)param = EXPP_ClampInt( number, min, max );
		return 0;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
			   "EXPP_setIValueClamped(): invalid type code" );
	}
}

int EXPP_setVec3Clamped( BPyVectorObject *value, float *param,
								float min, float max )
{	
	if( BPyVector_Check( value ) ) {
		if( value->size == 3 ) {
			param[0] = EXPP_ClampFloat( value->vec[0], min, max );
			param[1] = EXPP_ClampFloat( value->vec[1], min, max );
			param[2] = EXPP_ClampFloat( value->vec[2], min, max );
			return 0;
		}
	}
	
	if (1) {
		char errstr[128];
		sprintf ( errstr, "expected vector argument in [%f,%f]", min, max );
		return EXPP_ReturnIntError( PyExc_TypeError, errstr );
	}
}

/* Use for the getsets 'set' operations, so they dont have to
 * to the work of parsing the vec themselves, for use with vectors and color.
 * So for example these can be used for setting a verts location, a color
 * or any other vector, remember this will not clamp values.
 * 
 * These functions take a PyObject and assign it to float's
 * This could of course be done with the sequence protocol but
 * its faster to use the Color and Vector PyTypes directly and
 * fallback on generic methods.
 * 
 * PLEASE KEEP ANY CHANGES TO THESE FUNCTIONS IN SYNC - (Campbell)
 *  */
int EXPP_setVec2(PyObject * value, float * p1, float * p2)
{
	float f1, f2;
	
	if (PyTuple_Check(value)) {
		if (PyTuple_Size(value) != 2)
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec2(): the tuple does not contain 2 items" );
		f1 = ( float )PyFloat_AsDouble( PyTuple_GET_ITEM(value, 0) );
		f2 = ( float )PyFloat_AsDouble( PyTuple_GET_ITEM(value, 1) );

		if (PyErr_Occurred()) /* did PyFloat_AsDouble have an error ? */
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec2(): one or more values in the tuple is not a number" );
		
	} else if (PyList_Check(value)) {
		if (PyList_Size(value) != 2)
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec2(): the list does not contain 2 items" );
		f1 = ( float )PyFloat_AsDouble( PyList_GET_ITEM(value, 0) );
		f2 = ( float )PyFloat_AsDouble( PyList_GET_ITEM(value, 1) );

		if (PyErr_Occurred()) /* did PyFloat_AsDouble have an error ? */
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec2(): one or more values in the list is not a number" );
		
	} else if (PySequence_Check(value)) {
		PyObject *v1, *v2;
		if (PySequence_Size(value) != 2)
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec2(): the sequence does not contain 2 items" );
		
		v1 = PySequence_GetItem(value, 0);
		v2 = PySequence_GetItem(value, 1);
		f1 = ( float )PyFloat_AsDouble( v1 );
		f2 = ( float )PyFloat_AsDouble( v2 );
		Py_DECREF( v1 );
		Py_DECREF( v2 );
		if (PyErr_Occurred()) /* did PyFloat_AsDouble have an error ? */
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec2(): one or more values in the sequence is not a number" );
	} else if (BPyVector_Check(value)) {
		BPyVectorObject *vec = (BPyVectorObject *)value;
		if (vec->size != 2)
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec2(): the Vector does not contain 2 items" );
		if (!BPyVector_Check(vec))
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec2(): the Vector being assigned references invalid data" );
		
		/* dont need to do error checking */
		*p1 = vec->vec[0];
		*p2 = vec->vec[1];
		return 0;
	} else {
		return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec2(): Incompatible type assignment, only accept a tuple, list, sequence or Vector" );
	}/* else if (BPyColor_Check(value)) {
		//only for 3/4 channels
	}*/
	*p1 = f1; *p2 = f2;
	return 0;
}

int EXPP_setVec3(PyObject * value, float * p1, float * p2, float * p3)
{
	float f1, f2, f3;
	
	if (PyTuple_Check(value)) {
		if (PyTuple_Size(value) != 3)
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec3(): the tuple does not contain 3 items" );
		f1 = ( float )PyFloat_AsDouble( PyTuple_GET_ITEM(value, 0) );
		f2 = ( float )PyFloat_AsDouble( PyTuple_GET_ITEM(value, 1) );
		f3 = ( float )PyFloat_AsDouble( PyTuple_GET_ITEM(value, 2) );

		if (PyErr_Occurred()) /* did PyFloat_AsDouble have an error ? */
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec3(): one or more values in the tuple is not a number" );
		
	} else if (PyList_Check(value)) {
		if (PyList_Size(value) != 3)
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec3(): the list does not contain 3 items" );
		f1 = ( float )PyFloat_AsDouble( PyList_GET_ITEM(value, 0) );
		f2 = ( float )PyFloat_AsDouble( PyList_GET_ITEM(value, 1) );
		f3 = ( float )PyFloat_AsDouble( PyList_GET_ITEM(value, 2) );

		if (PyErr_Occurred()) /* did PyFloat_AsDouble have an error ? */
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec3(): one or more values in the list is not a number" );
		
	} else if (PySequence_Check(value)) {
		PyObject *v1, *v2, *v3;
		if (PySequence_Size(value) != 3)
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec3(): the sequence does not contain 3 items" );
		
		v1 = PySequence_GetItem(value, 0);
		v2 = PySequence_GetItem(value, 1);
		v3 = PySequence_GetItem(value, 2);
		f1 = ( float )PyFloat_AsDouble( v1 );
		f2 = ( float )PyFloat_AsDouble( v2 );
		f3 = ( float )PyFloat_AsDouble( v3 );
		Py_DECREF( v1 );
		Py_DECREF( v2 );
		Py_DECREF( v3 );
		if (PyErr_Occurred()) /* did PyFloat_AsDouble have an error ? */
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec3(): one or more values in the sequence is not a number" );
	} else if (BPyVector_Check(value)) {
		BPyVectorObject *vec = (BPyVectorObject *)value;
		if (vec->size != 3)
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec3(): the Vector does not contain 3 items" );
		
		if (!BPyVector_Check(vec))
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec3(): the Vector being assigned references invalid data" );
		
		/* dont need to do error checking */
		*p1 = vec->vec[0];
		*p2 = vec->vec[1];
		*p3 = vec->vec[2];
		return 0;
	} else if (BPyColor_Check(value)) {
		/*only for 3/4 channels*/
		BPyColorObject *col = (BPyColorObject *)value;
		
		if (col->size != 3)
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec3(): the Color does not contain 3 items" );
		
		if (!Color_CheckPyObject(col))
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec3(): the Color being assigned references invalid data" );
		
		*p1 = col->color[0];
		*p2 = col->color[1];
		*p3 = col->color[2];
		return 0;
	} else {
		return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec3(): Incompatible type assignment, only accept a tuple, list, sequence or Vector" );
	}
	*p1 = f1; *p2 = f2; *p3 = f3;
	return 0;
}

int EXPP_setVec4(PyObject * value, float * p1, float * p2, float * p3, float * p4)
{
	float f1, f2, f3, f4;
	
	if (PyTuple_Check(value)) {
		if (PyTuple_Size(value) != 4)
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec4(): the tuple does not contain 4 items" );
		f1 = ( float )PyFloat_AsDouble( PyTuple_GET_ITEM(value, 0) );
		f2 = ( float )PyFloat_AsDouble( PyTuple_GET_ITEM(value, 1) );
		f3 = ( float )PyFloat_AsDouble( PyTuple_GET_ITEM(value, 2) );
		f4 = ( float )PyFloat_AsDouble( PyTuple_GET_ITEM(value, 3) );

		if (PyErr_Occurred()) /* did PyFloat_AsDouble have an error ? */
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec4(): one or more values in the tuple is not a number" );
		
	} else if (PyList_Check(value)) {
		if (PyList_Size(value) != 4)
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec4(): the list does not contain 4 items" );
		f1 = ( float )PyFloat_AsDouble( PyList_GET_ITEM(value, 0) );
		f2 = ( float )PyFloat_AsDouble( PyList_GET_ITEM(value, 1) );
		f3 = ( float )PyFloat_AsDouble( PyList_GET_ITEM(value, 2) );
		f4 = ( float )PyFloat_AsDouble( PyList_GET_ITEM(value, 3) );

		if (PyErr_Occurred()) /* did PyFloat_AsDouble have an error ? */
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec4(): one or more values in the list is not a number" );
		
	} else if (PySequence_Check(value)) {
		PyObject *v1, *v2, *v3, *v4;
		if (PySequence_Size(value) != 4)
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec4(): the sequence does not contain 4 items" );
		
		v1 = PySequence_GetItem(value, 0);
		v2 = PySequence_GetItem(value, 1);
		v3 = PySequence_GetItem(value, 2);
		v4 = PySequence_GetItem(value, 3);
		f1 = ( float )PyFloat_AsDouble( v1 );
		f2 = ( float )PyFloat_AsDouble( v2 );
		f3 = ( float )PyFloat_AsDouble( v3 );
		f4 = ( float )PyFloat_AsDouble( v4 );
		Py_DECREF( v1 );
		Py_DECREF( v2 );
		Py_DECREF( v3 );
		Py_DECREF( v4 );
		if (PyErr_Occurred()) /* did PyFloat_AsDouble have an error ? */
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec4(): one or more values in the sequence is not a number" );
	} else if (BPyVector_Check(value)) {
		BPyVectorObject *vec = (BPyVectorObject *)value;
		if (vec->size != 4)
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec4(): the Vector does not contain 4 items" );
		
		if (!BPyVector_Check(vec))
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec4(): the Vector being assigned references invalid data" );
		
		/* dont need to do error checking */
		*p1 = vec->vec[0];
		*p2 = vec->vec[1];
		*p3 = vec->vec[2];
		*p4 = vec->vec[3];
		return 0;
	} else if (BPyColor_Check(value)) {
		/*only for 3/4 channels*/
		BPyColorObject *col = (BPyColorObject *)value;
		
		if (col->size != 4)
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec4(): the Color does not contain 4 items" );
		
		if (!Color_CheckPyObject(col))
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec4(): the Color being assigned references invalid data" );
		
		*p1 = col->color[0];
		*p2 = col->color[1];
		*p3 = col->color[2];
		*p4 = col->color[4];
		return 0;
	} else if (BPyQuat_Check(value)) {
		BPyQuatObject *quat = (BPyQuatObject *)value;
		
		if (!Quat_CheckPyObject(quat))
			return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec4(): the Color being assigned references invalid data" );
		
		*p1 = quat->quat[0];
		*p2 = quat->quat[2];
		*p3 = quat->quat[2];
		*p4 = quat->quat[4];
		return 0;	
	} else {
		return EXPP_ReturnIntError( PyExc_RuntimeError,
					"EXPP_setVec4(): Incompatible type assignment, only accept a tuple, list, sequence or Vector" );
	}
	*p1 = f1; *p2 = f2; *p3 = f3; *p4 = f4;
	return 0;
}

/*
 * Utility routines to range-check and store various datatypes.  The object 
 * type is checked and a exception is raised if it's not the correct type.  
 * An exception is also raised if the value lies outside of the specified
 * range.  
 *
 * Inputs:
 *    value: PyObject containing the new value
 *    param: pointer to destination variable
 *    max, min: valid range for value
 *    type: kind of pointer and data (uses the same characters as
 *       PyArgs_ParseTuple() and Py_BuildValue()
 *
 * Return 0 on success, -1 on error.
 */

int EXPP_setFloatRange( PyObject *value, float *param,
								float min, float max )
{
	char errstr[128];
	float number;

	sprintf ( errstr, "expected int argument in [%f,%f]", min, max );

	if( !PyNumber_Check ( value ) )
		return EXPP_ReturnIntError( PyExc_TypeError, errstr );

	number = (float)PyFloat_AsDouble( value );
	if ( number < min || number > max )
		return EXPP_ReturnIntError( PyExc_ValueError, errstr );

	*param = number;
	return 0;
}

int EXPP_setIValueRange( PyObject *value, void *param,
								int min, int max, char type )
{
	char errstr[128];
	int number;

	sprintf ( errstr, "expected int argument in [%d,%d]", min, max );

	if( !PyInt_Check( value ) )
		return EXPP_ReturnIntError( PyExc_TypeError, errstr );

	number = PyInt_AS_LONG( value );
	if( number < min || number > max )
		return EXPP_ReturnIntError( PyExc_ValueError, errstr );

	switch ( type ) {
	case 'b':
		*(char *)param = (char)number;
		return 0;
	case 'h':
		*(short *)param = (short)number;
		return 0;
	case 'H':
		*(unsigned short *)param = (unsigned short)number;
		return 0;
	case 'i':
		*(int *)param = number;
		return 0;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
			   "EXPP_setIValueRange(): invalid type code" );
	}
}

/*
 * Utility routines to get/set bits in bitfields.  Adapted from code in 
 * sceneRender.c (thanks, ascotan!).  
 *
 * Inputs:
 *    param: pointer to source/destination variable
 *    setting: the bit to get/set
 *    type: pointer type ('h' == short, 'i' == integer)
 */

PyObject *EXPP_getBitfield( void *param, int setting, char type )
{
	PyObject *ret;
	switch ( type ) {
	case 'b':
		ret = (*(char *)param & setting) ? Py_True : Py_False;
		break;
	case 'h':
		ret = (*(short *)param & setting) ? Py_True : Py_False;
		break;
	case 'i':
		ret = (*(int *)param & setting)	? Py_True : Py_False;
		break;
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
			   "EXPP_getBit(): invalid type code" );
	}
	Py_INCREF(ret);
	return ret;
}

int EXPP_setBitfield( PyObject * value, void *param, int setting, char type )
{
	int flag = PyObject_IsTrue( value );

	switch ( type ) {
	case 'b':
		if ( flag )
			*(char *)param |= setting;
		else
			*(char *)param &= ~setting;
		return 0;
	case 'h':
		if ( flag )
			*(short *)param |= setting;
		else
			*(short *)param &= ~setting;
		return 0;
	case 'i':
		if ( flag )
			*(int *)param |= setting;
		else
			*(int *)param &= ~setting;
		return 0;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
			   "EXPP_setBit(): invalid type code" );
	}
}

/*
 * Procedure to handle older setStuff() methods.  Assumes that argument 
 * is a tuple with one object, and so grabs the object and passes it to
 * the specified tp_getset setter for the corresponding attribute.
 */

PyObject *EXPP_setterWrapper ( PyObject * self, PyObject * args,
				setter func)
{
	int error;

	if ( !PyTuple_Check( args ) || PyTuple_Size( args ) != 1 )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "expected tuple of one item" );

	error = func ( self, PyTuple_GET_ITEM( args, 0 ), NULL );
	if ( !error ) {
		Py_INCREF( Py_None );
		return Py_None;
	} else
		return NULL;
}

/*
 * Procedure to handle older setStuff() methods.  Assumes that argument 
 * is a tuple, so just passes it to the specified tp_getset setter for 
 * the corresponding attribute.
 */

PyObject *EXPP_setterWrapperTuple ( PyObject * self, PyObject * args,
									setter func)
{
	int error;

	if ( !PyTuple_Check( args ) )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "expected tuple" );

	error = func ( self, args, NULL );
	if ( !error ) {
		Py_INCREF( Py_None );
		return Py_None;
	} else
		return NULL;
}

/*
 * Helper to keep dictionaries from causing memory leaks.  When some object
 * is just created to be added to the dictionary, its reference count needs
 * to be decremented so it can be reclaimed.
 */

int EXPP_dict_set_item_str( PyObject *dict, char *key, PyObject *value)
{
   	/* add value to dictionary */
	int ret = PyDict_SetItemString(dict, key, value);
	Py_DECREF( value ); /* delete original */
	return ret;
}

/*
 * Helper function for subtypes that use the base types methods.
 * The command below needs to have args modified to have 'self' added at the start
 * ret = PyObject_Call(PyDict_GetItemString(PyList_Type.tp_dict, "sort"), args, keywds);
 * 
 * This is not easy with the python API so adding a function here,
 * remember to Py_DECREF the tuple after
 */

PyObject * EXPP_PyTuple_New_Prepend(PyObject *tuple, PyObject *value)
{
	PyObject *item;
	PyObject *new_tuple;
	int i;
	
	i = PyTuple_Size(tuple);
	new_tuple = PyTuple_New(i+1);
	PyTuple_SetItem(new_tuple, 0, value);
	Py_INCREF(value);
	while (i) {
		i--;
		item = PyTuple_GetItem(tuple, i);
		PyTuple_SetItem(new_tuple, i+1, item);
		Py_INCREF(item);
	}
	return new_tuple;
}

void EXPP_PyMethodsToDict(PyObject *dict, struct PyMethodDef *meth)
{
	PyObject *value;
	while (meth->ml_name) {
		value = PyCFunction_New(meth, NULL);
		PyDict_SetItemString(dict, meth->ml_name, value);
		Py_DECREF(value);
		meth++;
	}
}

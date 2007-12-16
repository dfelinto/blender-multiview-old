/* 
 * $Id: charRGBA.c 12898 2007-12-15 21:44:40Z campbellbarton $
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
 * Contributor(s): Willian P. Germano
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include "charRGBA.h" /*This must come first */
#include "gen_utils.h"

/* This file is heavily based on the old bpython Constant object code in
	 Blender */

/*****************************************************************************/
/* Python charRGBA_Type callback function prototypes:			  */
/*****************************************************************************/
static PyObject *V24_charRGBA_repr( V24_BPy_charRGBA * self );

static int V24_charRGBALength( V24_BPy_charRGBA * self );

static PyObject *V24_charRGBASubscript( V24_BPy_charRGBA * self, PyObject * key );
static int V24_charRGBAAssSubscript( V24_BPy_charRGBA * self, PyObject * who,
				 PyObject * cares );

static PyObject *V24_charRGBAItem( V24_BPy_charRGBA * self, int i );
static int V24_charRGBAAssItem( V24_BPy_charRGBA * self, int i, PyObject * ob );
static PyObject *V24_charRGBASlice( V24_BPy_charRGBA * self, int begin, int end );
static int V24_charRGBAAssSlice( V24_BPy_charRGBA * self, int begin, int end,
			     PyObject * seq );
static PyObject *V24_charRGBA_getColor( V24_BPy_charRGBA * self, void * type);
static int V24_charRGBA_setColor( V24_BPy_charRGBA * self, PyObject * value, void * type);

/*****************************************************************************/
/* Python charRGBA_Type Mapping Methods table:			*/
/*****************************************************************************/
static PyMappingMethods charRGBAAsMapping = {
	( inquiry ) V24_charRGBALength,	/* mp_length                            */
	( binaryfunc ) V24_charRGBASubscript,	/* mp_subscript                 */
	( objobjargproc ) V24_charRGBAAssSubscript,	/* mp_ass_subscript */
};

/*****************************************************************************/
/* Python charRGBA_Type Sequence Methods table:			*/
/*****************************************************************************/
static PySequenceMethods charRGBAAsSequence = {
	( inquiry ) V24_charRGBALength,	/* sq_length */
	( binaryfunc ) 0,	/* sq_concat */
	( intargfunc ) 0,	/* sq_repeat */
	( intargfunc ) V24_charRGBAItem,	/* sq_item */
	( intintargfunc ) V24_charRGBASlice,	/* sq_slice */
	( intobjargproc ) V24_charRGBAAssItem,	/* sq_ass_item */
	( intintobjargproc ) V24_charRGBAAssSlice,	/* sq_ass_slice       */
};

static PyGetSetDef charRGBA_getseters[] = {
	{"R",
	 (getter)V24_charRGBA_getColor, (setter)V24_charRGBA_setColor,
	 "the red component",
	 (void *) 0},
	{"r",
	 (getter)V24_charRGBA_getColor, (setter)V24_charRGBA_setColor,
	 "the red component",
	 (void *) 0},
	{"G",
	 (getter)V24_charRGBA_getColor, (setter)V24_charRGBA_setColor,
	 "the green component",
	 (void *) 1},
	{"g",
	 (getter)V24_charRGBA_getColor, (setter)V24_charRGBA_setColor,
	 "the green component",
	 (void *) 1},
	{"B",
	 (getter)V24_charRGBA_getColor, (setter)V24_charRGBA_setColor,
	 "the blue component",
	 (void *) 2},
	{"b",
	 (getter)V24_charRGBA_getColor, (setter)V24_charRGBA_setColor,
	 "the blue component",
	 (void *) 2},
	{"A",
	 (getter)V24_charRGBA_getColor, (setter)V24_charRGBA_setColor,
	 "the alpha component",
	 (void *) 3},
	{"a",
	 (getter)V24_charRGBA_getColor, (setter)V24_charRGBA_setColor,
	 "the alpha component",
	 (void *) 3},
	{NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};

/*****************************************************************************/
/* Python charRGBA_Type structure definition:				*/
/*****************************************************************************/
PyTypeObject charRGBA_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"charRGBA",                 /* tp_name */
	sizeof( V24_BPy_charRGBA ),     /* tp_basicsize */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,			            /* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,                       /* cmpfunc tp_compare; */
	( reprfunc ) V24_charRGBA_repr,	/* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	&charRGBAAsSequence,	    /* PySequenceMethods *tp_as_sequence; */
	&charRGBAAsMapping,	        /* PyMappingMethods *tp_as_mapping; */

	/* More standard operations (here for binary compatibility) */

	NULL,                       /* hashfunc tp_hash; */
	NULL,                       /* ternaryfunc tp_call; */
	NULL,                       /* reprfunc tp_str; */
	NULL,                       /* getattrofunc tp_getattro; */
	NULL,                       /* setattrofunc tp_setattro; */

	/* Functions to access object as input/output buffer */
	NULL,                       /* PyBufferProcs *tp_as_buffer; */

  /*** Flags to define presence of optional/expanded features ***/
	Py_TPFLAGS_DEFAULT,         /* long tp_flags; */

	NULL,                       /*  char *tp_doc;  Documentation string */
  /*** Assigned meaning in release 2.0 ***/
	/* call function for all accessible objects */
	NULL,                       /* traverseproc tp_traverse; */

	/* delete references to contained objects */
	NULL,                       /* inquiry tp_clear; */

  /***  Assigned meaning in release 2.1 ***/
  /*** rich comparisons ***/
	NULL,                       /* richcmpfunc tp_richcompare; */

  /***  weak reference enabler ***/
	0,                          /* long tp_weaklistoffset; */

  /*** Added in release 2.2 ***/
	/*   Iterators */
	NULL,                       /* getiterfunc tp_iter; */
	NULL,                       /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	NULL,                       /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	charRGBA_getseters,         /* struct PyGetSetDef *tp_getset; */
	NULL,                       /* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	NULL,                       /* newfunc tp_new; */
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

/*****************************************************************************/
/* Function:	V24_charRGBA_New	                                        */
/*****************************************************************************/
PyObject *V24_charRGBA_New( char *rgba )
{
	V24_BPy_charRGBA *charRGBA = NULL;

	/*
	 * When called the first time, charRGBA_Type.tp_dealloc will be NULL.
	 * If that's the case, initialize the PyTypeObject.  If the
	 * initialization succeeds, then create a new object.
	 */

	if( charRGBA_Type.tp_dealloc || PyType_Ready( &charRGBA_Type ) >= 0 ) {
		charRGBA = ( V24_BPy_charRGBA * ) PyObject_NEW( V24_BPy_charRGBA,
				&charRGBA_Type );
	}

	if( charRGBA == NULL )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create charRGBA object" );

	/* rgba is a pointer to the first item of a char[4] array */
	charRGBA->rgba[0] = &rgba[0];
	charRGBA->rgba[1] = &rgba[1];
	charRGBA->rgba[2] = &rgba[2];
	charRGBA->rgba[3] = &rgba[3];

	return ( PyObject * ) charRGBA;
}

/*****************************************************************************/
/* Functions:	 V24_charRGBA_getCol and V24_charRGBA_setCol         	 */
/* Description:	 These functions get/set rgba color triplet values.	The  */
/*		 get function returns a tuple, the set one accepts three     */
/*		 chars (separated or in a tuple) as arguments.		    */
/*****************************************************************************/
PyObject *V24_charRGBA_getCol( V24_BPy_charRGBA * self )
{
	PyObject *list = PyList_New( 4 );

	if( !list )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create PyList" );

	PyList_SET_ITEM( list, 0, PyInt_FromLong( *(self->rgba[0])) );
	PyList_SET_ITEM( list, 1, PyInt_FromLong( *(self->rgba[1])) );
	PyList_SET_ITEM( list, 2, PyInt_FromLong( *(self->rgba[2])) );
	PyList_SET_ITEM( list, 3, PyInt_FromLong( *(self->rgba[3])) );
	return list;
}

PyObject *V24_charRGBA_setCol( V24_BPy_charRGBA * self, PyObject * args )
{
	int ok;
	char r = 0, g = 0, b = 0, a = 0;

	if( PyObject_Length( args ) == 4 )
		ok = PyArg_ParseTuple( args, "bbbb", &r, &g, &b, &a );

	else
		ok = PyArg_ParseTuple( args, "|(bbbb)", &r, &g, &b, &a );

	if( !ok )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected 1-byte ints [b,b,b,b] or b,b,b,b as arguments (or nothing)" );

	*( self->rgba[0] ) = (char)V24_EXPP_ClampInt( r, 0, 255 );
	*( self->rgba[1] ) = (char)V24_EXPP_ClampInt( g, 0, 255 );
	*( self->rgba[2] ) = (char)V24_EXPP_ClampInt( b, 0, 255 );
	*( self->rgba[3] ) = (char)V24_EXPP_ClampInt( a, 0, 255 );

	return V24_EXPP_incr_ret( Py_None );
}

/* return color value for one of the components */

static PyObject *V24_charRGBA_getColor( V24_BPy_charRGBA * self, void * type)
{
	int index = ((long)type) & 3; 
	return PyInt_FromLong ( *self->rgba[index] );
}

/* sets the color value of one of the components */

static int V24_charRGBA_setColor( V24_BPy_charRGBA * self, PyObject * value,
		void * type)
{
	int index = ((long)type) & 3; 
	PyObject *num = PyNumber_Int( value );

	/* argument must be a number */
	if( !num )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected char argument" );

	/* clamp valut to 0..255 then assign */
	*self->rgba[index] = (char)V24_EXPP_ClampInt( (int)PyInt_AS_LONG(value),
			0, 255 );
	Py_DECREF( num );
	return 0;
}

/*****************************************************************************/
/* Section:	 charRGBA as Mapping					 */
/*		 These functions provide code to access charRGBA objects as  */
/*		  mappings.						 */
/*****************************************************************************/
static int V24_charRGBALength( V24_BPy_charRGBA * self )
{
	return 4;
}

static PyObject *V24_charRGBASubscript( V24_BPy_charRGBA * self, PyObject * key )
{
	char *name = NULL;
	int i;

	if( PyNumber_Check( key ) )
		return V24_charRGBAItem( self, ( int ) PyInt_AsLong( key ) );

	if( !PyArg_ParseTuple( key, "s", &name ) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected int or string argument" );

	if( !strcmp( name, "R" ) || !strcmp( name, "r" ) )
		i = 0;
	else if( !strcmp( name, "G" ) || !strcmp( name, "g" ) )
		i = 1;
	else if( !strcmp( name, "B" ) || !strcmp( name, "b" ) )
		i = 2;
	else if( !strcmp( name, "A" ) || !strcmp( name, "a" ) )
		i = 3;
	else
		return V24_EXPP_ReturnPyObjError( PyExc_AttributeError, name );

	return PyInt_FromLong( (long)(*self->rgba[i]) );
}

static int V24_charRGBAAssSubscript( V24_BPy_charRGBA * self, PyObject * key,
				 PyObject * v )
{
	char *name = NULL;
	int i;

	if( !PyNumber_Check( v ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					    "value to assign must be a number" );

	if( PyNumber_Check( key ) )
		return V24_charRGBAAssItem( self, ( int ) PyInt_AsLong( key ), v );

	if( !PyArg_Parse( key, "s", &name ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					    "expected int or string argument" );

	if( !strcmp( name, "R" ) || !strcmp( name, "r" ) )
		i = 0;
	else if( !strcmp( name, "G" ) || !strcmp( name, "g" ) )
		i = 1;
	else if( !strcmp( name, "B" ) || !strcmp( name, "b" ) )
		i = 2;
	else if( !strcmp( name, "A" ) || !strcmp( name, "a" ) )
		i = 3;
	else
		return V24_EXPP_ReturnIntError( PyExc_AttributeError, name );

	*( self->rgba[i] ) = (char)V24_EXPP_ClampInt( PyInt_AsLong( v ), 0, 255 );

	return 0;
}

/*****************************************************************************/
/* Section:  charRGBA as Sequence					*/
/*	     These functions provide code to access charRGBA objects as	 */
/*		 sequences.						*/
/*****************************************************************************/
static PyObject *V24_charRGBAItem( V24_BPy_charRGBA * self, int i )
{
	if( i < 0 || i >= 4 )
		return V24_EXPP_ReturnPyObjError( PyExc_IndexError,
					      "array index out of range" );

	return PyInt_FromLong( *(self->rgba[i]) );
}

static PyObject *V24_charRGBASlice( V24_BPy_charRGBA * self, int begin, int end )
{
	PyObject *list;
	int count;

	if( begin < 0 )
		begin = 0;
	if( end > 4 )
		end = 4;
	if( begin > end )
		begin = end;

	list = PyList_New( end - begin );

	for( count = begin; count < end; count++ )
		PyList_SetItem( list, count - begin,
				PyInt_FromLong( *( self->rgba[count] ) ) );

	return list;
}

static int V24_charRGBAAssItem( V24_BPy_charRGBA * self, int i, PyObject * ob )
{
	if( i < 0 || i >= 4 )
		return V24_EXPP_ReturnIntError( PyExc_IndexError,
					    "array assignment index out of range" );

	if( !PyNumber_Check( ob ) )
		return V24_EXPP_ReturnIntError( PyExc_IndexError,
					    "color component must be a number" );

	*( self->rgba[i] ) = (char)V24_EXPP_ClampInt( PyInt_AsLong( ob ), 0, 255 );

	return 0;
}

static int V24_charRGBAAssSlice( V24_BPy_charRGBA * self, int begin, int end,
			     PyObject * seq )
{
	int count;

	if( begin < 0 )
		begin = 0;
	if( end > 4 )
		end = 4;
	if( begin > end )
		begin = end;

	if( !PySequence_Check( seq ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					    "illegal argument type for built-in operation" );

	if( PySequence_Length( seq ) != ( end - begin ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					    "size mismatch in slice assignment" );

	for( count = begin; count < end; count++ ) {
		char value;
		PyObject *ob = PySequence_GetItem( seq, count );

		if( !PyArg_Parse( ob, "b", &value ) ) {
			Py_DECREF( ob );
			return -1;
		}

		*( self->rgba[count] ) = (char)V24_EXPP_ClampInt( value, 0, 255 );

		Py_DECREF( ob );
	}

	return 0;
}

/*****************************************************************************/
/* Function:	V24_charRGBA_repr						*/
/* Description: This is a callback function for the V24_BPy_charRGBA type. It  */
/*		builds a meaninful string to represent charRGBA objects.   */
/*****************************************************************************/
static PyObject *V24_charRGBA_repr( V24_BPy_charRGBA * self )
{
	char r, g, b, a;

	r = *( self->rgba[0] );
	g = *( self->rgba[1] );
	b = *( self->rgba[2] );
	a = *( self->rgba[3] );

	return PyString_FromFormat( "[%d, %d, %d, %d]", r, g, b, a );
}

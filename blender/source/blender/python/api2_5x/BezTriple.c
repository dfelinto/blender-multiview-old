/*
 * $Id: BezTriple.c 11416 2007-07-29 14:30:06Z campbellbarton $
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
 * This is a new part of Blender.
 *
 * Contributor(s): Jacques Guignot RIP 2005,
 *    Stephen Swaney, Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "BezTriple.h" /*This must come first */
#include "DNA_ipo_types.h"

#include "MEM_guardedalloc.h"
#include "gen_utils.h"
#include "Const.h"

/****************************************************************************
  Python BPyBezTripleObject instance methods declarations:                        
****************************************************************************/
static int BezTriple_setPoints( BPyBezTripleObject * self, PyObject * args );
static PyObject *BezTriple_getPoints( BPyBezTripleObject * self );
static PyObject *BezTriple_getTriple( BPyBezTripleObject * self );

/*****************************************************************************/
/* PythonTypeObject constant declarations                                    */
/*****************************************************************************/

/* 
 * structure of "tuples" of constant's string name and int value
 *
 * For example, these two structures will define the constant category
 * "bpy.class.Object.DrawTypes" the constant 
 * "bpy.class.Object.DrawTypes.BOUNDBOX" and others.
 */

static constIdents handleTypesIdents[] = {
	{"FREE",   		{(int)HD_FREE}},
	{"AUTO",		{(int)HD_AUTO}},
	{"VECT",		{(int)HD_VECT}},
	{"ALIGN",		{(int)HD_ALIGN}},
	{"AUTOANIM",	{(int)HD_AUTO_ANIM}},
};

/* 
 * structure with constant internal type (int), constant category name,
 * and number of constant values in the category.
 */

static constDefinition handleTypes = {
	EXPP_CONST_INT, "HandleTypes",
		sizeof(handleTypesIdents)/sizeof(constIdents), handleTypesIdents
};

/*****************************************************************************/
/* Python BPyBezTripleObject methods table:                                        */
/*****************************************************************************/
static PyMethodDef BPyBezTriple_methods[] = {
	/* name, method, flags, doc */
	{"getTriple", ( PyCFunction ) BezTriple_getTriple, METH_NOARGS,
	 "() - return list of 3 floating point triplets.  order is H1, knot, H2"},
	{NULL, NULL, 0, NULL}
};


/****************************************************************************
 Function:    BezTriple_dealloc                                            
 Description: This is a callback function for the BPyBezTripleObject type. It is  
              the destructor function.                                     
****************************************************************************/
static void BezTriple_dealloc( BPyBezTripleObject * self )
{
	if( self->own_memory)
		MEM_freeN( self->beztriple );
	
	PyObject_DEL( self );
}

/*
 * BezTriple_getTriple
 * 
 * Get the coordinate data for a BezTriple.  Returns a list of 3 points.
 * List order is handle1, knot, handle2.  each point consists of a list
 * of x,y,z float values.
 */

static PyObject *BezTriple_getTriple( BPyBezTripleObject * self )
{
	BezTriple *bezt = self->beztriple;
	return Py_BuildValue( "[[fff][fff][fff]]",
				       bezt->vec[0][0], bezt->vec[0][1], bezt->vec[0][2],
				       bezt->vec[1][0], bezt->vec[1][1], bezt->vec[1][2],
				       bezt->vec[2][0], bezt->vec[2][1], bezt->vec[2][2] );
}

/*
 * BezTriple_setTriple
 *
 * Set the cordinate data for a BezTriple.  Takes a sequence of 3 points,
 * of the same format at BezTriple_getTriple.
 */

static int BezTriple_setTriple( BPyBezTripleObject * self, PyObject * args )
{
	int i, j;
	struct BezTriple *bezt = self->beztriple;
	float vec[3][3];

	if( !PySequence_Check( args ) || PySequence_Size( args ) != 3 )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected three sequences of three floats" );

	for( i = 0; i < 3; i++ ) {
		PyObject *obj1 = PySequence_ITEM( args, i );
		if( !PySequence_Check( obj1 ) || PySequence_Size( obj1 ) != 3 ) {
			Py_DECREF( obj1 );
			return EXPP_ReturnIntError( PyExc_TypeError,
					"expected three sequences of three floats" );
		}
		for( j = 0; j < 3; j++ ) {
			PyObject *obj2 = PySequence_ITEM( obj1, j );

			if( !PyNumber_Check(obj2) ) {
				Py_DECREF( obj1 );
				Py_DECREF( obj2 );
				return EXPP_ReturnIntError( PyExc_ValueError,
						"expected float parameter" );
			}
			vec[i][j] = ( float )PyFloat_AsDouble( obj2 );
		}
		Py_DECREF( obj1 );
	}

	for( i = 0; i < 3; i++ )
		for( j = 0; j < 3; j++ )
			bezt->vec[i][j] = vec[i][j];

	return 0;
}

/*
 * BezTriple_getPoint
 * 
 * Get the coordinate data for a BezTriple.  Returns the control point,
 * as a list of x,y float values.
 */

static PyObject *BezTriple_getPoints( BPyBezTripleObject * self )
{
	BezTriple *bezt = self->beztriple;
	return Py_BuildValue( "[ff]", bezt->vec[1][0], bezt->vec[1][1] );
}

/*
 * BezTriple_setPoint
 * 
 * Set the coordinate data for a BezTriple.  Accepts the x,y for the control
 * point and builds handle values based on control point.
 */

static int BezTriple_setPoints( BPyBezTripleObject * self, PyObject * args )
{
	int i;
	struct BezTriple *bezt = self->beztriple;
	float vec[2];

	if( !PySequence_Check( args ) || PySequence_Size( args ) != 2 )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected sequence of two floats" );

	for( i = 0; i < 2; i++ ) {
		PyObject *obj = PySequence_ITEM( args, i );
		vec[i] = ( float )PyFloat_AsDouble( obj );
		if (vec[i]==-1 && PyErr_Occurred()) {
			Py_DECREF(obj);
			return -1;
		}
	}

	for( i = 0; i < 2; i++ ) {
		bezt->vec[0][i] = vec[i] - 1;
		bezt->vec[1][i] = vec[i];
		bezt->vec[2][i] = vec[i] + 1;
	}

	/* experimental fussing with handles - ipo.c: calchandles_ipocurve */
	if( bezt->vec[0][0] > bezt->vec[1][0] )
		bezt->vec[0][0] = bezt->vec[1][0];

	if( bezt->vec[2][0] < bezt->vec[1][0] )
		bezt->vec[2][0] = bezt->vec[1][0];

	return 0;
}

static PyObject *BezTriple_getTilt( BPyBezTripleObject * self )
{
	return PyFloat_FromDouble( self->beztriple->alfa );
}

static int BezTriple_setTilt( BPyBezTripleObject * self, PyObject *value )
{
	float f =  ( float )PyFloat_AsDouble( value );
	if (f ==-1 && PyErr_Occurred())
		return -1;
	
	self->beztriple->alfa = f;
	return 0;
}

static PyObject *BezTriple_getWeight( BPyBezTripleObject * self )
{
	return PyFloat_FromDouble( self->beztriple->weight );
}

static int BezTriple_setWeight( BPyBezTripleObject * self, PyObject *value )
{
	float f =  ( float )PyFloat_AsDouble( value );
	if (f ==-1 && PyErr_Occurred())
		return -1;
	
	self->beztriple->weight = f;
	return 0;
}

static PyObject *BezTriple_getRadius( BPyBezTripleObject * self )
{
	return PyFloat_FromDouble( self->beztriple->radius );
}

static int BezTriple_setRadius( BPyBezTripleObject * self, PyObject *value )
{
	float f =  ( float )PyFloat_AsDouble( value );
	if (f ==-1 && PyErr_Occurred())
		return -1;
	
	self->beztriple->radius = f;
	return 0;
}

static PyObject *BezTriple_getHide( BPyBezTripleObject * self )
{
	return PyInt_FromLong( self->beztriple->hide == IPO_BEZ );
}

static int BezTriple_setHide( BPyBezTripleObject * self, PyObject *value )
{
	int pybool = PyObject_IsTrue( value );
	if (pybool==-1)
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected 1/0 or true/false" );
		
	if(pybool)
		self->beztriple->hide = IPO_BEZ;
	else
		self->beztriple->hide = 0;
	return 0;
}

static PyObject *BezTriple_getSelects( BPyBezTripleObject * self )
{
	BezTriple *bezt = self->beztriple;

	return Py_BuildValue( "[iii]", bezt->f1, bezt->f2, bezt->f3 );
}

static int BezTriple_setSelects( BPyBezTripleObject * self, PyObject *args )
{
	struct BezTriple *bezt = self->beztriple;
	PyObject *ob1, *ob2, *ob3;

       /* only accept a sequence of three booleans */

	if( !PySequence_Check( args ) || PySequence_Size( args ) != 3 )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected sequence of three integers" );

	ob1 = PySequence_ITEM( args, 0 );
	ob2 = PySequence_ITEM( args, 1 );
	ob3 = PySequence_ITEM( args, 2 );

       /* assign the selects */
	bezt->f1 = ( char )PyObject_IsTrue( ob1 );
	bezt->f2 = ( char )PyObject_IsTrue( ob2 );
	bezt->f3 = ( char )PyObject_IsTrue( ob3 );

	Py_DECREF( ob1 );
	Py_DECREF( ob2 );
	Py_DECREF( ob3 );

	return 0;
}

static PyObject *BezTriple_getHandle1( BPyBezTripleObject * self )
{
	return PyConst_NewInt( &handleTypes, self->beztriple->h1 );	
}

static PyObject *BezTriple_getHandle2( BPyBezTripleObject * self )
{
	return PyConst_NewInt( &handleTypes, self->beztriple->h2 );
}

static int BezTriple_setHandle1( BPyBezTripleObject * self, PyObject *value  )
{
	constValue *c = Const_FromPyObject( &handleTypes, value);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected HandleTypes constant or string" );
	
	self->beztriple->h1 = c->i;
	return 0;
}

static int BezTriple_setHandle2( BPyBezTripleObject * self, PyObject *value  )
{
	constValue *c = Const_FromPyObject( &handleTypes, value);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected HandleType constant or string" );
	
	self->beztriple->h2 = c->i;
	return 0;
}

/*
 * Python BezTriple attributes get/set structure
 */

static PyGetSetDef BPyBezTriple_getset[] = {
	{"pt",
	 (getter)BezTriple_getPoints, (setter)BezTriple_setPoints,
	 "point knot values",
	 NULL},
	{"vec",
	 (getter)BezTriple_getTriple, (setter)BezTriple_setTriple,
	 "point handle and knot values",
	 NULL},
	{"tilt",
	 (getter)BezTriple_getTilt, (setter)BezTriple_setTilt,
	 "point tilt",
	 NULL},
	{"hide",
	 (getter)BezTriple_getHide, (setter)BezTriple_setHide,
	 "point hide status",
	 NULL},
	{"selects",
	 (getter)BezTriple_getSelects, (setter)BezTriple_setSelects,
	 "point select statuses",
	 NULL},
	{"handleType1",
	 (getter)BezTriple_getHandle1, (setter)BezTriple_setHandle1,
	 "point handle type 1",
	 NULL},
	{"handleType2",
	 (getter)BezTriple_getHandle2, (setter)BezTriple_setHandle2,
	 "point handle type 2",
	 NULL},
	{"weight",
	 (getter)BezTriple_getWeight, (setter)BezTriple_setWeight,
	 "point weight",
	 NULL},
	{"radius",
	 (getter)BezTriple_getRadius, (setter)BezTriple_setRadius,
	 "point radius",
	 NULL},
	{NULL}  /* Sentinel */
};

/*****************************************************************************/
/* Function:    BezTriple_repr                                               */
/* Description: This is a callback function for the BPyBezTripleObject type. It   */
/*              builds a meaninful string to represent BezTriple objects.    */
/*****************************************************************************/
static PyObject *BezTriple_repr( BPyBezTripleObject * self )
{
	char str[512];
	sprintf( str,
		"[BezTriple [%.6f, %.6f, %.6f] [%.6f, %.6f, %.6f] [%.6f, %.6f, %.6f]\n",
		 self->beztriple->vec[0][0], self->beztriple->vec[0][1], self->beztriple->vec[0][2],
		 self->beztriple->vec[1][0], self->beztriple->vec[1][1], self->beztriple->vec[1][2],
		 self->beztriple->vec[2][0], self->beztriple->vec[2][1], self->beztriple->vec[2][2]);
	return PyString_FromString( str );
}


/****************************************************************************
 Function:              M_BezTriple_New                                   
 Python equivalent:     Blender.BezTriple.New                             
****************************************************************************/

static PyObject *BezTriple_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	float numbuf[9];
	PyObject* in_args = NULL;
	int length;

	/* accept list, tuple, or 3 or 9 args (which better be floats) */

	length = PyTuple_Size( args );
	if( length == 3 || length == 9 )
		in_args = args;
	else if( !PyArg_ParseTuple( args, "|O", &in_args) )
		goto TypeError;

	if( !in_args ) {
		numbuf[0] = 0.0f; numbuf[1] = 0.0f; numbuf[2] = 0.0f;
		numbuf[3] = 0.0f; numbuf[4] = 0.0f; numbuf[5] = 0.0f;
		numbuf[6] = 0.0f; numbuf[7] = 0.0f; numbuf[8] = 0.0f;
	} else {
		int i, length;
		if( !PySequence_Check( in_args ) )
			goto TypeError;

		length = PySequence_Length( in_args );
		if( length != 9 && length != 3 )
			goto TypeError;
		
		for( i = 0; i < length; i++ ) {
			PyObject *item, *pyfloat;
			item = PySequence_ITEM( in_args, i);
			if( !item )
				goto TypeError;
			pyfloat = PyNumber_Float( item );
			Py_DECREF( item );
			if( !pyfloat )
				goto TypeError;
			numbuf[i] = ( float )PyFloat_AS_DOUBLE( pyfloat );
			Py_DECREF( pyfloat );
		}

		if( length == 3 ) {
			numbuf[3] = numbuf[0]; numbuf[6] = numbuf[0];
			numbuf[4] = numbuf[1]; numbuf[7] = numbuf[1];
			numbuf[5] = numbuf[2]; numbuf[8] = numbuf[2];
		}
	}

	return newBezTriple( numbuf );

TypeError:
	return EXPP_ReturnPyObjError( PyExc_TypeError,
			"expected sequence of 3 or 9 floats or nothing" );
}

/************************************************************************
 *
 * Python BPyBezTriple_Type structure definition
 *
 ************************************************************************/

PyTypeObject BPyBezTriple_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"BezTriple",                /* char *tp_name; */
	sizeof( BPyBezTripleObject ),    /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) BezTriple_dealloc,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,                       /* cmpfunc tp_compare; */
	( reprfunc ) BezTriple_repr,     /* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	NULL,	        			/* PySequenceMethods *tp_as_sequence; */
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
	BPyBezTriple_methods,      /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyBezTriple_getset,    /* struct PyGetSetDef *tp_getset; */
	NULL,                       /* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc ) BezTriple_new,	/* newfunc tp_new; */
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



/* Three Python BPyBezTriple_Type helper functions needed by the Object module: */

/****************************************************************************
 Function:    BezTriple_CreatePyObject                                    
 Description: This function will create a new BPyBezTripleObject from an existing 
              Blender ipo structure.                                       
****************************************************************************/
PyObject *BezTriple_CreatePyObject( BezTriple * bzt )
{
	BPyBezTripleObject *pybeztriple;

	if (!bzt)
		Py_RETURN_NONE;
	
	pybeztriple =
		( BPyBezTripleObject * ) PyObject_NEW( BPyBezTripleObject, &BPyBezTriple_Type );

	if( !pybeztriple )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create BPyBezTripleObject object" );

	pybeztriple->beztriple = bzt;
	pybeztriple->own_memory = 0;

	return ( PyObject * ) pybeztriple;
}

/*
  Create a new BezTriple
  input args is a sequence - either 3 or 9 floats
*/

PyObject *newBezTriple( float *numbuf )
{
	int i, j, num;
	PyObject *pyobj = NULL;
	BezTriple *bzt = NULL;

	/* create our own beztriple data */
	bzt = MEM_callocN( sizeof( BezTriple ), "new bpytriple");

	/* check malloc */
	if( !bzt )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					       "MEM_callocN failed");

	/* copy the data */
	num = 0;
	for( i = 0; i < 3; i++ ) {
		for( j = 0; j < 3; j++) {
			bzt->vec[i][j] = numbuf[num++];
		}
	}
	bzt->h1 = HD_ALIGN;
	bzt->h2 = HD_ALIGN;

	/* wrap it */
	pyobj = BezTriple_CreatePyObject( bzt );

  	/* we own it. must free later */
	( ( BPyBezTripleObject * )pyobj)->own_memory = 1;

	return pyobj;
}

/*
  BezTriple_Init
*/

PyObject *BezTripleType_Init( void )
{
	/* only allocate new dictionary once */
	if( BPyBezTriple_Type.tp_dict == NULL ) {
		BPyBezTriple_Type.tp_dict = PyDict_New();
		PyConstCategory_AddObjectToDict( BPyBezTriple_Type.tp_dict, &handleTypes );
		PyType_Ready( &BPyBezTriple_Type ) ;
	}
	return (PyObject *) &BPyBezTriple_Type ;
}

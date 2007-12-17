/* 
 * $Id: Curve.c 11367 2007-07-25 13:01:44Z campbellbarton $
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
 * Contributor(s): Jacques Guignot, Stephen Swaney
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "Curve.h" /*This must come first*/
#include "CurveBase.h" /*This must come first*/

#include "BLI_blenlib.h"
#include "BKE_main.h"
#include "BKE_displist.h"
#include "BKE_global.h"
#include "BKE_library.h"
#include "BKE_curve.h"
#include "BKE_material.h"
#include "MEM_guardedalloc.h"	/* because we wil be mallocing memory */
#include "CurNurb.h"
#include "SurfNurb.h"
#include "Material.h"
#include "Object.h"
#include "Key.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "mydevice.h"
#include "bpy_list.h"


/*****************************************************************************/
/* The following string definitions are used for documentation strings.      */
/* In Python these will be written to the console when doing a               */
/*  Blender.Curve.__doc__                                                    */
/*****************************************************************************/

char M_Curve_doc[] = "";

/*****************************************************************************/
/*  Python API function prototypes for the Curve module.                     */
/*****************************************************************************/

/*****************************************************************************/
/*  Python BPyCurveBaseObject instance methods declarations:                          */
/*****************************************************************************/

static PyObject *Curve_getPathLen( BPyCurveBaseObject * self );
static int Curve_setPathLen( BPyCurveBaseObject * self, PyObject * args );
static PyObject *Curve_getLoc( BPyCurveBaseObject * self );
static int Curve_setLoc( BPyCurveBaseObject * self, PyObject * args );
static PyObject *Curve_getRot( BPyCurveBaseObject * self );
static int Curve_setRot( BPyCurveBaseObject * self, PyObject * args );
static PyObject *Curve_getSize( BPyCurveBaseObject * self );
static int Curve_setSize( BPyCurveBaseObject * self, PyObject * args );
static PyObject *Curve_getKey( BPyCurveBaseObject * self );

static PyObject *Curve_getBevOb( BPyCurveBaseObject * self );
static int Curve_setBevOb( BPyCurveBaseObject * self, PyObject * args );

static PyObject *Curve_getTaperOb( BPyCurveBaseObject * self );
static int Curve_setTaperOb( BPyCurveBaseObject * self, PyObject * args );
static PyObject *Curve_copy( BPyCurveBaseObject * self );


struct chartrans *text_to_curve( Object * ob, int mode );
/*****************************************************************************/
/* Python BPyCurveBaseObject methods:                                        */
/* gives access to                                                           */
/* name, pathlen totcol flag bevresol                                        */
/* resolu resolv width ext1 ext2                                             */
/* controlpoint loc rot size                                                 */
/* numpts                                                                    */
/*****************************************************************************/

static PyObject *Curve_getPathLen( BPyCurveBaseObject * self )
{
	return PyInt_FromLong( ( long ) self->curve->pathlen );
}


static int Curve_setPathLen( BPyCurveBaseObject * self, PyObject * value )
{
	PyObject *num;

	if( !PyNumber_Check( value ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected int argument" );

	num = PyNumber_Int( value );
	self->curve->pathlen = (short)PyInt_AS_LONG( num );
	Py_DECREF( num );

	return 0;
}

PyObject *Curve_getMode( BPyCurveBaseObject * self )
{
	return PyInt_FromLong( ( long ) self->curve->flag );
}


int Curve_setMode( BPyCurveBaseObject * self, PyObject * args )
{
	PyObject *num;

	if( !PyNumber_Check( args ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected int argument" );

	num = PyNumber_Int( args );
	self->curve->flag = (short)PyInt_AS_LONG( num );
	Py_DECREF( num );

	return 0;
}

PyObject *Curve_getBevresol( BPyCurveBaseObject * self )
{
	return PyInt_FromLong( ( long ) self->curve->bevresol );
}

int Curve_setBevresol( BPyCurveBaseObject * self, PyObject * args )
{
	short value;
	PyObject *num;

	if( !PyNumber_Check( args ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected int argument" );

	num = PyNumber_Int( args );
	value = (short)PyInt_AS_LONG( num );
	Py_DECREF( num );

	if( value > 10 || value < 0 )
		return EXPP_ReturnIntError( PyExc_ValueError,
				"acceptable values are between 0 and 10" );

	self->curve->bevresol = value;
	return 0;
}


PyObject *Curve_getResolu( BPyCurveBaseObject * self )
{
	return PyInt_FromLong( ( long ) self->curve->resolu );
}


int Curve_setResolu( BPyCurveBaseObject * self, PyObject * args )
{
	short value;
	Nurb *nu;
	PyObject *num;

	if( !PyNumber_Check( args ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected int argument" );

	num = PyNumber_Int( args );
	value = (short)PyInt_AS_LONG( num );
	Py_DECREF( num );

	if( value > 128 || value < 1 )
		return EXPP_ReturnIntError( PyExc_ValueError,
				"acceptable values are between 1 and 128" );

	self->curve->resolu = value;
	/* propagate the change through all the curves */
	for( nu = self->curve->nurb.first; nu; nu = nu->next )
		nu->resolu = value;

	return 0;
}

PyObject *Curve_getResolv( BPyCurveBaseObject * self )
{
	return PyInt_FromLong( ( long ) self->curve->resolv );
}

int Curve_setResolv( BPyCurveBaseObject * self, PyObject * args )
{
	short value;
	PyObject *num;

	if( !PyNumber_Check( args ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected int argument" );

	num = PyNumber_Int( args );
	value = (short)PyInt_AS_LONG( num );
	Py_DECREF( num );

	if(value > 128 || value < 1)
		return EXPP_ReturnIntError( PyExc_ValueError,
			"acceptable values are between 1 and 128" );
	self->curve->resolv = value;

	return 0;
}

PyObject *Curve_getWidth( BPyCurveBaseObject * self )
{
	return PyFloat_FromDouble( ( double ) self->curve->width );
}


int Curve_setWidth( BPyCurveBaseObject * self, PyObject * args )
{
	float value;
	PyObject *num;

	if( !PyNumber_Check( args ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected float argument" );

	num = PyNumber_Float( args );
	value = (float)PyFloat_AS_DOUBLE( num );
	Py_DECREF( num );

	if(value > 2.0f || value < 0.0f)
		return EXPP_ReturnIntError( PyExc_ValueError,
				"acceptable values are between 2.0 and 0.0" );
	self->curve->width = value;

	return 0;
}


PyObject *Curve_getExt1( BPyCurveBaseObject * self )
{
	return PyFloat_FromDouble( ( double ) self->curve->ext1 );
}


int Curve_setExt1( BPyCurveBaseObject * self, PyObject * args )
{
	float value;
	PyObject *num;

	if( !PyNumber_Check( args ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected float argument" );

	num = PyNumber_Float( args );
	value = (float)PyFloat_AS_DOUBLE( num );
	Py_DECREF( num );

	if(value > 5.0f || value < 0.0f)
		return EXPP_ReturnIntError( PyExc_ValueError,
				"acceptable values are between 0.0 and 5.0" );
	self->curve->ext1 = value;

	return 0;
}

PyObject *Curve_getExt2( BPyCurveBaseObject * self )
{
	return PyFloat_FromDouble( ( double ) self->curve->ext2 );
}


int Curve_setExt2( BPyCurveBaseObject * self, PyObject * args )
{
	float value;
	PyObject *num;

	if( !PyNumber_Check( args ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected float argument" );

	num = PyNumber_Float( args );
	value = (float)PyFloat_AS_DOUBLE( num );
	Py_DECREF( num );

	if(value > 2.0f || value < 0.0f)
		return EXPP_ReturnIntError( PyExc_ValueError,
				"acceptable values are between 0.0 and 2.0" );
	self->curve->ext2 = value;

	return 0;
}

static PyObject *Curve_getLoc( BPyCurveBaseObject * self )
{
	return Py_BuildValue( "[f,f,f]", self->curve->loc[0],
				self->curve->loc[1], self->curve->loc[2] );
}

static int Curve_setLoc( BPyCurveBaseObject * self, PyObject * args )
{
	float loc[3];
	int i;

	if( ( !PyList_Check( args ) && !PyTuple_Check( args ) ) ||
			PySequence_Size( args ) != 3 ) {
TypeError:
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected a sequence of three floats" );
	}

	for( i = 0; i < 3; i++ ) {
		PyObject *item = PySequence_GetItem( args, i );
		PyObject *num = PyNumber_Float( item );
		Py_DECREF( item );
		if( !num )
			goto TypeError;
		loc[i] = PyFloat_AS_DOUBLE( num );
		Py_DECREF( num );
	}
	memcpy( self->curve->loc, loc, sizeof( loc ) );

	return 0;
}

static PyObject *Curve_getRot( BPyCurveBaseObject * self )
{
	return Py_BuildValue( "[f,f,f]", self->curve->rot[0],
				self->curve->rot[1], self->curve->rot[2] );
}

static int Curve_setRot( BPyCurveBaseObject * self, PyObject * args )
{
	float rot[3];
	int i;

	if( ( !PyList_Check( args ) && !PyTuple_Check( args ) ) ||
			PySequence_Size( args ) != 3 ) {
TypeError:
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected a sequence of three floats" );
	}

	for( i = 0; i < 3; i++ ) {
		PyObject *item = PySequence_GetItem( args, i );
		PyObject *num = PyNumber_Float( item );
		Py_DECREF( item );
		if( !num )
			goto TypeError;
		rot[i] = PyFloat_AS_DOUBLE( num );
		Py_DECREF( num );
	}
	memcpy( self->curve->rot, rot, sizeof( rot ) );

	return 0;
}

static PyObject *Curve_getSize( BPyCurveBaseObject * self )
{
	return Py_BuildValue( "[f,f,f]", self->curve->size[0],
				self->curve->size[1], self->curve->size[2] );
}

static int Curve_setSize( BPyCurveBaseObject * self, PyObject * args )
{
	float size[3];
	int i;

	if( ( !PyList_Check( args ) && !PyTuple_Check( args ) ) ||
			PySequence_Size( args ) != 3 ) {
TypeError:
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected a sequence of three floats" );
	}

	for( i = 0; i < 3; i++ ) {
		PyObject *item = PySequence_GetItem( args, i );
		PyObject *num = PyNumber_Float( item );
		Py_DECREF( item );
		if( !num )
			goto TypeError;
		size[i] = PyFloat_AS_DOUBLE( num );
		Py_DECREF( num );
	}
	memcpy( self->curve->size, size, sizeof( size ) );

	return 0;
}

/*
 * get the key object linked to this curve
 */

static PyObject *Curve_getKey( BPyCurveBaseObject * self )
{
	return Key_CreatePyObject(self->curve->key);
}

/*****************************************************************************/
/* Function:    Curve_getBevOb                                               */
/* Description: Get the bevel object assign to the curve.                    */
/*****************************************************************************/
static PyObject *Curve_getBevOb( BPyCurveBaseObject * self)
{	/* None is ok */
	return Object_CreatePyObject( self->curve->bevobj );
}

/*****************************************************************************/
/* Function:    Curve_setBevOb                                            */
/* Description: Assign a bevel object to the curve.                          */
/*****************************************************************************/
static int Curve_setBevOb( BPyCurveBaseObject * self, PyObject * args )
{
	if (BPyObject_Check( args ) && ((BPyObject *)args)->object->data == self->curve )
		return EXPP_ReturnIntError( PyExc_ValueError,
				"Can't bevel an object to itself" );
	
	return GenericLib_assignData(args, (void **) &self->curve->bevobj, 0, 0, ID_OB, OB_CURVE);
}

/*****************************************************************************/
/* Function:    Curve_getTaperOb                                             */
/* Description: Get the taper object assign to the curve.                    */
/*****************************************************************************/

static PyObject *Curve_getTaperOb( BPyCurveBaseObject * self)
{	/* None is ok */
	return Object_CreatePyObject( self->curve->taperobj );
}

/*****************************************************************************/
/* Function:    Curve_setTaperOb                                          */
/* Description: Assign a taper object to the curve.                          */
/*****************************************************************************/

static int Curve_setTaperOb( BPyCurveBaseObject * self, PyObject * args )
{
	if (BPyObject_Check( args ) && ((BPyObject *)args)->object->data == self->curve )
		return EXPP_ReturnIntError( PyExc_ValueError,
				"Can't taper an object to itself" );
	
	return GenericLib_assignData(args, (void **) &self->curve->taperobj, 0, 0, ID_OB, OB_CURVE);
}

/* get floating point attributes */
static PyObject *getFlagAttr( BPyCurveBaseObject *self, void *type )
{
	if (self->curve->flag & (int)type)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

/* set floating point attributes which require clamping */
static int setFlagAttr( BPyCurveBaseObject *self, PyObject *value, void *type )
{
	if (PyObject_IsTrue(value))
		self->curve->flag |= (int)type;
	else
		self->curve->flag &= ~(int)type;
	return 0;
}

/*****************************************************************************/
/* Function:    Curve_copy                                                   */
/* Description: Return a copy of this curve data.                            */
/*****************************************************************************/

PyObject *Curve_copy( BPyCurveBaseObject * self )
{
	BPyCurveBaseObject *pycurve;	/* for Curve Data object wrapper in Python */
	Curve *blcurve = 0;	/* for actual Curve Data we create in Blender */

	/* copies the data */
	blcurve = copy_curve( self->curve );	/* first create the Curve Data in Blender */

	if( blcurve == NULL )	/* bail out if add_curve() failed */
		return ( EXPP_ReturnPyObjError
			 ( PyExc_RuntimeError,
			   "couldn't create Curve Data in Blender" ) );

	/* return user count to zero because add_curve() inc'd it */
	blcurve->id.us = 0;
	
	/* create python wrapper obj */
	pycurve = ( BPyCurveBaseObject * ) PyObject_NEW( BPyCurveBaseObject, &BPyCurveBase_Type );

	if( pycurve == NULL )
		return ( EXPP_ReturnPyObjError
			 ( PyExc_MemoryError,
			   "couldn't create Curve Data object" ) );

	pycurve->curve = blcurve;	/* link Python curve wrapper to Blender Curve */
	return ( PyObject * ) pycurve;
}

/* attributes for curves */

static PyGetSetDef CurveBase_getset[] = {
	GENERIC_LIB_GETSETATTR_MATERIAL,
	{"flag",
	 (getter)Curve_getMode, (setter)Curve_setMode,
	 "The flag bitmask",
	NULL},
	{"bevresol",
	 (getter)Curve_getBevresol, (setter)Curve_setBevresol,
	 "The bevel resolution",
	NULL},
	{"resolu",
	 (getter)Curve_getResolu, (setter)Curve_setResolu,
	 "The resolution in U direction",
	NULL},
	{"resolv",
	 (getter)Curve_getResolv, (setter)Curve_setResolv,
	 "The resolution in V direction",
	NULL},
	{"width",
	 (getter)Curve_getWidth, (setter)Curve_setWidth,
	 "The curve width",
	NULL},
	{"ext1",
	 (getter)Curve_getExt1, (setter)Curve_setExt1,
	 "The extent1 value (for bevels)",
	NULL},
	{"ext2",
	 (getter)Curve_getExt2, (setter)Curve_setExt2,
	 "The extent2 value (for bevels)",
	NULL},
	{"loc",
	 (getter)Curve_getLoc, (setter)Curve_setLoc,
	 "The data location (from the center)",
	NULL},
	{"rot",
	 (getter)Curve_getRot, (setter)Curve_setRot,
	 "The data rotation (from the center)",
	NULL},
	{"size",
	 (getter)Curve_getSize, (setter)Curve_setSize,
	 "The data size (from the center)",
	NULL},
	{"bevob",
	 (getter)Curve_getBevOb, (setter)Curve_setBevOb,
	 "The bevel object",
	NULL},
	{"taperob",
	 (getter)Curve_getTaperOb, (setter)Curve_setTaperOb,
	 "The taper object",
	NULL},
	{"key",
	 (getter)Curve_getKey, (setter)NULL,
	 "The shape key for the curve (if any)",
	NULL},
	{"pathLen",
	 (getter)Curve_getPathLen, (setter)Curve_setPathLen,
	 "The shape key for the curve (if any)",
	NULL},
	
	 /* flags */
	{"enableFastEdit",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "Don't fill polygons while editing",
	 (void *)CU_FAST},
	{"enableUVOrco",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "",
	 (void *)CU_UV_ORCO},
	 
	{"drawFront",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "Don't fill polygons while editing",
	 (void *)CU_FRONT},
	{"drawBack",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "Don't fill polygons while editing",
	 (void *)CU_BACK},
	{"draw3D",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "Don't fill polygons while editing",
	 (void *)CU_3D},
	
	
	{NULL}  /* Sentinel */

};

/*****************************************************************************/
/*  Python BPyCurveBaseObject instance methods table:                                 */
/*****************************************************************************/
static PyMethodDef BPyCurveBase_methods[] = {
	{"__copy__", ( PyCFunction ) Curve_copy, METH_NOARGS,
	 "() - make a copy of this curve data"},
	{"copy", ( PyCFunction ) Curve_copy, METH_NOARGS,
	 "() - make a copy of this curve data"},
	{NULL, NULL, 0, NULL}
};


/*****************************************************************************/
/* Python BPyCurveBase_Type structure definition:                                   */
/*****************************************************************************/
PyTypeObject BPyCurveBase_Type = {
	PyObject_HEAD_INIT( NULL )          /* required macro */ 
	0,                                  /* ob_size */
	"CurveBase",						/* tp_name */
	sizeof( BPyCurveBaseObject ),		/* tp_basicsize */
	0,									/* tp_itemsize */
	/* methods */
	NULL, 								/* tp_dealloc */
	0,                                  /* tp_print */
	NULL,								/* tp_getattr */
	NULL,								/* tp_setattr */
	NULL,								/* tp_compare */
	NULL,								/* tp_repr */
	/* Method suites for standard classes */

	NULL,                               /* PyNumberMethods *tp_as_number; */
	NULL,								/* PySequenceMethods *tp_as_sequence; */
	NULL,                               /* PyMappingMethods *tp_as_mapping; */

	/* More standard operations (here for binary compatibility) */

	NULL,								/* hashfunc tp_hash; */
	NULL,                       	    /* ternaryfunc tp_call; */
	NULL,                       	    /* reprfunc tp_str; */
	NULL,                       	    /* getattrofunc tp_getattro; */
	NULL,                      	    /* setattrofunc tp_setattro; */

	/* Functions to access object as input/output buffer */
	NULL,                       	    /* PyBufferProcs *tp_as_buffer; */

  /*** Flags to define presence of optional/expanded features ***/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,  /* long tp_flags; */

	NULL,                               /*  char *tp_doc;  */
  /*** Assigned meaning in release 2.0 ***/
	/* call function for all accessible objects */
	NULL,                               /* traverseproc tp_traverse; */

	/* delete references to contained objects */
	NULL,                       	    /* inquiry tp_clear; */

  /***  Assigned meaning in release 2.1 ***/
  /*** rich comparisons ***/
	NULL,                       	    /* richcmpfunc tp_richcompare; */

  /***  weak reference enabler ***/
	0,                          	    /* long tp_weaklistoffset; */

  /*** Added in release 2.2 ***/
	/*   Iterators */
	NULL,								/* getiterfunc tp_iter; */
	NULL,								/* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	BPyCurveBase_methods,				/* struct PyMethodDef *tp_methods; */
	NULL,                       	    /* struct PyMemberDef *tp_members; */
	CurveBase_getset,					/* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,				/* struct _typeobject *tp_base; */
	NULL,                       	    /* PyObject *tp_dict; */
	NULL,                       	    /* descrgetfunc tp_descr_get; */
	NULL,                       	    /* descrsetfunc tp_descr_set; */
	0,                          	    /* long tp_dictoffset; */
	NULL,                       	    /* initproc tp_init; */
	NULL,                       	    /* allocfunc tp_alloc; */
	NULL,								/* newfunc tp_new; - WARNING - this type  */
	/*  Low-level free-memory routine */
	NULL,                       	    /* freefunc tp_free;  */
	/* For PyObject_IS_GC */
	NULL,                       	    /* inquiry tp_is_gc;  */
	NULL,                       	    /* PyObject *tp_bases; */
	/* method resolution order */
	NULL,                       	    /* PyObject *tp_mro;  */
	NULL,                       	    /* PyObject *tp_cache; */
	NULL,                       	    /* PyObject *tp_subclasses; */
	NULL,                       	    /* PyObject *tp_weaklist; */
	NULL
};

/* Note that this type should never be accessed by the user,
 * only its subtypes */
PyObject *CurveBaseType_Init( void  )
{
	PyType_Ready( &BPyCurveBase_Type ) ;
	return (PyObject *) &BPyCurveBase_Type ;
}


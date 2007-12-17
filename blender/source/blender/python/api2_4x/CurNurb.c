/*
 * $Id: CurNurb.c 12898 2007-12-15 21:44:40Z campbellbarton $
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
 * Contributor(s): Stephen Swaney, Campbell Barton, Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "CurNurb.h" /*This must come first */

#include "BKE_curve.h"
#include "BDR_editcurve.h"	/* for convertspline */
#include "MEM_guardedalloc.h"
#include "gen_utils.h"
#include "BezTriple.h"

/* Only for ME_SMOOTH */
#include "DNA_meshdata_types.h"

/*
 * forward declarations go here
 */

static PyObject *V24_M_CurNurb_New( PyObject * self, PyObject * args );
static PyObject *V24_CurNurb_oldsetMatIndex( V24_BPy_CurNurb * self, PyObject * args );
static int V24_CurNurb_setMatIndex( V24_BPy_CurNurb * self, PyObject * args );
static PyObject *V24_CurNurb_getMatIndex( V24_BPy_CurNurb * self );
static PyObject *V24_CurNurb_getFlagU( V24_BPy_CurNurb * self );
static PyObject *V24_CurNurb_oldsetFlagU( V24_BPy_CurNurb * self, PyObject * args );
static int V24_CurNurb_setFlagU( V24_BPy_CurNurb * self, PyObject * args );
static PyObject *V24_CurNurb_getFlagV( V24_BPy_CurNurb * self );
static PyObject *V24_CurNurb_oldsetFlagV( V24_BPy_CurNurb * self, PyObject * args );
static int V24_CurNurb_setFlagV( V24_BPy_CurNurb * self, PyObject * args );
static PyObject *V24_CurNurb_getType( V24_BPy_CurNurb * self );
static PyObject *V24_CurNurb_oldsetType( V24_BPy_CurNurb * self, PyObject * args );
static int V24_CurNurb_setType( V24_BPy_CurNurb * self, PyObject * args );
static PyObject *V24_CurNurb_getKnotsU( V24_BPy_CurNurb * self );
static PyObject *V24_CurNurb_getKnotsV( V24_BPy_CurNurb * self );
static PyObject *V24_CurNurb_getPoints( V24_BPy_CurNurb * self );
/* static PyObject* CurNurb_setXXX( V24_BPy_CurNurb* self, PyObject* args ); */
static int V24_CurNurb_setPoint( V24_BPy_CurNurb * self, int index, PyObject * ob );
static int V24_CurNurb_length( PyInstanceObject * inst );
static PyObject *V24_CurNurb_getIter( V24_BPy_CurNurb * self );
static PyObject *V24_CurNurb_iterNext( V24_BPy_CurNurb * self );
PyObject *V24_CurNurb_append( V24_BPy_CurNurb * self, PyObject * value );

static PyObject *V24_CurNurb_isNurb( V24_BPy_CurNurb * self );
static PyObject *V24_CurNurb_isCyclic( V24_BPy_CurNurb * self );
static PyObject *V24_CurNurb_dump( V24_BPy_CurNurb * self );
static PyObject *V24_CurNurb_switchDirection( V24_BPy_CurNurb * self );
static PyObject *V24_CurNurb_recalc( V24_BPy_CurNurb * self );
static PyObject *V24_CurNurb_getFlagBits( V24_BPy_CurNurb * self, void *type );
static int V24_CurNurb_setFlagBits( V24_BPy_CurNurb * self, PyObject *value, void *type );
char V24_M_CurNurb_doc[] = "CurNurb";


/*	
  V24_CurNurb_Type callback function prototypes:                          
*/

static int V24_CurNurb_compare( V24_BPy_CurNurb * a, V24_BPy_CurNurb * b );
static PyObject *V24_CurNurb_repr( V24_BPy_CurNurb * self );

/*
   table of module methods
   these are the equivalent of class or static methods.
   you do not need an object instance to call one.
  
*/

static PyMethodDef V24_M_CurNurb_methods[] = {
/*   name, method, flags, doc_string                */
	{"New", ( PyCFunction ) V24_M_CurNurb_New, METH_VARARGS | METH_KEYWORDS,
	 " () - doc string"},
/*  {"Get", (PyCFunction) M_CurNurb_method, METH_NOARGS, " () - doc string"}, */
/*   {"method", (PyCFunction) M_CurNurb_method, METH_NOARGS, " () - doc string"}, */

	{NULL, NULL, 0, NULL}
};



/*
 * method table
 * table of instance methods
 * these methods are invoked on an instance of the type.
*/

static PyMethodDef V24_BPy_CurNurb_methods[] = {
/*   name,     method,                    flags,         doc               */
/*  {"method", (PyCFunction) CurNurb_method, METH_NOARGS, " () - doc string"} */
	{"setMatIndex", ( PyCFunction ) V24_CurNurb_oldsetMatIndex, METH_VARARGS,
	 "( index ) - set index into materials list"},
	{"getMatIndex", ( PyCFunction ) V24_CurNurb_getMatIndex, METH_NOARGS,
	 "( ) - get current material index"},
	{"setFlagU", ( PyCFunction ) V24_CurNurb_oldsetFlagU, METH_VARARGS,
	 "( index ) - set flagU and recalculate the knots (0: uniform, 1: endpoints, 2: bezier)"},
	{"getFlagU", ( PyCFunction ) V24_CurNurb_getFlagU, METH_NOARGS,
	 "( ) - get flagU of the knots"},
	{"setFlagV", ( PyCFunction ) V24_CurNurb_oldsetFlagV, METH_VARARGS,
	 "( index ) - set flagV and recalculate the knots (0: uniform, 1: endpoints, 2: bezier)"},
	{"getFlagV", ( PyCFunction ) V24_CurNurb_getFlagV, METH_NOARGS,
	 "( ) - get flagV of the knots"},
	{"setType", ( PyCFunction ) V24_CurNurb_oldsetType, METH_VARARGS,
	 "( type ) - change the type of the curve (Poly: 0, Bezier: 1, NURBS: 4)"},
	{"getType", ( PyCFunction ) V24_CurNurb_getType, METH_NOARGS,
	 "( ) - get the type of the curve (Poly: 0, Bezier: 1, NURBS: 4)"},
	{"append", ( PyCFunction ) V24_CurNurb_append, METH_O,
	 "( point ) - add a new point.  arg is BezTriple or list of x,y,z,w floats"},
	{"isNurb", ( PyCFunction ) V24_CurNurb_isNurb, METH_NOARGS,
	 "( ) - boolean function tests if this spline is type nurb or bezier"},
	{"isCyclic", ( PyCFunction ) V24_CurNurb_isCyclic, METH_NOARGS,
	 "( ) - boolean function tests if this spline is cyclic (closed) or not (open)"},
	{"dump", ( PyCFunction ) V24_CurNurb_dump, METH_NOARGS,
	 "( ) - dumps Nurb data)"},
	{"switchDirection", ( PyCFunction ) V24_CurNurb_switchDirection, METH_NOARGS,
	 "( ) - swaps curve beginning and end)"},
	{"recalc", ( PyCFunction ) V24_CurNurb_recalc, METH_NOARGS,
	 "( ) - recalc Nurb data)"},
	{NULL, NULL, 0, NULL}
};

/* 
 *   methods for CurNurb as sequece
 */

static PySequenceMethods V24_CurNurb_as_sequence = {
	( inquiry ) V24_CurNurb_length,	/* sq_length   */
	( binaryfunc ) 0,	/* sq_concat */
	( intargfunc ) 0,	/* sq_repeat */
	( intargfunc ) V24_CurNurb_getPoint,	/* sq_item */
	( intintargfunc ) 0,	/* sq_slice */
	( intobjargproc ) V24_CurNurb_setPoint,	/* sq_ass_item */
	0,			/* sq_ass_slice */
	( objobjproc ) 0,	/* sq_contains */
	0,
	0
};

static PyGetSetDef V24_BPy_CurNurb_getseters[] = {
	{"mat_index",
	 (getter)V24_CurNurb_getMatIndex, (setter)V24_CurNurb_setMatIndex,
	 "CurNurb's material index",
	 NULL},
	{"points",
	 (getter)V24_CurNurb_getPoints, (setter)NULL,
	 "The number of curve points",
	 NULL},
	{"flagU",
	 (getter)V24_CurNurb_getFlagU, (setter)V24_CurNurb_setFlagU,
	 "The knot type in the U direction",
	 NULL},
	{"flagV",
	 (getter)V24_CurNurb_getFlagV, (setter)V24_CurNurb_setFlagV,
	 "The knot type in the V direction",
	 NULL},
	{"type",
	 (getter)V24_CurNurb_getType, (setter)V24_CurNurb_setType,
	 "The curve type (poly: bezier, or NURBS)",
	 NULL},
	{"knotsU",
	 (getter)V24_CurNurb_getKnotsU, (setter)NULL,
	 "The The knot vector in the U direction",
	 NULL},
	{"knotsV",
	 (getter)V24_CurNurb_getKnotsV, (setter)NULL,
	 "The The knot vector in the V direction",
	 NULL},
	{"smooth",
	 (getter)V24_CurNurb_getFlagBits, (setter)V24_CurNurb_setFlagBits,
	 "The smooth bool setting",
	 (void *)ME_SMOOTH},
	{NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};

/*
  Object Type definition
  full blown 2.3 struct
  if you are having trouble building with an earlier version of python,
   this is why.
*/

PyTypeObject V24_CurNurb_Type = {
	PyObject_HEAD_INIT( NULL ) /* required py macro */
	0,	/* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"CurNurb",		/* char *tp_name; */
	sizeof( V24_CurNurb_Type ),	/* int tp_basicsize, */
	0,			/* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,					/*    destructor tp_dealloc; */
	NULL,					/*    printfunc tp_print; */
	NULL,					/*    getattrfunc tp_getattr; */
	NULL,					/*    setattrfunc tp_setattr; */
	( cmpfunc ) V24_CurNurb_compare,	/*    cmpfunc tp_compare; */
	( reprfunc ) V24_CurNurb_repr,	/*    reprfunc tp_repr; */

	/* Method suites for standard classes */

	0,			/*    PyNumberMethods *tp_as_number; */
	&V24_CurNurb_as_sequence,	/*    PySequenceMethods *tp_as_sequence; */
	0,			/*    PyMappingMethods *tp_as_mapping; */

	/* More standard operations (here for binary compatibility) */

	0,			/*    hashfunc tp_hash; */
	0,			/*    ternaryfunc tp_call; */
	0,			/*    reprfunc tp_str; */
	0,			/*    getattrofunc tp_getattro; */
	0,			/*    setattrofunc tp_setattro; */

	/* Functions to access object as input/output buffer */
	0,			/*    PyBufferProcs *tp_as_buffer; */

  /*** Flags to define presence of optional/expanded features ***/
	Py_TPFLAGS_DEFAULT,	/*    long tp_flags; */

	0,			/*  char *tp_doc;  Documentation string */
  /*** Assigned meaning in release 2.0 ***/
	/* call function for all accessible objects */
	0,			/*    traverseproc tp_traverse; */

	/* delete references to contained objects */
	0,			/*    inquiry tp_clear; */

  /***  Assigned meaning in release 2.1 ***/
  /*** rich comparisons ***/
	0,			/*  richcmpfunc tp_richcompare; */

  /***  weak reference enabler ***/
	0,			/* long tp_weaklistoffset; */

  /*** Added in release 2.2 ***/
	/*   Iterators */
	( getiterfunc ) V24_CurNurb_getIter,	/*    getiterfunc tp_iter; */
	( iternextfunc ) V24_CurNurb_iterNext,	/*    iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	V24_BPy_CurNurb_methods,	/*    struct PyMethodDef *tp_methods; */
	0,			/*    struct PyMemberDef *tp_members; */
	V24_BPy_CurNurb_getseters,			/*    struct PyGetSetDef *tp_getset; */
	0,			/*    struct _typeobject *tp_base; */
	0,			/*    PyObject *tp_dict; */
	0,			/*    descrgetfunc tp_descr_get; */
	0,			/*    descrsetfunc tp_descr_set; */
	0,			/*    long tp_dictoffset; */
	0,			/*    initproc tp_init; */
	0,			/*    allocfunc tp_alloc; */
	0,			/*    newfunc tp_new; */
	/*  Low-level free-memory routine */
	0,			/*    freefunc tp_free;  */
	/* For PyObject_IS_GC */
	0,			/*    inquiry tp_is_gc;  */
	0,			/*    PyObject *tp_bases; */
	/* method resolution order */
	0,			/*    PyObject *tp_mro;  */
	0,			/*    PyObject *tp_cache; */
	0,			/*    PyObject *tp_subclasses; */
	0,			/*    PyObject *tp_weaklist; */
	0
};

/*
  compare
  in this case, we consider two CurNurbs equal, if they point to the same
  blender data.
*/

static int V24_CurNurb_compare( V24_BPy_CurNurb * a, V24_BPy_CurNurb * b )
{
	Nurb *pa = a->nurb;
	Nurb *pb = b->nurb;

	return ( pa == pb ) ? 0 : -1;
}


/*
  factory method to create a V24_BPy_CurNurb from a Blender Nurb
*/

PyObject *V24_CurNurb_CreatePyObject( Nurb * blen_nurb )
{
	V24_BPy_CurNurb *pyNurb;

	pyNurb = ( V24_BPy_CurNurb * ) PyObject_NEW( V24_BPy_CurNurb, &V24_CurNurb_Type );

	if( !pyNurb )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "could not create V24_BPy_CurNurb PyObject" );

	pyNurb->nurb = blen_nurb;
	return ( PyObject * ) pyNurb;
}


/*
 *  V24_CurNurb_repr
 */
static PyObject *V24_CurNurb_repr( V24_BPy_CurNurb * self )
{				/* used by 'repr' */

	return PyString_FromFormat( "[CurNurb \"%d\"]", self->nurb->type );
}

/* XXX Can't this be simply removed? */
static PyObject *V24_M_CurNurb_New( PyObject * self, PyObject * args )
{
	return ( PyObject * ) 0;

}

/*
 *	Curve.getType
 */
static PyObject *V24_CurNurb_getType( V24_BPy_CurNurb * self )
{
	/* type is on 3 first bits only */
	return PyInt_FromLong( self->nurb->type & 7 );
}

/*
 *	Curve.setType
 *
 *	Convert the curve using Blender's convertspline fonction
 */
static int V24_CurNurb_setType( V24_BPy_CurNurb * self, PyObject * args )
{
	PyObject* integer = PyNumber_Int( args );
	short value;

	if( !integer )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected integer argument" );

	value = ( short )PyInt_AS_LONG( integer );
	Py_DECREF( integer );

	/* parameter value checking */
	if (value != CU_POLY && value != CU_BEZIER && value != CU_NURBS)
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
				"expected integer argument" );

	/* convert and raise error if impossible */
	if (convertspline(value, self->nurb))
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
				"Conversion Impossible" );

	return 0;
}

/*
 * V24_CurNurb_getKnotsU
 *
 * returns curve's knotsU in a tuple. Empty tuple is returned if curve
 * isn't Nurbs or it doesn't have knots in U
 */

static PyObject *V24_CurNurb_getKnotsU( V24_BPy_CurNurb * self )
{
	if(self->nurb->knotsu) {
		int len = KNOTSU(self->nurb);
		int i;
		PyObject *knotsu = PyTuple_New(len);
		if( !knotsu )
			return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					"could not get CurNurb.knotsU attribute" );

		for(i = 0; i < len; ++i)
			PyTuple_SetItem(knotsu, i,
					PyFloat_FromDouble(self->nurb->knotsu[i]));

		return knotsu;
	}
	return PyTuple_New(0);
}

/*
 * V24_CurNurb_getKnotsV
 *
 * returns curve's knotsV in a tuple. Empty tuple is returned if curve doesn't have knots in V
 */

static PyObject *V24_CurNurb_getKnotsV( V24_BPy_CurNurb * self )
{
	if(self->nurb->knotsv) {
		int len = KNOTSV(self->nurb);
		int i;
		PyObject *knotsv = PyTuple_New(len);
		if( !knotsv )
			return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					"could not get CurNurb.knotsV index" );

		for(i = 0; i < len; ++i)
			PyTuple_SetItem(knotsv, i,
					PyFloat_FromDouble(self->nurb->knotsv[i] ));
	
		return knotsv;
	}
	return PyTuple_New(0);
}

static PyObject *V24_CurNurb_getPoints( V24_BPy_CurNurb * self )
{
	return PyInt_FromLong( ( long ) self->nurb->pntsu );
}

static PyObject *V24_CurNurb_getFlagBits( V24_BPy_CurNurb * self, void *type )
{
	return V24_EXPP_getBitfield( (void *)&self->nurb->flag,
							  (int)type, 'h' );
}

static int V24_CurNurb_setFlagBits( V24_BPy_CurNurb * self, PyObject *value,
									void *type )
{
	return V24_EXPP_setBitfield( value, (void *)&self->nurb->flag,
							 (int)type, 'h' );
}

/*
 * V24_CurNurb_append( point )
 * append a new point to a nurb curve.
 * arg is BezTriple or list of xyzw floats 
 */

PyObject *V24_CurNurb_append( V24_BPy_CurNurb * self, PyObject * value )
{
	return V24_CurNurb_appendPointToNurb( self->nurb, value );
}


/*
 * V24_CurNurb_appendPointToNurb
 * this is a non-bpy utility func to add a point to a given nurb.
 * notice the first arg is Nurb*.
 */

PyObject *V24_CurNurb_appendPointToNurb( Nurb * nurb, PyObject * value )
{

	int i;
	int size;
	int npoints = nurb->pntsu;

	/*
	   do we have a list of four floats or a BezTriple?
	*/
	
	/* if curve is empty, adjust type depending on input type */
	if (nurb->bezt==NULL && nurb->bp==NULL) {
		if (V24_BPy_BezTriple_Check( value ))
			nurb->type |= CU_BEZIER;
		else if (PySequence_Check( value ))
			nurb->type |= CU_NURBS;
		else
			return( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					  "Expected a BezTriple or a Sequence of 4 (or 5) floats" ) );
	}



	if ((nurb->type & 7)==CU_BEZIER) {
		BezTriple *tmp;

		if( !V24_BPy_BezTriple_Check( value ) )
			return( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					  "Expected a BezTriple\n" ) );

/*		printf("\ndbg: got a BezTriple\n"); */
		tmp = nurb->bezt;	/* save old points */
		nurb->bezt =
			( BezTriple * ) MEM_mallocN( sizeof( BezTriple ) *
						     ( npoints + 1 ),
						     "V24_CurNurb_append2" );

		if( !nurb->bezt )
			return ( V24_EXPP_ReturnPyObjError
				 ( PyExc_MemoryError, "allocation failed" ) );

		/* copy old points to new */
		if( tmp ) {
			memmove( nurb->bezt, tmp, sizeof( BezTriple ) * npoints );
			MEM_freeN( tmp );
		}

		nurb->pntsu++;
		/* add new point to end of list */
		memcpy( nurb->bezt + npoints,
			V24_BezTriple_FromPyObject( value ), sizeof( BezTriple ) );

	}
	else if( PySequence_Check( value ) ) {
		size = PySequence_Size( value );
/*		printf("\ndbg: got a sequence of size %d\n", size );  */
		if( size == 4 || size == 5 ) {
			BPoint *tmp;

			tmp = nurb->bp;	/* save old pts */

			nurb->bp =
				( BPoint * ) MEM_mallocN( sizeof( BPoint ) *
							  ( npoints + 1 ),
							  "V24_CurNurb_append1" );
			if( !nurb->bp )
				return ( V24_EXPP_ReturnPyObjError
					 ( PyExc_MemoryError,
					   "allocation failed" ) );

			memmove( nurb->bp, tmp, sizeof( BPoint ) * npoints );
			if( tmp )
				MEM_freeN( tmp );

			++nurb->pntsu;
			/* initialize new BPoint from old */
			memcpy( nurb->bp + npoints, nurb->bp,
				sizeof( BPoint ) );

			for( i = 0; i < 4; ++i ) {
				PyObject *item = PySequence_GetItem( value, i );

				if (item == NULL)
					return NULL;


				nurb->bp[npoints].vec[i] = ( float ) PyFloat_AsDouble( item );
				Py_DECREF( item );
			}

			if (size == 5) {
				PyObject *item = PySequence_GetItem( value, i );

				if (item == NULL)
					return NULL;

				nurb->bp[npoints].alfa = ( float ) PyFloat_AsDouble( item );
				Py_DECREF( item );
			}
			else {
				nurb->bp[npoints].alfa = 0.0f;
			}

			makeknots( nurb, 1, nurb->flagu >> 1 );

		} else {
			return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					"expected a sequence of 4 or 5 floats" );
		}

	} else {
		/* bail with error */
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					"expected a sequence of 4 or 5 floats" );

	}

	Py_RETURN_NONE;
}


/*
 *  V24_CurNurb_setMatIndex
 *
 *  set index into material list
 */

static int V24_CurNurb_setMatIndex( V24_BPy_CurNurb * self, PyObject * args )
{
	printf ("%d\n", self->nurb->mat_nr);
	return V24_EXPP_setIValueRange( args, &self->nurb->mat_nr, 0, 15, 'h' );
}

/*
 * V24_CurNurb_getMatIndex
 *
 * returns index into material list
 */

static PyObject *V24_CurNurb_getMatIndex( V24_BPy_CurNurb * self )
{
	PyObject *index = PyInt_FromLong( ( long ) self->nurb->mat_nr );

	if( index )
		return index;

	return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
			"could not get material index" );
}

/*
 * V24_CurNurb_getFlagU
 *
 * returns curve's flagu
 */

static PyObject *V24_CurNurb_getFlagU( V24_BPy_CurNurb * self )
{
	PyObject *flagu = PyInt_FromLong( ( long ) self->nurb->flagu );

	if( flagu )
		return flagu;

	return ( V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					"could not get CurNurb.flagu index" ) );
}

/*
 *  V24_CurNurb_setFlagU
 *
 *  set curve's flagu and recalculate the knots
 *
 *  Possible values: 0 - uniform, 2 - endpoints, 4 - bezier
 *    bit 0 controls CU_CYCLIC
 */

static int V24_CurNurb_setFlagU( V24_BPy_CurNurb * self, PyObject * args )
{
	PyObject* integer = PyNumber_Int( args );
	short value;

	if( !integer )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected integer argument" );

	value = ( short )PyInt_AS_LONG( integer );
	Py_DECREF( integer );

	if( value < 0 || value > 5 )
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
				"expected integer argument in range [0,5]" );

	if( self->nurb->flagu != value ) {
		self->nurb->flagu = (short)value;
		makeknots( self->nurb, 1, self->nurb->flagu >> 1 );
	}

	return 0;
}

/*
 * V24_CurNurb_getFlagV
 *
 * returns curve's flagu
 */

static PyObject *V24_CurNurb_getFlagV( V24_BPy_CurNurb * self )
{
	PyObject *flagv = PyInt_FromLong( ( long ) self->nurb->flagv );

	if( flagv )
		return flagv;

	return ( V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					"could not get CurNurb.flagv" ) );
}

/*
 *  V24_CurNurb_setFlagV
 *
 *  set curve's flagu and recalculate the knots
 *
 *  Possible values: 0 - uniform, 1 - endpoints, 2 - bezier
 */

static int V24_CurNurb_setFlagV( V24_BPy_CurNurb * self, PyObject * args )
{
	PyObject* integer = PyNumber_Int( args );
	short value;

	if( !integer )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected integer argument" );

	value = ( short )PyInt_AS_LONG( integer );
	Py_DECREF( integer );

	if( value < 0 || value > 5 )
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
				"expected integer argument in range [0,5]" );

	if( self->nurb->flagv != value ) {
		self->nurb->flagv = (short)value;
		makeknots( self->nurb, 2, self->nurb->flagv >> 1 );
	}

	return 0;
}

/*
 * V24_CurNurb_getIter
 *
 * create an iterator for our CurNurb.
 * this iterator returns the points for this CurNurb.
 */

static PyObject *V24_CurNurb_getIter( V24_BPy_CurNurb * self )
{
	self->bp = self->nurb->bp;
	self->bezt = self->nurb->bezt;
	self->atEnd = 0;
	self->nextPoint = 0;

	/* set exhausted flag if both bp and bezt are zero */
	if( ( !self->bp ) && ( !self->bezt ) )
		self->atEnd = 1;

	Py_INCREF( self );
	return ( PyObject * ) self;
}


static PyObject *V24_CurNurb_iterNext( V24_BPy_CurNurb * self )
{
	PyObject *po;		/* return value */
	Nurb *pnurb = self->nurb;
	int npoints = pnurb->pntsu;

	/* are we at end already? */
	if( self->atEnd )
		return ( V24_EXPP_ReturnPyObjError( PyExc_StopIteration,
						"iterator at end" ) );

	if( self->nextPoint < npoints ) {

		po = V24_CurNurb_pointAtIndex( self->nurb, self->nextPoint );
		self->nextPoint++;

		return po;

	} else {
		self->atEnd = 1;	/* set flag true */
	}

	return ( V24_EXPP_ReturnPyObjError( PyExc_StopIteration,
					"iterator at end" ) );
}



/*
 * V24_CurNurb_isNurb()
 * test whether spline nurb or bezier
 */

static PyObject *V24_CurNurb_isNurb( V24_BPy_CurNurb * self )
{
	/* NOTE: a Nurb has bp and bezt pointers
	 * depending on type.
	 * It is possible both are NULL if no points exist.
	 * in that case, we return False
	 */

	if( self->nurb->bp ) {
		Py_RETURN_TRUE;
	} else {
		Py_RETURN_FALSE;
	}
}

/*
 * V24_CurNurb_isCyclic()
 * test whether spline cyclic (closed) or not (open)
 */

static PyObject *V24_CurNurb_isCyclic( V24_BPy_CurNurb * self )
{
        /* supposing that the flagu is always set */ 

	if( self->nurb->flagu & CU_CYCLIC ) {
		Py_RETURN_TRUE;
	} else {
		Py_RETURN_FALSE;
	}
}

/*
 * V24_CurNurb_length
 * returns the number of points in a Nurb
 * this is a tp_as_sequence method, not a regular instance method.
 */

static int V24_CurNurb_length( PyInstanceObject * inst )
{
	Nurb *nurb;
	int len;

	if( V24_BPy_CurNurb_Check( ( PyObject * ) inst ) ) {
		nurb = ( ( V24_BPy_CurNurb * ) inst )->nurb;
		len = nurb->pntsu;
		return len;
	}

	return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
				    "arg is not a V24_BPy_CurNurb" );
}

/*
 * V24_CurNurb_getPoint
 * returns the Nth point in a Nurb
 * this is one of the tp_as_sequence methods, hence the int N argument.
 * it is called via the [] operator, not as a usual instance method.
 */

PyObject *V24_CurNurb_getPoint( V24_BPy_CurNurb * self, int index )
{
	Nurb *myNurb;

	int npoints;

	/* for convenince */
	myNurb = self->nurb;
	npoints = myNurb->pntsu;

	/* DELETED: bail if index < 0 */
	/* actually, this check is not needed since python treats */
	/* negative indices as starting from the right end of a sequence */
	/* 
	THAT IS WRONG, when passing a negative index, python adjusts it to be positive
	BUT it can still overflow in the negatives if the index is too small.
	For example, list[-6] when list contains 5 items means index = -1 in here.
	(theeth)
	*/

	/* bail if no Nurbs in Curve */
	if( npoints == 0 )
		return ( V24_EXPP_ReturnPyObjError( PyExc_IndexError,
						"no points in this CurNurb" ) );

	/* check index limits */
	if( index >= npoints || index < 0 )
		return ( V24_EXPP_ReturnPyObjError( PyExc_IndexError,
						"index out of range" ) );

	return V24_CurNurb_pointAtIndex( myNurb, index );
}

/*
 * V24_CurNurb_setPoint
 * modifies the Nth point in a Nurb
 * this is one of the tp_as_sequence methods, hence the int N argument.
 * it is called via the [] = operator, not as a usual instance method.
 */
static int V24_CurNurb_setPoint( V24_BPy_CurNurb * self, int index, PyObject * pyOb )
{
	Nurb *nurb = self->nurb;
	int size;

	/* check index limits */
	if( index < 0 || index >= nurb->pntsu )
		return V24_EXPP_ReturnIntError( PyExc_IndexError,
					    "array assignment index out of range" );


	/* branch by curve type */
	if ((nurb->type & 7)==CU_BEZIER) {	/* BEZIER */
		/* check parameter type */
		if( !V24_BPy_BezTriple_Check( pyOb ) )
			return V24_EXPP_ReturnIntError( PyExc_TypeError,
							"expected a BezTriple" );

		/* copy bezier in array */
		memcpy( nurb->bezt + index,
			V24_BezTriple_FromPyObject( pyOb ), sizeof( BezTriple ) );

		return 0;	/* finished correctly */
	}
	else {	/* NURBS or POLY */
		int i;

		/* check parameter type */
		if (!PySequence_Check( pyOb ))
			return V24_EXPP_ReturnIntError( PyExc_TypeError,
							"expected a list of 4 (or optionally 5 if the curve is 3D) floats" );

		size = PySequence_Size( pyOb );

		/* check sequence size */
		if( size != 4 && size != 5 ) 
			return V24_EXPP_ReturnIntError( PyExc_TypeError,
							"expected a list of 4 (or optionally 5 if the curve is 3D) floats" );

		/* copy x, y, z, w */
		for( i = 0; i < 4; ++i ) {
			PyObject *item = PySequence_GetItem( pyOb, i );

			if (item == NULL)
				return -1;

			nurb->bp[index].vec[i] = ( float ) PyFloat_AsDouble( item );
			Py_DECREF( item );
		}

		if (size == 5) {	/* set tilt, if present */
			PyObject *item = PySequence_GetItem( pyOb, i );

			if (item == NULL)
				return -1;

			nurb->bp[index].alfa = ( float ) PyFloat_AsDouble( item );
			Py_DECREF( item );
		}
		else {				/* if not, set default	*/
			nurb->bp[index].alfa = 0.0f;
		}

		return 0;	/* finished correctly */
	}
}


/* 
 * this is an internal routine.  not callable directly from python
 */

PyObject *V24_CurNurb_pointAtIndex( Nurb * nurb, int index )
{
	PyObject *pyo;

	if( nurb->bp ) {	/* we have a nurb curve */
		int i;

		/* add Tilt only if curve is 3D */
		if (nurb->flag & CU_3D)
			pyo = PyList_New( 5 );
		else
			pyo = PyList_New( 4 );

		for( i = 0; i < 4; i++ ) {
			PyList_SetItem( pyo, i,
					PyFloat_FromDouble( nurb->bp[index].vec[i] ) );
		}

		/* add Tilt only if curve is 3D */
		if (nurb->flag & CU_3D)
			PyList_SetItem( pyo, 4,	PyFloat_FromDouble( nurb->bp[index].alfa ) );

	} else if( nurb->bezt ) {	/* we have a bezier */
		/* if an error occurs, we just pass it on */
		pyo = V24_BezTriple_CreatePyObject( &( nurb->bezt[index] ) );

	} else			/* something is horribly wrong */
		/* neither bp or bezt is set && pntsu != 0 */
		return V24_EXPP_ReturnPyObjError( PyExc_SystemError,
						"inconsistant structure found" );

	return pyo;
}

/*
  dump nurb
*/

PyObject *V24_CurNurb_dump( V24_BPy_CurNurb * self )
{
	BPoint *bp = NULL;
	BezTriple *bezt = NULL;
	Nurb *nurb = self->nurb;
	int npoints = 0;

	if( !self->nurb )
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"no Nurb in this CurNurb");

	printf(" type: %d, mat_nr: %d hide: %d flag: %d",
		   nurb->type, nurb->mat_nr, nurb->hide, nurb->flag);
	printf("\n pntsu: %d, pntsv: %d, resolu: %d resolv: %d",
		   nurb->pntsu, nurb->pntsv, nurb->resolu, nurb->resolv );
	printf("\n orderu: %d  orderv: %d", nurb->orderu, nurb->orderv );
	printf("\n flagu: %d flagv: %d",
		   nurb->flagu, nurb->flagv );

	npoints = nurb->pntsu;

	if( nurb->bp ) { /* we have a BPoint  */
		int n;
		for( n = 0, bp = nurb->bp;
			 n < npoints;
			 n++, bp++ )
		{
			/* vec[4] */
			printf( "\ncoords[%d]: ", n);
			{
				int i;
				for( i = 0; i < 4; i++){
					printf("%10.3f ", bp->vec[i] );
				}
			}
		
			/* alfa, s[2] */
			printf("\n alpha: %5.2f", bp->alfa);
			/* f1, hide */
			printf(" f1 %d  hide %d", bp->f1, bp->hide );
			printf("\n");
		}
	}
	else { /* we have a BezTriple */
		int n;
		for( n = 0, bezt = nurb->bezt;
			 n < npoints;
			 n++, bezt++ )
		{
			int i, j;
			printf("\npoint %d: ", n);
			for( i = 0; i < 3; i++ ) {
				printf("\nvec[%i] ",i );
				for( j = 0; j < 3; j++ ) {
					printf(" %5.2f ", bezt->vec[i][j] );
				}
			}
				

		}
		printf("\n");
	}

	Py_RETURN_NONE;
}

/*
  recalc nurb
*/

static PyObject *V24_CurNurb_recalc( V24_BPy_CurNurb * self )
{
	calchandlesNurb ( self->nurb );
	Py_RETURN_NONE;
}

PyObject *V24_CurNurb_switchDirection( V24_BPy_CurNurb * self )
{
	if( !self->nurb )
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"no Nurb in this CurNurb");
	
	switchdirectionNurb( self->nurb );
	
	Py_RETURN_NONE;
}

PyObject *V24_CurNurb_Init( void )
{
	if( PyType_Ready( &V24_CurNurb_Type ) < 0)
		return NULL;

	return Py_InitModule3( "Blender.CurNurb", V24_M_CurNurb_methods,
				V24_M_CurNurb_doc );
}

/* #####DEPRECATED###### */

static PyObject *V24_CurNurb_oldsetType( V24_BPy_CurNurb * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args,
			(setter)V24_CurNurb_setType );
}

static PyObject *V24_CurNurb_oldsetMatIndex( V24_BPy_CurNurb * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args,
			(setter)V24_CurNurb_setMatIndex );
}

static PyObject *V24_CurNurb_oldsetFlagU( V24_BPy_CurNurb * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args,
			(setter)V24_CurNurb_setFlagU );
}

static PyObject *V24_CurNurb_oldsetFlagV( V24_BPy_CurNurb * self, PyObject * args )
{
	return V24_EXPP_setterWrapper( (void *)self, args,
			(setter)V24_CurNurb_setFlagV );
}

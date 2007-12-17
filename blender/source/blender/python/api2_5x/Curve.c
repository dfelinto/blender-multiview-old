/* 
 * $Id: Curve.c 12802 2007-12-06 00:01:46Z campbellbarton $
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

#include "Curve.h"		/* This must come first */
#include "CurveBase.h"	/* subtype CurveBase */

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



/* 
 *   Curve_update( )
 *   method to update display list for a Curve.
 *   used. after messing with control points
 */

PyObject *Curve_update( BPyCurveObject * self )
{
	Nurb *nu = self->bpycurvebase.curve->nurb.first;

	/* recalculate handles for each curve: calchandlesNurb() will make
	 * sure curves are bezier first */
	while( nu ) {
		calchandlesNurb ( nu );
		nu = nu->next;
	}

	Object_updateDag( (void*) self->bpycurvebase.curve );

	Py_RETURN_NONE;
}


/*
 * Curve_appendPoint( numcurve, new_point )
 * append a new point to indicated spline
 */

static PyObject *Curve_appendPoint( BPyCurveBaseObject * self, PyObject * args )
{
	int i;
	int nurb_num;		/* index of curve we append to */
	PyObject *coord_args;	/* coords for new point */
	Nurb *nurb = self->curve->nurb.first;	/* first nurb in Curve */

/* fixme - need to malloc new Nurb */
	if( !nurb )
		return ( EXPP_ReturnPyObjError
			 ( PyExc_AttributeError, "no nurbs in this Curve" ) );

	if( !PyArg_ParseTuple( args, "iO", &nurb_num, &coord_args ) )
		return ( EXPP_ReturnPyObjError
			 ( PyExc_TypeError,
			   "expected int, coords as arguments" ) );

	/* 
	   chase down the list of Nurbs looking for our curve.
	 */
	for( i = 0; i < nurb_num; i++ ) {
		nurb = nurb->next;
		if( !nurb )	/* we ran off end of list */
			return EXPP_ReturnPyObjError( PyExc_ValueError,
					"curve index out of range" );
	}
	
	return CurNurb_appendPointToNurb( nurb, coord_args );
}


/****
  appendNurb( new_point )
  create a new nurb in the Curve and add the point param to it.
  returns a refernce to the newly created nurb.
*****/

static PyObject *Curve_appendNurb( BPyCurveBaseObject * self, PyObject * value )
{
	Nurb *new_nurb = ( Nurb * ) MEM_callocN( sizeof( Nurb ), "appendNurb" );
	if( !new_nurb )
		return EXPP_ReturnPyObjError
			( PyExc_MemoryError, "unable to malloc Nurb" );
	
	if( CurNurb_appendPointToNurb( new_nurb, value ) ) {
		new_nurb->resolu = self->curve->resolu;
		new_nurb->resolv = self->curve->resolv;
		new_nurb->hide = 0;
		new_nurb->flag = 1;


		if( new_nurb->bezt ) {	/* do setup for bezt */
			new_nurb->type = CU_BEZIER;
			new_nurb->bezt->h1 = HD_ALIGN;
			new_nurb->bezt->h2 = HD_ALIGN;
			new_nurb->bezt->f1 = SELECT;
			new_nurb->bezt->f2 = SELECT;
			new_nurb->bezt->f3 = SELECT;
			new_nurb->bezt->hide = 0;
			/* calchandlesNurb( new_nurb ); */
		} else {	/* set up bp */
			new_nurb->pntsv = 1;
			new_nurb->type = CU_NURBS;
			new_nurb->orderu = 4;
			new_nurb->flagu = 0;
			new_nurb->flagv = 0;
			new_nurb->bp->f1 = 0;
			new_nurb->bp->hide = 0;
			new_nurb->knotsu = 0;
			/*makenots( new_nurb, 1, new_nurb->flagu >> 1); */
		}
		BLI_addtail( &self->curve->nurb, new_nurb);
	} else {
		freeNurb( new_nurb );
		return NULL;	/* with PyErr already set */
	}

	return CurNurb_CreatePyObject( new_nurb );
}


static PyObject *Curve_getPathLen( BPyCurveBaseObject * self )
{
	return PyInt_FromLong( ( long ) self->curve->pathlen );
}


static int Curve_setPathLen( BPyCurveBaseObject * self, PyObject * args )
{
	PyObject *num;

	if( !PyNumber_Check( args ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected int argument" );

	num = PyNumber_Int( args );
	self->curve->pathlen = (short)PyInt_AS_LONG( num );
	Py_DECREF( num );

	return 0;
}


static PyGetSetDef BPyCurve_getset[] = {
	{"pathlen",
	 (getter)Curve_getPathLen, (setter)Curve_setPathLen,
	 "The path length,  used to set the number of frames for an animation (not the physical length)",
	NULL},
	{NULL}
};
static PyMethodDef BPyCurve_methods[] = {
	{"appendPoint", ( PyCFunction ) Curve_appendPoint, METH_VARARGS,
	 "( int numcurve, list of coordinates) - adds a new point to end of curve"},
	{"appendNurb", ( PyCFunction ) Curve_appendNurb, METH_O,
	 "( new_nurb ) - adds a new nurb to the Curve"},
	{"update", ( PyCFunction ) Curve_update, METH_NOARGS,
	 "( ) - updates display lists after changes to Curve"},
	{NULL}
};

/* internal use only */
static Nurb *CurveIter_get_pointer( BPyCurveObject * self )
{
	Nurb *nu = self->bpycurvebase.curve->nurb.first;
	int i;
	printf("_%i_\n", self->index);
	for (i=0; (nu && i< self->index); i++, nu= nu->next) {}
	return nu; /* can be NULL */
}


/*
 * Curve_getIter
 *
 * create an iterator for our Curve.
 * this iterator returns the Nurbs for this Curve.
 * the iter_pointer always points to the next available item or null
 */

static PyObject *Curve_getIter( BPyCurveObject * self )
{
	if (self->index == 0) {
		return EXPP_incr_ret ( (PyObject *) self );
	} else { /* were alredy looping */
		return Curve_CreatePyObject( self->bpycurvebase.curve );
	}
}

/*
 * Curve_iterNext
 *  get the next item.
 *  iter_pointer always points to the next available element
 *   or NULL if at the end of the list.
 */

static PyObject *Curve_iterNext( BPyCurveObject * self )
{
	Nurb *nu = CurveIter_get_pointer(self);

	if( !(nu) )
		return EXPP_ReturnPyObjError( PyExc_StopIteration,
				"iterator at end" );
	
	self->index++;
	
	if( (nu->type & 7) == CU_BEZIER || nu->pntsv <= 1 )
		return CurNurb_CreatePyObject( nu ); /* make a bpy_curnurb */
	else
		return SurfNurb_CreatePyObject( nu ); /* make a bpy_surfnurb */

}

/* tp_sequence methods */

/*
 * Curve_length
 * returns the number of curves in a Curve
 * this is a tp_as_sequence method, not a regular instance method.
 */

static int Curve_length( BPyCurveBaseObject * self )
{
	return BLI_countlist( &self->curve->nurb );
}

/*
 * Curve_getNurb
 * returns the Nth nurb in a Curve.
 * this is one of the tp_as_sequence methods, hence the int N argument.
 * it is called via the [] operator, not as a usual instance method.
 */

PyObject *Curve_getNurb( BPyCurveBaseObject * self, int n )
{
	Nurb *pNurb;
	int i;

	/* bail if index < 0 */
	if( n < 0 )
		return ( EXPP_ReturnPyObjError( PyExc_IndexError,
						"index less than 0" ) );
	/* bail if no Nurbs in Curve */
	if( self->curve->nurb.first == 0 )
		return ( EXPP_ReturnPyObjError( PyExc_IndexError,
						"no Nurbs in this Curve" ) );
	/* set pointer to nth Nurb */
	for( pNurb = self->curve->nurb.first, i = 0;
	     pNurb != 0 && i < n; pNurb = pNurb->next, ++i )
		/**/;

	if( !pNurb )		/* we came to the end of the list */
		return ( EXPP_ReturnPyObjError( PyExc_IndexError,
						"index out of range" ) );

	/* until there is a Surface BPyType, distinquish between a curve and a
	 * surface based on whether it's a Bezier and the v size */
	if( (pNurb->type & 7) == CU_BEZIER || pNurb->pntsv <= 1 )
		return CurNurb_CreatePyObject( pNurb );	/* make a bpy_curnurb */
	else
		return SurfNurb_CreatePyObject( pNurb );	/* make a bpy_surfnurb */

}

/*
 * Curve_setNurb
 * In this case only remove the item, we could allow adding later.
 */
static int Curve_setNurb( BPyCurveBaseObject * self, int n, PyObject * value )
{
	Nurb *pNurb;
	int i;

	/* bail if index < 0 */
	if( n < 0 )
		return ( EXPP_ReturnIntError( PyExc_IndexError,
				 "index less than 0" ) );
	/* bail if no Nurbs in Curve */
	if( self->curve->nurb.first == 0 )
		return ( EXPP_ReturnIntError( PyExc_IndexError,
				 "no Nurbs in this Curve" ) );
	/* set pointer to nth Nurb */
	for( pNurb = self->curve->nurb.first, i = 0;
			pNurb != 0 && i < n; pNurb = pNurb->next, ++i )
		/**/;

	if( !pNurb )		/* we came to the end of the list */
		return ( EXPP_ReturnIntError( PyExc_IndexError,
				 "index out of range" ) );
	
	if (value) {
		return ( EXPP_ReturnIntError( PyExc_RuntimeError,
				 "assigning curves is not yet supported" ) );
	} else {
		BLI_remlink(&self->curve->nurb, pNurb);
		freeNurb(pNurb);
	}
	return 0;
}

static PySequenceMethods Curve_as_sequence = {
	( inquiry ) Curve_length,	/* sq_length   */
	( binaryfunc ) 0,	/* sq_concat */
	( intargfunc ) 0,	/* sq_repeat */
	( intargfunc ) Curve_getNurb,	/* sq_item */
	( intintargfunc ) 0,	/* sq_slice */
	( intobjargproc ) Curve_setNurb,	/* sq_ass_item - only so you can do del curve[i] */
	0,			/* sq_ass_slice */
	( objobjproc ) 0,	/* sq_contains */
	0,
	0
};

/* this types constructor */
static PyObject *Curve_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	char *name="Curve";
	ID *id;
	
	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"(name) - name must be a string argument" );
	
	id = (ID *)add_curve( name, OB_CURVE );
	
	if (!id)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, could not create new curve data" );
	
	id->us = 0;
	return Curve_CreatePyObject((Curve *)id);
}

PyTypeObject BPyCurve_Type = {
	PyObject_HEAD_INIT(NULL)
	0,
	"BPyCurve",
	sizeof(BPyCurveObject),
	0,
	0,					/* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	0,					/* tp_repr */
	0,					/* tp_as_number */
	&Curve_as_sequence,	/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0,					/* tp_hash */
	0,					/* tp_call */
	0,					/* tp_str */
	0,					/* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
	0,					/* tp_doc */
	0,					/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	( getiterfunc ) Curve_getIter,	    /* getiterfunc tp_iter; */
	( iternextfunc ) Curve_iterNext,	/* iternextfunc tp_iternext; */
	BPyCurve_methods,	/* tp_methods */
	0,					/* tp_members */
	BPyCurve_getset,	/* tp_getset */
	&BPyCurveBase_Type,	/* tp_base */ 
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */ 
	0,					/* tp_dictoffset */
	0,					/* tp_init */
	0,					/* tp_alloc */
	( newfunc ) Curve_new,	/* tp_new */
};

/*
 * Curve_CreatePyObject
 * constructor to build a py object from blender data 
 */

PyObject *Curve_CreatePyObject( struct Curve * curve )
{
	BPyCurveObject *blen_object;

	blen_object = ( BPyCurveObject * ) PyObject_NEW( BPyCurveObject, &BPyCurve_Type );

	if( blen_object == NULL ) {
		return ( NULL );
	}
	blen_object->bpycurvebase.curve = curve;
	blen_object->index = 0;
	return ( ( PyObject * ) blen_object );
}

PyObject *CurveType_Init( void  )
{
	PyType_Ready( &BPyCurve_Type ) ;
	return (PyObject *) &BPyCurve_Type ;
}

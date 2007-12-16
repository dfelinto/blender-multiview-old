/* 
 * $Id: Lattice.c 11446 2007-07-31 16:11:32Z campbellbarton $
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
 * Contributor(s): Joseph Gilbert
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "Lattice.h" /*This must come first*/

#include "BKE_utildefines.h"
#include "BKE_main.h"
#include "BKE_global.h"
#include "BKE_library.h"
#include "BKE_lattice.h"
#include "BLI_blenlib.h"
#include "DNA_object_types.h"
#include "DNA_key_types.h"
#include "DNA_mesh_types.h"
#include "DNA_meshdata_types.h"
#include "DNA_curve_types.h"
#include "DNA_scene_types.h"
#include "BIF_editkey.h"
#include "BIF_editdeform.h"
#include "BIF_space.h"
#include "blendef.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "vector.h"

#include "Key.h"

static BPoint * LPoint_get_pointer( BPyLPointObject * self );
static PyObject *LPoint_CreatePyObject( BPyLatticeObject *bpylat, int i );

/*****************************************************************************/
/* Python API function prototypes for the Lattice module.	*/
/*****************************************************************************/

/*****************************************************************************/
/*  Lattice Strings			 */
/* The following string definitions are used for documentation strings.	 */
/* In Python these will be written to the console when doing a		 */
/* Blender.Lattice.__doc__			*/
/*****************************************************************************/

static char Lattice_insertKey_doc[] =
	"(str) - Set a new key for the lattice at specified frame";

static char Lattice_copy_doc[] =
	"() - Return a copy of the lattice.";

//***************************************************************************
// Function:      Lattice_CreatePyObject   
//***************************************************************************
PyObject *Lattice_CreatePyObject( Lattice * lt )
{
	BPyLatticeObject *pyLat;

	pyLat = ( BPyLatticeObject * ) PyObject_NEW( BPyLatticeObject, &BPyLattice_Type );

	if( !pyLat )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create BPyLatticeObject PyObject" );

	pyLat->lattice = lt;

	return ( PyObject * ) pyLat;
}

static PyObject *Lattice_getKey( BPyLatticeObject * self )
{
	return Key_CreatePyObject(self->lattice->key);
}

static PyObject *Lattice_setOutside( BPyLatticeObject * self, PyObject * value )
{
	if ( PyObject_IsTrue(value) ) {
		self->lattice->flag = LT_GRID;
	} else {
		outside_lattice( self->lattice );
		self->lattice->flag = LT_OUTSIDE + LT_GRID;
	}
	return 0;
}

static PyObject *Lattice_getOutside(BPyLatticeObject * self)
{
	if( self->lattice->flag & LT_OUTSIDE ) {
		Py_RETURN_TRUE;
	} else {
		Py_RETURN_FALSE;
	}
}

static PyObject *Lattice_insertKey( BPyLatticeObject * self, PyObject * args )
{
	Lattice *lt;
	int frame = -1, oldfra = -1;

	if( !PyArg_ParseTuple( args, "i", &frame ) )
		return ( EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected int argument" ) );

	lt = self->lattice;

	//set the current frame
	if( frame > 0 ) {
		frame = EXPP_ClampInt( frame, 1, MAXFRAME );
		oldfra = G.scene->r.cfra;
		G.scene->r.cfra = (int)frame;
	}
//      else just use current frame, then
//              return (EXPP_ReturnPyObjError (PyExc_RuntimeError,
//                                              "frame value has to be greater than 0"));

	//insert a keybock for the lattice (1=relative)
	insert_lattkey( lt , 1);
	allspace(REMAKEIPO, 0);

	if( frame > 0 )
		G.scene->r.cfra = (int)oldfra;

	Py_RETURN_NONE;
}

static PyObject *Lattice_copy( BPyLatticeObject * self )
{
	Lattice *bl_Lattice;	// blender Lattice object 
	PyObject *py_Lattice;	// python wrapper 

	bl_Lattice = copy_lattice( self->lattice );
	bl_Lattice->id.us = 0;

	if( bl_Lattice )
		py_Lattice = Lattice_CreatePyObject( bl_Lattice );
	else
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "couldn't create Lattice Object in Blender" );
	if( !py_Lattice )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create Lattice Object wrapper" );

	return py_Lattice;
}

/*****************************************************************************/
/* Python BPyLatticeObject methods table:	*/
/*****************************************************************************/
static PyMethodDef BPyLattice_methods[] = {
	/* name, method, flags, doc */
	{"insertKey", ( PyCFunction ) Lattice_insertKey, METH_VARARGS,
	 Lattice_insertKey_doc},
	{"__copy__", ( PyCFunction ) Lattice_copy, METH_NOARGS,
	 Lattice_copy_doc},
	{"copy", ( PyCFunction ) Lattice_copy, METH_NOARGS,
	 Lattice_copy_doc},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python attributes get/set functions:                                      */
/*****************************************************************************/
static PyObject *Lattice_getSubD(BPyLatticeObject * self, void * type)
{
	switch ((long)type) {
	case 0:
		return PyInt_FromLong( self->lattice->pntsu ); 
	case 1:
		return PyInt_FromLong( self->lattice->pntsv );
	case 2:
		return PyInt_FromLong( self->lattice->pntsw );
	}
	Py_RETURN_NONE;
}

static int Lattice_setSubD(BPyLatticeObject * self, PyObject *value, void * type)
{
	short u= self->lattice->pntsu, v= self->lattice->pntsv, w= self->lattice->pntsv;
	short param = (short)PyInt_AsLong(value);
	
	if (!PyInt_Check(value))
		return EXPP_ReturnIntError( PyExc_ValueError,
				"extected an int value between 1 and 64" );
	
	CLAMP(param,  1, 64);
	
	switch ((long)type) {
	case 0:
		resizelattice(self->lattice, param, v, w, NULL);
		break;
	case 1:
		resizelattice(self->lattice, u, param, w, NULL);
		break;
	case 2:
		resizelattice(self->lattice, u, v, param, NULL);
		break;
	}
	
	return 0;
}

/* TODO - constants */
static PyObject *Lattice_getInterp(BPyLatticeObject * self, void * type)
{
	long interp_type = 0;
	switch ( (int)type ) {
	case 0:
		interp_type = (long)self->lattice->typeu;
		break;
	case 1:
		interp_type = (long)self->lattice->typev;
		break;
	case 2:
		interp_type = (long)self->lattice->typew;
		break;
	}
	
	return PyInt_FromLong(interp_type);
}

/* TODO - constants */
static int Lattice_setInterp(BPyLatticeObject * self, PyObject * value, void * type)
{
	long interp_type;
	
	if (!PyInt_Check(value))
		return EXPP_ReturnIntError( PyExc_ValueError,
				"expected an int - 0,1 or 2" );
	
	interp_type = PyInt_AsLong(value);
	CLAMP(interp_type, 0, 2);
	
	switch ( (int)type ) {
	case 0:
		self->lattice->typeu = interp_type;
		break;
	case 1:
		self->lattice->typev = interp_type;
		break;
	case 2:
		self->lattice->typew = interp_type;
		break;
	}
	return 0;
}

/************************************************************************
 *
 * Python BPyMeshObject attributes
 *
 ************************************************************************/

static PyObject *LPointSeq_CreatePyObject( BPyLatticeObject * bpylat )
{
	
	BPyLPointSeqObject *obj = PyObject_NEW( BPyLPointSeqObject, &BPyLPointSeq_Type);
	obj->bpylat = bpylat;
	Py_INCREF(bpylat);
	
	/*
	an iter of -1 means this seq has not been used as an iterator yet
	once it is, then any other calls on getIter will return a new copy of BPyMPointSeq
	This means you can loop do nested loops with the same iterator without worrying about
	the iter variable being used twice and messing up the loops.
	*/
	obj->iter = -1;
	return (PyObject *)obj;
}

static PyObject *Lattice_getPoints( BPyLatticeObject * self )
{
	return LPointSeq_CreatePyObject(self);
}

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef BPyLattice_getset[] = {
	{"subdU", (getter)Lattice_getSubD, (setter)Lattice_setSubD,
	 "lattice U subdivision ", (void *)0},
	{"subdV", (getter)Lattice_getSubD, (setter)Lattice_setSubD,
	 "lattice V subdivision", (void *)1},
	{"subdW", (getter)Lattice_getSubD, (setter)Lattice_setSubD,
	 "lattice W subdivision", (void *)2},
	 
	{"interpU", (getter)Lattice_getInterp, (setter)Lattice_setInterp,
	 "lattice U interpolation type", (void *)0},
	{"interpV", (getter)Lattice_getInterp, (setter)Lattice_setInterp,
	 "lattice V interpolation type", (void *)1},
	{"interpW", (getter)Lattice_getInterp, (setter)Lattice_setInterp,
	 "lattice W interpolation type", (void *)2},

	{"outside", (getter)Lattice_getOutside, (setter)Lattice_setOutside,
	 "if enabled, only draw the outer grid", NULL},

	 {"points", (getter)Lattice_getPoints, NULL,
	 "lattice verts", NULL},

	{"key", (getter)Lattice_getKey, NULL,
	 "lattice key", NULL},
	 {NULL}  /* Sentinel */
};

/* this types constructor */
static PyObject *Lattice_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	char *name="Lattice";
	ID *id;
	
	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"(name) - name must be a string argument" );
	
	id = (ID *)add_lattice( name );
	
	if (!id)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, could not create new latice data" );
	
	id->us = 0;
	return Lattice_CreatePyObject((Lattice *)id);
}

/*****************************************************************************/
/* Python BPyLattice_Type structure definition:		*/
/*****************************************************************************/
PyTypeObject BPyLattice_Type = {
	PyObject_HEAD_INIT( NULL ) 
	0,	/* ob_size */
	"Blender Lattice",	/* tp_name */
	sizeof( BPyLatticeObject ),	/* tp_basicsize */
	0,			/* tp_itemsize */
	/* methods */
	NULL,	/* tp_dealloc */
	0,		/* tp_print */
	NULL,	/* tp_getattr */
	NULL,	/* tp_setattr */
	NULL,	/* tp_compare */
	NULL,	/* tp_repr */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	NULL,                       /* PySequenceMethods *tp_as_sequence; */
	NULL,                       /* PyMappingMethods *tp_as_mapping; */

	/* More standard operations (here for binary compatibility) */

	NULL,						/* hashfunc tp_hash; */
	NULL,                       /* ternaryfunc tp_call; */
	NULL,                       /* reprfunc tp_str; */
	NULL,                       /* getattrofunc tp_getattro; */
	NULL,                       /* setattrofunc tp_setattro; */

	/* Functions to access object as input/output buffer */
	NULL,                       /* PyBufferProcs *tp_as_buffer; */

  /*** Flags to define presence of optional/expanded features ***/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* long tp_flags; */

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
	BPyLattice_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyLattice_getset,         /* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,                       /* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc ) Lattice_new,	/* newfunc tp_new; */
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



/************************************************************************
 *
 * Pointex sequence 
 *
 ************************************************************************/

static int LPointSeq_len( BPyLPointSeqObject * self )
{
	return LATTICE_TOTPOINTS(self->bpylat->lattice);
}

/*
 * retrive a single LPoint from somewhere in the vertex list
 */

static PyObject *LPointSeq_item( BPyLPointSeqObject * self, int i )
{
	if( i < 0 || i >= LATTICE_TOTPOINTS(self->bpylat->lattice) )
		return EXPP_ReturnPyObjError( PyExc_IndexError,
					      "array index out of range" );

	return LPoint_CreatePyObject( self->bpylat, i );
}

static PySequenceMethods LPointSeq_as_sequence = {
	( inquiry ) LPointSeq_len,	/* sq_length */
	( binaryfunc ) 0,	/* sq_concat */
	( intargfunc ) 0,	/* sq_repeat */
	( intargfunc ) LPointSeq_item,	/* sq_item */
	( intintargfunc ) NULL,	/* sq_slice */
	( intobjargproc ) NULL,	/* sq_ass_item */
	( intintobjargproc ) NULL,	/* sq_ass_slice */
	0,0,0,
};

/************************************************************************
 *
 * Python BPyLPointSeq_Type iterator (iterates over vertices)
 *
 ************************************************************************/

/*
 * Initialize the interator index
 */

static PyObject *LPointSeq_getIter( BPyLPointSeqObject * self )
{
	if (self->iter==-1) { /* iteration for this pyobject is not yet used, just return self */
		self->iter = 0;
		return EXPP_incr_ret ( (PyObject *) self );
	} else {
		/* were alredy using this as an iterator, make a copy to loop on */
		BPyLPointSeqObject *seq = (BPyLPointSeqObject *)LPointSeq_CreatePyObject(self->bpylat);
		seq->iter = 0;
		return (PyObject *)seq;
	}
}

/*
 * Return next LPoint.
 */

static PyObject *LPointSeq_nextIter( BPyLPointSeqObject * self )
{
	if( self->iter == LATTICE_TOTPOINTS(self->bpylat->lattice) ) {
		self->iter= -1; /* allow it to be used as an iterator again without creating a new BPyLPointSeqObject */
		return EXPP_ReturnPyObjError( PyExc_StopIteration,
				"iterator at end" );
	}

	return LPoint_CreatePyObject( self->bpylat, self->iter++ );
}

static struct PyMethodDef BPyLPointSeq_methods[] = {
	{NULL, NULL, 0, NULL}
};

static PyGetSetDef BPyLPointSeq_getseters[] = {
	/*{"selected",
	(getter)LPointSeq_PropertyList, (setter)NULL,
	"vertex property layers, read only",
	NULL},*/
	{NULL}  /* Sentinel */
};

static void LPointSeq_dealloc( BPyLPointSeqObject * self )
{
	Py_DECREF(self->bpylat);
	PyObject_DEL( self );
}


/*****************************************************************************/
/* Python BPyLPointSeq_Type structure definition:                               */
/*****************************************************************************/
PyTypeObject BPyLPointSeq_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender LPointSeq",           /* char *tp_name; */
	sizeof( BPyLPointSeqObject ),       /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) LPointSeq_dealloc,	/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,                       /* cmpfunc tp_compare; */
	NULL,                       /* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	&LPointSeq_as_sequence,	    /* PySequenceMethods *tp_as_sequence; */
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
	( getiterfunc) LPointSeq_getIter, /* getiterfunc tp_iter; */
	( iternextfunc ) LPointSeq_nextIter, /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	BPyLPointSeq_methods,       /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyLPointSeq_getseters,     /* struct PyGetSetDef *tp_getset; */
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



/*
 * get a vertex's coordinate
 */

static PyObject *LPoint_getCoord( BPyLPointObject * self )
{
	BPoint *p = LPoint_get_pointer( self );
	if( !p )
		return NULL; /* error is set */

	/* having the Vector use the LPoint for genlib is an exception
	 * to the rule and needs to be especially accounted for 
	 * 
	 * We need this so a vector can make sure its data has not been
	 * removed and reallocated since it was created */
	return Vector_CreatePyObject( p->vec, 3, (PyObject *)self );
}

/*
 * set a vertex's coordinate
 */

static int LPoint_setCoord( BPyLPointObject * self, BPyVectorObject * value )
{
	int i;
	BPoint *p = LPoint_get_pointer( self );
	if( !p )
		return -1; /* error is set */

	if( !BPyVector_Check( value ) || value->size != 3 )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected vector argument of size 3" );

	for( i=0; i<3 ; ++i)
		p->vec[i] = value->vec[i];

	return 0;
}


/*
 * get a vertex's selection
 */

static PyObject *LPoint_getSel( BPyLPointObject * self )
{
	BPoint *p = LPoint_get_pointer( self );
	if( !p )
		return NULL; /* error is set */
	
	if (p->f1 & SELECT)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

/*
 * set a vertex's selection
 */

static int LPoint_setSel( BPyLPointObject * self, PyObject * value )
{
	BPoint *p = LPoint_get_pointer( self );
	if( !p )
		return -1; /* error is set */
	
	if (PyObject_IsTrue(value))
		p->f1 |= SELECT;
	else
		p->f1 &= ~SELECT;
		
	return 0;
}


/*
 * get a vertex's index
 */

static PyObject *LPoint_getIndex( BPyLPointObject * self )
{
	if( self->index >= LATTICE_TOTPOINTS(self->bpylat->lattice) )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"LPoint is no longer valid" );

	return PyInt_FromLong( self->index );
}



/************************************************************************
 *
 * Python BPyLPoint_Type attributes get/set structure
 *
 ************************************************************************/

static PyGetSetDef BPyLPoint_getseters[] = {
	{"co",
	 (getter)LPoint_getCoord, (setter)LPoint_setCoord,
	 "vertex's coordinate",
	 NULL},
	{"index",
	 (getter)LPoint_getIndex, (setter)NULL,
	 "vertex's index",
	 NULL},
	{"sel",
	 (getter)LPoint_getSel, (setter)LPoint_setSel,
	 "vertex's select status",
	 NULL},
	{NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};

/************************************************************************
 *
 * Python BPyLPoint_Type standard operations
 *
 ************************************************************************/


static BPoint * LPoint_get_pointer( BPyLPointObject * self )
{
	if (self->index >= LATTICE_TOTPOINTS(self->bpylat->lattice))
		return (BPoint *)EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"LPoint is no longer valid" );
	
	return &(self->bpylat->lattice)->def[self->index];
}


static void LPoint_dealloc( BPyLPointObject * self )
{
	Py_DECREF(self->bpylat);
	PyObject_DEL( self );
}

static int LPoint_compare( BPyLPointObject * a, BPyLPointObject * b )
{
	return( a->bpylat->lattice == b->bpylat->lattice && a->index == b->index ) ? 0 : -1;
}

static PyObject *LPoint_repr( BPyLPointObject * self )
{
	char format[512];
	BPoint *p = LPoint_get_pointer( self );

	if( !p ) {
		PyErr_Clear(); /* clear error from MFace_get_pointer */
		return PyString_FromString( "[LPoint <deleted>]");		
	}

	sprintf( format, "[LPoint (%f %f %f) %i]",
			p->vec[0], p->vec[1], p->vec[2], self->index );

	return PyString_FromString( format );
}


static long LPoint_hash( BPyLPointObject *self )
{
	return (long)self->index;
}


/************************************************************************
 *
 * Python BPyLPoint_Type structure definition
 *
 ************************************************************************/

PyTypeObject BPyLPoint_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender LPoint",            /* char *tp_name; */
	sizeof( BPyLPointObject ),        /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) LPoint_dealloc,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) LPoint_compare,  /* cmpfunc tp_compare; */
	( reprfunc ) LPoint_repr,    /* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	NULL,	        			/* PySequenceMethods *tp_as_sequence; */
	NULL,                       /* PyMappingMethods *tp_as_mapping; */

	/* More standard operations (here for binary compatibility) */

	( hashfunc ) LPoint_hash,    /* hashfunc tp_hash; */
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
	NULL,          /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyLPoint_getseters,        /* struct PyGetSetDef *tp_getset; */
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


static PyObject *LPoint_CreatePyObject( BPyLatticeObject *bpylat, int i )
{
	BPyLPointObject *obj = (BPyLPointObject *)PyObject_NEW( BPyLPointObject, &BPyLPoint_Type );

	if( !obj )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"PyObject_New() failed" );

	obj->index = i;
	obj->bpylat = bpylat;
	Py_INCREF(bpylat);
	return (PyObject *)obj;
}

PyObject *LatticeType_Init( void )
{
	PyType_Ready( &BPyLattice_Type );
	return (PyObject *) &BPyLattice_Type;
}
PyObject *LatticePointSeqType_Init( void )
{
	PyType_Ready( &BPyLPointSeq_Type );
	return (PyObject *) &BPyLPointSeq_Type;
}
PyObject *LatticePointType_Init( void )
{
	PyType_Ready( &BPyLPoint_Type );
	return (PyObject *) &BPyLPoint_Type;
}

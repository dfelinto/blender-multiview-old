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
 * Contributor(s): Pontus Lidman, Johnny Matthews, Ken Hughes,
 *   Michael Reimpell
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "Key.h" /*This must come first*/
#include "vector.h"

#include "DNA_scene_types.h"

#include <BLI_blenlib.h>
#include <BKE_global.h>
#include <BKE_main.h>
#include <BKE_curve.h>
#include <BKE_library.h>
#include <BKE_utildefines.h>
#include "BIF_space.h"

#include "Ipocurve.h"
#include "NMesh.h" /* we create NMesh.NMVert objects */
#include "Ipo.h"
#include "BezTriple.h"

#include "BSE_editipo.h"
#include "mydevice.h"
#include "BKE_depsgraph.h"
#include "blendef.h"
#include "constant.h"
#include "gen_utils.h"
#include "gen_library.h"

#define KEY_TYPE_MESH    0
#define KEY_TYPE_CURVE   1
#define KEY_TYPE_LATTICE 2

static int V24_Key_compare( V24_BPy_Key * a, V24_BPy_Key * b );
static PyObject *V24_Key_repr( V24_BPy_Key * self );
static void V24_Key_dealloc( V24_BPy_Key * self );

static PyObject *V24_Key_getBlocks( V24_BPy_Key * self );
static PyObject *V24_Key_getType( V24_BPy_Key * self );
static PyObject *V24_Key_getRelative( V24_BPy_Key * self );
static PyObject *V24_Key_getIpo( V24_BPy_Key * self );
static int V24_Key_setIpo( V24_BPy_Key * self, PyObject * args );
static PyObject *V24_Key_getValue( V24_BPy_Key * self );
static int V24_Key_setRelative( V24_BPy_Key * self, PyObject * value );

static struct PyMethodDef V24_Key_methods[] = {
	{ "getBlocks", (PyCFunction) V24_Key_getBlocks, METH_NOARGS, "Get key blocks" },
	{ "getIpo", (PyCFunction) V24_Key_getIpo, METH_NOARGS, "Get key Ipo" },
	{ 0, 0, 0, 0 }
};

static PyGetSetDef V24_BPy_Key_getsetters[] = {
	{"type",(getter)V24_Key_getType, (setter)NULL,
	 "Key Type",NULL},
	{"value",(getter)V24_Key_getValue, (setter)NULL,
	 "Key value",NULL},
	{"ipo",(getter)V24_Key_getIpo, (setter)V24_Key_setIpo,
	 "Ipo linked to key",NULL},
	{"blocks",(getter)V24_Key_getBlocks, (setter)NULL,
	 "Blocks linked to the key",NULL},
	{"relative",(getter)V24_Key_getRelative, (setter)V24_Key_setRelative,
	 "Non-zero is key is relative",NULL},
	{NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};

static PyObject *V24_KeyBlock_getData( PyObject * self );
static PyObject *V24_KeyBlock_getCurval( V24_BPy_KeyBlock * self );
static PyObject *V24_KeyBlock_getName( V24_BPy_KeyBlock * self );
static PyObject *V24_KeyBlock_getPos( V24_BPy_KeyBlock * self );
static PyObject *V24_KeyBlock_getSlidermin( V24_BPy_KeyBlock * self );
static PyObject *V24_KeyBlock_getSlidermax( V24_BPy_KeyBlock * self );
static PyObject *V24_KeyBlock_getVgroup( V24_BPy_KeyBlock * self );

static int V24_KeyBlock_setName( V24_BPy_KeyBlock *, PyObject * args  );
static int V24_KeyBlock_setVgroup( V24_BPy_KeyBlock *, PyObject * args  );
static int V24_KeyBlock_setSlidermin( V24_BPy_KeyBlock *, PyObject * args  );
static int V24_KeyBlock_setSlidermax( V24_BPy_KeyBlock *, PyObject * args  );

static void V24_KeyBlock_dealloc( V24_BPy_KeyBlock * self );
static int V24_KeyBlock_compare( V24_BPy_KeyBlock * a, V24_BPy_KeyBlock * b );
static PyObject *V24_KeyBlock_repr( V24_BPy_KeyBlock * self );

static struct PyMethodDef V24_KeyBlock_methods[] = {
	{ "getData", (PyCFunction) V24_KeyBlock_getData, METH_NOARGS,
		"Get keyblock data" },
	{ 0, 0, 0, 0 }
};

static PyGetSetDef V24_BPy_KeyBlock_getsetters[] = {
		{"curval",(getter)V24_KeyBlock_getCurval, (setter)NULL,
		 "Current value of the corresponding IpoCurve",NULL},
		{"name",(getter)V24_KeyBlock_getName, (setter)V24_KeyBlock_setName,
		 "Keyblock Name",NULL},
		{"pos",(getter)V24_KeyBlock_getPos, (setter)NULL,
		 "Keyblock Pos",NULL},
		{"slidermin",(getter)V24_KeyBlock_getSlidermin, (setter)V24_KeyBlock_setSlidermin,
		 "Keyblock Slider Minimum",NULL},
		{"slidermax",(getter)V24_KeyBlock_getSlidermax, (setter)V24_KeyBlock_setSlidermax,
		 "Keyblock Slider Maximum",NULL},
		{"vgroup",(getter)V24_KeyBlock_getVgroup, (setter)V24_KeyBlock_setVgroup,
		 "Keyblock VGroup",NULL},
		{"data",(getter)V24_KeyBlock_getData, (setter)NULL,
		 "Keyblock VGroup",NULL},
		{NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};

PyTypeObject V24_Key_Type = {
	PyObject_HEAD_INIT( NULL ) 0,	/*ob_size */
	"Blender Key",					/*tp_name */
	sizeof( V24_BPy_Key ),				/*tp_basicsize */
	0,								/*tp_itemsize */
	/* methods */
	( destructor ) V24_Key_dealloc,/* destructor tp_dealloc; */
	( printfunc ) 0,				/*tp_print */
	( getattrfunc ) 0,	/*tp_getattr */
	( setattrfunc ) 0,			 	/*tp_setattr */
	( cmpfunc) V24_Key_compare, 		/*tp_compare*/
	( reprfunc ) V24_Key_repr, 			/* tp_repr */
	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	NULL,                       /* PySequenceMethods *tp_as_sequence; */
	NULL,                       /* PyMappingMethods *tp_as_mapping; */

	/* More standard operations (here for binary compatibility) */

	( hashfunc ) V24_GenericLib_hash,	/* hashfunc tp_hash; */
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
	V24_Key_methods,           		/* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	V24_BPy_Key_getsetters,     	/* struct PyGetSetDef *tp_getset; */
	NULL,                       /* struct _typeobject *tp_base; */
	NULL,                      	/* PyObject *tp_dict; */
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

PyTypeObject V24_KeyBlock_Type = {
	PyObject_HEAD_INIT( NULL ) 0,	/*ob_size */
	"Blender KeyBlock",	/*tp_name */
	sizeof( V24_BPy_KeyBlock ),	/*tp_basicsize */
	0,			/*tp_itemsize */
	/* methods */
	( destructor ) V24_KeyBlock_dealloc,/* destructor tp_dealloc; */
	( printfunc ) 0,				/*tp_print */
	( getattrfunc ) 0,	/*tp_getattr */
	( setattrfunc ) 0,			 	/*tp_setattr */
	( cmpfunc) V24_KeyBlock_compare, 		/*tp_compare*/
	( reprfunc ) V24_KeyBlock_repr, 			/* tp_repr */
	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	NULL,                       /* PySequenceMethods *tp_as_sequence; */
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
	V24_KeyBlock_methods, 			/* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	V24_BPy_KeyBlock_getsetters,    /* struct PyGetSetDef *tp_getset; */
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

PyObject *V24_Key_CreatePyObject( Key * blenkey )
{
	V24_BPy_Key *bpykey = PyObject_NEW( V24_BPy_Key, &V24_Key_Type );
	/* blenkey may be NULL so be careful */
	bpykey->key = blenkey;
	return ( PyObject * ) bpykey;
}

static void V24_Key_dealloc( V24_BPy_Key * self )
{
	PyObject_DEL( self );
}

static int V24_Key_compare( V24_BPy_Key * a, V24_BPy_Key * b )
{
	return ( a->key == b->key ) ? 0 : -1;
}

static PyObject *V24_Key_repr( V24_BPy_Key * self )
{
	return PyString_FromFormat( "[Key \"%s\"]", self->key->id.name + 2 );
}

static PyObject *V24_Key_getIpo( V24_BPy_Key * self )
{
	if (self->key->ipo)
		return V24_Ipo_CreatePyObject( self->key->ipo );
	Py_RETURN_NONE;
}

static int V24_Key_setIpo( V24_BPy_Key * self, PyObject * value )
{
	return V24_GenericLib_assignData(value, (void **) &self->key->ipo, 0, 1, ID_IP, ID_KE);
}

static PyObject *V24_Key_getRelative( V24_BPy_Key * self )
{
	if( self->key->type == KEY_RELATIVE )
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static int V24_Key_setRelative( V24_BPy_Key * self, PyObject * value )
{
	int param = PyObject_IsTrue( value );
	if( param == -1 )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected True/False or 0/1" );
	
	if( param )
		self->key->type = KEY_RELATIVE;
	else
		self->key->type = KEY_NORMAL;
	allqueue(REDRAWIPO, 0);
	allspace(REMAKEIPO, 0);

	return 0;
}

static PyObject *V24_Key_getType( V24_BPy_Key * self )
{
	int idcode;
	int type = -1;

	idcode = GS( self->key->from->name );

	switch( idcode ) {
	case ID_ME:
		type = KEY_TYPE_MESH;
		break;
	case ID_CU:
		type = KEY_TYPE_CURVE;
		break;
	case ID_LT:
		type = KEY_TYPE_LATTICE;
		break;
	}

	return PyInt_FromLong( type );
}

static PyObject *V24_Key_getBlocks( V24_BPy_Key * self )
{
	Key *key = self->key;
	KeyBlock *kb;
	int i=0;
	PyObject *l = PyList_New( BLI_countlist( &(key->block)) );

	for (kb = key->block.first; kb; kb = kb->next, i++)
		PyList_SET_ITEM( l, i, V24_KeyBlock_CreatePyObject( kb, key ) );
	
	return l;
}

static PyObject *V24_Key_getValue( V24_BPy_Key * self )
{
	V24_BPy_Key *k = ( V24_BPy_Key * ) self;

	return PyFloat_FromDouble( k->key->curval );
}

/* ------------ Key Block Functions -------------- */
PyObject *V24_KeyBlock_CreatePyObject( KeyBlock * keyblock, Key *parentKey )
{
	V24_BPy_KeyBlock *bpykb = PyObject_NEW( V24_BPy_KeyBlock, &V24_KeyBlock_Type );
	bpykb->key = parentKey;
	bpykb->keyblock = keyblock; /* keyblock maye be NULL, thats ok */
	return ( PyObject * ) bpykb;
}

static PyObject *V24_KeyBlock_getCurval( V24_BPy_KeyBlock * self ) {
	return PyFloat_FromDouble( self->keyblock->curval );
}

static PyObject *V24_KeyBlock_getName( V24_BPy_KeyBlock * self ) {
	return PyString_FromString(self->keyblock->name);
}

static PyObject *V24_KeyBlock_getPos( V24_BPy_KeyBlock * self ){
	return PyFloat_FromDouble( self->keyblock->pos );			
}

static PyObject *V24_KeyBlock_getSlidermin( V24_BPy_KeyBlock * self ){
	return PyFloat_FromDouble( self->keyblock->slidermin );	
}

static PyObject *V24_KeyBlock_getSlidermax( V24_BPy_KeyBlock * self ){
	return PyFloat_FromDouble( self->keyblock->slidermax );
}

static PyObject *V24_KeyBlock_getVgroup( V24_BPy_KeyBlock * self ){
	return PyString_FromString(self->keyblock->vgroup);
}

static int V24_KeyBlock_setName( V24_BPy_KeyBlock * self, PyObject * args ){
	char* text = NULL;
 
	text = PyString_AsString ( args );
	if( !text )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					      "expected string argument" );							
	strncpy( self->keyblock->name, text , 32);

	return 0;	
}

static int V24_KeyBlock_setVgroup( V24_BPy_KeyBlock * self, PyObject * args  ){
	char* text = NULL;

	text = PyString_AsString ( args );
	if( !text )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					      "expected string argument" );							
	strncpy( self->keyblock->vgroup, text , 32);

	return 0;	
}
static int V24_KeyBlock_setSlidermin( V24_BPy_KeyBlock * self, PyObject * args  ){
	return V24_EXPP_setFloatClamped ( args, &self->keyblock->slidermin,
								-10.0f,
								10.0f );	
}
static int V24_KeyBlock_setSlidermax( V24_BPy_KeyBlock * self, PyObject * args  ){
	return V24_EXPP_setFloatClamped ( args, &self->keyblock->slidermax,
								-10.0f,
								10.0f );
}

static void V24_KeyBlock_dealloc( V24_BPy_KeyBlock * self )
{
	PyObject_DEL( self );
}

static int V24_KeyBlock_compare( V24_BPy_KeyBlock * a, V24_BPy_KeyBlock * b )
{
	return ( a->keyblock == b->keyblock ) ? 0 : -1;
}

static PyObject *V24_KeyBlock_repr( V24_BPy_KeyBlock * self )
{
	return PyString_FromFormat( "[KeyBlock \"%s\"]", self->keyblock->name );
}


static Curve *find_curve( Key *key )
{
	Curve *cu;

	if( !key )
		return NULL;

	for( cu = G.main->curve.first; cu; cu = cu->id.next ) {
		if( cu->key == key )
			break;
	}
	return cu;
}

static PyObject *V24_KeyBlock_getData( PyObject * self )
{
	/* If this is a mesh key, data is an array of MVert coords.
	   If lattice, data is an array of BPoint coords
	   If curve, data is an array of BezTriple or BPoint */

	char *datap;
	int datasize;
	int idcode;
	int i;
	Curve *cu;
	Nurb* nu;
	PyObject *l;
	V24_BPy_KeyBlock *kb = ( V24_BPy_KeyBlock * ) self;
	Key *key = kb->key;

	if( !kb->keyblock->data ) {
		Py_RETURN_NONE;
	}

	l = PyList_New( kb->keyblock->totelem );
	if( !l )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
				"PyList_New() failed" );							

	idcode = GS( key->from->name );

	switch(idcode) {
	case ID_ME:

		for (i=0, datap = kb->keyblock->data; i<kb->keyblock->totelem; i++) {
			PyObject *vec = V24_newVectorObject((float*)datap, 3, Py_WRAP);
			
			if (!vec) return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					  "could not allocate memory for Blender.Mathutils.Vector wrapper!" );

			PyList_SetItem(l, i, vec);
			datap += kb->key->elemsize;
		}
		break;

	case ID_CU:
		cu = find_curve ( key );
		if( !cu )
			return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "key is no linked to any curve!" );							
		datasize = count_curveverts(&cu->nurb);
		nu = cu->nurb.first;
		if( nu->bezt ) {
			datasize /= 3;
			Py_DECREF (l);	
			l = PyList_New( datasize );
			for( i = 0, datap = kb->keyblock->data; i < datasize;
					i++, datap += sizeof(float)*3*4) {
				PyObject *tuple = PyTuple_New(4), *vec;
				float *vecs = (float*)datap;
				
				if (!tuple) return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					  "PyTuple_New() failed!" );
					  
				vec = V24_newVectorObject(vecs, 3, Py_WRAP);
				if (!vec) return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					  "Could not allocate memory for Blender.Mathutils.Vector wrapper!" );
					  
				PyTuple_SET_ITEM( tuple, 0, vec);
				
				vecs += 3;
				vec = V24_newVectorObject(vecs, 3, Py_WRAP);
				if (!vec) return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					  "Could not allocate memory for Blender.Mathutils.Vector wrapper!" );

				PyTuple_SET_ITEM( tuple, 1, vec);
				
				vecs += 3;
				vec = V24_newVectorObject(vecs, 3, Py_WRAP);
				if (!vec) return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					  "Could not allocate memory for Blender.Mathutils.Vector wrapper!" );

				PyTuple_SET_ITEM( tuple, 2, vec);
				
				/*tilts*/
				vecs += 3;				
				vec = V24_newVectorObject(vecs, 3, Py_WRAP);
				if (!vec) return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					  "Could not allocate memory for Blender.Mathutils.Vector wrapper!" );

				PyTuple_SET_ITEM( tuple, 3, vec);
				
				PyList_SetItem( l, i, tuple );
			}
		} else {
			for( i = 0, datap = kb->keyblock->data; i < datasize;
					i++, datap += kb->key->elemsize ) {
				PyObject *vec = V24_newVectorObject((float*)datap, 4, Py_WRAP);
				if (!vec) return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					  "could not allocate memory for Blender.Mathutils.Vector wrapper!" );
				
				PyList_SetItem( l, i, vec );
			}
		}
		break;

	case ID_LT:
		for( i = 0, datap = kb->keyblock->data; i < kb->keyblock->totelem;
				i++, datap += kb->key->elemsize ) {
			PyObject *vec = V24_newVectorObject((float*)datap, 3, Py_WRAP);
			if (!vec) return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
					  "Could not allocate memory for Blender.Mathutils.Vector wrapper!" );
			
			PyList_SetItem( l, i, vec );
		}
		break;
	}
	
	return l;
}

static PyObject *V24_M_Key_Get( PyObject * self, PyObject * args )
{
	char *name = NULL;
	Key *key_iter;
	char error_msg[64];
	int i;

	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
			"expected string argument (or nothing)" ) );

	if ( name ) {
		for (key_iter = G.main->key.first; key_iter; 
				key_iter=key_iter->id.next) {
			if  (strcmp ( key_iter->id.name + 2, name ) == 0 ) {
				return V24_Key_CreatePyObject( key_iter );
			}
		}

		PyOS_snprintf( error_msg, sizeof( error_msg ),
			"Key \"%s\" not found", name );
		return V24_EXPP_ReturnPyObjError ( PyExc_NameError, error_msg );
		
	} else {

		PyObject *keylist;

		keylist = PyList_New( BLI_countlist( &( G.main->key ) ) );

		for ( i=0, key_iter = G.main->key.first; key_iter;
				key_iter=key_iter->id.next, i++ ) {
			PyList_SetItem(keylist, i, V24_Key_CreatePyObject(key_iter));
		}
		return keylist;
	}
}

struct PyMethodDef V24_M_Key_methods[] = {
	{"Get", V24_M_Key_Get, METH_VARARGS, "Get a key or all key names"},
	{NULL, NULL, 0, NULL}
};

static PyObject *V24_M_Key_TypesDict( void )
{
	PyObject *T = V24_PyConstant_New(  );

	if( T ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) T;

		V24_PyConstant_Insert( d, "MESH", PyInt_FromLong( KEY_TYPE_MESH ) );
		V24_PyConstant_Insert( d, "CURVE", PyInt_FromLong( KEY_TYPE_CURVE ) );
		V24_PyConstant_Insert( d, "LATTICE", PyInt_FromLong( KEY_TYPE_LATTICE ) );
	}

	return T;
}

PyObject *V24_Key_Init( void )
{
	PyObject *V24_submodule;
	PyObject *Types = NULL;

	if( PyType_Ready( &V24_Key_Type ) < 0 || PyType_Ready( &V24_KeyBlock_Type ) < 0 )
		return NULL;

	V24_submodule =
		Py_InitModule3( "Blender.Key", V24_M_Key_methods, "Key module" );

	Types = V24_M_Key_TypesDict(  );
	if( Types )
		PyModule_AddObject( V24_submodule, "Types", Types );

	return V24_submodule;
}


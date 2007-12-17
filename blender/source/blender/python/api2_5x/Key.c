/* 
 * $Id: Key.c 11446 2007-07-31 16:11:32Z campbellbarton $
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

#include "DNA_scene_types.h"
#include "DNA_meshdata_types.h"

#include <BLI_blenlib.h>
#include <BKE_global.h>
#include <BKE_main.h>
#include <BKE_curve.h>
#include <BKE_library.h>
#include "BIF_space.h"

#include "Ipocurve.h"
#include "Ipo.h"
#include "BezTriple.h"
#include "vector.h"

/* for their types */
#include "Mesh.h"
#include "Lattice.h"
#include "Curve.h"

#include "BSE_editipo.h"
#include "mydevice.h"
#include "BKE_depsgraph.h"
#include "blendef.h"
#include "gen_utils.h"
#include "gen_library.h"

#define KEY_TYPE_MESH    0
#define KEY_TYPE_CURVE   1
#define KEY_TYPE_LATTICE 2

/* macro from blenkernel/intern/key.c:98 */
#define GS(a)	(*((short *)(a)))

static PyObject *Key_repr( BPyKeyObject * self );

static PyObject *Key_getBlocks( BPyKeyObject * self );
static PyObject *Key_getType( BPyKeyObject * self );
static PyObject *Key_getRelative( BPyKeyObject * self );
static PyObject *Key_getIpo( BPyKeyObject * self );
static int Key_setIpo( BPyKeyObject * self, PyObject * args );
static PyObject *Key_getValue( BPyKeyObject * self );
static int Key_setRelative( BPyKeyObject * self, PyObject * value );

static PyGetSetDef BPyKey_getset[] = {
	{"type",(getter)Key_getType, (setter)NULL,
	 "Key Type",NULL},
	{"value",(getter)Key_getValue, (setter)NULL,
	 "Key value",NULL},
	{"ipo",(getter)Key_getIpo, (setter)Key_setIpo,
	 "Ipo linked to key",NULL},
	{"blocks",(getter)Key_getBlocks, (setter)NULL,
	 "Blocks linked to the key",NULL},
	{"relative",(getter)Key_getRelative, (setter)Key_setRelative,
	 "Non-zero is key is relative",NULL},
	{NULL}  /* Sentinel */
};

static PyObject *KeyBlock_getData( PyObject * self );
static PyObject *KeyBlock_getCurval( BPyKeyBlockObject * self );
static PyObject *KeyBlock_getName( BPyKeyBlockObject * self );
static PyObject *KeyBlock_getPos( BPyKeyBlockObject * self );
static PyObject *KeyBlock_getSlidermin( BPyKeyBlockObject * self );
static PyObject *KeyBlock_getSlidermax( BPyKeyBlockObject * self );
static PyObject *KeyBlock_getVgroup( BPyKeyBlockObject * self );

static int KeyBlock_setName( BPyKeyBlockObject *, PyObject * args  );
static int KeyBlock_setVgroup( BPyKeyBlockObject *, PyObject * args  );
static int KeyBlock_setSlidermin( BPyKeyBlockObject *, PyObject * args  );
static int KeyBlock_setSlidermax( BPyKeyBlockObject *, PyObject * args  );

static PyObject *KeyBlock_repr( BPyKeyBlockObject * self );

static struct PyMethodDef KeyBlock_methods[] = {
	{ 0, 0, 0, 0 }
};

static PyGetSetDef BPyKeyBlockObject_getset[] = {
		{"curval",(getter)KeyBlock_getCurval, (setter)NULL,
		 "Current value of the corresponding IpoCurve",NULL},
		{"name",(getter)KeyBlock_getName, (setter)KeyBlock_setName,
		 "Keyblock Name",NULL},
		{"pos",(getter)KeyBlock_getPos, (setter)NULL,
		 "Keyblock Pos",NULL},
		{"slidermin",(getter)KeyBlock_getSlidermin, (setter)KeyBlock_setSlidermin,
		 "Keyblock Slider Minimum",NULL},
		{"slidermax",(getter)KeyBlock_getSlidermax, (setter)KeyBlock_setSlidermax,
		 "Keyblock Slider Maximum",NULL},
		{"vgroup",(getter)KeyBlock_getVgroup, (setter)KeyBlock_setVgroup,
		 "Keyblock VGroup",NULL},
		{"data",(getter)KeyBlock_getData, (setter)NULL,
		 "Keyblock VGroup",NULL},
		{NULL}  /* Sentinel */
};

PyTypeObject BPyKey_Type = {
	PyObject_HEAD_INIT( NULL ) 0,	/*ob_size */
	"Blender Key",					/*tp_name */
	sizeof( BPyKeyObject ),				/*tp_basicsize */
	0,								/*tp_itemsize */
	/* methods */
	NULL,                       /* tp_dealloc; */
	0,								/*tp_print */
	0,								/*tp_getattr */
	0,								/*tp_setattr */
	NULL,							/*tp_compare*/
	( reprfunc ) Key_repr, 			/* tp_repr */
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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,	/* long tp_flags; */

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
	NULL,						/* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyKey_getset,				/* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,                       /* struct _typeobject *tp_base; */
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

PyTypeObject BPyKeyBlock_Type = {
	PyObject_HEAD_INIT( NULL ) 0,	/*ob_size */
	"Blender KeyBlock",	/*tp_name */
	sizeof( BPyKeyBlockObject ),	/*tp_basicsize */
	0,			/*tp_itemsize */
	/* methods */
	NULL,                       /* tp_dealloc; */
	( printfunc ) 0,				/*tp_print */
	( getattrfunc ) 0,	/*tp_getattr */
	( setattrfunc ) 0,			 	/*tp_setattr */
	( cmpfunc) EXPP_Anonymous_compare, 		/*tp_compare*/
	( reprfunc ) KeyBlock_repr, 			/* tp_repr */
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
	KeyBlock_methods, 			/* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyKeyBlockObject_getset,		/* struct PyGetSetDef *tp_getset; */
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

PyObject *Key_CreatePyObject( Key * blenkey )
{
	BPyKeyObject *bpykey;
	
	if (!blenkey)
		Py_RETURN_NONE;
	
	bpykey = PyObject_NEW( BPyKeyObject, &BPyKey_Type );
	/* blenkey may be NULL so be careful */
	bpykey->key = blenkey;
	return ( PyObject * ) bpykey;
}

static PyObject *Key_repr( BPyKeyObject * self )
{
	return PyString_FromFormat( "[Key \"%s\"]", self->key->id.name + 2 );
}

static PyObject *Key_getIpo( BPyKeyObject * self )
{	/* self->key->ipo can be NULL, will return None */
	return Ipo_CreatePyObject( self->key->ipo );
}

static int Key_setIpo( BPyKeyObject * self, PyObject * value )
{
	return GenericLib_assignData(value, (void **) &self->key->ipo, 0, 1, ID_IP, ID_KE);
}

static PyObject *Key_getRelative( BPyKeyObject * self )
{
	if( self->key->type == KEY_RELATIVE )
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static int Key_setRelative( BPyKeyObject * self, PyObject * value )
{
	if( PyObject_IsTrue( value ) )
		self->key->type = KEY_RELATIVE;
	else
		self->key->type = KEY_NORMAL;
	allqueue(REDRAWIPO, 0);
	allspace(REMAKEIPO, 0);

	return 0;
}

static PyObject *Key_getType( BPyKeyObject * self )
{
	int idcode;
	PyObject *type;

	idcode = GS( self->key->from->name );

	switch( idcode ) {
	case ID_ME:
		type = (PyObject *)&BPyMesh_Type;
		break;
	case ID_CU:
		type = (PyObject *)&BPyCurve_Type;
		break;
	case ID_LT:
		type = (PyObject *)&BPyLattice_Type;
		break;
	}
	Py_INCREF(type);
	return type;
}

static PyObject *Key_getBlocks( BPyKeyObject * self )
{
	Key *key = self->key;
	KeyBlock *kb;
	int i=0;
	PyObject *l = PyList_New( BLI_countlist( &(key->block)) );

	for (kb = key->block.first; kb; kb = kb->next, i++)
		PyList_SET_ITEM( l, i, KeyBlock_CreatePyObject( kb, key ) );
	
	return l;
}

static PyObject *Key_getValue( BPyKeyObject * self )
{
	BPyKeyObject *k = ( BPyKeyObject * ) self;

	return PyFloat_FromDouble( k->key->curval );
}

/* ------------ Key Block Functions -------------- */
PyObject *KeyBlock_CreatePyObject( KeyBlock * keyblock, Key *parentKey )
{
	BPyKeyBlockObject *bpykb;
	
	/* WARNING, No situation where this is the case exists yet
	 * keyblock should never be NULL */
	if (!keyblock)
		Py_RETURN_NONE;
	
	bpykb = PyObject_NEW( BPyKeyBlockObject, &BPyKeyBlock_Type );
	bpykb->key = parentKey;
	bpykb->keyblock = keyblock; /* keyblock maye be NULL, thats ok */
	return ( PyObject * ) bpykb;
}

static PyObject *KeyBlock_getCurval( BPyKeyBlockObject * self ) {
	return PyFloat_FromDouble( self->keyblock->curval );
}

static PyObject *KeyBlock_getName( BPyKeyBlockObject * self ) {
	return PyString_FromString(self->keyblock->name);
}

static PyObject *KeyBlock_getPos( BPyKeyBlockObject * self ){
	return PyFloat_FromDouble( self->keyblock->pos );			
}

static PyObject *KeyBlock_getSlidermin( BPyKeyBlockObject * self ){
	return PyFloat_FromDouble( self->keyblock->slidermin );	
}

static PyObject *KeyBlock_getSlidermax( BPyKeyBlockObject * self ){
	return PyFloat_FromDouble( self->keyblock->slidermax );
}

static PyObject *KeyBlock_getVgroup( BPyKeyBlockObject * self ){
	return PyString_FromString(self->keyblock->vgroup);
}

static int KeyBlock_setName( BPyKeyBlockObject * self, PyObject * args ){
	char* text = NULL;
 
	text = PyString_AsString ( args );
	if( !text )
		return EXPP_ReturnIntError( PyExc_TypeError,
					      "expected string argument" );							
	strncpy( self->keyblock->name, text , 32);

	return 0;	
}

static int KeyBlock_setVgroup( BPyKeyBlockObject * self, PyObject * args  ){
	char* text = NULL;

	text = PyString_AsString ( args );
	if( !text )
		return EXPP_ReturnIntError( PyExc_TypeError,
					      "expected string argument" );							
	strncpy( self->keyblock->vgroup, text , 32);

	return 0;	
}
static int KeyBlock_setSlidermin( BPyKeyBlockObject * self, PyObject * args  ){
	return EXPP_setFloatClamped ( args, &self->keyblock->slidermin,
								-10.0f,
								10.0f );	
}
static int KeyBlock_setSlidermax( BPyKeyBlockObject * self, PyObject * args  ){
	return EXPP_setFloatClamped ( args, &self->keyblock->slidermax,
								-10.0f,
								10.0f );
}

static PyObject *KeyBlock_repr( BPyKeyBlockObject * self )
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

static PyObject *KeyBlock_getData( PyObject * self )
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
	BPyKeyBlockObject *kb = ( BPyKeyBlockObject * ) self;
	Key *key = kb->key;

	if( !kb->keyblock->data ) {
		Py_RETURN_NONE;
	}

	l = PyList_New( kb->keyblock->totelem );
	if( !l )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
				"PyList_New() failed" );							

	idcode = GS( key->from->name );

	switch(idcode) {
	case ID_ME:

		for (i=0, datap = kb->keyblock->data; i<kb->keyblock->totelem; i++) {
			MVert *vert = (MVert *) datap;
			PyList_SetItem(l, i, ( PyObject * ) Vector_CreatePyObject( vert->co, 3, (PyObject *)NULL));
			datap += kb->key->elemsize;
		}
		break;

	case ID_CU:
		cu = find_curve ( key );
		if( !cu )
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "key is no linked to any curve!" );							
		datasize = count_curveverts(&cu->nurb);
		nu = cu->nurb.first;
		if( nu->bezt ) {
			datasize /= 3;
			Py_DECREF (l);	
			l = PyList_New( datasize );
			for( i = 0, datap = kb->keyblock->data; i < datasize;
					i++, datap += sizeof(float)*12 ) {
				/* 
				 * since the key only stores the control point and not the
				 * other BezTriple attributes, build a Py_NEW BezTriple
				 */
				PyObject *pybt = newBezTriple( (float *)datap );
				PyList_SetItem( l, i, pybt );
			}
		} else {
			for( i = 0, datap = kb->keyblock->data; i < datasize;
					i++, datap += kb->key->elemsize ) {
				PyObject *pybt;
				float *fp = (float *)datap;
				pybt = Py_BuildValue( "[f,f,f]", fp[0],fp[1],fp[2]);
				if( !pybt ) {
					Py_DECREF( l );
					return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "Py_BuildValue() failed" );							
				}
				PyList_SetItem( l, i, pybt );
			}
		}
		break;

	case ID_LT:

		for( i = 0, datap = kb->keyblock->data; i < kb->keyblock->totelem;
				i++, datap += kb->key->elemsize ) {
			/* Lacking a python class for BPoint, use a list of three floats */
			PyObject *pybt;
			float *fp = (float *)datap;
			pybt = Py_BuildValue( "[f,f,f]", fp[0],fp[1],fp[2]);
			if( !pybt ) {
				Py_DECREF( l );
				return EXPP_ReturnPyObjError( PyExc_MemoryError,
					  "Py_BuildValue() failed" );							
			}
			PyList_SetItem( l, i, pybt );
		}
		break;
	}
	
	return l;
}

PyObject *KeyType_Init( void )
{
	PyType_Ready( &BPyKey_Type );
	BPyKey_Type.tp_dealloc = (destructor)&PyObject_Del;
	return (PyObject *) &BPyKey_Type;
}

PyObject *KeyBlockType_Init( void )
{
	PyType_Ready( &BPyKeyBlock_Type );
	BPyKeyBlock_Type.tp_dealloc = (destructor)&PyObject_Del;
	return (PyObject *) &BPyKeyBlock_Type;
}

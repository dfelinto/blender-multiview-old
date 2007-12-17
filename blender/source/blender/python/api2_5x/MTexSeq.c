/* 
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
 * Contributor(s): Willian P. Germano, Jacques Guignot, Joseph Gilbert,
 * Campbell Barton, Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include "Scene.h" /*This must come first */
#include "../BPY_extern.h"
#include "BKE_global.h"
#include "BKE_main.h"
#include "MEM_guardedalloc.h"	/* for MEM_callocN */
#include "DNA_screen_types.h"
#include "DNA_object_types.h" /* MTexSeq_new */
#include "BIF_space.h" /* for copy_view3d_lock */
#include "BKE_depsgraph.h"
#include "BKE_library.h"
#include "BKE_object.h"
#include "BKE_scene.h"
#include "BKE_font.h"
#include "BKE_idprop.h"
#include "BLI_blenlib.h" /* only for MTexSeq_new */
#include "BDR_editobject.h"		/* free_and_unlink_base() */
#include "mydevice.h"		/* for #define REDRAW */
#include "DNA_view3d_types.h"
#include "DNA_space_types.h"	/* SPACE_VIEW3D, SPACE_SEQ */
/* python types */
#include "Object.h"
#include "Camera.h"
/* only for MTexSeq_new */
#include "BKE_material.h"
#include "BLI_arithb.h"
#include "Armature.h"
#include "Lamp.h"
#include "Curve.h"
#include "Mesh.h"
#include "World.h"
#include "Lattice.h"
#include "Metaball.h"
#include "IDProp.h"
#include "CurveText.h"
#include "Library.h"

#include "gen_utils.h"
#include "gen_library.h"
#include "MTexSeq.h"
#include "MTex.h"



#include "BKE_utildefines.h" /* vec copy */
#include "vector.h"
#include "bpy_list.h"

/* internal use only - not used yet */
/*
MTex *MTexSeq_get_pointer( BPyMTexSeqObject *self )
{
	switch (GS(self->id)) {
	case ID_MA:
		return (((Material *)self->id)->mtex)[0];
	case ID_LA:
		return (((Lamp *)self->id)->mtex)[0];
	case ID_WO:
		return (((World *)self->id)->mtex)[0];
	}
	return NULL;
}*/

/************************************************************************
 *
 * Object Sequence 
 *
 ************************************************************************/


/*
 * create a thin wrapper for the scenes objects
 * this can be called directly from a getset's getattr as
 * long as the type is genlib compatible
 */

PyObject *MTexSeq_CreatePyObject( BPyGenericLibObject * genlib )
{
	int i;
	BPyMTexSeqObject *seq = PyObject_NEW( BPyMTexSeqObject, &BPyMTexSeq_Type);
	
	seq->id = genlib->id;
	seq->index = 0;
	
	for(i=0;i<10;i++)
		seq->bpymtex[i] = NULL;

	return (PyObject *)seq;
}

static int MTexSeq_len( BPyMTexSeqObject * self )
{
	return 10;
}

/*
 * retrive a single Object from somewhere in the Object list
 */

static PyObject *MTexSeq_item( BPyMTexSeqObject * self, int i )
{
	if (i>9 || i<0)
		return EXPP_ReturnPyObjError(PyExc_IndexError,
				"textures[index]: out of range");
	
	/*   odd design decision - always return an MTex, even if its invalid.
	     can then assign a texture to initialize the mtex.
	//if (!mtex[i])
	//	Py_RETURN_NONE;
	 */

	if (!self->bpymtex[i])
		self->bpymtex[i] = MTex_CreatePyObject( self->id, (short)i );
	
	Py_INCREF(self->bpymtex[i]);
	
	return self->bpymtex[i];
}

static PySequenceMethods MTexSeq_as_sequence = {
	( inquiry ) MTexSeq_len,	/* sq_length */
	( binaryfunc ) 0,	/* sq_concat */
	( intargfunc ) 0,	/* sq_repeat */
	( intargfunc ) MTexSeq_item,	/* sq_item */
	( intintargfunc ) 0,	/* sq_slice */
	( intobjargproc ) 0,	/* sq_ass_item */
	( intintobjargproc ) 0,	/* sq_ass_slice */
	0,0,0,
};


/************************************************************************
 *
 * Python MTexSeq_Type iterator (iterates over GroupObjects)
 *
 ************************************************************************/

/*
 * Initialize the interator index
 */

static PyObject *MTexSeq_getIter( BPyMTexSeqObject * self )
{
	/* create a new iterator if were alredy using this one */
	if (self->index == 0) {
		return EXPP_incr_ret ( (PyObject *) self );
	} else {
		return MTexSeq_CreatePyObject( (BPyGenericLibObject *)self );
	}
}

/*
 * Return next MTex.
 */

static PyObject *MTexSeq_nextIter( BPyMTexSeqObject * self )
{
	if (self->index>9)
		return EXPP_ReturnPyObjError( PyExc_StopIteration, "iterator at end" );
	return MTexSeq_item(self, self->index++);
}

/************************************************************************
 *
 * Python MTexSeq_Type standard operations
 *
 ************************************************************************/

static void MTexSeq_dealloc( BPyMTexSeqObject * self )
{
	int i;
	for (i=0;i<10;i++)
		Py_XDECREF(self->bpymtex[i]);

	PyObject_DEL( self );
}

/*
 * repr function
 * callback functions building meaninful string to representations
 */
static PyObject *MTexSeq_repr( BPyMTexSeqObject * self )
{
	
	return PyString_FromFormat( "[MTexSeq \"%s\"]", self->id->name + 2 );
}


/*****************************************************************************/
/* Python MTexSeq_Type structure definition:                               */
/*****************************************************************************/
PyTypeObject BPyMTexSeq_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender MTexSeq",           /* char *tp_name; */
	sizeof( BPyMTexSeqObject ),       /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) MTexSeq_dealloc,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) EXPP_Anonymous_compare, /* cmpfunc tp_compare; */
	( reprfunc ) MTexSeq_repr,   /* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	&MTexSeq_as_sequence,	/* PySequenceMethods *tp_as_sequence; */
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
	( getiterfunc) MTexSeq_getIter, /* getiterfunc tp_iter; */
	( iternextfunc ) MTexSeq_nextIter, /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	NULL,						/* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	NULL,						/* struct PyGetSetDef *tp_getset; */
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

PyObject *MTexSeqType_Init( void )
{
	PyType_Ready( &BPyMTexSeq_Type );
	return (PyObject *) &BPyMTexSeq_Type;
}

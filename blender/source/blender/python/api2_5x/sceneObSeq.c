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
#include "DNA_object_types.h" /* SceneObSeq_new */
#include "BIF_space.h" /* for copy_view3d_lock */
#include "BKE_depsgraph.h"
#include "BKE_library.h"
#include "BKE_object.h"
#include "BKE_scene.h"
#include "BKE_font.h"
#include "BKE_idprop.h"
#include "BLI_blenlib.h" /* only for SceneObSeq_new */
#include "BDR_editobject.h"		/* free_and_unlink_base() */
#include "mydevice.h"		/* for #define REDRAW */
#include "DNA_view3d_types.h"
#include "DNA_space_types.h"	/* SPACE_VIEW3D, SPACE_SEQ */
/* python types */
#include "Object.h"
#include "Camera.h"
/* only for SceneObSeq_new */
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
#include "sceneObSeq.h"



#include "BKE_utildefines.h" /* vec copy */
#include "vector.h"
#include "bpy_list.h"


enum obj_consts {
	EXPP_OBSEQ_NORMAL = 0,
	EXPP_OBSEQ_SELECTED,
	EXPP_OBSEQ_CONTEXT
};

/************************************************************************
 *
 * Object Sequence 
 *
 ************************************************************************/

/*
 * create a thin wrapper for the scenes objects
 */

/* accessed from scn.objects.selected or scn.objects.context */
static PyObject *SceneObSeq_getObjects( BPySceneObSeqObject *self, void *mode) 
{
	SCENE_DEL_CHECK_PY(self->bpyscene);
	return SceneObSeq_CreatePyObject(self->bpyscene, (char)((long)mode));
}

int SceneObSeq_setObjects( BPySceneObSeqObject *self, PyObject *value, void *_mode_) 
{
	/*
	ONLY SUPPORTS scn.objects.selected and scn.objects.context 
	cannot assign to scn.objects yet!!!
	*/
	PyObject *item;
	Scene *scene= self->bpyscene->scene;
	Object *blen_ob;
	Base *base;
	int size, mode = (int)_mode_;
	
	SCENE_DEL_CHECK_INT(self->bpyscene);
	
	/* scn.objects.selected = scn.objects  - shortcut to select all */
	if (BPySceneObSeq_Check(value)) {
		BPySceneObSeqObject *bpy_sceneseq = (BPySceneObSeqObject *)value;
		if (self->bpyscene->scene != bpy_sceneseq->bpyscene->scene)
			return EXPP_ReturnIntError( PyExc_ValueError,
					"Cannot assign a SceneObSeq type from another scene" );
		if (bpy_sceneseq->mode != EXPP_OBSEQ_NORMAL)
			return EXPP_ReturnIntError( PyExc_ValueError,
					"Can only assign scn.objects to scn.objects.context or scn.objects.selected" );
		
		for (base= scene->base.first; base; base= base->next) {
			base->flag |= SELECT;
			base->object->flag |= SELECT;
			
			if (mode==EXPP_OBSEQ_CONTEXT && G.vd) {
				base->object->lay= base->lay= G.vd->lay;
			}
		}
		return 0;
	}

	if (!PySequence_Check(value))
		return EXPP_ReturnIntError( PyExc_ValueError,
				"Error, must assign a sequence of objects to scn.objects.selected" );
	
	/* for context and selected, just deselect, dont remove */
	for (base= scene->base.first; base; base= base->next) {
		base->flag &= ~SELECT;
		base->object->flag &= ~SELECT;
	}
	
	size = PySequence_Length(value);
	while (size) {
		size--;
		item = PySequence_GetItem(value, size);
		if ( BPyObject_Check(item) ) {
			blen_ob= ((BPyObject *)item)->object;
			base = object_in_scene( blen_ob, scene );
			if (base) {
				blen_ob->flag |= SELECT;
				base->flag |= SELECT;
				if (mode==EXPP_OBSEQ_CONTEXT && G.vd) {
					blen_ob->restrictflag &= ~OB_RESTRICT_VIEW;
					blen_ob->lay= base->lay= G.vd->lay;
				}
			}
		}
		Py_DECREF(item);
	}
	return 0;
}


PyObject *SceneObSeq_CreatePyObject( BPySceneObject *self, int mode )
{
	BPySceneObSeqObject *seq = PyObject_NEW( BPySceneObSeqObject, &SceneObSeq_Type);
	seq->bpyscene = self; Py_INCREF(self);
	seq->index = 0;
	seq->mode = mode;
	return (PyObject *)seq;
}

static int SceneObSeq_len( BPySceneObSeqObject * self )
{
	Scene *scene= self->bpyscene->scene;
	SCENE_DEL_CHECK_INT(self->bpyscene);
	
	if (self->mode == EXPP_OBSEQ_NORMAL)
		return BLI_countlist( &( scene->base ) );
	else if (self->mode == EXPP_OBSEQ_SELECTED) {
		int len=0;
		Base *base;
		for (base= scene->base.first; base; base= base->next) {
			if (base->flag & SELECT) {
				len++;
			}
		}
		return len;
	} else if (self->mode == EXPP_OBSEQ_CONTEXT) {
		int len=0;
		Base *base;
		
		if( G.vd == NULL ) /* No 3d view has been initialized yet, simply return an empty list */
			return 0;
		
		for (base= scene->base.first; base; base= base->next) {
			if TESTBASE(base) {
				len++;
			}
		}
		return len;
	}
	/*should never run this */
	return 0;
}

/*
 * retrive a single Object from somewhere in the Object list
 */

static PyObject *SceneObSeq_item( BPySceneObSeqObject * self, int i )
{
	int index=0;
	Base *base= NULL;
	Scene *scene= self->bpyscene->scene;
	
	SCENE_DEL_CHECK_PY(self->bpyscene);
	
	/* objects */
	if (self->mode==EXPP_OBSEQ_NORMAL)
		for (base= scene->base.first; base && i!=index; base= base->next, index++) {}
	/* selected */
	else if (self->mode==EXPP_OBSEQ_SELECTED) {
		for (base= scene->base.first; base && i!=index; base= base->next)
			if (base->flag & SELECT)
				index++;
	}
	/* context */
	else if (self->mode==EXPP_OBSEQ_CONTEXT) {
		if (G.vd)
			for (base= scene->base.first; base && i!=index; base= base->next)
				if TESTBASE(base)
					index++;
	}
	
	if (!(base))
		return EXPP_ReturnPyObjError( PyExc_IndexError,
					      "array index out of range" );
	
	return Object_CreatePyObject( base->object );
}

static PySequenceMethods SceneObSeq_as_sequence = {
	( inquiry ) SceneObSeq_len,	/* sq_length */
	( binaryfunc ) 0,	/* sq_concat */
	( intargfunc ) 0,	/* sq_repeat */
	( intargfunc ) SceneObSeq_item,	/* sq_item */
	( intintargfunc ) 0,	/* sq_slice */
	( intobjargproc ) 0,	/* sq_ass_item */
	( intintobjargproc ) 0,	/* sq_ass_slice */
	0,0,0,
};


/************************************************************************
 *
 * Python SceneObSeq_Type iterator (iterates over GroupObjects)
 *
 ************************************************************************/

/*
 * Initialize the interator index
 */

static PyObject *SceneObSeq_getIter( BPySceneObSeqObject * self )
{	
	SCENE_DEL_CHECK_PY(self->bpyscene);
	
	/* create a new iterator if were alredy using this one */
	if (self->index == 0) {
		return EXPP_incr_ret ( (PyObject *) self );
	} else {
		return SceneObSeq_CreatePyObject(self->bpyscene, self->mode);
	}
}

/*
 * Return next SceneOb.
 */

/* internal use only */
static  Base *SceneObSeq_get_pointer( BPySceneObSeqObject * self )
{
	Base *base;
	int i;
	
	if (!self->bpyscene->scene)
		return NULL;
	
	base = self->bpyscene->scene->base.first;
	for (i=0; (base && i<self->index); i++, base= base->next) {}
	return base; /* can be NULL */
}

static PyObject *SceneObSeq_nextIter( BPySceneObSeqObject * self )
{
	Base *base = SceneObSeq_get_pointer(self);
	
	if (self->mode==EXPP_OBSEQ_SELECTED) {
		while (base && !(base->flag & SELECT)) {
			base= base->next;
			self->index++;
		}
	}
	else if (self->mode==EXPP_OBSEQ_CONTEXT) {
		if (!G.vd)
			base= NULL; /* will never iterate if we have no */
		else {
			while (base && !TESTBASE(base)) {
				base= base->next;
				self->index++;
			}
		}
	}
	
	if( !base )
		return EXPP_ReturnPyObjError( PyExc_StopIteration,
				"iterator at end" );
	
	self->index++; /* next item */
	return Object_CreatePyObject( base->object );
}

static PyObject *SceneObSeq_link( BPySceneObSeqObject * self, PyObject *args )
{	
	Scene *scene;
	BPyObject *bpy_obj;
	Object *object = NULL;
	
	SCENE_DEL_CHECK_PY(self->bpyscene);
	
	/* this shold eventually replace Scene_link */
	if (self->mode != EXPP_OBSEQ_NORMAL)
		return (EXPP_ReturnPyObjError( PyExc_TypeError,
					      "Cannot link to objects.selection or objects.context!" ));
	
	if( PyTuple_Size(args) == 1 ) {
		BPyLibraryDataObject *seq = ( BPyLibraryDataObject * )PyTuple_GET_ITEM( args, 0 );
		if( BPyLibraryData_Check( seq ) )
			return LibraryData_importLibData( seq, seq->name,
					( seq->kind == OBJECT_IS_LINK ? FILE_LINK : 0 ),
					self->bpyscene->scene );
	}
	
	/* link the object */
	if( !PyArg_ParseTuple( args, "O!", &BPyObject_Type, &bpy_obj ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected Object argument" );
	
	
		/*return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					          "Could not create data on demand for this object type!" );*/
	scene = self->bpyscene->scene;
	object = bpy_obj->object;
	
	/* Object.c's EXPP_add_obdata does not support these objects */
	if (!object->data && (object->type == OB_SURF || object->type == OB_FONT || object->type == OB_WAVE )) {
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "Object has no data and new data cant be automaticaly created for Surf, Text or Wave type objects!" );
	} else {
		/* Ok, all is fine, let's try to link it */
		Base *base;

		/* We need to link the object to a 'Base', then link this base
		 * to the scene.        See DNA_scene_types.h ... */

		/* check if this object has obdata, case not, try to create it */
		if( !object->data && ( object->type != OB_EMPTY ) )
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "This object has no data" );
		
		/* First, check if the object isn't already in the scene */
		base = object_in_scene( object, scene );
		/* if base is not NULL ... */
		if( base )	/* ... the object is already in one of the Scene Bases */
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
						      "object already in scene!" );
		
		/* not linked, go get mem for a new base object */
		base = MEM_callocN( sizeof( Base ), "pynewbase" );

		if( !base )
			return EXPP_ReturnPyObjError( PyExc_MemoryError,
						      "couldn't allocate new Base for object" );

		/*  if the object has not yet been linked to object data, then
		 *  set the real type before we try creating data */

		if( bpy_obj->realtype != OB_EMPTY ) {
			object->type = bpy_obj->realtype;
			bpy_obj->realtype = OB_EMPTY;
		}

		base->object = object;	/* link object to the new base */
		base->lay = object->lay;
		base->flag = object->flag;

		object->id.us += 1;	/* incref the object user count in Blender */

		BLI_addhead( &scene->base, base );	/* finally, link new base to scene */
	}

	Py_RETURN_NONE;
}

/* This is buggy with new object data not already linked to an object, for now use the above code */


PyObject *Scene_object_new( Scene * scene, PyObject *args )
{
	
	void *data = NULL;
	char *name = NULL;
	char *desc = NULL;
	short type = OB_EMPTY;
	struct Object *object;
	Base *base;
	PyObject *py_data;	

	if( !PyArg_ParseTuple( args, "O|s", &py_data, &name ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"scene.objects.new(obdata) - expected obdata to be\n\ta python obdata type or the string 'Empty'" );

	if( BPyArmature_Check( py_data ) ) {
		data = ( void * ) Armature_FromPyObject( py_data );
		type = OB_ARMATURE;
	} else if( BPyCamera_Check( py_data ) ) {
		data = ( void * ) Camera_FromPyObject( py_data );
		type = OB_CAMERA;
	} else if( BPyLamp_Check( py_data ) ) {
		data = ( void * ) Lamp_FromPyObject( py_data );
		type = OB_LAMP;
	} else if( BPyCurveBase_Check( py_data ) ) { /* curve or text3d */
		data = ( void * ) CurveBase_FromPyObject( py_data );
		type = OB_CURVE;
	} else if( BPyMesh_Check( py_data ) ) {
		data = ( void * ) Mesh_FromPyObject( py_data, NULL );
		type = OB_MESH;
	} else if( BPyLattice_Check( py_data ) ) {
		data = ( void * ) Lattice_FromPyObject( py_data );
		type = OB_LATTICE;
	} else if( BPyMetaball_Check( py_data ) ) {
		data = ( void * ) Metaball_FromPyObject( py_data );
		type = OB_MBALL;
	} else if( BPyCurveText_Check( py_data ) ) {
		data = ( void * ) CurveBase_FromPyObject( py_data );
		type = OB_FONT;
	} else if( ( desc = PyString_AsString( (PyObject *)py_data ) ) != NULL ) {
		if( !strcmp( desc, "Empty" ) ) {
			type = OB_EMPTY;
			data = NULL;
		} else
			goto typeError;
	} else {
typeError:
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected an object and optionally a string as arguments" );
	}

	if (!name) {
		if (type == OB_EMPTY)
			name = "Empty";
		else
			name = ((ID *)data)->name + 2;
	}
	
	object = add_only_object(type, name);
	
	if( data ) {
		object->data = data;
		id_us_plus((ID *)data);
	}
	
	object->flag = SELECT;
	
	/* creates the curve for the text object */
	if (type == OB_FONT) 
		text_to_curve(object, 0);
	
	/* link to scene */
	base = MEM_callocN( sizeof( Base ), "pynewbase" );

	if( !base )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
						  "couldn't allocate new Base for object" );

	base->object = object;	/* link object to the new base */
	
	if (scene == G.scene && G.vd) {
		if (G.vd->localview) {
			object->lay= G.vd->layact + G.vd->lay;
		} else {
			object->lay= G.vd->layact;
		}
	} else {
		base->lay= object->lay = scene->lay & ((1<<20)-1);	/* Layer, by default visible*/	
	}
	
	base->lay= object->lay;
	
	base->flag = SELECT;
	object->id.us = 1; /* we will exist once in this scene */

	BLI_addhead( &(scene->base), base );	/* finally, link new base to scene */
	
	/* make sure data and object materials are consistent */
	test_object_materials( (ID *)object->data );
	
	/* so we can deal with vertex groups */
	if (type == OB_MESH)
		((BPyMeshObject *)py_data)->object = object;
	
	return Object_CreatePyObject( object );
}

static PyObject *SceneObSeq_new( BPySceneObSeqObject * self, PyObject *args )
{
	
	SCENE_DEL_CHECK_PY(self->bpyscene);
	
	if (self->mode != EXPP_OBSEQ_NORMAL)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					"Cannot add new to objects.selection or objects.context!" );

	return Scene_object_new( self->bpyscene->scene, args );
}

static PyObject *SceneObSeq_unlink( BPySceneObSeqObject * self, PyObject *args )
{
	PyObject *pyobj;
	Object *blen_ob;
	Scene *scene;
	Base *base= NULL;
	
	SCENE_DEL_CHECK_PY(self->bpyscene);
	
	if (self->mode != EXPP_OBSEQ_NORMAL)
		return (EXPP_ReturnPyObjError( PyExc_TypeError,
					      "Cannot add new to objects.selection or objects.context!" ));	
	
	if( !PyArg_ParseTuple( args, "O!", &BPyObject_Type, &pyobj ) )
		return ( EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a python object as an argument" ) );
	
	blen_ob = ( ( BPyObject * ) pyobj )->object;
	
	scene = self->bpyscene->scene;
	
	/* is the object really in the scene? */
	base = object_in_scene( blen_ob, scene);
	if( base ) { /* if it is, remove it */
		if (scene->basact==base)
			scene->basact= NULL;	/* in case the object was selected */
		free_and_unlink_base_from_scene(scene, base);
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

PyObject *SceneObSeq_getActive(BPySceneObSeqObject *self)
{
	Base *base;	
	SCENE_DEL_CHECK_PY(self->bpyscene);
	
	if (self->mode!=EXPP_OBSEQ_NORMAL)
			return (EXPP_ReturnPyObjError( PyExc_TypeError,
						"cannot get active from objects.selected or objects.context" ));
	
	base= self->bpyscene->scene->basact;
	if (!base)
		Py_RETURN_NONE;
	
	return Object_CreatePyObject( base->object );
}

static int SceneObSeq_setActive(BPySceneObSeqObject *self, PyObject *value)
{
	Base *base;
	
	SCENE_DEL_CHECK_INT(self->bpyscene);
	
	if (self->mode!=EXPP_OBSEQ_NORMAL)
			return (EXPP_ReturnIntError( PyExc_TypeError,
						"cannot set active from objects.selected or objects.context" ));
	
	if (value==Py_None) {
		self->bpyscene->scene->basact= NULL;
		return 0;
	}
	
	if (!BPyObject_Check(value))
		return (EXPP_ReturnIntError( PyExc_ValueError,
					      "Object or None types can only be assigned to active!" ));
	
	base = object_in_scene( ((BPyObject *)value)->object, self->bpyscene->scene );
	
	if (!base)
		return (EXPP_ReturnIntError( PyExc_ValueError,
					"cannot assign an active object outside the scene." ));
	
	self->bpyscene->scene->basact= base;
	return 0;
}

PyObject *SceneObSeq_getCamera(BPySceneObSeqObject *self)
{
	SCENE_DEL_CHECK_PY(self->bpyscene);
	
	if (self->mode!=EXPP_OBSEQ_NORMAL)
			return (EXPP_ReturnPyObjError( PyExc_TypeError,
						"cannot get camera from objects.selected or objects.context" ));
	
	return Object_CreatePyObject( self->bpyscene->scene->camera );
}

static int SceneObSeq_setCamera(BPySceneObSeqObject *self, PyObject *value)
{
	int ret;

	SCENE_DEL_CHECK_INT(self->bpyscene);
	if (self->mode!=EXPP_OBSEQ_NORMAL)
			return EXPP_ReturnIntError( PyExc_TypeError,
					"cannot set camera from objects.selected or objects.context" );
	
	ret = GenericLib_assignData(value, (void **) &self->bpyscene->scene->camera, 0, 0, ID_OB, 0);
	
	/* if this is the current scene, update its window now */
	if( ret == 0 && !G.background && self->bpyscene->scene == G.scene ) /* Traced a crash to redrawing while in background mode -Campbell */
		copy_view3d_lock( REDRAW );

/* XXX copy_view3d_lock(REDRAW) prints "bad call to addqueue: 0 (18, 1)".
 * The same happens in bpython. */

	return ret;
}


static struct PyMethodDef BPySceneObSeq_methods[] = {
	{"link", (PyCFunction)SceneObSeq_link, METH_VARARGS,
		"link object to this scene"},
	{"new", (PyCFunction)SceneObSeq_new, METH_VARARGS,
		"Create a new object in this scene from the obdata given and return a new object"},
	{"unlink", (PyCFunction)SceneObSeq_unlink, METH_VARARGS,
		"unlinks the object from the scene"},
	{NULL, NULL, 0, NULL}
};

/************************************************************************
 *
 * Python SceneObSeq_Type standard operations
 *
 ************************************************************************/

static void SceneObSeq_dealloc( BPySceneObSeqObject * self )
{
	Py_DECREF(self->bpyscene);
	PyObject_DEL( self );
}

static int SceneObSeq_compare( BPySceneObSeqObject * a, BPySceneObSeqObject * b )
{
	return ( a->bpyscene->scene == b->bpyscene->scene && a->mode == b->mode) ? 0 : -1;	
}

/*
 * repr function
 * callback functions building meaninful string to representations
 */
static PyObject *SceneObSeq_repr( BPySceneObSeqObject * self )
{
	if( !(self->bpyscene->scene) )
		return PyString_FromFormat( "[Scene ObjectSeq Removed]" );
	else if (self->mode==EXPP_OBSEQ_SELECTED)
		return PyString_FromFormat( "[Scene ObjectSeq Selected \"%s\"]",
						self->bpyscene->scene->id.name + 2 );
	else if (self->mode==EXPP_OBSEQ_CONTEXT)
		return PyString_FromFormat( "[Scene ObjectSeq Context \"%s\"]",
						self->bpyscene->scene->id.name + 2 );
	
	/*self->mode==0*/
	return PyString_FromFormat( "[Scene ObjectSeq \"%s\"]",
					self->bpyscene->scene->id.name + 2 );
}

static PyGetSetDef SceneObSeq_getset[] = {
	{"selected",
	 (getter)SceneObSeq_getObjects, (setter)SceneObSeq_setObjects,
	 "sequence of selected objects",
	 (void *)EXPP_OBSEQ_SELECTED},
	{"context",
	 (getter)SceneObSeq_getObjects, (setter)SceneObSeq_setObjects,
	 "sequence of user context objects",
	 (void *)EXPP_OBSEQ_CONTEXT},
	{"active",
	 (getter)SceneObSeq_getActive, (setter)SceneObSeq_setActive,
	 "active object",
	 NULL},
	{"camera",
	 (getter)SceneObSeq_getCamera, (setter)SceneObSeq_setCamera,
	 "camera object",
	 NULL},
	{NULL}  /* Sentinel */
};

/*****************************************************************************/
/* Python SceneObSeq_Type structure definition:                               */
/*****************************************************************************/
PyTypeObject SceneObSeq_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender SceneObSeq",           /* char *tp_name; */
	sizeof( BPySceneObSeqObject ),       /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) SceneObSeq_dealloc,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) SceneObSeq_compare, /* cmpfunc tp_compare; */
	( reprfunc ) SceneObSeq_repr,   /* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	&SceneObSeq_as_sequence,	/* PySequenceMethods *tp_as_sequence; */
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
	( getiterfunc) SceneObSeq_getIter, /* getiterfunc tp_iter; */
	( iternextfunc ) SceneObSeq_nextIter, /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	BPySceneObSeq_methods,       /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	SceneObSeq_getset,       /* struct PyGetSetDef *tp_getset; */
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

PyObject *SceneObSeqType_Init( void )
{
	PyType_Ready( &SceneObSeq_Type );
	return (PyObject *) &SceneObSeq_Type;
}

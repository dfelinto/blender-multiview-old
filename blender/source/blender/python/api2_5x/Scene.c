/* 
 *
 * $Id: Scene.c 12802 2007-12-06 00:01:46Z campbellbarton $
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
struct View3D;

#include "Scene.h" /*This must come first */
#include "../BPY_extern.h"
#include "BKE_global.h"
#include "BKE_main.h"
#include "MEM_guardedalloc.h"	/* for MEM_callocN */
#include "DNA_screen_types.h"
#include "DNA_userdef_types.h" /* U.userdefs */
#include "DNA_object_types.h" /* SceneObSeq_new */
#include "BKE_depsgraph.h"
#include "BKE_library.h"
#include "BKE_object.h"
#include "BKE_scene.h"
#include "BKE_font.h"
#include "BKE_idprop.h"
#include "BSE_headerbuttons.h"	/* for copy_scene */
#include "BIF_drawscene.h"	/* for set_scene */
#include "BIF_screen.h"		/* curarea */
#include "DNA_view3d_types.h"
/* python types */
#include "Object.h"
#include "Camera.h"
/* only for SceneObSeq_new */
#include "BKE_material.h"
#include "BLI_arithb.h"

#include "gen_utils.h"
#include "gen_library.h"
#include "sceneRender.h"
#include "sceneRadio.h"
#include "sceneObSeq.h"
#include "sceneTimeLine.h"
#include "sceneSequence.h"
#include "World.h"


#include "BKE_utildefines.h" /* vec copy */
#include "vector.h"
#include "bpy_list.h"

/*-----------------------BPySceneObject  method declarations--------------------*/
static PyObject *Scene_copy( BPySceneObject * self, PyObject * arg );
static PyObject *Scene_update( BPySceneObject * self, PyObject * args );
static PyObject *Scene_getRadiosityContext( BPySceneObject * self );
static PyObject *Scene_getSequence( BPySceneObject * self );

/*-----------------------BPySceneObject method def------------------------------*/
static PyMethodDef BPyScene_methods[] = {
	/* name, method, flags, doc */
	{"copy", ( PyCFunction ) Scene_copy, METH_VARARGS,
	 "(duplicate_objects = 1) - Return a copy of this scene\n"
	 "The optional argument duplicate_objects defines how the scene\n"
	 "children are duplicated:\n\t0: Link Objects\n\t1: Link Object Data"
	 "\n\t2: Full copy\n"},
	{"update", ( PyCFunction ) Scene_update, METH_VARARGS,
	 "(full = 0) - Update scene self.\n"
	 "full = 0: sort the base list of objects."
	 "full = 1: full update -- also regroups, does ipos, keys"},
	{NULL, NULL, 0, NULL}
};


/*****************************************************************************/
/* Python BPySceneObject getset funcs:                                            */
/*****************************************************************************/
static PyObject *Scene_getLayerMask( BPySceneObject * self )
{
	SCENE_DEL_CHECK_PY(self);
	return PyInt_FromLong( self->scene->lay & ((1<<20)-1) );
}

static int Scene_setLayerMask( BPySceneObject * self, PyObject * value )
{
	int laymask = 0;
	
	SCENE_DEL_CHECK_INT(self);
	
	if (!PyInt_Check(value)) {
		return EXPP_ReturnIntError( PyExc_AttributeError,
			"expected an integer (bitmask) as argument" );
	}
	
	laymask = PyInt_AsLong(value);

	if (laymask <= 0 || laymask > (1<<20) - 1) /* binary: 1111 1111 1111 1111 1111 */
		return EXPP_ReturnIntError( PyExc_AttributeError,
			"bitmask must have from 1 up to 20 bits set");

	self->scene->lay = laymask;
	/* if this is the current scene then apply the scene layers value
	 * to the view layers value: */
	if (G.vd && (self->scene == G.scene)) {
		int val, bit = 0;
		G.vd->lay = laymask;

		while( bit < 20 ) {
			val = 1 << bit;
			if( laymask & val ) {
				G.vd->layact = val;
				break;
			}
			bit++;
		}
	}

	return 0;
}

static PyObject *Scene_getWorld( BPySceneObject * self )
{
	SCENE_DEL_CHECK_PY(self);
	return World_CreatePyObject(self->scene->world);
}

static int Scene_setWorld( BPySceneObject * self, PyObject * value )
{
	SCENE_DEL_CHECK_INT(self);
	return GenericLib_assignData(value, (void **) &self->scene->world, NULL, 1, ID_WO, 0);
}

/* accessed from scn.objects */
static PyObject *Scene_getObjects( BPySceneObject *self) 
{
	SCENE_DEL_CHECK_PY(self);
	return SceneObSeq_CreatePyObject(self, 0);
}

static PyObject *Scene_getCursor( BPySceneObject * self )
{
	SCENE_DEL_CHECK_PY(self);
	return Vector_CreatePyObject( self->scene->cursor, 3, (PyObject *)self );
}

static int Scene_setCursor( BPySceneObject * self, PyObject * value )
{
	SCENE_DEL_CHECK_INT(self);
	return EXPP_setVec3(value, &self->scene->cursor[0], &self->scene->cursor[1], &self->scene->cursor[2]);
}

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef BPyScene_getset[] = {
	GENERIC_LIB_GETSETATTR_SCRIPTLINK,
	GENERIC_LIB_GETSETATTR_LAYER,
	{"layerMask",
	 (getter)Scene_getLayerMask, (setter)Scene_setLayerMask,
	 "Scene layer bitmask",
	 NULL},
	{"world",
	 (getter)Scene_getWorld, (setter)Scene_setWorld,
	 "Scene layer bitmask",
	 NULL},
	{"cursor",
	 (getter)Scene_getCursor, (setter)Scene_setCursor,
	 "Scene layer bitmask",
	 NULL},
	{"timeline",
	 (getter)TimeLine_CreatePyObject, (setter)NULL,
	 "Scenes timeline (read only)",
	 NULL},
	{"render",
	 (getter)RenderData_CreatePyObject, (setter)NULL,
	 "Scenes rendering context (read only)",
	 NULL},
	{"radiosity",
	 (getter)Scene_getRadiosityContext, (setter)NULL,
	 "Scenes radiosity context (read only)",
	 NULL},
	{"sequence",
	 (getter)Scene_getSequence, (setter)NULL,
	 "Scene sequencer data (read only)",
	 NULL},
	 
	{"objects",
	 (getter)Scene_getObjects, (setter)NULL,
	 "Scene object iterator",
	 NULL},
	 /* TODO - edge/face/vert editmode settings - are stored in Scene */
	{NULL}  /* Sentinel */
};


/*
 * Scene dealloc - free from memory and free from text pool
 */
static void Scene_dealloc( BPySceneObject * self )
{
	ID *id = (ID *)(self->scene);
	
	if (id)
		BPY_idhash_remove(id);
	
	PyObject_DEL( self );
}


/* this types constructor */
static PyObject *Scene_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	char *name="Scene";
	ID *id;
	
	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"(name) - name must be a string argument" );
	
	id = (ID *)add_scene( name );
	
	if (!id)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, could not create new scene data" );

	/* user is ok left at 1 */
	return Scene_CreatePyObject((Scene *)id);
}

/*-----------------------BPySceneObject method def------------------------------*/
PyTypeObject BPyScene_Type = {
	PyObject_HEAD_INIT( NULL ) 
	0,	/* ob_size */
	"Scene",		/* tp_name */
	sizeof( BPySceneObject ),	/* tp_basicsize */
	0,			/* tp_itemsize */
	/* methods */
	( destructor ) Scene_dealloc,	/* tp_dealloc */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,						/* tp_compare */
	NULL,						/* tp_repr */

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
	BPyScene_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyScene_getset,         /* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,                       /* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc ) Scene_new,		/* newfunc tp_new; */
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


/*-----------------------CreatePyObject---------------------------------*/
PyObject *Scene_CreatePyObject( Scene * scene )
{
	BPySceneObject *pyscene;

	if (!scene)
		Py_RETURN_NONE;
	
	/* REUSE EXISTING DATA FROM HASH */
	pyscene = (BPySceneObject *)BPY_idhash_get((ID *)scene);
	if (pyscene)
		return EXPP_incr_ret((PyObject *)pyscene);
	
	pyscene = ( BPySceneObject * ) PyObject_NEW( BPySceneObject, &BPyScene_Type );

	if( !pyscene )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create BPySceneObject object" );

	pyscene->scene = scene;

	/* Setup sub data */
	pyscene->cursor = NULL;
	
	BPY_idhash_add((void *)pyscene);
	return ( PyObject * ) pyscene;
}

/*-----------------------Scene module function defintions---------------*/

/*-----------------------Scene.copy()------------------------------------*/
static PyObject *Scene_copy( BPySceneObject * self, PyObject * args )
{
	short dup_objs = 1;
	Scene *scene = self->scene;

	SCENE_DEL_CHECK_PY(self);

	if( !PyArg_ParseTuple( args, "|h", &dup_objs ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected int in [0,2] or nothing as argument" );

	return Scene_CreatePyObject( copy_scene( scene, dup_objs ) );
}

/*-----------------------Scene.update()----------------------------------*/
static PyObject *Scene_update( BPySceneObject * self, PyObject * args )
{
	Scene *scene = self->scene;
	int full = 0;
	
	SCENE_DEL_CHECK_PY(self);
	if( !PyArg_ParseTuple( args, "|i", &full ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected nothing or int (0 or 1) argument" );

/* Under certain circunstances, DAG_scene_sort *here* can crash Blender.
 * A "RuntimeError: max recursion limit" happens when a scriptlink
 * on frame change has scene.update(1).
 * Investigate better how to avoid this. */
	if( !full )
		DAG_scene_sort( scene );

	else if( full == 1 ) {
		int enablescripts = G.f & G_DOSCRIPTLINKS;
		
		/*Disable scriptlinks to prevent firing off newframe scriptlink
		  events.*/
		G.f &= ~G_DOSCRIPTLINKS;
		set_scene_bg( scene );
		scene_update_for_newframe( scene, scene->lay );
		
		/*re-enabled scriptlinks if necassary.*/
		if (enablescripts) G.f |= G_DOSCRIPTLINKS;
	} else
		return EXPP_ReturnPyObjError( PyExc_ValueError,
					      "in method scene.update(full), full should be:\n"
					      "0: to only sort scene elements (old behavior); or\n"
					      "1: for a full update (regroups, does ipos, keys, etc.)" );

	Py_RETURN_NONE;
}

static PyObject *Scene_getRadiosityContext( BPySceneObject * self )
{
	SCENE_DEL_CHECK_PY(self);
	return Radio_CreatePyObject( self->scene );
}

static PyObject *Scene_getSequence( BPySceneObject * self )
{
	SCENE_DEL_CHECK_PY(self);
	if (!self->scene->ed) { /* we should create this if its not there :/ */
		self->scene->ed = MEM_callocN( sizeof(Editing), "addseq");
		((Editing *)self->scene->ed)->seqbasep= &((Editing *)self->scene->ed)->seqbase;
	}
	return SceneSeq_CreatePyObject( self->scene, NULL );
}

PyObject *SceneType_Init( void )
{
	PyType_Ready( &BPyScene_Type );
	return (PyObject *) &BPyScene_Type;
}

// TODO - move constant
	//PyDict_SetItemString( dict, "Radio", Radio_Init(  ) );

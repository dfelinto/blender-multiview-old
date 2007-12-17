/*
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
 * Contributor(s): Campbell Barton
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "Group.h" /* This must come first */

#include "MEM_guardedalloc.h"

#include "DNA_group_types.h"
#include "DNA_scene_types.h" /* for Base */

#include "BKE_mesh.h"
#include "BKE_library.h"
#include "BKE_global.h"
#include "BKE_main.h"
#include "BKE_scene.h"
#include "BKE_group.h"

#include "BLI_blenlib.h"

#include "blendef.h"
#include "Object.h"
#include "gen_utils.h"
#include "gen_library.h"

/* checks for the group being removed */
#define GROUP_DEL_CHECK_PY(bpy_group) if (!(bpy_group->group)) return ( EXPP_ReturnPyObjError( PyExc_RuntimeError, "Group has been removed" ) )
#define GROUP_DEL_CHECK_INT(bpy_group) if (!(bpy_group->group)) return ( EXPP_ReturnIntError( PyExc_RuntimeError, "Group has been removed" ) )

static char BPyGroup_Type_doc[] ="\
class Group:\n\
	'''\n\
	The Group object\n\
	================\n\
		This object gives access to Groups in Blender.\n\
		\n\
		Example::\n\
		\n\
			# Make Dupli's Real, as a python script.\n\
		\n\
			from Blender import *\n\
			import bpy\n\
		\n\
			sce= bpy.data.scenes.active\n\
			for ob in sce.objects:\n\
				print 'Object Group Settings'\n\
				print ob.name, ob.type\n\
				print 'enableDupVerts:', ob.enableDupVerts\n\
				print 'enableDupFrames:', ob.enableDupFrames\n\
				print 'enableDupGroup:', ob.enableDupGroup\n\
				print 'DupGroup:', ob.DupGroup\n\
				dupe_obs= ob.DupObjects\n\
				print 'num dup obs:', len(dupe_obs)\n\
		\n\
				for dup_ob, dup_matrix in dupe_obs:\n\
					print '\tDupOb', dup_ob.name\n\
					scn.objects.new(dup_ob.data)\n\
					new_ob.setMatrix(dup_matrix)\n\
					new_ob.sel= 1 # select all real instances.\n\
		\n\
				ob.sel=0 # Desel the original object\n\
		\n\
			Window.RedrawAll()\n\
		\n\
		Example::\n\
		\n\
			# Make a new group with the selected objects, and add an instance of this group.\n\
		\n\
			from Blender import *\n\
			import bpy\n\
			\n\
			sce= bpy.data.scenes.active\n\
			\n\
			# New Group\n\
			grp= bpy.data.groups.new('mygroup')\n\
			grp.objects= list(scn.objects)\n\
			\n\
			# Instance the group at an empty using dupligroups\n\
			ob= sce.objects.new(None)\n\
			ob.enableDupGroup= True\n\
			ob.DupGroup= grp\n\
			Window.RedrawAll()\n\
		\n\
		Example::\n\
		\n\
			# Remove all non mesh objects from a group.\n\
		\n\
			from Blender import *\n\
			import bpy\n\
			\n\
			sce= bpy.data.scenes.active\n\
			\n\
			# New Group\n\
			grp= bpy.data.groups.get('mygroup')\n\
			for ob in list(grp.objects): # Convert to a list before looping because we are removing items\n\
				if ob.type != 'Mesh':\n\
					grp.objects.unlink(ob)\n\
";

static char Group_copy_doc[] ="\
():\n\
	'''\n\
	Make a copy of this group\n\
	@rtype: Group\n\
	@return:  a copy of this group\n\
	'''\n\
";
	
static char BPyGroupObSeq_Type_doc[] ="\
class GroupObSeq:\n\
	'''\n\
	The Group Object Sequence\n\
	=========================\n\
		This object gives access to Groups in Blender.\n\
";

static char GroupObSeq_link_doc[] ="\
(object):\n\
	'''\n\
	Add an object to this group\n\
	@type object: Blender Object\n\
	@param object: Object to be added to the group.\n\
	'''\n\
";

static char GroupObSeq_unlink_doc[] ="\
(object):\n\
	'''\n\
	Remove the an object from this group\n\
	@type object: Blender Object\n\
	@param object: Object to be removed from the group.\n\
	'''\n\
";
	
	
/*****************************************************************************/
/* Python API function prototypes for the Blender module.		 */
/*****************************************************************************/

/* internal */
static PyObject *Group_getObjects( BPyGroupObject *self );

/*****************************************************************************/
/* Python BPyGroupObject methods table:					   */
/*****************************************************************************/
static PyObject *BPyGroup_copy( BPyGroupObject * self );

static PyMethodDef Group_methods[] = {
	/* name, method, flags, doc */
	{"__copy__", ( PyCFunction ) BPyGroup_copy, METH_VARARGS, Group_copy_doc},
	{"copy", ( PyCFunction ) BPyGroup_copy, METH_VARARGS, Group_copy_doc},
	{NULL, NULL, 0, NULL}
};


static PyObject *BPyGroup_copy( BPyGroupObject * self )
{
	BPyGroupObject *py_group;	/* for Group Data object wrapper in Python */
	struct Group *bl_group;
	GroupObject *group_ob, *group_ob_new; /* Group object, copied and added to the groups */
	
	GROUP_DEL_CHECK_PY(self);
	
	bl_group= add_group( self->group->id.name + 2 );
	
	if (!bl_group)
		return ( EXPP_ReturnPyObjError( PyExc_RuntimeError,
					"couldn't create Group Data in Blender" ) );
	bl_group->id.us = 1;
	
	py_group = ( BPyGroupObject * ) Group_CreatePyObject( bl_group );
	if (!py_group)
		return ( EXPP_ReturnPyObjError( PyExc_RuntimeError,
					"couldn't create Group Python Data" ) );
	
	/* Now add the objects to the group */
	group_ob= self->group->gobject.first;
	while(group_ob) {
		/* save time by not using */
		group_ob_new= MEM_callocN(sizeof(GroupObject), "groupobject");
		group_ob_new->ob= group_ob->ob;
		BLI_addtail( &bl_group->gobject, group_ob_new);
		group_ob= group_ob->next;
	}
	
	return ( PyObject * ) py_group;
	
}


/************************************************************************
 *
 * Python BPyObject attributes
 *
 ************************************************************************/

static void add_to_group_wraper(Group *group, Object *ob) {
	Base *base;
	add_to_group(group, ob);
	
	if (!(ob->flag & OB_FROMGROUP)) { /* do this to avoid a listbase lookup */
		ob->flag |= OB_FROMGROUP;
		
		base= object_in_scene(ob, G.scene);
		if (base)
			base->flag |= OB_FROMGROUP;
	}
}

/* could also be called GroupObSeq_CreatePyObject*/
static PyObject *Group_getObjects( BPyGroupObject *self )
{
	BPyGroupObSeqObject *seq = PyObject_NEW( BPyGroupObSeqObject, &BPyGroupObSeq_Type);
	seq->bpygroup = self; Py_INCREF(self);
	seq->index= 0;
	return (PyObject *)seq;
}

/* only for internal use Blender.Group.Get("MyGroup").objects= []*/
static int Group_setObjects( BPyGroupObject * self, PyObject * args )
{
	int i, list_size;
	Group *group;
	Object *blen_ob;
	group= self->group;
	
	GROUP_DEL_CHECK_INT(self);
	
	if( PyList_Check( args ) ) {
		if( EXPP_check_sequence_consistency( args, &BPyObject_Type ) != 1)
			return ( EXPP_ReturnIntError( PyExc_TypeError, 
					"expected a list of objects" ) );
		
		/* remove all from the list and add the new items */
		free_group(group); /* unlink all objects from this group, keep the group */
		list_size= PyList_Size( args );
		for( i = 0; i < list_size; i++ ) {
			blen_ob= ((BPyObject *)PyList_GET_ITEM( args, i ))->object;
			add_to_group_wraper(group, blen_ob);
		}
	} else if (PyIter_Check(args)) {
		PyObject *iterator = PyObject_GetIter(args);
		PyObject *item;
		if (iterator == NULL) {
			Py_DECREF(iterator);
			return EXPP_ReturnIntError( PyExc_TypeError, 
			"expected a list of objects, This iterator cannot be used." );
		}
		free_group(group); /* unlink all objects from this group, keep the group */
		item = PyIter_Next(iterator);
		while (item) {
			if ( BPyObject_Check(item) ) {
				blen_ob= ((BPyObject *)item)->object;
				add_to_group_wraper(group, blen_ob);
			}
			Py_DECREF(item);
			item = PyIter_Next(iterator);
		}

		Py_DECREF(iterator);

		if (PyErr_Occurred()) {
			return EXPP_ReturnIntError( PyExc_RuntimeError, 
			"An unknown error occured while adding iterator objects to the group.\nThe group has been modified." );
		}

	} else
		return EXPP_ReturnIntError( PyExc_TypeError, 
				"expected a list or sequence of objects" );
	return 0;
}

/*****************************************************************************/
/* Python BPyGroupObject getset funcs:                                        */
/*****************************************************************************/
static int Group_setLayerMask( BPyGroupObject * self, PyObject * value )
{
	unsigned int laymask = 0;
	
	GROUP_DEL_CHECK_INT(self);
	
	if( !PyInt_Check( value ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
			"expected an integer (bitmask) as argument" );
	
	laymask = ( unsigned int )PyInt_AS_LONG( value );
	
	if( laymask <= 0 )
		return EXPP_ReturnIntError( PyExc_ValueError,
					      "layer value cannot be zero or below" );
	
	self->group->layer= laymask & ((1<<20) - 1);
	
	return 0;
}

static PyObject *Group_getLayerMask( BPyGroupObject * self )
{
	return PyInt_FromLong( self->group->layer );
}

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/

static PyGetSetDef Group_getset[] = {
	GENERIC_LIB_GETSETATTR_LAYER,
	{"layerMask",
	 (getter)Group_getLayerMask, (setter)Group_setLayerMask, "\
@ivar layerMask: Layer set for this group. all modifications to this set are copied back to \n\
@type layerMask: set subclass",
	 NULL},
	{"objects",
	 (getter)Group_getObjects, (setter)Group_setObjects, "\
@ivar objects: object seq TODO - link\n\
@type objects: GroupObSeq",
	 NULL},
	{NULL}  /* Sentinel */
};

/* this types constructor */
static PyObject *Group_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	char *name="Group";
	ID *id;
	
	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"(name) - name must be a string argument" );
	
	id = (ID *)add_group( name );
	
	if (!id)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, could not create new group data" );
	
	id->us = 1;
	return Group_CreatePyObject((Group *)id);
}

/*****************************************************************************/
/* Python TypeGroup structure definition:                                     */
/*****************************************************************************/
PyTypeObject BPyGroup_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Group",             /* char *tp_name; */
	sizeof( BPyGroupObject ),         /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,                       /* tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,						/* cmpfunc tp_compare; */
	NULL,						/* reprfunc tp_repr; */

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

	BPyGroup_Type_doc,                       /*  char *tp_doc;  Documentation string */
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
	Group_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	Group_getset,         /* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,		/* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc ) Group_new,		/* newfunc tp_new; */
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

/*****************************************************************************/
/* Function:	Group_CreatePyObject					 */
/* Description: This function will create a new BlenObject from an existing  */
/*		Object structure.					 */
/*****************************************************************************/
PyObject *Group_CreatePyObject( struct Group * grp )
{
	BPyGroupObject *pygrp;

	if( !grp )
		Py_RETURN_NONE;

	pygrp = ( BPyGroupObject * ) PyObject_NEW( BPyGroupObject, &BPyGroup_Type );

	if( pygrp == NULL ) {
		return ( NULL );
	}
	pygrp->group = grp;
	return ( ( PyObject * ) pygrp );
}

/************************************************************************
 *
 * GroupOb sequence 
 *
 ************************************************************************/
/*
 * create a thin GroupOb object
 */

/* internal use only */
static  GroupObject *GroupObSeq_get_pointer( BPyGroupObSeqObject* self )
{
	GroupObject *group_ob;
	int i;
	
	if (!self->bpygroup->group)
		return NULL;
	
	group_ob = self->bpygroup->group->gobject.first;
	for (i=0; (group_ob && i<self->index); i++, group_ob= group_ob->next) {}
	return group_ob; /* can be NULL */
}

static int GroupObSeq_len( BPyGroupObSeqObject * self )
{
	GROUP_DEL_CHECK_INT(self->bpygroup);
	return BLI_countlist( &( self->bpygroup->group->gobject ) );
}

static PySequenceMethods GroupObSeq_as_sequence = {
	( inquiry ) GroupObSeq_len,	/* sq_length */
	( binaryfunc ) 0,	/* sq_concat */
	( intargfunc ) 0,	/* sq_repeat */
	( intargfunc ) 0,	/* sq_item */
	( intintargfunc ) 0,	/* sq_slice */
	( intobjargproc ) 0,	/* sq_ass_item */
	( intintobjargproc ) 0,	/* sq_ass_slice */
	0,0,0,
};

/************************************************************************
 *
 * Python BPyGroupObSeq_Type iterator (iterates over GroupObjects)
 *
 ************************************************************************/

/*
 * Initialize the interator index
 */

static PyObject *GroupObSeq_getIter( BPyGroupObSeqObject * self )
{
	GROUP_DEL_CHECK_PY(self->bpygroup);
	
	if (self->index == 0) {
		return EXPP_incr_ret ( (PyObject *) self );
	} else { /* were alredy looping */
		return Group_getObjects(self->bpygroup);
	}
}

/*
 * Return next GroupOb.
 */

static PyObject *GroupObSeq_nextIter( BPyGroupObSeqObject * self )
{
	GroupObject *group_ob = GroupObSeq_get_pointer(self);

	if( !(group_ob) )
		return EXPP_ReturnPyObjError( PyExc_StopIteration,
				"iterator at end" );

	self->index++;

	return Object_CreatePyObject( group_ob->ob ); 
}


static PyObject *GroupObSeq_link( BPyGroupObSeqObject * self, BPyObject *value )
{	
	GROUP_DEL_CHECK_PY(self->bpygroup);
	
	if( !BPyObject_Check(value) )
		return ( EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a python object as an argument" ) );
	
	/*
	if (self->iter != NULL)
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "Cannot modify group objects while iterating" );
	*/
	
	add_to_group_wraper(self->bpygroup->group, value->object); /* this checks so as not to add the object into the group twice*/
	
	Py_RETURN_NONE;
}

static PyObject *GroupObSeq_unlink( BPyGroupObSeqObject * self, BPyObject *value )
{	
	GROUP_DEL_CHECK_PY(self->bpygroup);
	
	if( !BPyObject_Check(value) )
		return ( EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a python object as an argument" ) );

	rem_from_group(self->bpygroup->group, value->object);
	
	if(find_group(value->object)==NULL) {
		Base *base;
		
		value->object->flag &= ~OB_FROMGROUP;
		
		base = object_in_scene(value->object, G.scene); 
		if (base)
			base->flag &= ~OB_FROMGROUP;
	}
	Py_RETURN_NONE;
}

static struct PyMethodDef BPyGroupObSeq_methods[] = {
	{"link", (PyCFunction)GroupObSeq_link, METH_O, GroupObSeq_link_doc},
	{"unlink", (PyCFunction)GroupObSeq_unlink, METH_O, GroupObSeq_unlink_doc},
	{NULL, NULL, 0, NULL}	
};

/************************************************************************
 *
 * Python BPyGroupObSeq_Type standard operations
 *
 ************************************************************************/

static void GroupObSeq_dealloc( BPyGroupObSeqObject * self )
{
	Py_DECREF(self->bpygroup);
	PyObject_DEL( self );
}

/*****************************************************************************/
/* Python BPyGroupObSeq_Type structure definition:                               */
/*****************************************************************************/
PyTypeObject BPyGroupObSeq_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender GroupObSeq",           /* char *tp_name; */
	sizeof( BPyGroupObSeqObject ),       /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) GroupObSeq_dealloc,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,                       /* cmpfunc tp_compare; */
	NULL,                       /* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	&GroupObSeq_as_sequence,	    /* PySequenceMethods *tp_as_sequence; */
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

	BPyGroupObSeq_Type_doc,		/*  char *tp_doc;  Documentation string */
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
	( getiterfunc) GroupObSeq_getIter, /* getiterfunc tp_iter; */
	( iternextfunc ) GroupObSeq_nextIter, /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	BPyGroupObSeq_methods,       /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	NULL,                       /* struct PyGetSetDef *tp_getset; */
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

/*****************************************************************************/
/* Function:	 initObject						*/
/*****************************************************************************/
PyObject * GroupType_Init( void )
{
	PyType_Ready( &BPyGroup_Type );
	BPyGroup_Type.tp_dealloc = (destructor)&PyObject_Del;
	return (PyObject *) &BPyGroup_Type;
}

PyObject * GroupSeqType_Init( void )
{
	PyType_Ready( &BPyGroupObSeq_Type );
	return (PyObject *) &BPyGroupObSeq_Type;
}

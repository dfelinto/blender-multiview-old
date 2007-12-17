/* 
 * $Id: Metaball.c 11446 2007-07-31 16:11:32Z campbellbarton $
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
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * This is a new part of Blender.
 *
 * Contributor(s): Jacques Guignot
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "Metaball.h" /*This must come first*/

#include "BKE_main.h"
#include "BKE_utildefines.h"
#include "BKE_global.h"
#include "BKE_mball.h"
#include "BKE_library.h"
#include "BLI_blenlib.h"
#include "BLI_arithb.h" /* for quat normal */
#include "DNA_object_types.h"
#include "Mathutils.h"
#include "Material.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "Const.h"

/* for dealing with materials */
#include "MEM_guardedalloc.h"
#include "BKE_material.h"

#import "bpy_list.h"

/* checks for the metaelement being removed */
#define METAELEM_DEL_CHECK_PY(_ml) if (!_ml) return ( EXPP_ReturnPyObjError( PyExc_RuntimeError, "Metaball has been removed" ) );
#define METAELEM_DEL_CHECK_INT(_ml) if (!_ml) return ( EXPP_ReturnIntError( PyExc_RuntimeError, "Metaball has been removed" ) );

/* internal use only */
static MetaElem *MetaElem_get_pointer( MetaBall *mb, int index )
{
	MetaElem *ml;
	int i;
	
	if (index==-1) /* removed */
		return NULL;
	
	ml = mb->elems.first;
	
	for (i=0; (ml && i<index); i++, ml= ml->next) {}
	return ml; /* can be NULL */
}

/* 
 * structure of "tuples" of constant's string name and int value
 *
 * For example, these two structures will define the constant category
 * "bpy.class.Object.DrawTypes" the constant 
 * "bpy.class.Object.DrawTypes.BOUNDBOX" and others.
 */

static constIdents metaElemTypesIdents[] = {
	{"BALL",   		{(int)MB_BALL}},
	/* - deprecated
	{"TUBEX",		{(int)MB_TUBEX}}, 
	{"TUBEY",		{(int)MB_TUBEY}},
	{"TUBEZ",		{(int)MB_TUBEZ}},*/
	{"TUBE",		{(int)MB_TUBE}},
	{"PLANE",		{(int)MB_PLANE}},
	{"ELIPSOID",	{(int)MB_ELIPSOID}},
	{"CUBE",		{(int)MB_CUBE}},
};

/* 
 * structure with constant internal type (int), constant category name,
 * and number of constant values in the category.
 */

static constDefinition metaElemTypes = {
	EXPP_CONST_INT, "Types",
		sizeof(metaElemTypesIdents)/sizeof(constIdents), metaElemTypesIdents
};

static constIdents updateTypesIdents[] = {
	{"ALWAYS",  	{(int)MB_UPDATE_ALWAYS}},
	{"HALFRES",		{(int)MB_UPDATE_HALFRES}},
	{"FAST",		{(int)MB_UPDATE_FAST}},
	{"NEVER",		{(int)MB_UPDATE_NEVER}},
};

/* 
 * structure with constant internal type (int), constant category name,
 * and number of constant values in the category.
 */

static constDefinition updateTypes = {
	EXPP_CONST_INT, "UpdateTypes",
		sizeof(updateTypesIdents)/sizeof(constIdents), updateTypesIdents
};

/*****************************************************************************/
/* Python BPyMetaballObject methods declarations:                                */
/*****************************************************************************/
static PyObject *Metaball_getElements( BPyMetaballObject * self );
static PyObject *Metaball_getWiresize( BPyMetaballObject * self );
static int Metaball_setWiresize( BPyMetaballObject * self, PyObject * value );
static PyObject *Metaball_getRendersize( BPyMetaballObject * self );
static int Metaball_setRendersize( BPyMetaballObject * self, PyObject * value);
static PyObject *Metaball_getThresh( BPyMetaballObject * self );
static int Metaball_setThresh( BPyMetaballObject * self, PyObject * args );
static PyObject *Metaball_getUpdate( BPyMetaballObject * self );
static int Metaball_setUpdate( BPyMetaballObject * self, PyObject * args );
static PyObject *Metaball_copy( BPyMetaballObject * self );

/*****************************************************************************/
/* Python BPyMetaballObject methods table:                                       */
/*****************************************************************************/
static PyMethodDef BPyMetaball_methods[] = {
	/* name, method, flags, doc */
	{"__copy__", ( PyCFunction ) Metaball_copy,
	 METH_NOARGS, "() - Return a copy of this metaball"},
	{"copy", ( PyCFunction ) Metaball_copy,
	 METH_NOARGS, "() - Return a copy of this metaball"},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python BPyMetaelem_Type callback function prototypes:                        */
/*****************************************************************************/
static void Metaelem_dealloc( BPyMetaelemObject * self );
static PyObject *Metaelem_repr( BPyMetaelemObject * self );
static int Metaelem_compare( BPyMetaelemObject * a, BPyMetaelemObject * b );

static PyObject *Metaelem_getType( BPyMetaelemObject *self );
static int Metaelem_setType( BPyMetaelemObject * self,  PyObject * args );
static PyObject *Metaelem_getCoord( BPyMetaelemObject * self );
static int Metaelem_setCoord( BPyMetaelemObject * self,  PyObject * value );
static PyObject *Metaelem_getDims( BPyMetaelemObject * self );
static int Metaelem_setDims( BPyMetaelemObject * self,  PyObject * value );
static PyObject *Metaelem_getQuat( BPyMetaelemObject * self );
static int Metaelem_setQuat( BPyMetaelemObject * self,  PyObject * value );
static PyObject *Metaelem_getStiffness( BPyMetaelemObject * self );
static int Metaelem_setStiffness( BPyMetaelemObject * self,  PyObject * value );
static PyObject *Metaelem_getRadius( BPyMetaelemObject * self );
static int Metaelem_setRadius( BPyMetaelemObject * self,  PyObject * value );

static PyObject *Metaelem_getMFlagBits( BPyMetaelemObject * self, void * type );
static int Metaelem_setMFlagBits( BPyMetaelemObject * self, PyObject * value, void * type );

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef BPyMetaball_getset[] = {
	GENERIC_LIB_GETSETATTR_MATERIAL,
	{"elements",
	 (getter)Metaball_getElements, (setter)NULL,
	 "Elements in this metaball",
	 NULL},
	{"wiresize",
	 (getter)Metaball_getWiresize, (setter)Metaball_setWiresize,
	 "The density to draw the metaball in the 3D view",
	 NULL},
	{"rendersize",
	 (getter)Metaball_getRendersize, (setter)Metaball_setRendersize,
	 "The density to render wire",
	 NULL},
	{"thresh",
	 (getter)Metaball_getThresh, (setter)Metaball_setThresh,
	 "The density to render wire",
	 NULL},
	{"update",
	 (getter)Metaball_getUpdate, (setter)Metaball_setUpdate,
	 "The setting for updating this metaball data",
	 NULL},
	{NULL}  /* Sentinel */
};

/* this types constructor */
static PyObject *Metaball_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	char *name="MBall";
	ID *id;
	
	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"(name) - name must be a string argument" );
	
	id = (ID *)add_mball( name );
	
	if (!id)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, could not create new lamp data" );
	
	id->us = 0;
	return Metaball_CreatePyObject((MetaBall *)id);
}

/*****************************************************************************/
/* Python TypeMetaball structure definition:                                     */
/*****************************************************************************/
PyTypeObject BPyMetaball_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Metaball",             /* char *tp_name; */
	sizeof( BPyMetaballObject ),         /* int tp_basicsize; */
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
	BPyMetaball_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyMetaball_getset,         /* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,		/* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc ) Metaball_new,	/* newfunc tp_new; */
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


static PyGetSetDef BPyMetaelem_getset[] = {
	{"type",
	 (getter)Metaelem_getType, (setter)Metaelem_setType,
	 "Metaelem Type",
	 NULL},
	{"co",
	 (getter)Metaelem_getCoord, (setter)Metaelem_setCoord,
	 "Metaelem Location",
	 NULL},
	{"quat",
	 (getter)Metaelem_getQuat, (setter)Metaelem_setQuat,
	 "Metaelem Rotation Quat",
	 NULL},
	{"dims",
	 (getter)Metaelem_getDims, (setter)Metaelem_setDims,
	 "Metaelem Dimensions",
	 NULL},
	{"stiffness",
	 (getter)Metaelem_getStiffness, (setter)Metaelem_setStiffness,
	 "MetaElem stiffness",
	 NULL},
	{"radius",
	 (getter)Metaelem_getRadius, (setter)Metaelem_setRadius,
	 "The radius of the MetaElem",
	 NULL},
	{"negative",
	 (getter)Metaelem_getMFlagBits, (setter)Metaelem_setMFlagBits,
	 "The density to render wire",
	 (void *)MB_NEGATIVE},
	{"hide",
	 (getter)Metaelem_getMFlagBits, (setter)Metaelem_setMFlagBits,
	 "The density to render wire",
	 (void *)MB_HIDE},
	{NULL}  /* Sentinel */
};


/*****************************************************************************/
/* Python TypeMetaelem structure definition:                                     */
/*****************************************************************************/
PyTypeObject BPyMetaelem_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Metaelem",             /* char *tp_name; */
	sizeof( BPyMetaelemObject ),         /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) Metaelem_dealloc,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) Metaelem_compare,   /* cmpfunc tp_compare; */
	( reprfunc ) Metaelem_repr,     /* reprfunc tp_repr; */

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
	NULL,		/* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyMetaelem_getset,        /* struct PyGetSetDef *tp_getset; */
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


static PyObject *Metaball_getWiresize( BPyMetaballObject * self )
{
	return PyFloat_FromDouble( self->metaball->wiresize );
}

static int Metaball_setWiresize( BPyMetaballObject * self, PyObject * value )
{
	float param;
	if( !PyNumber_Check( value ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
					"metaball.wiresize - expected float argument" );

	param = (float)PyFloat_AsDouble( value );

	self->metaball->wiresize = EXPP_ClampFloat(param, 0.05f, 1.0);
	return 0;

}
static PyObject *Metaball_getRendersize( BPyMetaballObject * self )
{
	return PyFloat_FromDouble( self->metaball->rendersize );
}

static int Metaball_setRendersize( BPyMetaballObject * self, PyObject * value )
{

	float param;
	if( !PyNumber_Check( value ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
					"metaball.rendersize - expected float argument" );

	param = (float)PyFloat_AsDouble( value );

	self->metaball->rendersize = EXPP_ClampFloat(param, 0.05f, 1.0);
	return 0;
}

static PyObject *Metaball_getThresh( BPyMetaballObject * self )
{
	return PyFloat_FromDouble( self->metaball->thresh );
}

static int Metaball_setThresh( BPyMetaballObject * self, PyObject * value )
{

	float param;
	if( !PyNumber_Check( value ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
					"metaball.thresh - expected float argument" );

	param = (float)PyFloat_AsDouble( value );

	self->metaball->thresh = EXPP_ClampFloat(param, 0.0, 5.0);
	return 0;
}

static PyObject *Metaball_getUpdate( BPyMetaballObject * self )
{
	return PyConst_NewInt( &updateTypes, self->metaball->flag );
}

static int Metaball_setUpdate( BPyMetaballObject * self, PyObject * value )
{
	constValue *c = Const_FromPyObject( &updateTypes, value);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected HandleType constant or string" );
	
	self->metaball->flag = c->i;
	return 0;
}

static PyObject *Metaball_copy( BPyMetaballObject * self )
{
	MetaBall *blmball;	/* for actual Data we create in Blender */
	
	blmball = copy_mball( self->metaball );	/* first create the MetaBall Data in Blender */

	if (!blmball)
		return ( EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"metaball.__copy__() - couldn't create data in Blender" ) );
	
	/* return user count to zero since add_mball() incref'ed it */
	blmball->id.us = 0;
	return Metaball_CreatePyObject(blmball);
}


/* These are needed by Object.c */
PyObject *Metaball_CreatePyObject( MetaBall * mball)
{
	BPyMetaballObject *py_mball= PyObject_NEW( BPyMetaballObject, &BPyMetaball_Type );

	if( !py_mball )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
				"couldn't create BPyMetaballObject object" );

	py_mball->metaball= mball;

	return ( PyObject * ) py_mball;
}


static PyObject *MetaelemSeq_CreatePyObject( BPyMetaballObject *self )
{
	BPyMetaelemSeqObject *seq = PyObject_NEW( BPyMetaelemSeqObject, &BPyMetaelemSeq_Type);
	seq->bpymetaball = self; Py_INCREF(self);
	seq->index= 0;
	return (PyObject *)seq;
}

/*
 * Element, get an instance of the iterator.
 */
static PyObject *Metaball_getElements( BPyMetaballObject * self )
{
	return MetaelemSeq_CreatePyObject(self);
}

/*
 * Metaelem dealloc - free from memory
 */
/* This is a callback function for the BPyMetaelemObject type. It is */
static void Metaelem_dealloc( BPyMetaelemObject * self )
{
	Py_DECREF( self->bpymetaball );
	PyObject_DEL( self );
}

/*
 * elem.type - int to set the shape of the element
 */
static PyObject *Metaelem_getType( BPyMetaelemObject *self )
{	
	MetaElem *ml = MetaElem_get_pointer(self->bpymetaball->metaball, self->index); 
	METAELEM_DEL_CHECK_PY(ml);
	return PyConst_NewInt( &metaElemTypes, ml->type );
}

static int Metaelem_setType( BPyMetaelemObject * self,  PyObject * value )
{
	MetaElem *ml = MetaElem_get_pointer(self->bpymetaball->metaball, self->index);
	constValue *c = Const_FromPyObject( &metaElemTypes, value);
	METAELEM_DEL_CHECK_INT(ml);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected Types constant or string" );
	
	ml->type = c->i;
	return 0;
}

/*
 * elem.co - non wrapped vector representing location
 */
static PyObject *Metaelem_getCoord( BPyMetaelemObject * self )
{
	float co[3];
	MetaElem *ml = MetaElem_get_pointer(self->bpymetaball->metaball, self->index); 
	METAELEM_DEL_CHECK_PY(ml);
	
	co[0]= ml->x;
	co[1]= ml->y;
	co[2]= ml->z;
	
	return Vector_CreatePyObject( co, 3, (PyObject *)NULL );
}
static int Metaelem_setCoord( BPyMetaelemObject * self,  PyObject * value )
{
	MetaElem *ml = MetaElem_get_pointer(self->bpymetaball->metaball, self->index); 
	METAELEM_DEL_CHECK_INT(ml);
	return EXPP_setVec3(value, &ml->x, &ml->y, &ml->z);
}

/*
 * elem.dims - non wrapped vector representing the xyz dimensions
 * only effects some element types
 */
static PyObject *Metaelem_getDims( BPyMetaelemObject * self )
{
	float co[3];
	MetaElem *ml = MetaElem_get_pointer(self->bpymetaball->metaball, self->index);
	METAELEM_DEL_CHECK_PY(ml);

	co[0]= ml->expx;
	co[1]= ml->expy;
	co[2]= ml->expz;
	return Vector_CreatePyObject( co, 3, (PyObject *)NULL );
}
static int Metaelem_setDims( BPyMetaelemObject * self,  PyObject * value )
{
	int ret;
	MetaElem *ml = MetaElem_get_pointer(self->bpymetaball->metaball, self->index); 
	METAELEM_DEL_CHECK_INT(ml);
	
	ret = EXPP_setVec3(value, &ml->expx, &ml->expy, &ml->expz);
	
	if (ret == -1)
		return -1;
	
	CLAMP(ml->expx, 0.0f, 1.0f);
	CLAMP(ml->expy, 0.0f, 1.0f);
	CLAMP(ml->expz, 0.0f, 1.0f);
	return 0;
}

/*
 * elem.quat - non wrapped quat representing the rotation
 * only effects some element types - a rotated ball has no effect for eg.
 */
static PyObject *Metaelem_getQuat( BPyMetaelemObject * self )
{
	MetaElem *ml = MetaElem_get_pointer(self->bpymetaball->metaball, self->index);
	METAELEM_DEL_CHECK_PY(ml);
	return Quat_CreatePyObject(ml->quat, (PyObject *)NULL); 
}
static int Metaelem_setQuat( BPyMetaelemObject * self,  PyObject * value )
{
	int ret;
	MetaElem *ml = MetaElem_get_pointer(self->bpymetaball->metaball, self->index); 
	METAELEM_DEL_CHECK_INT(ml);
	
	ret = EXPP_setVec4(value, &ml->quat[0], &ml->quat[1], &ml->quat[2], &ml->quat[3]);
	
	if (ret == -1)
		return -1;

	/* need to normalize or metaball drawing can go into an infinate loop */
	NormalQuat(ml->quat);
	return 0;
}

/*
 * elem.hide and elem.sel - get/set true false
 */
static PyObject *Metaelem_getMFlagBits( BPyMetaelemObject * self, void * type )
{
	MetaElem *ml = MetaElem_get_pointer(self->bpymetaball->metaball, self->index);
	METAELEM_DEL_CHECK_PY(ml);
	return EXPP_getBitfield( &(ml->flag), (int)((long)type ), 'h' );
}
static int Metaelem_setMFlagBits( BPyMetaelemObject * self, PyObject * value,
		void * type )
{
	MetaElem *ml = MetaElem_get_pointer(self->bpymetaball->metaball, self->index);
	METAELEM_DEL_CHECK_INT(ml);
	return EXPP_setBitfield( value, &(ml->flag), (int)((long)type), 'h' );
}

/*
 * elem.stiffness - floating point, the volume of this element.
 */
static PyObject *Metaelem_getStiffness( BPyMetaelemObject *self )
{
	MetaElem *ml = MetaElem_get_pointer(self->bpymetaball->metaball, self->index);
	METAELEM_DEL_CHECK_PY(ml);
	return PyFloat_FromDouble( ml->s );
}
static int Metaelem_setStiffness( BPyMetaelemObject *self, PyObject *value)
{
	MetaElem *ml = MetaElem_get_pointer(self->bpymetaball->metaball, self->index);
	METAELEM_DEL_CHECK_INT(ml);
	
	if( !PyFloat_Check( value ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
					"metaelem.stiffness - expected float argument" );

	ml->s = EXPP_ClampFloat((float)PyFloat_AsDouble( value ), 0.0, 10.0);
	return 0;
}

/*
 * elem.radius- floating point, the size if the element
 */
static PyObject *Metaelem_getRadius( BPyMetaelemObject *self )
{
	MetaElem *ml = MetaElem_get_pointer(self->bpymetaball->metaball, self->index);
	METAELEM_DEL_CHECK_PY(ml);
	return PyFloat_FromDouble( ml->rad );
}
static int Metaelem_setRadius( BPyMetaelemObject *self, PyObject *value)
{
	MetaElem *ml = MetaElem_get_pointer(self->bpymetaball->metaball, self->index);
	METAELEM_DEL_CHECK_INT(ml);
	if( !PyFloat_Check( value ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
					"metaelem.radius - expected float argument" );

	ml->rad = /* is 5000 too small? */
			EXPP_ClampFloat((float)PyFloat_AsDouble( value ), 0.0, 5000.0);
	
	return 0;
}

static int MetaelemSeq_compare( BPyMetaelemSeqObject * a, BPyMetaelemSeqObject * b )
{
	MetaBall *pa = a->bpymetaball->metaball, *pb = b->bpymetaball->metaball;
	return ( pa == pb ) ? 0 : -1;
}

static int Metaelem_compare( BPyMetaelemObject * a, BPyMetaelemObject * b )
{
	return ( a->index == b->index && a->bpymetaball->metaball == b->bpymetaball->metaball ) ? 0 : -1;
}

/*
 * repr function
 * callback functions building meaninful string to representations
 */

static PyObject *Metaelem_repr( BPyMetaelemObject * self )
{
	if (self->index==-1)
		return PyString_FromString( "[Metaelem <deleted>]");
	
	return PyString_FromFormat( "[Metaelem (%i)]", self->index );
}

static PyObject *MetaelemSeq_repr( BPyMetaelemSeqObject * self )
{
	return PyString_FromFormat( "[MetaElemIter \"%s\"]",
				    self->bpymetaball->metaball->id.name + 2 );
}


/*
 * MeteElem Seq sequence 
 */

static PyObject *MetaElem_CreatePyObject( BPyMetaballObject *bpymetaball, int index )
{
	BPyMetaelemObject *bml= PyObject_NEW( BPyMetaelemObject, &BPyMetaelem_Type);
	bml->index = index;
	bml->bpymetaball= bpymetaball;
	Py_INCREF(bpymetaball);
	return (PyObject *)bml;
}

static int MetaelemSeq_len( BPyMetaelemSeqObject * self )
{
	return BLI_countlist( &( self->bpymetaball->metaball->elems ) );
}


static PySequenceMethods MetaelemSeq_as_sequence = {
	( inquiry ) MetaelemSeq_len,	/* sq_length */
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
 * Python BPyMetaelemSeq_Type iterator (iterates over Metaballs)
 *
 ************************************************************************/

/*
 * Initialize the interator
 */

static PyObject *MetaelemSeq_getIter( BPyMetaelemSeqObject * self )
{
	if (self->index==0) /* not alredy looping on this data, */
		return EXPP_incr_ret ( (PyObject *) self );
	else
		return MetaelemSeq_CreatePyObject(self->bpymetaball);
}

/*
 * Return next MetaElem.
 */

static PyObject *MetaelemSeq_nextIter( BPyMetaelemSeqObject * self )
{
	MetaElem *ml = MetaElem_get_pointer(self->bpymetaball->metaball, self->index);
	
	if( !(ml) ) {
		self->index= -1; /* so we dont use again, invalidate */
		return EXPP_ReturnPyObjError( PyExc_StopIteration,
				"iterator at end" );
	}
	
	self->index++;
	return MetaElem_CreatePyObject( self->bpymetaball, self->index-1 );
}

/*
 * Adds and returns a new metaelement, 
 * no args are taken so the returned metaball must be modified after adding.
 * Accessed as mball.elements.add() where mball is a python metaball data type.
 */
static PyObject *MetaelemSeq_add( BPyMetaelemSeqObject * self, PyObject *value )
{
	MetaElem *ml;
	float x, y, z;
	int ret; 
	
	ret = EXPP_setVec3(value, &x, &y, &z);
	
	if (ret==-1)
		return NULL;
	
	ml = MEM_callocN( sizeof( MetaElem ), "metaelem" );
	BLI_addhead( &( self->bpymetaball->metaball->elems ), ml );
	ml->x= z;
	ml->y= y;
	ml->z= z;
	ml->quat[0]= 1.0;
	ml->quat[1]= 0.0;
	ml->quat[2]= 0.0;
	ml->quat[3]= 0.0;
	ml->rad = 2;
	ml->s = 2.0;
	ml->flag = SELECT;
	ml->type = 0;
	ml->expx = 1;
	ml->expy = 1;
	ml->expz = 1;
	ml->type = MB_BALL;

	return MetaElem_CreatePyObject( self->bpymetaball, BLI_countlist(&(self->bpymetaball->metaball->elems)) - 1 );
}


/*
 * removes a metaelement if it is a part of the metaball, 
 * no args are taken so the returned metaball must be modified after adding.
 * Accessed as mball.elements.add() where mball is a python metaball data type.
 */
static PyObject *MetaelemSeq_remove( BPyMetaelemSeqObject * self, BPyMetaelemObject *bml )
{
	MetaElem *ml_iter;
	int index=0;
	
	if( !BPyMetaelem_Check( bml ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"elements.remove(metaelem) - expected a Metaball element" );
	
	if (bml->bpymetaball->metaball != self->bpymetaball->metaball)
		return EXPP_ReturnPyObjError( PyExc_ValueError,
			"elements.remove(elem): elem is not from this metaball" );
	
	if (bml->index==-1)
		return EXPP_ReturnPyObjError( PyExc_ValueError,
			"elements.remove(elem): elem alredy removed" );
	
	for (ml_iter= self->bpymetaball->metaball->elems.first; ml_iter; ml_iter= ml_iter->next) {
		if (bml->index == index) {
			bml->index= -1; /* invalidate */
			BLI_freelinkN( &(self->bpymetaball->metaball->elems), ml_iter);
			Py_RETURN_NONE;
		}
		index++;
	}
	
	return EXPP_ReturnPyObjError( PyExc_ValueError,
		"elements.remove(elem): elem not in meta elements" );	
	
}

static struct PyMethodDef BPyMetaelemSeq_methods[] = {
	{"add", (PyCFunction)MetaelemSeq_add, METH_O,
		"add metaelem to metaball data"},
	{"remove", (PyCFunction)MetaelemSeq_remove, METH_O,
		"remove element from metaball data"},
	{NULL, NULL, 0, NULL}
};

/************************************************************************
 *
 * Python BPyMetaelemSeq_Type standard operations
 *
 ************************************************************************/

static void MetaelemSeq_dealloc( BPyMetaelemSeqObject * self )
{
	Py_DECREF(self->bpymetaball);
	PyObject_DEL( self );
}

/*****************************************************************************/
/* Python BPyMetaelemSeq_Type structure definition:                               */
/*****************************************************************************/
PyTypeObject BPyMetaelemSeq_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender MetaelemSeq",           /* char *tp_name; */
	sizeof( BPyMetaelemSeqObject ),       /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) MetaelemSeq_dealloc,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) MetaelemSeq_compare,   /* cmpfunc tp_compare; */
	( reprfunc ) MetaelemSeq_repr,     /* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	&MetaelemSeq_as_sequence,	    /* PySequenceMethods *tp_as_sequence; */
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
	( getiterfunc) MetaelemSeq_getIter, /* getiterfunc tp_iter; */
	( iternextfunc ) MetaelemSeq_nextIter, /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	BPyMetaelemSeq_methods,       /* struct PyMethodDef *tp_methods; */
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



PyObject *MetaballType_Init( void )
{
	/* only allocate new dictionary once */
	if( BPyMetaball_Type.tp_dict == NULL ) {
		BPyMetaball_Type.tp_dict = PyDict_New();
		PyConstCategory_AddObjectToDict( BPyMetaball_Type.tp_dict, &updateTypes);
		PyType_Ready( &BPyMetaball_Type ) ;
		BPyMetaball_Type.tp_dealloc = (destructor)&PyObject_Del;
	}
	return (PyObject *) &BPyMetaball_Type ;
}

PyObject *MetaelemType_Init( void )
{
	/* only allocate new dictionary once */
	if( BPyMetaelem_Type.tp_dict == NULL ) {
		BPyMetaelem_Type.tp_dict = PyDict_New();
		PyConstCategory_AddObjectToDict( BPyMetaelem_Type.tp_dict, &metaElemTypes);
		PyType_Ready( &BPyMetaelem_Type ) ;
	}
	return (PyObject *) &BPyMetaelem_Type ;
}

PyObject *MetaelemSeqType_Init( void )
{
	/* only allocate new dictionary once */
	if( BPyMetaelemSeq_Type.tp_dict == NULL ) {
		PyType_Ready( &BPyMetaelemSeq_Type ) ;
	}
	return (PyObject *) &BPyMetaelemSeq_Type ;
}

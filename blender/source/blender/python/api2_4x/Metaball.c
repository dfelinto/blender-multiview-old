/* 
 * $Id: Metaball.c 12898 2007-12-15 21:44:40Z campbellbarton $
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

/* for dealing with materials */
#include "MEM_guardedalloc.h"
#include "BKE_material.h"

/* checks for the metaelement being removed */
#define METAELEM_DEL_CHECK_PY(bpy_meta_elem) if (!(bpy_meta_elem->metaelem)) return ( V24_EXPP_ReturnPyObjError( PyExc_RuntimeError, "Metaball has been removed" ) );
#define METAELEM_DEL_CHECK_INT(bpy_meta_elem) if (!(bpy_meta_elem->metaelem)) return ( V24_EXPP_ReturnIntError( PyExc_RuntimeError, "Metaball has been removed" ) );

/*****************************************************************************/
/* Python API function prototypes for the Metaball module.                   */
/*****************************************************************************/
static PyObject *V24_M_Metaball_New( PyObject * self, PyObject * args );
static PyObject *V24_M_Metaball_Get( PyObject * self, PyObject * args );

/*****************************************************************************/
/* The following string definitions are used for documentation strings.      */
/* In Python these will be written to the console when doing a               */
/* Blender.Metaball.__doc__                                                  */
/*****************************************************************************/
static char V24_M_Metaball_doc[] =
	"The Blender Metaball module\n\n\nMetaballs are primitive shapes\
 such as balls, pipes, boxes and planes,\
 that can join each other to create smooth,\
 organic volumes\n. The shapes themseves are called\
 'Metaelements' and can be accessed from the Metaball module.";

static char V24_M_Metaball_New_doc[] = "Creates new metaball object data";

static char V24_M_Metaball_Get_doc[] = "Retreives an existing metaball object data";

/*****************************************************************************/
/* Python method structure definition for Blender.Metaball module:           */
/*****************************************************************************/
struct PyMethodDef V24_M_Metaball_methods[] = {
	{"New", V24_M_Metaball_New, METH_VARARGS, V24_M_Metaball_New_doc},
	{"Get", V24_M_Metaball_Get, METH_VARARGS, V24_M_Metaball_Get_doc},
	{NULL, NULL, 0, NULL}
};

static PyObject *V24_M_MetaElem_TypesDict( void )
{
	PyObject *Types = V24_PyConstant_New(  );

	if( Types ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) Types;

		V24_PyConstant_Insert( d, "BALL",	PyInt_FromLong( MB_BALL ) );
		/* V24_PyConstant_Insert( d, "TUBEX",	PyInt_FromLong( MB_TUBEX ) );  - DEPRICATED */
		/* V24_PyConstant_Insert( d, "TUBEY",	PyInt_FromLong( MB_TUBEY ) );  - DEPRICATED */
		/* V24_PyConstant_Insert( d, "TUBEZ",	PyInt_FromLong( MB_TUBEZ ) );  - DEPRICATED */
		V24_PyConstant_Insert( d, "TUBE",	PyInt_FromLong( MB_TUBE ) );
		V24_PyConstant_Insert( d, "PLANE",	PyInt_FromLong( MB_PLANE ) );
		V24_PyConstant_Insert( d, "ELIPSOID",PyInt_FromLong( MB_ELIPSOID ) );
		V24_PyConstant_Insert( d, "CUBE",	PyInt_FromLong( MB_CUBE ) );
	}

	return Types;
}

static PyObject *V24_M_MetaElem_UpdateDict( void )
{
	PyObject *Update = V24_PyConstant_New(  );

	if( Update ) {
		V24_BPy_constant *d = ( V24_BPy_constant * ) Update;
		V24_PyConstant_Insert( d, "ALWAYS",	PyInt_FromLong( MB_UPDATE_ALWAYS ) );
		V24_PyConstant_Insert( d, "HALFRES",PyInt_FromLong( MB_UPDATE_HALFRES ) );
		V24_PyConstant_Insert( d, "FAST",	PyInt_FromLong( MB_UPDATE_FAST ) );
		V24_PyConstant_Insert( d, "NEVER",	PyInt_FromLong( MB_UPDATE_NEVER ) );
	}

	return Update;
}

/*****************************************************************************/
/* Python V24_BPy_Metaball methods declarations:                                */
/*****************************************************************************/
static PyObject *V24_Metaball_getElements( V24_BPy_Metaball * self );
static PyObject *V24_Metaball_getMaterials( V24_BPy_Metaball * self );
static int V24_Metaball_setMaterials( V24_BPy_Metaball * self, PyObject * value );
static PyObject *V24_Metaball_getWiresize( V24_BPy_Metaball * self );
static int V24_Metaball_setWiresize( V24_BPy_Metaball * self, PyObject * value );
static PyObject *V24_Metaball_getRendersize( V24_BPy_Metaball * self );
static int V24_Metaball_setRendersize( V24_BPy_Metaball * self, PyObject * value);
static PyObject *V24_Metaball_getThresh( V24_BPy_Metaball * self );
static int V24_Metaball_setThresh( V24_BPy_Metaball * self, PyObject * args );
static PyObject *V24_Metaball_getUpdate( V24_BPy_Metaball * self );
static int V24_Metaball_setUpdate( V24_BPy_Metaball * self, PyObject * args );
static PyObject *V24_Metaball_copy( V24_BPy_Metaball * self );

/*****************************************************************************/
/* Python V24_BPy_Metaball methods table:                                       */
/*****************************************************************************/
static PyMethodDef V24_BPy_Metaball_methods[] = {
	/* name, method, flags, doc */
	{"__copy__", ( PyCFunction ) V24_Metaball_copy,
	 METH_NOARGS, "() - Return a copy of this metaball"},
	{"copy", ( PyCFunction ) V24_Metaball_copy,
	 METH_NOARGS, "() - Return a copy of this metaball"},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python V24_BPy_Metaelem methods table:                                        */
/*****************************************************************************/
static PyMethodDef V24_BPy_Metaelem_methods[] = {
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python V24_Metaball_Type callback function prototypes:                       */
/*****************************************************************************/
static PyObject *V24_Metaball_repr( V24_BPy_Metaball * self );
static int V24_Metaball_compare( V24_BPy_Metaball * a, V24_BPy_Metaball * b );

/*****************************************************************************/
/* Python V24_Metaelem_Type callback function prototypes:                        */
/*****************************************************************************/
static void V24_Metaelem_dealloc( V24_BPy_Metaelem * self );
static PyObject *V24_Metaelem_repr( V24_BPy_Metaelem * self );
static int V24_Metaelem_compare( V24_BPy_Metaelem * a, V24_BPy_Metaelem * b );

static PyObject *V24_Metaelem_getType( V24_BPy_Metaelem *self );
static int V24_Metaelem_setType( V24_BPy_Metaelem * self,  PyObject * args );
static PyObject *V24_Metaelem_getCoord( V24_BPy_Metaelem * self );
static int V24_Metaelem_setCoord( V24_BPy_Metaelem * self,  V24_VectorObject * value );
static PyObject *V24_Metaelem_getDims( V24_BPy_Metaelem * self );
static int V24_Metaelem_setDims( V24_BPy_Metaelem * self,  V24_VectorObject * value );
static PyObject *V24_Metaelem_getQuat( V24_BPy_Metaelem * self );
static int V24_Metaelem_setQuat( V24_BPy_Metaelem * self,  V24_QuaternionObject * value );
static PyObject *V24_Metaelem_getStiffness( V24_BPy_Metaelem * self );
static int V24_Metaelem_setStiffness( V24_BPy_Metaelem * self,  PyObject * value );
static PyObject *V24_Metaelem_getRadius( V24_BPy_Metaelem * self );
static int V24_Metaelem_setRadius( V24_BPy_Metaelem * self,  PyObject * value );

static PyObject *V24_Metaelem_getMFlagBits( V24_BPy_Metaelem * self, void * type );
static int V24_Metaelem_setMFlagBits( V24_BPy_Metaelem * self, PyObject * value, void * type );

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef V24_BPy_Metaball_getseters[] = {
	GENERIC_LIB_GETSETATTR,
	{"materials",
	 (getter)V24_Metaball_getMaterials, (setter)V24_Metaball_setMaterials,
	 "Number of metaball users",
	 NULL},
	{"elements",
	 (getter)V24_Metaball_getElements, (setter)NULL,
	 "Elements in this metaball",
	 NULL},
	{"wiresize",
	 (getter)V24_Metaball_getWiresize, (setter)V24_Metaball_setWiresize,
	 "The density to draw the metaball in the 3D view",
	 NULL},
	{"rendersize",
	 (getter)V24_Metaball_getRendersize, (setter)V24_Metaball_setRendersize,
	 "The density to render wire",
	 NULL},
	{"thresh",
	 (getter)V24_Metaball_getThresh, (setter)V24_Metaball_setThresh,
	 "The density to render wire",
	 NULL},
	{"update",
	 (getter)V24_Metaball_getUpdate, (setter)V24_Metaball_setUpdate,
	 "The setting for updating this metaball data",
	 NULL},
	{NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};


/*****************************************************************************/
/* Python TypeMetaball structure definition:                                     */
/*****************************************************************************/
PyTypeObject V24_Metaball_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Metaball",             /* char *tp_name; */
	sizeof( V24_BPy_Metaball ),         /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,						/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) V24_Metaball_compare,   /* cmpfunc tp_compare; */
	( reprfunc ) V24_Metaball_repr,     /* reprfunc tp_repr; */

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
	V24_BPy_Metaball_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	V24_BPy_Metaball_getseters,         /* struct PyGetSetDef *tp_getset; */
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


static PyGetSetDef V24_BPy_Metaelem_getseters[] = {
	{"type",
	 (getter)V24_Metaelem_getType, (setter)V24_Metaelem_setType,
	 "Metaelem Type",
	 NULL},
	{"co",
	 (getter)V24_Metaelem_getCoord, (setter)V24_Metaelem_setCoord,
	 "Metaelem Location",
	 NULL},
	{"quat",
	 (getter)V24_Metaelem_getQuat, (setter)V24_Metaelem_setQuat,
	 "Metaelem Rotation Quat",
	 NULL},
	{"dims",
	 (getter)V24_Metaelem_getDims, (setter)V24_Metaelem_setDims,
	 "Metaelem Dimensions",
	 NULL},
	{"stiffness",
	 (getter)V24_Metaelem_getStiffness, (setter)V24_Metaelem_setStiffness,
	 "MetaElem stiffness",
	 NULL},
	{"radius",
	 (getter)V24_Metaelem_getRadius, (setter)V24_Metaelem_setRadius,
	 "The radius of the MetaElem",
	 NULL},
	{"negative",
	 (getter)V24_Metaelem_getMFlagBits, (setter)V24_Metaelem_setMFlagBits,
	 "The density to render wire",
	 (void *)MB_NEGATIVE},
	{"hide",
	 (getter)V24_Metaelem_getMFlagBits, (setter)V24_Metaelem_setMFlagBits,
	 "The density to render wire",
	 (void *)MB_HIDE},
	{NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};


/*****************************************************************************/
/* Python TypeMetaelem structure definition:                                     */
/*****************************************************************************/
PyTypeObject V24_Metaelem_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Metaelem",             /* char *tp_name; */
	sizeof( V24_BPy_Metaelem ),         /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) V24_Metaelem_dealloc,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) V24_Metaelem_compare,   /* cmpfunc tp_compare; */
	( reprfunc ) V24_Metaelem_repr,     /* reprfunc tp_repr; */

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
	V24_BPy_Metaelem_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	V24_BPy_Metaelem_getseters,         /* struct PyGetSetDef *tp_getset; */
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
/* Function:              V24_M_Metaball_New                                     */
/* Python equivalent:     Blender.Metaball.New                               */
/*****************************************************************************/
static PyObject *V24_M_Metaball_New( PyObject * self, PyObject * args )
{
	char *name = 0;
	V24_BPy_Metaball *pymball;	/* for Data object wrapper in Python */
	MetaBall *blmball;	/* for actual Data we create in Blender */
	
	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"Metaball.New() - expected string argument (or nothing)" ) );

	/* first create the MetaBall Data in Blender */
	if (name)
		blmball = add_mball( name );
	else
		blmball = add_mball( "Meta" );

	if( blmball ) {
		/* return user count to zero since add_mball() incref'ed it */
		blmball->id.us = 0;
		/* now create the wrapper obj in Python */
		pymball =
			( V24_BPy_Metaball * ) PyObject_NEW( V24_BPy_Metaball,
							 &V24_Metaball_Type );
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"Metaball.New() - couldn't create data in Blender" ) );

	if( pymball == NULL )
		return ( V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
						"couldn't create MetaBall Data object" ) );

	pymball->metaball = blmball;
	/*link Python mballer wrapper to Blender MetaBall */
	
	return ( PyObject * ) pymball;
}


/*****************************************************************************/
/* Function:             V24_M_Metaball_Get                                     */
/* Python equivalent:    Blender.Metaball.Get                               */
/* Description:          Receives a string and returns the metaball data obj */
/*                       whose name matches the string.  If no argument is  */
/*                       passed in, a list of all metaball data names in the */
/*                       current scene is returned.                         */
/*****************************************************************************/
static PyObject *V24_M_Metaball_Get( PyObject * self, PyObject * args )
{
	char *name = NULL;
	MetaBall *mball_iter = NULL;

	if( !PyArg_ParseTuple( args, "|s", &name ) )
		return ( V24_EXPP_ReturnPyObjError( PyExc_TypeError,
						"Metaball.Get() - expected string argument (or nothing)" ) );

	if( name ) {		/* (name) - Search mball by name */
		mball_iter = ( MetaBall * ) V24_GetIdFromList( &( G.main->mball ), name );
		
		if (!mball_iter) {
			char error_msg[128];
			PyOS_snprintf( error_msg, sizeof( error_msg ),
				       "Metaball.Get(\"%s\") - not found", name );
			return ( V24_EXPP_ReturnPyObjError
				 ( PyExc_NameError, error_msg ) );
		}
		return V24_Metaball_CreatePyObject(mball_iter);
	
	} else {			/* () - return a list of all mballs in the scene */
		
		PyObject *mballlist = PyList_New( BLI_countlist( &( G.main->mball ) ) );
		int index=0;
		
		if( mballlist == NULL )
			return ( V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
							"MetaBall.Get() - couldn't create PyList" ) );
		
		mball_iter = G.main->mball.first;
		while( mball_iter ) {
			PyList_SetItem( mballlist, index, V24_Metaball_CreatePyObject(mball_iter) );
			index++;
			mball_iter = mball_iter->id.next;
		}
		return mballlist;
	}

}


/*****************************************************************************/
/* Function:	 initObject						*/
/*****************************************************************************/
PyObject *V24_Metaball_Init( void )
{
	PyObject *V24_submodule;
	PyObject *Types=	V24_M_MetaElem_TypesDict( );
	PyObject *Update=	V24_M_MetaElem_UpdateDict( );
	
	if( PyType_Ready( &V24_Metaball_Type ) < 0 )
		return NULL;
	if( PyType_Ready( &V24_Metaelem_Type ) < 0 )
		return NULL;
	if( PyType_Ready( &V24_MetaElemSeq_Type ) < 0 )
		return NULL;
	
	V24_submodule = Py_InitModule3( "Blender.Metaball", V24_M_Metaball_methods, V24_M_Metaball_doc);

	if( Types )
		PyModule_AddObject( V24_submodule, "Types", Types );
		PyModule_AddObject( V24_submodule, "Update", Update );
	
	/*Add SUBMODULES to the module*/
	/*PyDict_SetItemString(dict, "Constraint", V24_Constraint_Init()); */ /*creates a *new* module*/
	return V24_submodule;
}

MetaBall *V24_Metaball_FromPyObject( PyObject * pyobj )
{
	return ( ( V24_BPy_Metaball * ) pyobj )->metaball;
}

static PyObject *V24_Metaball_getMaterials( V24_BPy_Metaball *self )
{
	return V24_EXPP_PyList_fromMaterialList( self->metaball->mat,
			self->metaball->totcol, 1 );
}
static int V24_Metaball_setMaterials( V24_BPy_Metaball *self, PyObject * value )
{
    Material **matlist;
	int len;

    if( !PySequence_Check( value ) ||
			!V24_EXPP_check_sequence_consistency( value, &V24_Material_Type ) )
        return V24_EXPP_ReturnIntError( PyExc_TypeError,
                  "metaball.materials - list should only contain materials or None)" );

    len = PyList_Size( value );
    if( len > 16 )
        return V24_EXPP_ReturnIntError( PyExc_TypeError,
                          "metaball.materials - list can't have more than 16 materials" );

	/* free old material list (if it exists) and adjust user counts */
	if( self->metaball->mat ) {
		MetaBall *mb = self->metaball;
		int i;
		for( i = mb->totcol; i-- > 0; )
			if( mb->mat[i] )
           		mb->mat[i]->id.us--;
		MEM_freeN( mb->mat );
	}

	/* build the new material list, increment user count, store it */

	matlist = V24_EXPP_newMaterialList_fromPyList( value );
	V24_EXPP_incr_mats_us( matlist, len );
	self->metaball->mat = matlist;
    	self->metaball->totcol = (short)len;

/**@ This is another ugly fix due to the weird material handling of blender.
    * it makes sure that object material lists get updated (by their length)
    * according to their data material lists, otherwise blender crashes.
    * It just stupidly runs through all objects...BAD BAD BAD.
    */

    	test_object_materials( ( ID * ) self->metaball );

	return 0;
}

static PyObject *V24_Metaball_getWiresize( V24_BPy_Metaball * self )
{
	return PyFloat_FromDouble( self->metaball->wiresize );
}

static int V24_Metaball_setWiresize( V24_BPy_Metaball * self, PyObject * value )
{
	float param;
	if( !PyNumber_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					"metaball.wiresize - expected float argument" );

	param = (float)PyFloat_AsDouble( value );

	self->metaball->wiresize = V24_EXPP_ClampFloat(param, 0.05f, 1.0);
	return 0;

}
static PyObject *V24_Metaball_getRendersize( V24_BPy_Metaball * self )
{
	return PyFloat_FromDouble( self->metaball->rendersize );
}

static int V24_Metaball_setRendersize( V24_BPy_Metaball * self, PyObject * value )
{

	float param;
	if( !PyNumber_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					"metaball.rendersize - expected float argument" );

	param = (float)PyFloat_AsDouble( value );

	self->metaball->rendersize = V24_EXPP_ClampFloat(param, 0.05f, 1.0);
	return 0;
}

static PyObject *V24_Metaball_getThresh( V24_BPy_Metaball * self )
{
	return PyFloat_FromDouble( self->metaball->thresh );
}

static int V24_Metaball_setThresh( V24_BPy_Metaball * self, PyObject * value )
{

	float param;
	if( !PyNumber_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					"metaball.thresh - expected float argument" );

	param = (float)PyFloat_AsDouble( value );

	self->metaball->thresh = V24_EXPP_ClampFloat(param, 0.0, 5.0);
	return 0;
}

static PyObject *V24_Metaball_getUpdate( V24_BPy_Metaball * self )
{
	return PyInt_FromLong( (long)self->metaball->flag );
}

static int V24_Metaball_setUpdate( V24_BPy_Metaball * self, PyObject * value )
{

	int param;
	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					"metaball.update - expected an int argument" );

	param = (int)PyInt_AS_LONG( value );

	self->metaball->flag = V24_EXPP_ClampInt( param, 0, 3 );
	return 0;
}

static PyObject *V24_Metaball_copy( V24_BPy_Metaball * self )
{
	V24_BPy_Metaball *pymball;	/* for Data object wrapper in Python */
	MetaBall *blmball;	/* for actual Data we create in Blender */
	
	blmball = copy_mball( self->metaball );	/* first create the MetaBall Data in Blender */

	if( blmball ) {
		/* return user count to zero since add_mball() incref'ed it */
		blmball->id.us = 0;
		/* now create the wrapper obj in Python */
		pymball =
			( V24_BPy_Metaball * ) PyObject_NEW( V24_BPy_Metaball,
							 &V24_Metaball_Type );
	} else
		return ( V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"metaball.__copy__() - couldn't create data in Blender" ) );

	if( pymball == NULL )
		return ( V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
						"metaball.__copy__() - couldn't create data in Blender" ) );

	pymball->metaball = blmball;
	
	return ( PyObject * ) pymball;
}


/* These are needed by Object.c */
PyObject *V24_Metaball_CreatePyObject( MetaBall * mball)
{
	V24_BPy_Metaball *py_mball= PyObject_NEW( V24_BPy_Metaball, &V24_Metaball_Type );

	if( !py_mball )
		return V24_EXPP_ReturnPyObjError( PyExc_MemoryError,
				"couldn't create V24_BPy_Metaball object" );

	py_mball->metaball= mball;

	return ( PyObject * ) py_mball;
}


static PyObject *V24_MetaElemSeq_CreatePyObject(V24_BPy_Metaball *self, MetaElem *iter)
{
	V24_BPy_MetaElemSeq *seq = PyObject_NEW( V24_BPy_MetaElemSeq, &V24_MetaElemSeq_Type);
	seq->bpymetaball = self; Py_INCREF(self);
	seq->iter= iter;
	return (PyObject *)seq;
}

/*
 * Element, get an instance of the iterator.
 */
static PyObject *V24_Metaball_getElements( V24_BPy_Metaball * self )
{
	return V24_MetaElemSeq_CreatePyObject(self, NULL);
}

/*
 * Metaelem dealloc - free from memory
 */
/* This is a callback function for the V24_BPy_Metaelem type. It is */
static void V24_Metaelem_dealloc( V24_BPy_Metaelem * self )
{
	self->metaelem= NULL; /* so any references to the same bpyobject will raise an error */
	PyObject_DEL( self );
}

/*
 * elem.type - int to set the shape of the element
 */
static PyObject *V24_Metaelem_getType( V24_BPy_Metaelem *self )
{	
	METAELEM_DEL_CHECK_PY(self);
	
	return PyInt_FromLong( self->metaelem->type );
}
static int V24_Metaelem_setType( V24_BPy_Metaelem * self,  PyObject * value )
{
	int type;
	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
			"metaelem.type - expected an integer (bitmask) as argument" );
	
	METAELEM_DEL_CHECK_INT(self);
	
	type = PyInt_AS_LONG( value );
	
	if( (type < 0) || ( type > ( MB_BALL | MB_TUBEX | MB_TUBEY | MB_TUBEZ | MB_TUBE | MB_PLANE | MB_ELIPSOID | MB_CUBE ) ))
		return V24_EXPP_ReturnIntError( PyExc_ValueError,
					      "metaelem.type - value out of range" );
	
	self->metaelem->type= type;
	return 0;
}

/*
 * elem.co - non wrapped vector representing location
 */
static PyObject *V24_Metaelem_getCoord( V24_BPy_Metaelem * self )
{
	float co[3];
	
	METAELEM_DEL_CHECK_PY(self);
	
	co[0]= self->metaelem->x;
	co[1]= self->metaelem->y;
	co[2]= self->metaelem->z;
	
	return V24_newVectorObject( co, 3, Py_NEW );
}
static int V24_Metaelem_setCoord( V24_BPy_Metaelem * self,  V24_VectorObject * value )
{

	if( !V24_VectorObject_Check( value ) || value->size != 3 )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"metaelem.co - expected vector argument of size 3" );
	
	METAELEM_DEL_CHECK_INT(self);
	
	self->metaelem->x= value->vec[0];
	self->metaelem->y= value->vec[1];
	self->metaelem->z= value->vec[2];
	return 0;
}

/*
 * elem.dims - non wrapped vector representing the xyz dimensions
 * only effects some element types
 */
static PyObject *V24_Metaelem_getDims( V24_BPy_Metaelem * self )
{
	float co[3];
	METAELEM_DEL_CHECK_PY(self);
	
	co[0]= self->metaelem->expx;
	co[1]= self->metaelem->expy;
	co[2]= self->metaelem->expz;
	return V24_newVectorObject( co, 3, Py_NEW );
}
static int V24_Metaelem_setDims( V24_BPy_Metaelem * self,  V24_VectorObject * value )
{
	if( !V24_VectorObject_Check( value ) || value->size != 3 )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"metaelem.dims - expected vector argument of size 3" );

	METAELEM_DEL_CHECK_INT(self);
	
	self->metaelem->expx= V24_EXPP_ClampFloat(value->vec[0], 0.0, 20.0);
	self->metaelem->expy= V24_EXPP_ClampFloat(value->vec[1], 0.0, 20.0);
	self->metaelem->expz= V24_EXPP_ClampFloat(value->vec[2], 0.0, 20.0);
	return 0;
}

/*
 * elem.quat - non wrapped quat representing the rotation
 * only effects some element types - a rotated ball has no effect for eg.
 */
static PyObject *V24_Metaelem_getQuat( V24_BPy_Metaelem * self )
{
	METAELEM_DEL_CHECK_PY(self);
	return V24_newQuaternionObject(self->metaelem->quat, Py_NEW); 
}
static int V24_Metaelem_setQuat( V24_BPy_Metaelem * self,  V24_QuaternionObject * value )
{
	int i;
	if( !V24_QuaternionObject_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"metaelem.quat - expected quat argument" );
	
	METAELEM_DEL_CHECK_INT(self);
	
	for (i = 0; i < 4; i++)
		self->metaelem->quat[i]= value->quat[i];
	
	/* need to normalize or metaball drawing can go into an infinate loop */
	NormalQuat(self->metaelem->quat);
	
	return 0;
}

/*
 * elem.hide and elem.sel - get/set true false
 */
static PyObject *V24_Metaelem_getMFlagBits( V24_BPy_Metaelem * self, void * type )
{
	METAELEM_DEL_CHECK_PY(self);
	return V24_EXPP_getBitfield( &(self->metaelem->flag), (int)((long)type ), 'h' );
}
static int V24_Metaelem_setMFlagBits( V24_BPy_Metaelem * self, PyObject * value,
		void * type )
{
	METAELEM_DEL_CHECK_INT(self);
	return V24_EXPP_setBitfield( value, &(self->metaelem->flag), 
			(int)((long)type), 'h' );
}

/*
 * elem.stiffness - floating point, the volume of this element.
 */
static PyObject *V24_Metaelem_getStiffness( V24_BPy_Metaelem *self )
{
	METAELEM_DEL_CHECK_PY(self);
	return PyFloat_FromDouble( self->metaelem->s );
}
static int V24_Metaelem_setStiffness( V24_BPy_Metaelem *self, PyObject *value)
{
	if( !PyNumber_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					"metaelem.stiffness - expected float argument" );

	self->metaelem->s = V24_EXPP_ClampFloat((float)PyFloat_AsDouble( value ), 0.0, 10.0);
	return 0;
}

/*
 * elem.radius- floating point, the size if the element
 */
static PyObject *V24_Metaelem_getRadius( V24_BPy_Metaelem *self )
{
	METAELEM_DEL_CHECK_PY(self);
	return PyFloat_FromDouble( self->metaelem->rad );
}
static int V24_Metaelem_setRadius( V24_BPy_Metaelem *self, PyObject *value)
{
	if( !PyNumber_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					"metaelem.radius - expected float argument" );

	self->metaelem->rad = /* is 5000 too small? */
			V24_EXPP_ClampFloat((float)PyFloat_AsDouble( value ), 0.0, 5000.0);
	
	return 0;
}


/*
 * callback functions for comparison.
 * It compares two V24_Metaball_Type objects. Only the "==" and "!="
 * comparisons are meaninful. Returns 0 for equality and -1 if
 * they don't point to the same Blender struct.
 * In Python it becomes 1 if they are equal, 0 otherwise.	
 */
static int V24_Metaball_compare( V24_BPy_Metaball * a, V24_BPy_Metaball * b )
{
	MetaBall *pa = a->metaball, *pb = b->metaball;
	return ( pa == pb ) ? 0 : -1;
}

static int V24_MetaElemSeq_compare( V24_BPy_MetaElemSeq * a, V24_BPy_MetaElemSeq * b )
{
	MetaBall *pa = a->bpymetaball->metaball, *pb = b->bpymetaball->metaball;
	return ( pa == pb ) ? 0 : -1;
}

static int V24_Metaelem_compare( V24_BPy_Metaelem * a, V24_BPy_Metaelem * b )
{
	MetaElem *pa = a->metaelem, *pb = b->metaelem;
	return ( pa == pb ) ? 0 : -1;
}

/*
 * repr function
 * callback functions building meaninful string to representations
 */
static PyObject *V24_Metaball_repr( V24_BPy_Metaball * self )
{
	return PyString_FromFormat( "[Metaball \"%s\"]",
				    self->metaball->id.name + 2 );
}

static PyObject *V24_Metaelem_repr( V24_BPy_Metaelem * self )
{
	return PyString_FromString( "Metaelem" );
}

static PyObject *V24_MetaElemSeq_repr( V24_BPy_MetaElemSeq * self )
{
	return PyString_FromFormat( "[Metaball Iterator \"%s\"]",
				    self->bpymetaball->metaball->id.name + 2 );
}



/*
 * MeteElem Seq sequence 
 */

static PyObject *V24_MetaElem_CreatePyObject( MetaElem *metaelem )
{
	V24_BPy_Metaelem *elem= PyObject_NEW( V24_BPy_Metaelem, &V24_Metaelem_Type);
	elem->metaelem = metaelem; Py_INCREF(elem);
	return (PyObject *)elem;
}

static int V24_MetaElemSeq_len( V24_BPy_MetaElemSeq * self )
{
	return BLI_countlist( &( self->bpymetaball->metaball->elems ) );
}


static PySequenceMethods V24_MetaElemSeq_as_sequence = {
	( inquiry ) V24_MetaElemSeq_len,	/* sq_length */
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
 * Python V24_MetaElemSeq_Type iterator (iterates over Metaballs)
 *
 ************************************************************************/

/*
 * Initialize the interator
 */

static PyObject *V24_MetaElemSeq_getIter( V24_BPy_MetaElemSeq * self )
{
	if (!self->iter) { /* not alredy looping on this data, */
		self->iter = self->bpymetaball->metaball->elems.first;
		return V24_EXPP_incr_ret ( (PyObject *) self );
	} else
		return V24_MetaElemSeq_CreatePyObject(self->bpymetaball, self->bpymetaball->metaball->elems.first);
}

/*
 * Return next MetaElem.
 */

static PyObject *V24_MetaElemSeq_nextIter( V24_BPy_MetaElemSeq * self )
{
	PyObject *object;
	if( !(self->iter) ||  !(self->bpymetaball->metaball) ) {
		self->iter= NULL;
		return V24_EXPP_ReturnPyObjError( PyExc_StopIteration,
				"iterator at end" );
	}
	
	object= V24_MetaElem_CreatePyObject( self->iter ); 
	self->iter= self->iter->next;
	return object;
}

/*
 * Adds and returns a new metaelement, 
 * no args are taken so the returned metaball must be modified after adding.
 * Accessed as mball.elements.add() where mball is a python metaball data type.
 */
static PyObject *V24_MetaElemSeq_add( V24_BPy_MetaElemSeq * self )
{
	MetaElem *ml;

	ml = MEM_callocN( sizeof( MetaElem ), "metaelem" );
	BLI_addhead( &( self->bpymetaball->metaball->elems ), ml );
	ml->x = 0;
	ml->y = 0;
	ml->z = 0;
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

	return V24_MetaElem_CreatePyObject(ml);
}


/*
 * removes a metaelement if it is a part of the metaball, 
 * no args are taken so the returned metaball must be modified after adding.
 * Accessed as mball.elements.add() where mball is a python metaball data type.
 */
static PyObject *V24_MetaElemSeq_remove( V24_BPy_MetaElemSeq * self, V24_BPy_Metaelem *elem )
{
	MetaElem *ml_iter, *ml_py;
	
	if( !V24_BPy_Metaelem_Check(elem) )
		return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
			"elements.remove(metaelem) - expected a Metaball element" );
	
	METAELEM_DEL_CHECK_PY(elem);
	
	ml_py= elem->metaelem;
	
	for (ml_iter= self->bpymetaball->metaball->elems.first; ml_iter; ml_iter= ml_iter->next) {
		if (ml_py == ml_iter) {
			elem->metaelem= NULL;
			BLI_freelinkN( &(self->bpymetaball->metaball->elems), ml_py);
			Py_RETURN_NONE;
		}
	}
	
	return V24_EXPP_ReturnPyObjError( PyExc_ValueError,
		"elements.remove(elem): elem not in meta elements" );	
	
}

static struct PyMethodDef V24_BPy_MetaElemSeq_methods[] = {
	{"add", (PyCFunction)V24_MetaElemSeq_add, METH_NOARGS,
		"add metaelem to metaball data"},
	{"remove", (PyCFunction)V24_MetaElemSeq_remove, METH_O,
		"remove element from metaball data"},
	{NULL, NULL, 0, NULL}
};

/************************************************************************
 *
 * Python V24_MetaElemSeq_Type standard operations
 *
 ************************************************************************/

static void V24_MetaElemSeq_dealloc( V24_BPy_MetaElemSeq * self )
{
	Py_DECREF(self->bpymetaball);
	PyObject_DEL( self );
}

/*****************************************************************************/
/* Python V24_MetaElemSeq_Type structure definition:                               */
/*****************************************************************************/
PyTypeObject V24_MetaElemSeq_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender MetaElemSeq",           /* char *tp_name; */
	sizeof( V24_BPy_MetaElemSeq ),       /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) V24_MetaElemSeq_dealloc,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) V24_MetaElemSeq_compare,   /* cmpfunc tp_compare; */
	( reprfunc ) V24_MetaElemSeq_repr,     /* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	&V24_MetaElemSeq_as_sequence,	    /* PySequenceMethods *tp_as_sequence; */
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
	( getiterfunc) V24_MetaElemSeq_getIter, /* getiterfunc tp_iter; */
	( iternextfunc ) V24_MetaElemSeq_nextIter, /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	V24_BPy_MetaElemSeq_methods,       /* struct PyMethodDef *tp_methods; */
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


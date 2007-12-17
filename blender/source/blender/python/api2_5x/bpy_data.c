/* 
 * $Id: bpy_data.c 11934 2007-09-03 20:57:12Z campbellbarton $
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
 * Contributor(s): Campbell Barton
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include "bpy_data.h"

#include "MEM_guardedalloc.h"	/* for MEM_callocN */
#include "DNA_space_types.h"	/* SPACE_VIEW3D, SPACE_SEQ */
#include "DNA_scene_types.h"
#include "DNA_object_types.h" /* LibBlockSeq_new */
#include "DNA_texture_types.h"
#include "DNA_curve_types.h"
#include "DNA_ipo_types.h"
#include "DNA_group_types.h"
#include "DNA_world_types.h"
#include "DNA_vfont_types.h"
#include "DNA_armature_types.h"
#include "DNA_sound_types.h"
#include "DNA_text_types.h"
#include "DNA_action_types.h"
#include "DNA_meta_types.h"

/* Use the add_* from BKE_* */
#include "BKE_global.h"
#include "BKE_utildefines.h" /* clamp */
#include "BKE_armature.h"
#include "BKE_ipo.h"
#include "BKE_image.h"
#include "BKE_main.h"
#include "BKE_library.h"
#include "BKE_lattice.h"
#include "BKE_object.h"
#include "BKE_scene.h"
#include "BKE_sca.h" /*free_text_controllers*/
#include "BKE_font.h"
#include "BKE_mball.h"
#include "BKE_mesh.h"
#include "BKE_curve.h"
#include "BKE_material.h"
#include "BKE_group.h"
#include "BKE_text.h"
#include "BKE_texture.h"
#include "BKE_world.h"

#include "BLI_blenlib.h" /* BLI_countlist */
#include "BIF_drawscene.h"	/* for set_scene */
#include "BIF_screen.h"		/* curarea */
#include "BIF_drawimage.h" /* what image */
#include "BIF_drawtext.h" /* unlink_text */
#include "BIF_editsound.h" /* sound_new_sound */
#include "BIF_editaction.h" /* add_empty_action */

/* python types */
#include "../BPY_extern.h" /* clearing scriptlinks */

#include "gen_utils.h"
#include "gen_library.h" /* generic ID functions */

#include "Object.h"
#include "Camera.h"
#include "Armature.h"
#include "Lamp.h"
#include "Curve.h"
#include "Mesh.h"
#include "Lattice.h"
#include "Metaball.h"
#include "Text.h"
#include "CurveText.h"
#include "Font.h"
#include "Group.h"
#include "World.h"
#include "Texture.h"
#include "Ipo.h"
#include "Text.h"
#include "Sound.h"
#include "NLA.h"
#include "Scene.h"
#include "Library.h"

#include "bpy_config.h" /* config pydata */

/* used only for texts.active */
#include "BIF_screen.h"
#include "DNA_space_types.h"
#include "DNA_screen_types.h"

/* need these for load/save/pack functions */
#include <fcntl.h>
#include "BDR_editobject.h"	/* exit_editmode() */
#include "BKE_blender.h"
#include "BIF_usiblender.h"
#include "BPI_script.h"
#include "BKE_exotic.h"
#include "BKE_packedFile.h"
#include "BLO_writefile.h"

extern VFont *get_builtin_font(void);

static PyObject *LibBlockSeq_CreatePyObject( Link *iter, int type )
{
	BPyLibBlockSeqObject *seq = PyObject_NEW( BPyLibBlockSeqObject, &BPyLibBlockSeq_Type);
	seq->iter = iter;
	seq->type = type;
	return (PyObject *)seq;
}


static int LibBlockSeq_len( BPyLibBlockSeqObject * self )
{
	ListBase *lb = wich_libbase(G.main, self->type);
	return BLI_countlist( lb );
}

static PyObject * LibBlockSeq_subscript(BPyLibBlockSeqObject * self, PyObject *key)
{
	char *name;
	char *lib= NULL;
	char use_lib = 0;
	ID *id;
	
	id = (ID *)wich_libbase(G.main, self->type)->first;
	
	if ( PyString_Check(key) ) {
		name = PyString_AsString ( key );
	} else if (PyTuple_Check(key) && (PyTuple_Size(key) == 2) ) {
		PyObject *pydata;
		use_lib = 1;
		
		/* Get the first arg */
		pydata = PyTuple_GET_ITEM(key, 0);
		if (!PyString_Check(pydata)) {
			return EXPP_ReturnPyObjError( PyExc_TypeError,
				"the data name must be a string" );
		}
		
		name = PyString_AsString ( pydata );
		
		/* Get the second arg */
		pydata = PyTuple_GET_ITEM(key, 1);
		if (pydata == Py_None) {
			lib = NULL; /* data must be local */
		} else if (PyString_Check(pydata)) {
			lib = PyString_AsString ( pydata );
			if (!strcmp( "", lib)) {
				lib = NULL; /* and empty string also means data must be local */
			}
		} else {
			return EXPP_ReturnPyObjError( PyExc_TypeError,
				"the lib name must be a string or None" );
		}
	} else {
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected the a name string or a tuple (lib, name)" );
	}
	
	for (; id; id = id->next) {
		if(!strcmp( name, id->name+2 )) {
			if (
				(!use_lib) || /* any data, local or external lib data */
				(use_lib && !lib && !id->lib) || /* only local */
				(lib && use_lib && id->lib && (!strcmp( id->lib->name, lib))) /* only external lib */
			)
			{
				return GetPyObjectFromID(id);
			}
		}
	}
	return ( EXPP_ReturnPyObjError
				 ( PyExc_KeyError, "Requested data does not exist") );
}

static PyMappingMethods LibBlockSeq_as_mapping = {
	( inquiry ) LibBlockSeq_len,	/* mp_length */
	( binaryfunc ) LibBlockSeq_subscript,	/* mp_subscript */
	( objobjargproc ) 0,	/* mp_ass_subscript */
};


/************************************************************************
 *
 * Python BPyLibBlockSeq_Type iterator (iterates over GroupObjects)
 *
 ************************************************************************/

/*
 * Initialize the interator index
 */

static PyObject *LibBlockSeq_getIter( BPyLibBlockSeqObject * self )
{
	/* we need to get the first base, but for selected context we may need to advance
	to the first selected or first conext base */
	
	ListBase *lb;
	Link *link;
	lb = wich_libbase(G.main, self->type);
	
	link = lb->first;
	
	/* create a new iterator if were alredy using this one */
	if (self->iter==NULL) {
		self->iter = link;
		return EXPP_incr_ret ( (PyObject *) self );
	} else {
		return LibBlockSeq_CreatePyObject(link, self->type);
	}
}

/*
 * Return next LibBlockSeq iter.
 */

static PyObject *LibBlockSeq_nextIter( BPyLibBlockSeqObject * self )
{
	PyObject *object;
	Link *link;
	if( !(self->iter) ) {
		self->iter= NULL;
		return EXPP_ReturnPyObjError( PyExc_StopIteration,
				"iterator at end" );
	}
	
	object = GetPyObjectFromID((ID *)self->iter);
	
	link= self->iter->next;
	self->iter= link;
	return object;
}

PyObject *LibBlockSeq_getActive(BPyLibBlockSeqObject *self)
{
	switch (self->type) {
	case ID_SCE:
		if ( !G.scene ) {
			Py_RETURN_NONE;
		} else {
			return Scene_CreatePyObject( ( Scene * ) G.scene );
		}
		
		break;
	case ID_IM:
		if (!G.sima || !G.sima->image) {
			Py_RETURN_NONE;
		} else {
			what_image( G.sima );	/* make sure image data exists */
			return Image_CreatePyObject( G.sima->image );
		}
		break;
	case ID_TXT: {
			SpaceText *st= curarea->spacedata.first;
			
			if (st->spacetype!=SPACE_TEXT || st->text==NULL) {
				Py_RETURN_NONE;
			} else {
				return Text_CreatePyObject( st->text );
			}
		}
	}
	
	return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Only Scene and Image types have the active attribute" );
}

static int LibBlockSeq_setActive(BPyLibBlockSeqObject *self, PyObject *value)
{
	switch (self->type) {
	case ID_SCE:
		if (!BPyScene_Check(value)) {
			return EXPP_ReturnIntError(PyExc_TypeError,
					"Must be a scene" );
		} else {
			BPySceneObject *bpydata;
			Scene *data;
			
			bpydata = (BPySceneObject *)value;
			data= bpydata->scene;
			
			if (!data)
				return EXPP_ReturnIntError(PyExc_RuntimeError,
					"This Scene has been removed" );
			
			if (data != G.scene) {
				set_scene( data );
				scene_update_for_newframe(data, data->lay);
			}
		}
		return 0;
		
	case ID_IM:
		if (!BPyImage_Check(value)) {
			return EXPP_ReturnIntError(PyExc_TypeError,
					"Must be a scene" );
		} else {
			BPyImageObject *bpydata;
			Image *data;
			
			if (!G.sima) 
				return 0;
			
			bpydata = (BPyImageObject *)value;
			data= bpydata->image;
			
			if (!data)
				return EXPP_ReturnIntError(PyExc_RuntimeError,
					"This Scene has been removed" );
			
			if (data != G.sima->image)
				G.sima->image= data;
		}
		return 0;
	
	case ID_TXT:
		if (!BPyText_Check(value)) {
			return EXPP_ReturnIntError(PyExc_TypeError,
					"Must be a text" );
		} else {
			SpaceText *st= curarea->spacedata.first;	
			Text *data = ((BPyTextObject *)value)->text;
			
			if( !data )
				return EXPP_ReturnIntError( PyExc_RuntimeError,
						      "This object isn't linked to a Blender Text Object" );
			if(st->spacetype!=SPACE_TEXT)
				return 0;
			st->text = data;
		}
		return 0;
	}
	
	return EXPP_ReturnIntError( PyExc_TypeError,
			"Only Scene and Image types have the active attribute" );
}

static int LibBlockSeq_setTag(BPyLibBlockSeqObject *self, PyObject *value)
{
	int param = PyObject_IsTrue( value );
	ID *id;
	
	if( param == -1 )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected int argument in range [0,1]" );
	
	id = (ID *)wich_libbase(G.main, self->type)->first;
	
	if (param) {
		for (; id; id = id->next) {
			id->flag |= LIB_DOIT;
		}
	} else {
		for (; id; id = id->next) {
			id->flag &= ~LIB_DOIT;
		}
	}
	
	return 0;	
}

/* used for new and load */
PyObject *BPyData_newByType(int type, PyObject * args, PyObject *kwds)
{
	char *name=NULL, *data_type=NULL;
	
	switch (type) {
	case ID_VF: 
		return BPyFont_Type.tp_new(&BPyFont_Type, args, kwds);
	case ID_SCE: 
		return BPyScene_Type.tp_new(&BPyScene_Type, args, kwds);
	case ID_OB:
		return BPyObject_Type.tp_new(&BPyObject_Type, args, kwds); /* this is a bit odd, assumes active scene */
	case ID_ME:
		return BPyMesh_Type.tp_new(&BPyMesh_Type, args, kwds);
	case ID_CU:
		if( !PyArg_ParseTuple( args, "ss", &name, &data_type ) )
			return EXPP_ReturnPyObjError( PyExc_TypeError,
				"two strings expected as arguments" );
		if(!strcmp( data_type, "Curve" ) )
			return BPyCurve_Type.tp_new(&BPyCurve_Type, args, kwds);
		else if(!strcmp( data_type, "Text3d" ) )
			return BPyCurveText_Type.tp_new(&BPyCurveText_Type, args, kwds);
		/*else if( !strcmp( data_type, "Surf" ) )
			return BPySurfNurb_Type.tp_new(&BPySurfNurb_Type, args, kwds);*/
		else return EXPP_ReturnPyObjError( PyExc_TypeError,
				"Second argument for Curve type incorrect\t\nmust be a string in (Curve or Text - Surf is not supported yet)" );
	case ID_MB:
		return BPyMetaball_Type.tp_new(&BPyMetaball_Type, args, kwds);
	case ID_MA:
		return BPyMaterial_Type.tp_new(&BPyMaterial_Type, args, kwds);
	case ID_TE:
		return BPyTexture_Type.tp_new(&BPyTexture_Type, args, kwds);
	case ID_IM:
		return BPyImage_Type.tp_new(&BPyImage_Type, args, kwds);
	case ID_LT:
		return BPyLattice_Type.tp_new(&BPyLattice_Type, args, kwds);
	case ID_LA:
		return BPyLamp_Type.tp_new(&BPyLamp_Type, args, kwds);
	case ID_CA:
		return BPyCamera_Type.tp_new(&BPyCamera_Type, args, kwds);
	case ID_IP:
		return BPyIpo_Type.tp_new(&BPyIpo_Type, args, kwds);
	case ID_WO:
		return BPyWorld_Type.tp_new(&BPyWorld_Type, args, kwds);
	case ID_TXT:
		return BPyText_Type.tp_new(&BPyText_Type, args, kwds);
	case ID_GR:
		return BPyGroup_Type.tp_new(&BPyGroup_Type, args, kwds);
	case ID_AR:
		return BPyArmature_Type.tp_new(&BPyArmature_Type, args, kwds);
	case ID_AC:
		return BPyAction_Type.tp_new(&BPyAction_Type, args, kwds);
	}
	
	return EXPP_ReturnPyObjError( PyExc_TypeError,
		"Could not find data for this type" );
}

PyObject *LibBlockSeq_new(BPyLibBlockSeqObject *self, PyObject * args, PyObject *kwds)
{
	return BPyData_newByType(self->type, args, kwds);
}


PyObject *LibBlockSeq_unlink(BPyLibBlockSeqObject *self, PyObject * pyobj)
{	
	switch (self->type) {
	case ID_SCE:
		if( BPyScene_Check(pyobj) ) {
			return EXPP_ReturnPyObjError( PyExc_TypeError,
					"expected Scene object" );
		} else {
			BPySceneObject *bpydata;
			Scene *data;
			
			bpydata = (BPySceneObject *)pyobj;
			data = bpydata->scene;
			
			if (!data)
				return EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"This Scene has been removed" );
			
			/* Run the removal code */
			free_libblock( &G.main->scene, data );
			bpydata->scene= NULL;
			
			Py_RETURN_NONE;
		}
	case ID_GR:
		if( BPyGroup_Check(pyobj) ) {
			return EXPP_ReturnPyObjError( PyExc_TypeError,
					"expected Group object" );
		} else {
			BPyGroupObject *bpydata;
			Group *data;
			
			bpydata = (BPyGroupObject *)pyobj;
			data = bpydata->group;
			
			if (!data)
				return EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"This Group has been removed alredy" );
			
			/* Run the removal code */
			free_group(data);
			unlink_group(data);
			data->id.us= 0;
			free_libblock( &G.main->group, data );
			bpydata->group= NULL;
			
			Py_RETURN_NONE;
		}

	case ID_TXT:
		if( BPyText_Check(pyobj) ) {
			return EXPP_ReturnPyObjError( PyExc_TypeError,
					"expected Text object" );
		} else {
			BPyTextObject *bpydata;
			Text *data;
			bpydata = (BPyTextObject *)pyobj;
			data = bpydata->text;
			
			if (!data)
				return EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"This Group has been removed alredy" );
			
			/* Run the removal code */
			BPY_clear_bad_scriptlinks( data );
			free_text_controllers( data );
			unlink_text( data );
			free_libblock( &G.main->text, data );
			bpydata->text = NULL;
			
			Py_RETURN_NONE;
		}
	}
	return EXPP_ReturnPyObjError( PyExc_TypeError,
				      "Only types Scene, Group and Text can unlink" );	
}

static int LibBlockSeq_compare( BPyLibBlockSeqObject * a, BPyLibBlockSeqObject * b )
{
	return ( a->type == b->type) ? 0 : -1;	
}

/*
 * repr function
 * callback functions building meaninful string to representations
 */
static PyObject *LibBlockSeq_repr( BPyLibBlockSeqObject * self )
{
	return PyString_FromFormat( "[library iter \"%s\"]", GenericLib_libname( self->type ) );
}

static PyGetSetDef LibBlockSeq_getset[] = {
	{"active",
	 (getter)LibBlockSeq_getActive, (setter)LibBlockSeq_setActive,
	 "active object",
	 NULL},
	{"tag",
	 (getter)NULL, (setter)LibBlockSeq_setTag,
	 "tag all data in True or False (write only)",
	 NULL},
	{NULL}  /* Sentinel */
};

static struct PyMethodDef BPyLibBlockSeq_methods[] = {
	{"new", (PyCFunction)LibBlockSeq_new, METH_VARARGS | METH_KEYWORDS,
		"(name) - Create a new object in this scene from the obdata given and return a new object"},
	{"unlink", (PyCFunction)LibBlockSeq_unlink, METH_O,
		"unlinks the object from the scene"},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Python BPyLibBlockSeq_Type structure definition:                               */
/*****************************************************************************/
PyTypeObject BPyLibBlockSeq_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender LibBlockSeq",           /* char *tp_name; */
	sizeof( BPyLibBlockSeqObject ),       /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,                       /* tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) LibBlockSeq_compare, /* cmpfunc tp_compare; */
	( reprfunc ) LibBlockSeq_repr,   /* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	NULL,	    /* PySequenceMethods *tp_as_sequence; */
	&LibBlockSeq_as_mapping,                       /* PyMappingMethods *tp_as_mapping; */

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
	( getiterfunc) LibBlockSeq_getIter, /* getiterfunc tp_iter; */
	( iternextfunc ) LibBlockSeq_nextIter, /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	BPyLibBlockSeq_methods,       /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	LibBlockSeq_getset,       /* struct PyGetSetDef *tp_getset; */
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

PyObject *LibBlockSeqType_Init( void )
{
	PyType_Ready( &BPyLibBlockSeq_Type );
	BPyLibBlockSeq_Type.tp_dealloc = (destructor)&PyObject_Del;
	return (PyObject *) &BPyLibBlockSeq_Type;
}


/*-----------------------------BPy module Funcs())----------------------------*/



static char m_data_load_doc[] = "(filename) - Load the given file.\n\
Supported formats:\n\
Blender, DXF, Inventor 1.0 ASCII, VRML 1.0 asc, STL, Videoscape, radiogour.\n\
\n\
Notes:\n\
1 - () - an empty argument loads the default .B.blend file;\n\
2 - if the substring '.B.blend' occurs inside 'filename', the default\n\
.B.blend file is loaded;\n\
3 - If a Blender file is loaded the script ends immediately.\n\
4 - The current data is always preserved as an autosave file, for safety;\n\
5 - This function only works if the script where it's executed is the\n\
only one running at the moment.";

static char m_data_save_doc[] =
	"(filename) - Save data to a file based on the filename's extension.\n\
Supported are: Blender's .blend and the builtin exporters:\n\
VRML 1.0 (.wrl), Videoscape (.obj), DXF (.dxf) and STL (.stl)\n\
(filename) - A filename with one of the supported extensions.\n\
Note 1: 'filename' should not contain the substring \".B.blend\" in it.\n\
Note 2: only .blend raises an error if file wasn't saved.\n\
\tYou can use Blender.sys.exists(filename) to make sure the file was saved\n\
\twhen writing to one of the other formats.";

static char m_data_packAll_doc[] =
"() - Pack all files.\n\
All files will packed into the blend file.";
static char m_data_unpackAll_doc[] =
"(mode) - Unpack files.\n\
All files will be unpacked using specified mode.\n\n\
(mode) - the unpack mode.";

static char m_data_countPackedFiles_doc[] =
"() - Returns the number of packed files.";

/**
 * Blender.Load
 * loads Blender's .blend, DXF, radiogour(?), STL, Videoscape,
 * Inventor 1.0 ASCII, VRML 1.0 asc.
 */
static PyObject *m_data_load( PyObject * self, PyObject * args )
{
	char *fname = NULL;
	int keep_oldfname = 0;
	Script *script = NULL;
	char str[32], name[FILE_MAXDIR];
	int file, is_blend_file = 0;

	if( !PyArg_ParseTuple( args, "|si", &fname, &keep_oldfname ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"expected filename and optional int or nothing as arguments" );

	if( fname ) {
		if( strlen( fname ) > FILE_MAXDIR )	/* G.main->name's max length */
			return EXPP_ReturnPyObjError( PyExc_AttributeError,
						      "filename too long!" );
		else if( !BLI_exists( fname ) )
			return EXPP_ReturnPyObjError( PyExc_AttributeError,
						      "requested file doesn't exist!" );

		if( keep_oldfname )
			BLI_strncpy( name, G.sce, FILE_MAXDIR );
	}

	/* We won't let a new .blend file be loaded if there are still other
	 * scripts running, since loading a new file will close and remove them. */

	if( G.main->script.first != G.main->script.last )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "there are other scripts running at the Scripts win, close them first!" );

	if( fname ) {
		file = open( fname, O_BINARY | O_RDONLY );

		if( file <= 0 ) {
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
						      "cannot open file!" );
		} else {
			read( file, str, 31 );
			close( file );

			if( strncmp( str, "BLEN", 4 ) == 0 )
				is_blend_file = 1;
		}
	} else
		is_blend_file = 1;	/* no arg given means default: .B.blend */

	if( is_blend_file ) {

		int during_slink = during_scriptlink(  );

		/* when loading a .blend file from a scriptlink, the scriptlink pointer
		 * in BPY_do_pyscript becomes invalid during a loop.  Inform it here.
		 * Also do not allow a nested scriptlink (called from inside another)
		 * to load .blend files, to avoid nasty problems. */
		if( during_slink >= 1 ) {
			if( during_slink == 1 )
				disable_where_scriptlink( -1 );
			else {
				return EXPP_ReturnPyObjError
					( PyExc_EnvironmentError,
					  "Blender.Load: cannot load .blend files from a nested scriptlink." );
			}
		}

		/* trick: mark the script so that its script struct won't be freed after
		 * the script is executed (to avoid a double free warning on exit): */
		script = G.main->script.first;
		if( script )
			script->flags |= SCRIPT_GUI;

		BIF_write_autosave(  );	/* for safety let's preserve the current data */
	}

	if( G.obedit )
		exit_editmode(EM_FREEDATA);

	if (G.background) { /* background mode */
		if (is_blend_file)
			BKE_read_file(fname, NULL);
		else {
			return EXPP_ReturnPyObjError(PyExc_AttributeError,
				"only .blend files can be loaded from command line,\n\
	other file types require interactive mode.");
		}
	}
	else { /* interactive mode */
		/* for safety, any filename with .B.blend is considered the default one.
		 * It doesn't seem necessary to compare file attributes (like st_ino and
		 * st_dev, according to the glibc info pages) to find out if the given
		 * filename, that may have been given with a twisted misgiving path, is the
		 * default one for sure.  Taking any .B.blend file as the default is good
		 * enough here.  Note: the default file requires extra clean-up done by
		 * BIF_read_homefile: freeing the user theme data. */
		if( !fname || ( strstr( fname, ".B.blend" ) && is_blend_file ) )
			BIF_read_homefile(0);
		else
			BIF_read_file( fname );

		if( fname && keep_oldfname ) {
			/*BLI_strncpy(G.main->name, name, FILE_MAXDIR); */
			BLI_strncpy( G.sce, name, FILE_MAXDIR );
		}
	}

	Py_RETURN_NONE;
}

static PyObject *m_data_save( PyObject * self, PyObject * args )
{
	char *fname = NULL;
	int overwrite = 0, len = 0;
	char *error = NULL;
	Library *li;

	if( !PyArg_ParseTuple( args, "s|i", &fname, &overwrite ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected filename and optional int (overwrite flag) as arguments" );

	for( li = G.main->library.first; li; li = li->id.next ) {
		if( BLI_streq( li->name, fname ) ) {
			return EXPP_ReturnPyObjError( PyExc_AttributeError,
						      "cannot overwrite used library" );
		}
	}

	/* for safety, any filename with .B.blend is considered the default one
	 * and not accepted here. */
	if( strstr( fname, ".B.blend" ) )
		return EXPP_ReturnPyObjError( PyExc_AttributeError,
					      "filename can't contain the substring \".B.blend\" in it." );

	len = strlen( fname );

	if( len > FILE_MAXFILE )
		return EXPP_ReturnPyObjError( PyExc_AttributeError,
					      "filename is too long!" );
	else if( BLI_exists( fname ) && !overwrite )
		return EXPP_ReturnPyObjError( PyExc_AttributeError,
					      "file already exists and overwrite flag was not given." );

	disable_where_script( 1 );	/* to avoid error popups in the write_* functions */

	if( BLI_testextensie( fname, ".blend" ) ) {		
		if( G.fileflags & G_AUTOPACK )
			packAll(  );
		if( !BLO_write_file( fname, G.fileflags, &error ) ) {
			disable_where_script( 0 );
			return EXPP_ReturnPyObjError( PyExc_SystemError,
						      error );
		}
	} else if( BLI_testextensie( fname, ".dxf" ) )
		write_dxf( fname );
	else if( BLI_testextensie( fname, ".stl" ) )
		write_stl( fname );
	else if( BLI_testextensie( fname, ".wrl" ) )
		write_vrml( fname );
	else if( BLI_testextensie( fname, ".obj" ) )
		write_videoscape( fname );
	else {
		disable_where_script( 0 );
		return EXPP_ReturnPyObjError( PyExc_AttributeError,
					      "unknown file extension." );
	}

	disable_where_script( 0 );

	Py_RETURN_NONE;
}

/*****************************************************************************/
/* Function:		Blender_PackAll		 */
/* Python equivalent:	Blender.PackAll			 */
/*****************************************************************************/
static PyObject *m_data_packAll( PyObject * self)
{
	packAll();
	Py_RETURN_NONE;
}

/*****************************************************************************/
/* Function:		Blender_UnpackAll		 */
/* Python equivalent:	Blender.UnpackAll			 */
/*****************************************************************************/
static PyObject *m_data_unpackAll( PyObject * self, PyObject *args)
{
	int mode;
	PyArg_ParseTuple( args, "i", &mode );
	unpackAll(mode);
	Py_RETURN_NONE;
}
 
/*****************************************************************************/
/* Function:		Blender_CountPackedFiles		 */
/* Python equivalent:	Blender.CountPackedFiles			 */
/*****************************************************************************/
static PyObject *m_data_countPackedFiles( PyObject * self )
{
	int nfiles = countPackedFiles();
	return PyInt_FromLong( nfiles );
}

/*****************************************************************************/
/* Python method structure definition.		 */
/*****************************************************************************/

static struct PyMethodDef m_data_methods[] = {
	{"load", m_data_load, METH_VARARGS, m_data_load_doc},
	{"save", m_data_save, METH_VARARGS, m_data_save_doc},
	{"countPackedFiles", ( PyCFunction ) m_data_countPackedFiles, METH_NOARGS, m_data_countPackedFiles_doc},
	{"pack", ( PyCFunction ) m_data_packAll, METH_NOARGS, m_data_packAll_doc},
	{"unpack", m_data_unpackAll, METH_VARARGS, m_data_unpackAll_doc},
	{NULL, NULL, 0, NULL}
};


/*-----------------------------BPy module Init())-----------------------------*/

PyObject * m_data_init( void )
{
	PyObject *module;
	
	module = Py_InitModule3( "bpy.data", m_data_methods, "The bpy.data submodule" );
	
	/* TODO - figure out how to get bpy.data.filename to work - may need to become a type :/ */
	
	/*
	if( !PyArg_ParseTuple( args, "s", &str ) )
		return EXPP_ReturnPyObjError (PyExc_TypeError,
			"expected string argument");

	else if( strcmp( str, "filename" )==0 ) {
		if ( strstr(G.main->name, ".B.blend") != 0)
			ret = PyString_FromString("");
		else
			ret = PyString_FromString(G.main->name);
	*/
	
	/* Python Data Types */
	PyModule_AddObject( module, "scenes", 	LibBlockSeq_CreatePyObject(NULL, ID_SCE) );
	PyModule_AddObject( module, "objects",	LibBlockSeq_CreatePyObject(NULL, ID_OB) );
	PyModule_AddObject( module, "meshes",	LibBlockSeq_CreatePyObject(NULL, ID_ME) );
	PyModule_AddObject( module, "curves",	LibBlockSeq_CreatePyObject(NULL, ID_CU) );
	PyModule_AddObject( module, "metaballs",LibBlockSeq_CreatePyObject(NULL, ID_MB) );
	PyModule_AddObject( module, "materials",LibBlockSeq_CreatePyObject(NULL, ID_MA) );
	PyModule_AddObject( module, "textures",	LibBlockSeq_CreatePyObject(NULL, ID_TE) );
	PyModule_AddObject( module, "images",	LibBlockSeq_CreatePyObject(NULL, ID_IM) );
	PyModule_AddObject( module, "lattices",	LibBlockSeq_CreatePyObject(NULL, ID_LT) );
	PyModule_AddObject( module, "lamps",	LibBlockSeq_CreatePyObject(NULL, ID_LA) );
	PyModule_AddObject( module, "cameras",	LibBlockSeq_CreatePyObject(NULL, ID_CA) );
	PyModule_AddObject( module, "ipos",		LibBlockSeq_CreatePyObject(NULL, ID_IP) );
	PyModule_AddObject( module, "worlds",	LibBlockSeq_CreatePyObject(NULL, ID_WO) );
	PyModule_AddObject( module, "fonts",	LibBlockSeq_CreatePyObject(NULL, ID_VF) );
	PyModule_AddObject( module, "texts",	LibBlockSeq_CreatePyObject(NULL, ID_TXT) );
	PyModule_AddObject( module, "sounds",	LibBlockSeq_CreatePyObject(NULL, ID_SO) );
	PyModule_AddObject( module, "groups",	LibBlockSeq_CreatePyObject(NULL, ID_GR) );
	PyModule_AddObject( module, "armatures",LibBlockSeq_CreatePyObject(NULL, ID_AR) );
	PyModule_AddObject( module, "actions",	LibBlockSeq_CreatePyObject(NULL, ID_AC) );
	
	/* save/load/pack/unpack filename? */
	
	
	
	return module;
}

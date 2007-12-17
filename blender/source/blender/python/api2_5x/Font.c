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
 * Contributor(s): Joilnen Leite
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "Font.h" /*This must come first*/

#include "DNA_packedFile_types.h"
#include "BKE_packedFile.h"
#include "BKE_global.h"
#include "BKE_font.h"
#include "BKE_library.h" /* for rename_id() */
#include "BLI_blenlib.h"
#include "gen_utils.h"
#include "gen_library.h"

#include "BKE_main.h" /* so we can access G.main->vfont.first */
#include "DNA_space_types.h" /* for FILE_MAXDIR only */

/*------------------------Python API Doc strings for the Font module--------*/
char M_Font_doc[] = "The Blender Font module\n\n\
This module provides control over **Font Data** objects in Blender.\n\n\
Example::\n\n\
	from Blender import Text3d.Font\n\
	l = Text3d.Font.Load('/usr/share/fonts/verdata.ttf')\n";

/*--------------- Python BPyFontObject methods declarations:-------------------*/
static PyObject *Font_pack( BPyFontObject * self );
static PyObject *Font_unpack( BPyFontObject * self, PyObject * args );

/*--------------- Python BPyFontObject methods table:--------------------------*/
static PyMethodDef BPyFont_methods[] = {
	{"pack", ( PyCFunction ) Font_pack, METH_NOARGS,
	 "() - pack this Font"},
	{"unpack", ( PyCFunction ) Font_unpack, METH_VARARGS,
	 "(mode) - unpack this packed font"},
	{NULL, NULL, 0, NULL}
};

/*--------------- Python BPyFontObject getsetattr funcs ---------------------*/

/*--------------- BPyFontObject.filename-------------------------------------*/
static PyObject *Font_getFilename( BPyFontObject * self )
{
	if( self->font )
		return PyString_FromString( self->font->name );
	
	Py_RETURN_NONE;
}

static int Font_setFilename( BPyFontObject * self, PyObject * value )
{
	char *name = NULL;

	/* max len is FILE_MAXDIR = 160 chars like done in DNA_image_types.h */
	
	name = PyString_AsString ( value );
	if( !name )
		return EXPP_ReturnIntError( PyExc_TypeError,
					      "expected string argument" );

	PyOS_snprintf( self->font->name, FILE_MAXDIR * sizeof( char ), "%s",
		       name );

	return 0;
}

/*--------------- BPyFontObject.pack()---------------------------------*/
static PyObject *Font_pack( BPyFontObject * self ) 
{
	if( !self->font->packedfile ) 
		self->font->packedfile = newPackedFile(self->font->name);
	Py_RETURN_NONE;
}

/*--------------- BPyFontObject.unpack()---------------------------------*/
static PyObject *Font_unpack( BPyFontObject * self, PyObject * args ) 
{
	int mode= 0;
	VFont *font= self->font;
	
	if( !PyArg_ParseTuple( args, "i", &mode ) )
		return ( EXPP_ReturnPyObjError
			 ( PyExc_AttributeError,
			   "expected int argument from Blender.UnpackModes" ) );
	
	if (font->packedfile)
		if (unpackVFont(font, mode) == RET_ERROR)
                return EXPP_ReturnPyObjError( PyExc_RuntimeError,
                                "error unpacking font" );

	Py_RETURN_NONE;
}

/*--------------- BPyFontObject.packed---------------------------------*/
static PyObject *Font_getPacked( BPyFontObject * self ) 
{
	if (G.fileflags & G_AUTOPACK)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef BPyFont_getset[] = {
	{"filename",
	 (getter)Font_getFilename, (setter)Font_setFilename,
	 "Font filepath",
	 NULL},
	{"packed",
	 (getter)Font_getPacked, (setter)NULL,
	 "Packed status",
	 NULL},
	{NULL}  /* Sentinel */
};

/* this types constructor - very similar to Sound.c */
static PyObject *Font_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	static char *kwlist[] = {"name", "filename", NULL};
	char *name=NULL, *filename=NULL;
	ID *id;
	
	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ss", kwlist, &name, &filename))
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"(name, filename) - expected 2 string arguments" );
	
	if (!filename)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"filename argument is compulsery for font datatypes" );
	
	if (strlen(filename) > FILE_MAXDIR + FILE_MAXFILE - 1)
		return ( EXPP_ReturnPyObjError( PyExc_IOError,
					"filename too long" ) );
	
	id = (ID *) load_vfont(filename);
	
	if (!id)
		return EXPP_ReturnPyObjError( PyExc_IOError,
			"Error, could not load font data" );

	if (name)
		rename_id( id, name );
	
	return Font_CreatePyObject((VFont *)id);
}

/*****************************************************************************/
/* Python TypeFont structure definition:                                     */
/*****************************************************************************/
PyTypeObject BPyFont_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Font",             /* char *tp_name; */
	sizeof( BPyFontObject ),         /* int tp_basicsize; */
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
	BPyFont_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyFont_getset,				/* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,		/* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc ) Font_new,		/* newfunc tp_new; */
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

/*--------------- Font_CreatePyObject---------------------------------*/
PyObject *Font_CreatePyObject( struct VFont * font )
{
	BPyFontObject *blen_font;

	if (!font)
		Py_RETURN_NONE;
	
	blen_font = ( BPyFontObject * ) PyObject_NEW( BPyFontObject, &BPyFont_Type );
	blen_font->font = font;
	return ( ( PyObject * ) blen_font );
}

PyObject *FontType_Init( void  )
{
	PyType_Ready( &BPyFont_Type ) ;
	BPyFont_Type.tp_dealloc = (destructor)&PyObject_Del;
	return (PyObject *) &BPyFont_Type ;
}

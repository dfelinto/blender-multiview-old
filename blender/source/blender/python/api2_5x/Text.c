/* 
 * $Id: Text.c 11446 2007-07-31 16:11:32Z campbellbarton $
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
 * Contributor(s): Willian P. Germano
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include "Text.h" /*This must come first*/

#include "BKE_library.h"
#include "BKE_sca.h"
#include "BKE_global.h"
#include "BKE_main.h"
#include "BIF_drawtext.h"
#include "BKE_text.h"
#include "BLI_blenlib.h"
#include "DNA_space_types.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "../BPY_extern.h"

#define EXPP_TEXT_MODE_FOLLOW TXT_FOLLOW

/*****************************************************************************/
/* Python BPyTextObject methods declarations:                                     */
/*****************************************************************************/
static PyObject *Text_getFilename( BPyTextObject * self );
static PyObject *Text_getNLines( BPyTextObject * self );
static PyObject *Text_clear( BPyTextObject * self );
static PyObject *Text_write( BPyTextObject * self, PyObject * args );
static PyObject *Text_set( BPyTextObject * self, PyObject * args );
static PyObject *Text_asLines( BPyTextObject * self );

/*****************************************************************************/
/* Python BPyTextObject methods table:                                            */
/*****************************************************************************/
static PyMethodDef BPyText_methods[] = {
	/* name, method, flags, doc */
	{"clear", ( PyCFunction ) Text_clear, METH_NOARGS,
	 "() - Clear Text buffer"},
	{"write", ( PyCFunction ) Text_write, METH_VARARGS,
	 "(line) - Append string 'str' to Text buffer"},
	{"set", ( PyCFunction ) Text_set, METH_VARARGS,
	 "(name, val) - Set attribute 'name' to value 'val'"},
	{"asLines", ( PyCFunction ) Text_asLines, METH_NOARGS,
	 "() - Return text buffer as a list of lines"},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* Function:              Text_CreatePyObject                                */
/*****************************************************************************/
PyObject *Text_CreatePyObject( Text * txt )
{
	BPyTextObject *pytxt;
	
	/* REUSE EXISTING DATA FROM HASH */
	pytxt = BPY_idhash_get((ID *)txt);
	if (pytxt)
		return EXPP_incr_ret((PyObject *)pytxt);
	
	pytxt = ( BPyTextObject * ) PyObject_NEW( BPyTextObject, &BPyText_Type );

	if( !pytxt )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create BPyTextObject PyObject" );

	pytxt->text = txt;

	BPY_idhash_add((void *)pytxt);
	
	return ( PyObject * ) pytxt;
}

/*****************************************************************************/
/* Python BPyTextObject methods:                                                  */
/*****************************************************************************/
static PyObject *Text_getFilename( BPyTextObject * self )
{
	PyObject *attr;
	char *name = self->text->name;

	if( name )
		attr = PyString_FromString( self->text->name );
	else
		attr = Py_None;

	if( attr )
		return attr;

	return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				      "couldn't get Text.filename attribute" );
}

static PyObject *Text_getNLines( BPyTextObject * self )
{				/* text->nlines isn't updated in Blender (?) */
	int nlines = 0;
	TextLine *line;
	PyObject *attr;

	line = self->text->lines.first;

	while( line ) {		/* so we have to count them ourselves */
		line = line->next;
		nlines++;
	}

	self->text->nlines = nlines;	/* and update Blender, too (should we?) */

	attr = PyInt_FromLong( nlines );

	if( attr )
		return attr;

	return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				      "couldn't get Text.nlines attribute" );
}

static PyObject *Text_clear( BPyTextObject * self)
{
	int oldstate;

	if( !self->text )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "This object isn't linked to a Blender Text Object" );

	oldstate = txt_get_undostate(  );
	txt_set_undostate( 1 );
	txt_sel_all( self->text );
	txt_cut_sel( self->text );
	txt_set_undostate( oldstate );

	Py_RETURN_NONE;
}

static PyObject *Text_set( BPyTextObject * self, PyObject * args )
{
	int ival;
	char *attr;

	if( !PyArg_ParseTuple( args, "si", &attr, &ival ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected a string and an int as arguments" );

	if( strcmp( "follow_cursor", attr ) == 0 ) {
		if( ival )
			self->text->flags |= EXPP_TEXT_MODE_FOLLOW;
		else
			self->text->flags &= EXPP_TEXT_MODE_FOLLOW;
	}

	Py_RETURN_NONE;
}

static PyObject *Text_write( BPyTextObject * self, PyObject * args )
{
	char *str;
	int oldstate;

	if( !self->text )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "This object isn't linked to a Blender Text Object" );

	if( !PyArg_ParseTuple( args, "s", &str ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected string argument" );

	oldstate = txt_get_undostate(  );
	txt_insert_buf( self->text, str );
	txt_move_eof( self->text, 0 );
	txt_set_undostate( oldstate );

	Py_RETURN_NONE;
}

static PyObject *Text_asLines( BPyTextObject * self )
{
	TextLine *line;
	PyObject *list, *tmpstr;

	if( !self->text )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "This object isn't linked to a Blender Text Object" );

	line = self->text->lines.first;
	list = PyList_New( 0 );

	if( !list )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create PyList" );

	while( line ) {
		tmpstr = PyString_FromString( line->line );
		PyList_Append( list, tmpstr );
		Py_DECREF(tmpstr);
		line = line->next;
	}

	return list;
}

/*****************************************************************************/
/* Python attributes get/set functions:                                      */
/*****************************************************************************/
static PyObject *Text_getMode(BPyTextObject * self)
{
	return PyInt_FromLong( self->text->flags );
}

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef BPyText_getset[] = {
	{"filename", (getter)Text_getFilename, (setter)NULL,
	 "text filename", NULL},
	{"mode", (getter)Text_getMode, (setter)NULL,
	 "text mode flag", NULL},
	{"nlines", (getter)Text_getNLines, (setter)NULL,
	 "number of lines", NULL},
	{NULL}  /* Sentinel */
};

/*
 * Text dealloc - free from memory and free from text pool
 */
static void Text_dealloc( BPyTextObject * self )
{
	ID *id = (ID *)(self->text);
	if (id) {
		BPY_idhash_remove(id);
	}
	
	PyObject_DEL( self );
}

/* this types constructor */
static PyObject *Text_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	static char *kwlist[] = {"name", "filename", NULL};
	char *name="Text", *filename=NULL;
	ID *id;
	
	if(
		!PyArg_ParseTupleAndKeywords(args, kwds, "|ss", kwlist, &name, &filename) &&
		!PyArg_ParseTuple( args, "|s", &name )
	)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"(name) - name must be a string argument for a new text or (name, filename) - for loading" );
	
	PyErr_Clear(); /* one of the above would set an error */ 
	
	if (filename) {/* Load Text*/	
		id = (ID *)add_text( filename );
		if (!id)
			return ( EXPP_ReturnPyObjError( PyExc_IOError,
					"couldn't load the text" ) );
		
		if (name)
			rename_id( id, name );
	
	} else { /* New text */
		id = (ID *)add_empty_text( name );
		if( !id )
			return ( EXPP_ReturnPyObjError( PyExc_MemoryError,
				"couldn't create new text data" ) );
	}
	
	id->us = 1;
	return Text_CreatePyObject((Text *)id);
}

/*****************************************************************************/
/* Python BPyText_Type structure definition:                                    */
/*****************************************************************************/
PyTypeObject BPyText_Type = {
	PyObject_HEAD_INIT( NULL ) 
	0,	/* ob_size */
	"Blender Text",		/* tp_name */
	sizeof( BPyTextObject ),	/* tp_basicsize */
	0,			/* tp_itemsize */
	/* methods */
	( destructor ) Text_dealloc,	/* tp_dealloc */
	NULL,			/* tp_print */
	NULL,	/* tp_getattr */
	NULL,	/* tp_setattr */
	NULL,	/* tp_compare */
	NULL,	/* tp_repr */

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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,         /* long tp_flags; */

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
	BPyText_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyText_getset,				/* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,		/* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc ) Text_new,		/* newfunc tp_new; */
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
/* Function:              Text_Init                                          */
/*****************************************************************************/
PyObject *TextType_Init( void )
{
	PyType_Ready( &BPyText_Type );
	return (PyObject *) &BPyText_Type;
}

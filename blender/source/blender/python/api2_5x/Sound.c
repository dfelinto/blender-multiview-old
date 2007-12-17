/* 
 * $Id: Sound.c 11446 2007-07-31 16:11:32Z campbellbarton $
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
 * Contributor(s): Chris Keith
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include "Sound.h" /*This must come first*/

#include "BKE_utildefines.h"
#include "BKE_global.h"
#include "BKE_main.h"
#include "BLI_blenlib.h"
#include "BKE_sound.h"
#include "BKE_library.h"
#include "BIF_editsound.h"
#include "BKE_packedFile.h"
#include "mydevice.h"		/* redraw defines */
#include "gen_utils.h"
#include "gen_library.h"
#include "DNA_space_types.h" /* for FILE_MAXDIR only */

/*****************************************************************************/
/* Python BPySoundObject methods declarations:				*/
/*****************************************************************************/
static PyObject *Sound_getFilename( BPySoundObject * self );
static int 		 Sound_setFilename( BPySoundObject * self, PyObject * args );
static PyObject *Sound_play( BPySoundObject * self );
static PyObject *Sound_unpack( BPySoundObject * self, PyObject * value);
static PyObject *Sound_pack( BPySoundObject * self );

/*****************************************************************************/
/* Python BPySoundObject methods table:				         */
/*****************************************************************************/
static PyMethodDef BPySound_methods[] = {
	/* name, method, flags, doc */

	{"play", ( PyCFunction ) Sound_play, METH_NOARGS,
				 "() - play this sound"},
	{"unpack", ( PyCFunction ) Sound_unpack, METH_O,
		         "(int) - Unpack sound. Uses one of the values defined in Blender.UnpackModes."},
	{"pack", ( PyCFunction ) Sound_pack, METH_NOARGS,
		         "() Pack the sound"},
/*
	{"reload", ( PyCFunction ) Sound_setCurrent, METH_NOARGS,
	 "() - reload this Sound object's sample.\n\
    This is only useful if the original sound file has changed."},
*/
	{NULL, NULL, 0, NULL}
};

/* NOTE: these were copied and modified from image.h.  To Be Done TBD:
 * macro-ize them, or C++ templates eventually?
 */

/************************/
/*** The Sound PyType ***/
/************************/


/*****************************************************************************/
/* Function:	Sound_CreatePyObject					*/
/* Description: This function will create a new BPySoundObject from an existing  */
/*		Blender Sound structure.				*/
/*****************************************************************************/
PyObject *Sound_CreatePyObject( bSound * snd )
{
	BPySoundObject *py_snd;
	
	if (!snd)
		Py_RETURN_NONE;
	
	py_snd = ( BPySoundObject * ) PyObject_NEW( BPySoundObject, &BPySound_Type );

	if( !py_snd )
		return EXPP_ReturnPyObjError( PyExc_MemoryError,
					      "couldn't create BPySoundObject object" );

	py_snd->sound = snd;
	return ( PyObject * ) py_snd;
}

/*****************************************************************************/
/* Python BPySoundObject methods:	*/
/*****************************************************************************/

static PyObject *Sound_getFilename( BPySoundObject * self )
{
	return PyString_FromString( self->sound->name );
}

static PyObject *Sound_getPacked( BPySoundObject * self )
{
	if (!sound_sample_is_null(self->sound))	{
		bSample *sample = sound_find_sample(self->sound);
		if (sample->packedfile)
			Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

static PyObject *Sound_getVolume( BPySoundObject * self )
{
	return PyFloat_FromDouble( (double)self->sound->volume );
}

static int Sound_setVolume( BPySoundObject * self, PyObject * value )
{
	float f = (float)PyFloat_AsDouble( value );
	if ( f == -1 && PyErr_Occurred())
		return -1;
	self->sound->volume = f;
	CLAMP(self->sound->panning, 0.0f, 1.0f);
	return 0;
}

static PyObject *Sound_getPan( BPySoundObject * self )
{
	return PyFloat_FromDouble( (double)self->sound->volume );
}

static int Sound_setPan( BPySoundObject * self, PyObject * value )
{
	float f = (float)PyFloat_AsDouble( value );
	if ( f == -1 && PyErr_Occurred())
		return -1;
	self->sound->panning = f;
	CLAMP(self->sound->panning, -1.0f, 1.0f);
	return 0;
}

static PyObject *Sound_getAttenuation( BPySoundObject * self )
{
	return PyFloat_FromDouble( (double)self->sound->attenuation );
}

static int Sound_setAttenuation( BPySoundObject * self, PyObject * value )
{
	float f = (float)PyFloat_AsDouble( value );
	if ( f == -1 && PyErr_Occurred())
		return -1;
	self->sound->attenuation = f;
	CLAMP(self->sound->attenuation, 0.0f, 5.0f);
	return 0;
}

static PyObject *Sound_getPitch( BPySoundObject * self )
{
	return PyFloat_FromDouble( (double)self->sound->pitch );
}

static int Sound_setPitch( BPySoundObject * self, PyObject * value )
{
	float f = (float)PyFloat_AsDouble( value );
	if ( f == -1 && PyErr_Occurred())
		return -1;
	self->sound->attenuation = f;
	CLAMP(self->sound->pitch, -12.0f, 12.0f);
	return 0;
}

static int Sound_setFilename( BPySoundObject * self, PyObject * value )
{
	char *name = PyString_AsString(value);

	/* max len is FILE_MAXDIR = 160 chars like in DNA_image_types.h */
	if (!name || strlen(name) > FILE_MAXDIR)
		return ( EXPP_ReturnIntError( PyExc_ValueError,
						"string argument is limited to 160 chars at most" ) );
	strcpy( self->sound->name, name );
	return 0;
}

static PyObject *Sound_play( BPySoundObject * self )
{
	sound_play_sound( self->sound );
	Py_RETURN_NONE;
}

/* unpack sound */

static PyObject *Sound_unpack( BPySoundObject * self, PyObject * value )
{
	bSound *sound = self->sound;
	int mode = (int)PyInt_AsLong(value);
	
	if( mode== -1 )
			return EXPP_ReturnPyObjError( PyExc_TypeError,
							"expected an integer from Blender.UnpackModes" );
	

	if (!sound_sample_is_null(sound)) {
	    bSample *sample = sound_find_sample(sound);
		if (sample->packedfile) {
			if (unpackSample(sample, mode) == RET_ERROR)
					return EXPP_ReturnPyObjError( PyExc_RuntimeError,
									"error unpacking sound");
		}
	} else {
		return EXPP_ReturnPyObjError( PyExc_RuntimeError, "sound has no samples" );
	}
	Py_RETURN_NONE;
}

/* pack sound */

static PyObject *Sound_pack( BPySoundObject * self )
{
	bSound *sound = self->sound;
	if (!sound_sample_is_null(sound))
	{
		bSample *sample = sound_find_sample(sound);
		if (sample->packedfile )
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					"sound alredy packed" );
		sound_set_packedfile(sample, newPackedFile(sample->name));
	}
	else
	{
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"sound has no samples" );
	}
	Py_RETURN_NONE;
}

/*
static PyObject *Sound_reload( BPySoundObject * self)
{
	sound_free_sample();

	if (sound->snd_sound) {
		SND_RemoveSound(ghSoundScene, sound->snd_sound);
		sound->snd_sound = NULL;
	}

	Py_RETURN_NONE;
}
*/

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef BPySound_getset[] = {
	{"filename", (getter)Sound_getFilename, (setter)Sound_setFilename,
	 "text filename", NULL},
	{"packed", (getter)Sound_getPacked, (setter)NULL,
	 "text filename", NULL},
	{"pan", (getter)Sound_getPan, (setter)Sound_setPan,
	 "", NULL},
	{"pitch", (getter)Sound_getPitch, (setter)Sound_setPitch,
	 "", NULL},
	{"attenuation", (getter)Sound_getAttenuation, (setter)Sound_setAttenuation,
	 "", NULL},
	{"volume", (getter)Sound_getVolume, (setter)Sound_setVolume,
	 "", NULL},
	{NULL}  /* Sentinel */
};

/* this types constructor - very similar to Font.c */
static PyObject *Sound_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
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
	
	id = (ID *) sound_new_sound(filename);
	
	if (!id)
		return EXPP_ReturnPyObjError( PyExc_IOError,
			"Error, could not load sound data" );

	if (name)
		rename_id( id, name );
	
	return Sound_CreatePyObject((bSound *)id);
}

/*****************************************************************************/
/* Python BPySound_Type structure definition:				*/
/*****************************************************************************/
PyTypeObject BPySound_Type = {
	PyObject_HEAD_INIT( NULL )
	0,		/* ob_size */
	"Blender Sound",	/* tp_name */
	sizeof( BPySoundObject ),	/* tp_basicsize */
	0,			/* tp_itemsize */
	/* methods */
	NULL,	/* tp_dealloc */
	0,		/* tp_print */
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
	BPySound_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPySound_getset,         /* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,                       /* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc ) Sound_new,		/* newfunc tp_new; */
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

PyObject *SoundType_Init( void )
{
	PyType_Ready( &BPySound_Type );
	return (PyObject *) &BPySound_Type;
}

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

#include "sceneSequence.h" /* This must come first */

#include "MEM_guardedalloc.h"

#include "DNA_sequence_types.h"
#include "DNA_scene_types.h" /* for Base */

#include "BKE_mesh.h"
#include "BKE_library.h"
#include "BKE_global.h"
#include "BKE_main.h"
#include "BKE_scene.h"

#include "BIF_editseq.h" /* get_last_seq */
#include "BLI_blenlib.h"
#include "BSE_sequence.h"
#include "Ipo.h"
#include "blendef.h"  /* CLAMP */
#include "BKE_utildefines.h"
#include "Scene.h"
#include "Sound.h"
#include "gen_utils.h"

enum seq_consts {
	EXPP_SEQ_ATTR_TYPE = 0,
	EXPP_SEQ_ATTR_CHAN,
	EXPP_SEQ_ATTR_LENGTH,
	EXPP_SEQ_ATTR_START,
	EXPP_SEQ_ATTR_STARTOFS,
	EXPP_SEQ_ATTR_ENDOFS,
	EXPP_SEQ_ATTR_STARTSTILL,
	EXPP_SEQ_ATTR_ENDSTILL
};


/*****************************************************************************/
/* Python API function prototypes for the Blender module.		 */
/*****************************************************************************/
/*PyObject *M_Sequence_Get( PyObject * self, PyObject * args );*/

/*****************************************************************************/
/* Python method structure definition for Blender.Object module:	 */
/*****************************************************************************/
/*struct PyMethodDef M_Sequence_methods[] = {
	{"Get", ( PyCFunction ) M_Sequence_Get, METH_VARARGS,
"(name) - return the sequence with the name 'name',\
returns None if notfound.\nIf 'name' is not specified, it returns a list of all sequences."},
	{NULL, NULL, 0, NULL}
};*/

/*****************************************************************************/
/* Python V24_BPy_Sequence methods table:					   */
/*****************************************************************************/
static PyObject *V24_Sequence_copy( V24_BPy_Sequence * self );
static PyObject *V24_Sequence_new( V24_BPy_Sequence * self, PyObject * args );
static PyObject *V24_Sequence_remove( V24_BPy_Sequence * self, PyObject * args );

static PyObject *V24_SceneSeq_new( V24_BPy_SceneSeq * self, PyObject * args );
static PyObject *V24_SceneSeq_remove( V24_BPy_SceneSeq * self, PyObject * args );
static void intern_pos_update(Sequence * seq); 

static PyMethodDef V24_BPy_Sequence_methods[] = {
	/* name, method, flags, doc */
	{"new", ( PyCFunction ) V24_Sequence_new, METH_VARARGS,
	 "(data) - Return a new sequence."},
	{"remove", ( PyCFunction ) V24_Sequence_remove, METH_VARARGS,
	 "(data) - Remove a strip."},
	{"__copy__", ( PyCFunction ) V24_Sequence_copy, METH_NOARGS,
	 "() - Return a copy of the sequence containing the same objects."},
	{"copy", ( PyCFunction ) V24_Sequence_copy, METH_NOARGS,
	 "() - Return a copy of the sequence containing the same objects."},
	{NULL, NULL, 0, NULL}
};

static PyMethodDef V24_BPy_SceneSeq_methods[] = {
	/* name, method, flags, doc */
	{"new", ( PyCFunction ) V24_SceneSeq_new, METH_VARARGS,
	 "(data) - Return a new sequence."},
	{"remove", ( PyCFunction ) V24_SceneSeq_remove, METH_VARARGS,
	 "(data) - Remove a strip."},
	{NULL, NULL, 0, NULL}
};

/* use to add a sequence to a scene or its listbase */
static PyObject *V24_NewSeq_internal(ListBase *seqbase, PyObject * args, Scene *sce)
{
	PyObject *py_data = NULL;
	
	Sequence *seq;
	int a;
	Strip *strip;
	StripElem *se;
	int start, machine;
	
	if( !PyArg_ParseTuple( args, "Oii", &py_data, &start, &machine ) )
		return V24_EXPP_ReturnPyObjError( PyExc_ValueError,
			"expect sequence data then 2 ints - (seqdata, start, track)" );
	
	seq = alloc_sequence(seqbase, start, machine); /* warning, this sets last */
	
	if (PyTuple_Check(py_data)) {
		/* Image */
		PyObject *list;
		char *name;
		
		if (!PyArg_ParseTuple( py_data, "sO!", &name, &PyList_Type, &list)) {
			BLI_remlink(seqbase, seq);
			MEM_freeN(seq);
			
			return V24_EXPP_ReturnPyObjError( PyExc_ValueError,
				"images data needs to be a tuple of a string and a list of images - (path, [filenames...])" );
		}
		
		seq->type= SEQ_IMAGE;
		
		seq->len = PyList_Size( list );
		
		
		/* strip and stripdata */
		seq->strip= strip= MEM_callocN(sizeof(Strip), "strip");
		strip->len= seq->len;
		strip->us= 1;
		strncpy(strip->dir, name, FILE_MAXDIR-1);
		strip->stripdata= se= MEM_callocN(seq->len*sizeof(StripElem), "stripelem");

		for(a=0; a<seq->len; a++) {
			name = PyString_AsString(PyList_GetItem( list, a ));
			strncpy(se->name, name, FILE_MAXFILE-1);
			se++;
		}		
		
	} else if (V24_BPy_Sound_Check(py_data)) {
		/* sound */
		int totframe;
		bSound *sound = (( V24_BPy_Sound * )py_data)->sound;
		
		
		seq->type= SEQ_RAM_SOUND;
		seq->sound = sound;
		
		totframe= (int) ( ((float)(sound->streamlen-1)/( (float)sce->audio.mixrate*4.0 ))* (float)sce->r.frs_sec / sce->r.frs_sec_base);
		
		sound->flags |= SOUND_FLAGS_SEQUENCE;
		
		
		/* strip and stripdata */
		seq->strip= strip= MEM_callocN(sizeof(Strip), "strip");
		strip->len= totframe;
		strip->us= 1;
		strncpy(strip->dir, sound->name, FILE_MAXDIR-1);
		strip->stripdata= se= MEM_callocN(sizeof(StripElem), "stripelem");

		/* name sound in first strip */
		strncpy(se->name, sound->name, FILE_MAXFILE-1);
		
	} else if (V24_BPy_Scene_Check(py_data)) {
		/* scene */
		Scene *sce = ((V24_BPy_Scene *)py_data)->scene;
		
		seq->type= SEQ_SCENE;
		seq->scene= sce;
		
		/*seq->sfra= sce->r.sfra;*/
		seq->len= sce->r.efra - sce->r.sfra + 1;

		seq->strip= strip= MEM_callocN(sizeof(Strip), "strip");
		strncpy(seq->name + 2, sce->id.name + 2, 
			sizeof(seq->name) - 2);
		strip->len= seq->len;
		strip->us= 1;
	} else {
		/* movie, pydata is a path to a movie file */
		char *name = PyString_AsString ( py_data );
		if (!name) {
			/* only free these 2 because other stuff isnt set */
			BLI_remlink(seqbase, seq);
			MEM_freeN(seq);
			
			return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
				"expects a string for chan/bone name and an int for the frame where to put the new key" );
		}
		
		seq->type= SEQ_MOVIE;
	}
	strncpy(seq->name+2, "Untitled", 21);
	intern_pos_update(seq);
	return V24_Sequence_CreatePyObject(seq, NULL, sce);
}

static PyObject *V24_Sequence_new( V24_BPy_Sequence * self, PyObject * args )
{
	return V24_NewSeq_internal(&self->seq->seqbase, args, self->scene);
}

static PyObject *V24_SceneSeq_new( V24_BPy_SceneSeq * self, PyObject * args )
{
	return V24_NewSeq_internal( &((Editing *)self->scene->ed)->seqbase, args, self->scene);
}

static void del_seq__internal(Sequence *seq)
{
	if(seq->ipo) seq->ipo->id.us--;
	
	if(seq->type==SEQ_RAM_SOUND && seq->sound) 
		seq->sound->id.us--;
	free_sequence(seq);
}

static void recurs_del_seq(ListBase *lb)
{
	Sequence *seq, *seqn;

	seq= lb->first;
	while(seq) {
		seqn= seq->next;
		BLI_remlink(lb, seq);
		if(seq->type==SEQ_META) recurs_del_seq(&seq->seqbase);
		del_seq__internal(seq);
		seq= seqn;
	}
}

static PyObject *V24_RemoveSeq_internal(ListBase *seqbase, PyObject * args, Scene *sce)
{
	V24_BPy_Sequence *bpy_seq = NULL;
	
	if( !PyArg_ParseTuple( args, "O!", &V24_Sequence_Type, &bpy_seq ) )
		return V24_EXPP_ReturnPyObjError( PyExc_ValueError,
			"expects a sequence object" );
	
	/* quick way to tell if we dont have the seq */
	if (sce != bpy_seq->scene)
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
			"Sequence does not exist here, cannot remove" );
	
	recurs_del_seq(&bpy_seq->seq->seqbase);
	del_seq__internal(bpy_seq->seq);
	clear_last_seq(); /* just incase */
	Py_RETURN_NONE;
}

static PyObject *V24_Sequence_remove( V24_BPy_Sequence * self, PyObject * args )
{
	return V24_RemoveSeq_internal(&self->seq->seqbase, args, self->scene);
}

static PyObject *V24_SceneSeq_remove( V24_BPy_SceneSeq * self, PyObject * args )
{
	return V24_RemoveSeq_internal( &((Editing *)self->scene->ed)->seqbase, args, self->scene);
}


static PyObject *V24_Sequence_copy( V24_BPy_Sequence * self )
{
	printf("Sequence Copy not implimented yet!\n");
	Py_RETURN_NONE;
}

/*****************************************************************************/
/* PythonTypeObject callback function prototypes			 */
/*****************************************************************************/
static PyObject *V24_Sequence_repr( V24_BPy_Sequence * obj );
static PyObject *V24_SceneSeq_repr( V24_BPy_SceneSeq * obj );
static int V24_Sequence_compare( V24_BPy_Sequence * a, V24_BPy_Sequence * b );
static int V24_SceneSeq_compare( V24_BPy_SceneSeq * a, V24_BPy_SceneSeq * b );

/*****************************************************************************/
/* Python V24_BPy_Sequence methods:                                                  */
/*****************************************************************************/


static PyObject *V24_Sequence_getIter( V24_BPy_Sequence * self )
{
	Sequence *iter = self->seq->seqbase.first;
	
	if (!self->iter) {
		self->iter = iter;
		return V24_EXPP_incr_ret ( (PyObject *) self );
	} else {
		return V24_Sequence_CreatePyObject(self->seq, iter, self->scene);
	}
}

static PyObject *V24_SceneSeq_getIter( V24_BPy_SceneSeq * self )
{
	Sequence *iter = ((Editing *)self->scene->ed)->seqbase.first;
	
	if (!self->iter) {
		self->iter = iter;
		return V24_EXPP_incr_ret ( (PyObject *) self );
	} else {
		return V24_SceneSeq_CreatePyObject(self->scene, iter);
	}
}


/*
 * Return next Seq
 */
static PyObject *V24_Sequence_nextIter( V24_BPy_Sequence * self )
{
	PyObject *object;
	if( !(self->iter) ) {
		self->iter = NULL; /* so we can add objects again */
		return V24_EXPP_ReturnPyObjError( PyExc_StopIteration,
				"iterator at end" );
	}
	
	object= V24_Sequence_CreatePyObject( self->iter, NULL, self->scene ); 
	self->iter= self->iter->next;
	return object;
}


/*
 * Return next Seq
 */
static PyObject *V24_SceneSeq_nextIter( V24_BPy_Sequence * self )
{
	PyObject *object;
	if( !(self->iter) ) {
		self->iter = NULL; /* so we can add objects again */
		return V24_EXPP_ReturnPyObjError( PyExc_StopIteration,
				"iterator at end" );
	}
	
	object= V24_Sequence_CreatePyObject( self->iter, NULL, self->scene );
	self->iter= self->iter->next;
	return object;
}



static PyObject *V24_Sequence_getName( V24_BPy_Sequence * self )
{
	return PyString_FromString( self->seq->name+2 );
}

static int V24_Sequence_setName( V24_BPy_Sequence * self, PyObject * value )
{
	char *name = NULL;
	
	name = PyString_AsString ( value );
	if( !name )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					      "expected string argument" );

	strncpy(self->seq->name+2, name, 21);
	return 0;
}


static PyObject *V24_Sequence_getSound( V24_BPy_Sequence * self )
{
	if (self->seq->type == SEQ_RAM_SOUND && self->seq->sound)
		return V24_Sound_CreatePyObject(self->seq->sound);
	Py_RETURN_NONE;
}

static PyObject *V24_Sequence_getIpo( V24_BPy_Sequence * self )
{
	struct Ipo *ipo;
	
	ipo = self->seq->ipo;

	if( ipo )
		return V24_Ipo_CreatePyObject( ipo );
	Py_RETURN_NONE;
}


static PyObject *V24_SceneSeq_getActive( V24_BPy_SceneSeq * self )
{
	Sequence *last_seq = NULL, *seq;
	Editing *ed = self->scene->ed;

	if (!ed)
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "scene has no sequence data to edit" );
	
	seq = ed->seqbasep->first;
	
	while (seq) {
		if (seq->flag & SELECT)
			last_seq = seq;
		
		seq = seq->next;
	}
	if (last_seq)
		return V24_Sequence_CreatePyObject(last_seq, NULL, self->scene );
	
	Py_RETURN_NONE;
}

static PyObject *V24_SceneSeq_getMetaStrip( V24_BPy_SceneSeq * self )
{
	Sequence *seq = NULL;
	Editing *ed = self->scene->ed;
	MetaStack *ms;
	if (!ed)
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError,
					      "scene has no sequence data to edit" );
	
	ms = ed->metastack.last;
	if (!ms)
		Py_RETURN_NONE;
	
	seq = ms->parseq;
	return V24_Sequence_CreatePyObject(seq, NULL, self->scene);
}


/*
 * this should accept a Py_None argument and just delete the Ipo link
 * (as V24_Object_clearIpo() does)
 */

static int V24_Sequence_setIpo( V24_BPy_Sequence * self, PyObject * value )
{
	Ipo *ipo = NULL;
	Ipo *oldipo;
	ID *id;
	
	oldipo = self->seq->ipo;
	
	/* if parameter is not None, check for valid Ipo */

	if ( value != Py_None ) {
		if ( !V24_BPy_Ipo_Check( value ) )
			return V24_EXPP_ReturnIntError( PyExc_TypeError,
					"expected an Ipo object" );

		ipo = V24_Ipo_FromPyObject( value );

		if( !ipo )
			return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
					"null ipo!" );

		if( ipo->blocktype != ID_SEQ )
			return V24_EXPP_ReturnIntError( PyExc_TypeError,
					"Ipo is not a sequence data Ipo" );
	}

	/* if already linked to Ipo, delete link */

	if ( oldipo ) {
		id = &oldipo->id;
		if( id->us > 0 )
			id->us--;
	}

	/* assign new Ipo and increment user count, or set to NULL if deleting */

	self->seq->ipo = ipo;
	if ( ipo )
		id_us_plus(&ipo->id);

	return 0;
}

static PyObject *V24_Sequence_getScene( V24_BPy_Sequence * self )
{
	struct Scene *scene;
	
	scene = self->seq->scene;

	if( scene )
		return V24_Scene_CreatePyObject( scene );
	Py_RETURN_NONE;
}


static PyObject *V24_Sequence_getImages( V24_BPy_Sequence * self )
{
	Strip *strip;
	StripElem *se;
	int i;
	PyObject *list, *ret;
	
	if (self->seq->type != SEQ_IMAGE) {
		list = PyList_New(0);
		ret= Py_BuildValue( "sO", "", list);
		Py_DECREF(list);
		return ret;
	}
	
			/*return V24_EXPP_ReturnPyObjError( PyExc_TypeError,
					"Sequence is not an image type" );*/
	
	
	strip = self->seq->strip;
	se = strip->stripdata;
	list = PyList_New(strip->len);
	
	for (i=0; i<strip->len; i++, se++) {
		PyList_SetItem( list, i, PyString_FromString(se->name) );
	}
	
	ret= Py_BuildValue( "sO", strip->dir, list);
	Py_DECREF(list);
	return ret;
}

static int V24_Sequence_setImages( V24_BPy_Sequence * self, PyObject *value )
{
	Strip *strip;
	StripElem *se;
	int i;
	PyObject *list;
	char *basepath, *name;
	
	if (self->seq->type != SEQ_IMAGE) {
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"Sequence is not an image type" );
	}
	
	if( !PyArg_ParseTuple
	    ( value, "sO!", &basepath, &PyList_Type, &list ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
					      "expected string and optional list argument" );
	
	strip = self->seq->strip;
	se = strip->stripdata;
	
	/* for now dont support different image list sizes */
	if (PyList_Size(list) != strip->len) {
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"at the moment only image lista with the same number of images as the strip are supported" );
	}
	
	strncpy(strip->dir, basepath, sizeof(strip->dir));
	
	for (i=0; i<strip->len; i++, se++) {
		name = PyString_AsString(PyList_GetItem(list, i));
		if (name) {
			strncpy(se->name, name, sizeof(se->name));
		} else {
			PyErr_Clear();
		}
	}
	
	return 0;
}

/*
 * get floating point attributes
 */
static PyObject *getIntAttr( V24_BPy_Sequence *self, void *type )
{
	int param;
	struct Sequence *seq= self->seq;
	
	/*printf("%i %i %i %i %i %i %i %i %i\n", seq->len, seq->start, seq->startofs, seq->endofs, seq->startstill, seq->endstill, seq->startdisp, seq->enddisp, seq->depth );*/
	switch( (int)type ) {
	case EXPP_SEQ_ATTR_TYPE: 
		param = seq->type;
		break;
	case EXPP_SEQ_ATTR_CHAN:
		param = seq->machine;
		break;
	case EXPP_SEQ_ATTR_LENGTH:
		param = seq->len;
		break;
	case EXPP_SEQ_ATTR_START:
		param = seq->start;
		break;
	case EXPP_SEQ_ATTR_STARTOFS:
		param = seq->startofs;
		break;
	case EXPP_SEQ_ATTR_ENDOFS:
		param = seq->endofs;
		break;
	case EXPP_SEQ_ATTR_STARTSTILL:
		param = seq->startstill;
		break;
	case EXPP_SEQ_ATTR_ENDSTILL:
		param = seq->endstill;
		break;
	default:
		return V24_EXPP_ReturnPyObjError( PyExc_RuntimeError, 
				"undefined type in getIntAttr" );
	}

	return PyInt_FromLong( param );
}

/* internal functions for recursivly updating metastrip locatons */
static void intern_pos_update(Sequence * seq) {
	/* update startdisp and enddisp */
	calc_sequence_disp(seq);
}

void intern_recursive_pos_update(Sequence * seq, int offset) {
	Sequence *iterseq;
	intern_pos_update(seq);
	if (seq->type != SEQ_META) return;
	
	for (iterseq = seq->seqbase.first; iterseq; iterseq= iterseq->next) {
		iterseq->start -= offset;
		intern_recursive_pos_update(iterseq, offset);
	}
}


static int setIntAttrClamp( V24_BPy_Sequence *self, PyObject *value, void *type )
{
	struct Sequence *seq= self->seq;
	int number, origval=0;

	if( !PyInt_Check( value ) )
		return V24_EXPP_ReturnIntError( PyExc_TypeError, "expected an int value" );
	
	number = PyInt_AS_LONG( value );
		
	switch( (int)type ) {
	case EXPP_SEQ_ATTR_CHAN:
		CLAMP(number, 1, 1024);
		seq->machine = number;
		break;
	case EXPP_SEQ_ATTR_START:
		if (self->seq->type == SEQ_EFFECT)
			return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
				"cannot set the location of an effect directly" );
		CLAMP(number, -MAXFRAME, MAXFRAME);
		origval = seq->start;
		seq->start = number;
		break;
	
	case EXPP_SEQ_ATTR_STARTOFS:
		if (self->seq->type == SEQ_EFFECT)
			return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
				"This property dosnt apply to an effect" );
		CLAMP(number, 0, seq->len - seq->endofs);
		seq->startofs = number;
		break;
	case EXPP_SEQ_ATTR_ENDOFS:
		if (self->seq->type == SEQ_EFFECT)
			return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
				"This property dosnt apply to an effect" );
		CLAMP(number, 0, seq->len - seq->startofs);
		seq->endofs = number;
		break;
	case EXPP_SEQ_ATTR_STARTSTILL:
		if (self->seq->type == SEQ_EFFECT)
			return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
				"This property dosnt apply to an effect" );
		CLAMP(number, 1, MAXFRAME);
		seq->startstill = number;
		break;
	case EXPP_SEQ_ATTR_ENDSTILL:
		if (self->seq->type == SEQ_EFFECT)
			return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
				"This property dosnt apply to an effect" );
		CLAMP(number, seq->startstill+1, MAXFRAME);
		seq->endstill = number;
		break;
	case EXPP_SEQ_ATTR_LENGTH:
		if (self->seq->type == SEQ_EFFECT)
			return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
				"cannot set the length of an effect directly" );
		CLAMP(number, 1, MAXFRAME);
		origval = seq->len;
		seq->start = number;
		break;
	default:
		return V24_EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setFloatAttrClamp" );
	}
	
	intern_pos_update(seq);
	
	if ((int)type == EXPP_SEQ_ATTR_START && number != origval )
		intern_recursive_pos_update(seq, origval - seq->start);
	
	return 0;
}


static PyObject *getFlagAttr( V24_BPy_Sequence *self, void *type )
{
	if (self->seq->flag & (int)type)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}


/*
 * set floating point attributes which require clamping
 */

static int setFlagAttr( V24_BPy_Sequence *self, PyObject *value, void *type )
{
	int t = (int)type;
	int param = PyObject_IsTrue( value );
	
	if( param == -1 )
		return V24_EXPP_ReturnIntError( PyExc_TypeError,
				"expected True/False or 0/1" );
	
	if (param)
		self->seq->flag |= t;
	else {
		/* dont allow leftsel and rightsel when its not selected */
		if (t == SELECT)
			t = t + SEQ_LEFTSEL + SEQ_RIGHTSEL;
		
		self->seq->flag &= ~t;
	}
	return 0;
}


/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef V24_BPy_Sequence_getseters[] = {
	{"name",
	 (getter)V24_Sequence_getName, (setter)V24_Sequence_setName,
	 "Sequence name",
	  NULL},
	{"ipo",
	 (getter)V24_Sequence_getIpo, (setter)V24_Sequence_setIpo,
	 "Sequence ipo",
	  NULL},

	{"scene",
	 (getter)V24_Sequence_getScene, (setter)NULL,
	 "Sequence scene",
	  NULL},
	{"sound",
	 (getter)V24_Sequence_getSound, (setter)NULL,
	 "Sequence name",
	  NULL},
	{"images",
	 (getter)V24_Sequence_getImages, (setter)V24_Sequence_setImages,
	 "Sequence scene",
	  NULL},
	  
	{"type",
	 (getter)getIntAttr, (setter)NULL,
	 "",
	 (void *) EXPP_SEQ_ATTR_TYPE},
	{"channel",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "",
	 (void *) EXPP_SEQ_ATTR_CHAN},
	 
	{"length",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "",
	 (void *) EXPP_SEQ_ATTR_LENGTH},
	{"start",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "",
	 (void *) EXPP_SEQ_ATTR_START},
	{"startOffset",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "",
	 (void *) EXPP_SEQ_ATTR_STARTOFS},
	{"endOffset",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "",
	 (void *) EXPP_SEQ_ATTR_ENDOFS},
	{"startStill",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "",
	 (void *) EXPP_SEQ_ATTR_STARTSTILL},
	{"endStill",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "",
	 (void *) EXPP_SEQ_ATTR_ENDSTILL},
	 
	{"sel",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "Sequence audio mute option",
	 (void *)SELECT},
	{"selLeft",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "",
	 (void *)SEQ_LEFTSEL},
	{"selRight",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "",
	 (void *)SEQ_RIGHTSEL},
	{"filtery",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "",
	 (void *)SEQ_FILTERY},
	{"mute",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "",
	 (void *)SEQ_MUTE},
	{"premul",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "",
	 (void *)SEQ_MAKE_PREMUL},
	{"reversed",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "",
	 (void *)SEQ_REVERSE_FRAMES},
	{"ipoLocked",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "",
	 (void *)SEQ_IPO_FRAME_LOCKED},
	{"ipoLocked",
	 (getter)getFlagAttr, (setter)setFlagAttr,
	 "",
	 (void *)SEQ_IPO_FRAME_LOCKED},
	 
	{NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};

/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef V24_BPy_SceneSeq_getseters[] = {
	{"active",
	 (getter)V24_SceneSeq_getActive, (setter)NULL,
	 "the active strip",
	  NULL},
	{"metastrip",
	 (getter)V24_SceneSeq_getMetaStrip, (setter)NULL,
	 "The currently active metastrip the user is editing",
	  NULL},
	{NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};

/*****************************************************************************/
/* Python TypeSequence structure definition:                                 */
/*****************************************************************************/
PyTypeObject V24_Sequence_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Sequence",             /* char *tp_name; */
	sizeof( V24_BPy_Sequence ),         /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) V24_Sequence_compare,   /* cmpfunc tp_compare; */
	( reprfunc ) V24_Sequence_repr,     /* reprfunc tp_repr; */

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
	( getiterfunc ) V24_Sequence_getIter,           /* getiterfunc tp_iter; */
	( iternextfunc ) V24_Sequence_nextIter,           /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	V24_BPy_Sequence_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	V24_BPy_Sequence_getseters,         /* struct PyGetSetDef *tp_getset; */
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
/* Python TypeSequence structure definition:                                 */
/*****************************************************************************/
PyTypeObject V24_SceneSeq_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender SceneSeq",             /* char *tp_name; */
	sizeof( V24_BPy_Sequence ),         /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	NULL,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	( cmpfunc ) V24_SceneSeq_compare,   /* cmpfunc tp_compare; */
	( reprfunc ) V24_SceneSeq_repr,     /* reprfunc tp_repr; */

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
	( getiterfunc ) V24_SceneSeq_getIter,           /* getiterfunc tp_iter; */
	( iternextfunc ) V24_SceneSeq_nextIter,           /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	V24_BPy_SceneSeq_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	V24_BPy_SceneSeq_getseters,         /* struct PyGetSetDef *tp_getset; */
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
/* Function:	  M_Sequence_Get						*/
/* Python equivalent:	  Blender.Sequence.Get				*/
/*****************************************************************************/
/*
PyObject *M_Sequence_Get( PyObject * self, PyObject * args )
{
	return V24_SceneSeq_CreatePyObject( G.scene, NULL );
}
*/

/*****************************************************************************/
/* Function:	 initObject						*/
/*****************************************************************************/
PyObject *V24_Sequence_Init( void )
{
	PyObject *submodule;
	if( PyType_Ready( &V24_Sequence_Type ) < 0 )
		return NULL;
	if( PyType_Ready( &V24_SceneSeq_Type ) < 0 )
		return NULL;
	
	/* NULL was M_Sequence_methods*/
	submodule = Py_InitModule3( "Blender.Scene.Sequence", NULL,
"The Blender Sequence module\n\n\
This module provides access to **Sequence Data** in Blender.\n" );

	/*Add SUBMODULES to the module*/
	/*PyDict_SetItemString(dict, "Constraint", V24_Constraint_Init()); //creates a *new* module*/
	return submodule;
}


/*****************************************************************************/
/* Function:	V24_Sequence_CreatePyObject					 */
/* Description: This function will create a new BlenObject from an existing  */
/*		Object structure.					 */
/*****************************************************************************/
PyObject *V24_Sequence_CreatePyObject( struct Sequence * seq, struct Sequence * iter, struct Scene *sce)
{
	V24_BPy_Sequence *pyseq;

	if( !seq )
		Py_RETURN_NONE;

	pyseq =
		( V24_BPy_Sequence * ) PyObject_NEW( V24_BPy_Sequence, &V24_Sequence_Type );

	if( pyseq == NULL ) {
		return ( NULL );
	}
	pyseq->seq = seq;
	pyseq->iter = iter;
	pyseq->scene = sce;
	
	return ( ( PyObject * ) pyseq );
}

/*****************************************************************************/
/* Function:	V24_SceneSeq_CreatePyObject					 */
/* Description: This function will create a new BlenObject from an existing  */
/*		Object structure.					 */
/*****************************************************************************/
PyObject *V24_SceneSeq_CreatePyObject( struct Scene * scn, struct Sequence * iter)
{
	V24_BPy_SceneSeq *pysceseq;

	if( !scn )
		Py_RETURN_NONE;

	pysceseq =
		( V24_BPy_SceneSeq * ) PyObject_NEW( V24_BPy_SceneSeq, &V24_SceneSeq_Type );

	if( pysceseq == NULL ) {
		return ( NULL );
	}
	pysceseq->scene = scn;
	pysceseq->iter = iter;
	
	return ( ( PyObject * ) pysceseq );
}

/*****************************************************************************/
/* Function:	Sequence_FromPyObject					 */
/* Description: This function returns the Blender sequence from the given	 */
/*		PyObject.						 */
/*****************************************************************************/
struct Sequence *Sequence_FromPyObject( PyObject * py_seq )
{
	V24_BPy_Sequence *blen_seq;

	blen_seq = ( V24_BPy_Sequence * ) py_seq;
	return ( blen_seq->seq );
}

/*****************************************************************************/
/* Function:	V24_Sequence_compare						 */
/* Description: This is a callback function for the V24_BPy_Sequence type. It	 */
/*		compares two V24_Sequence_Type objects. Only the "==" and "!="  */
/*		comparisons are meaninful. Returns 0 for equality and -1 if  */
/*		they don't point to the same Blender Object struct.	 */
/*		In Python it becomes 1 if they are equal, 0 otherwise.	 */
/*****************************************************************************/
static int V24_Sequence_compare( V24_BPy_Sequence * a, V24_BPy_Sequence * b )
{
	Sequence *pa = a->seq, *pb = b->seq;
	return ( pa == pb ) ? 0 : -1;
}

static int V24_SceneSeq_compare( V24_BPy_SceneSeq * a, V24_BPy_SceneSeq * b )
{
	
	Scene *pa = a->scene, *pb = b->scene;
	return ( pa == pb ) ? 0 : -1;
}

/*****************************************************************************/
/* Function:	V24_Sequence_repr / V24_SceneSeq_repr						 */
/* Description: This is a callback function for the V24_BPy_Sequence type. It	 */
/*		builds a meaninful string to represent object objects.	 */
/*****************************************************************************/
static PyObject *V24_Sequence_repr( V24_BPy_Sequence * self )
{
	return PyString_FromFormat( "[Sequence Strip \"%s\"]",
					self->seq->name + 2 );
}
static PyObject *V24_SceneSeq_repr( V24_BPy_SceneSeq * self )
{
	return PyString_FromFormat( "[Scene Sequence \"%s\"]",
				self->scene->id.name + 2 );
}


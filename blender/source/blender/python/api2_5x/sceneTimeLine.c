/* $Id: sceneTimeLine.c 11367 2007-07-25 13:01:44Z campbellbarton $
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
 * Contributor(s): Joilnen Leite
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#include <BLI_blenlib.h>
#include "Scene.h"

#include <stdio.h>
#include <MEM_guardedalloc.h>	/* for MEM_callocN */

#include "gen_utils.h"
#include "sceneTimeLine.h"

#define MARKER_DEL_CHECK_PY(marker)  if ( !marker ) return ( EXPP_ReturnPyObjError( PyExc_RuntimeError, "Scene has been removed" ) )
#define MARKER_DEL_CHECK_INT(marker) if ( !marker ) return ( EXPP_ReturnIntError( PyExc_RuntimeError, "Scene has been removed" ) )

static PyObject *TimeLine_items(BPyTimeLineObject *self );
static PyObject *TimeLine_keys(BPyTimeLineObject *self );
static PyObject *TimeLine_values( BPyTimeLineObject *self );
static PyObject *TimeLine_keys( BPyTimeLineObject *self );

static PyObject *TimeLine_repr (BPyTimeLineObject *self)
{
	if (self->bpysce->scene)
		return PyString_FromFormat("[TimeLine \"%s\"]", self->bpysce->scene->id.name+2);
	else
		return PyString_FromString("[TimeLine <delete>]");
}

static PyMethodDef BPyTimeLine_methods[] = {
	{"items", (PyCFunction) TimeLine_items,
	 METH_NOARGS, ""},
	{"keys", (PyCFunction) TimeLine_keys,
	 METH_NOARGS, ""},
	{"values", (PyCFunction) TimeLine_values,
	 METH_NOARGS, ""},
	{NULL}
};

ListBase * Marker_get_pointer( BPyTimeLineObject * self )
{
	if (self->bpysce && self->bpysce->scene )
		return &self->bpysce->scene->markers;
	
	return NULL;
}

/*-----------------------dealloc----------------------------------------*/
static void TimeLine_dealloc( BPyTimeLineObject * self )
{
	Py_DECREF(self->bpysce);
	PyObject_DEL( self );
}

static int TimeLine_len( BPyTimeLineObject * self )
{
	ListBase *m = Marker_get_pointer(self);
	MARKER_DEL_CHECK_INT( m );
	return BLI_countlist( m );
}


static PyObject * TimeLine_subscript(BPyTimeLineObject * self, PyObject *key)
{
	PyObject *list, *item;
	TimeMarker *marker= NULL;
	ListBase *m = Marker_get_pointer(self);
	int frame = PyInt_AsLong(key);
	
	if (frame == -1 && PyErr_Occurred())
		return ( EXPP_ReturnPyObjError
				 ( PyExc_KeyError, "Requested data must be an int") );		
	
	MARKER_DEL_CHECK_PY(m);
	
	list = PyList_New( 0 );
	for (marker = m->first; marker; marker = marker->next) {
		if ( marker->frame == frame ) {
			item = PyString_FromString( marker->name );
			PyList_Append( list, item );
			Py_DECREF( item );
		}
	}
	return list; 
}

static int TimeLine_ass_subscript(BPyTimeLineObject * self, PyObject *key, PyObject *val)
{
	TimeMarker *marker= NULL;
	ListBase *m = Marker_get_pointer(self);
	int i, frame = PyInt_AsLong(key);
	
	if (frame == -1 && PyErr_Occurred())
		return ( EXPP_ReturnIntError
				 ( PyExc_KeyError, "Requested data must be an int") );		
	
	MARKER_DEL_CHECK_INT(m);

	
	/* del timeline[frame] 
	 * ...or
	 * timeline[frame] = []
	 * */
	if (!val || (PyList_Check(val) && PyList_Size(val)==0) ) {
		for (marker = m->first; marker; marker = marker->next) {
			if ( marker->frame == frame ) {
				BLI_freelinkN (m, marker);
			}
		}
		return 0;
	}
	
	if (!PyList_Check(val))
		return ( EXPP_ReturnIntError
			 ( PyExc_TypeError, "Assigned data must be a list of strings") );
	
	/* check all list values are strings */
	i = PyList_GET_SIZE(val);
	while (i) {
		i--;
		if (!PyString_Check(PyList_GET_ITEM(val, i))) {
			return ( EXPP_ReturnIntError
					( PyExc_TypeError, "Assigned data must be a list of strings") );
		}
	}
	
	/* timeline[frame] = value
	 * 
	 * lets be a bit tricky here and re-use existing
	 * keyframes on this frame if there are any
	 */
	
	
	
	i = PyList_GET_SIZE(val)-1;
	for (marker = m->first; marker; marker = marker->next) {
		if ( marker->frame == frame ) {
			if (i >= 0)
				strncpy(marker->name, PyString_AsString(PyList_GET_ITEM(val, i)), 64 );
			else
				BLI_freelinkN (m, marker);
			
			i--;
		}
	}
	
	if ( i<0 )
		return 0;
	
	marker= MEM_callocN (sizeof(TimeMarker) * (i+1), "TimeMarker");
	
	while (i >= 0) {
		strncpy(marker->name, PyString_AsString(PyList_GET_ITEM(val, i)), 64 );
		marker->frame= frame;
		BLI_addtail( m, marker );
		marker++;
		i--;
	}
	return 0; 
}

/* internal use only */
static TimeMarker *TimeLineIter_get_pointer( BPyTimeLineObject * self )
{
	ListBase *m;
	TimeMarker *marker;
	int i;
	
	if (!self->bpysce->scene)
		return NULL;
	
	m = Marker_get_pointer(self);
	marker = m->first;
	for (i=0; (marker && i<self->iter); i++, marker= marker->next) {}
	return marker; /* can be NULL */
}

static PyObject *TimeLine_getIter( BPyTimeLineObject * self )
{
	MARKER_DEL_CHECK_PY(self->bpysce->scene);
	
	if (self->iter == 0) { /* were alredy looping */
		return EXPP_incr_ret ( (PyObject *) self );
	} else {
		return TimeLine_CreatePyObject( self->bpysce );
	}
}

/*
 * Return next GroupOb.
 */

static PyObject *TimeLine_nextIter( BPyTimeLineObject * self )
{
	TimeMarker *marker = TimeLineIter_get_pointer(self);

	if( !(marker) )
		return EXPP_ReturnPyObjError( PyExc_StopIteration,
				"iterator at end" );

	self->iter++; /* next, if this is un-initialized will go from -1 to 0 */

	return  PyInt_FromLong( (long)marker->frame ); 
}


/* Return 1 if `key` is in dict `op`, 0 if not, and -1 on error. */
int TimeLine_contains(PyObject *self, PyObject *key)
{
	TimeMarker *marker= NULL;
	ListBase *m = Marker_get_pointer((BPyTimeLineObject *)self);
	int frame = PyInt_AsLong(key);
	if (frame == -1 && PyErr_Occurred()) {
		PyErr_Clear();
		return 0;
	}
	
	if (!m)
		return 0;

	for (marker = m->first; marker; marker = marker->next) {
		if (marker->frame == frame)
			return 1;
	}
	return 0;
}

static PyMappingMethods TimeLine_as_mapping = {
	( inquiry ) TimeLine_len,					/* mp_length */
	( binaryfunc ) TimeLine_subscript,			/* mp_subscript */
	( objobjargproc ) TimeLine_ass_subscript,	/* mp_ass_subscript */
};

/* Hack to implement "key in dict" */
static PySequenceMethods TimeLine_as_sequence = {
	0,			/* sq_length */
	0,			/* sq_concat */
	0,			/* sq_repeat */
	0,			/* sq_item */
	0,			/* sq_slice */
	0,			/* sq_ass_item */
	0,			/* sq_ass_slice */
	TimeLine_contains,	/* sq_contains */
	0,			/* sq_inplace_concat */
	0,			/* sq_inplace_repeat */
};


//-----------------------BPyTimeLineObject method def------------------------------
PyTypeObject BPyTimeLine_Type = {
	PyObject_HEAD_INIT (NULL) 0,		/* ob_size */
	"TimeLine",							/* tp_name */
	sizeof (BPyTimeLineObject),			/* tp_basicsize */
	0,									/* tp_itemsize */
	/* methods */
	( destructor ) TimeLine_dealloc,	/* tp_dealloc */
	(printfunc) 0,						/* tp_print */
	0,									/* tp_getattr */
	0,									/* tp_setattr */
	0,
	(reprfunc) TimeLine_repr,			/* tp_repr */
	0,									/* tp_as_number */
	&TimeLine_as_sequence,				/* tp_as_sequence */
	&TimeLine_as_mapping,				/* tp_as_mapping */
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

	NULL,						/*  char *tp_doc;  Documentation string */
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
	( getiterfunc) TimeLine_getIter, /* getiterfunc tp_iter; */
	( iternextfunc ) TimeLine_nextIter, /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	BPyTimeLine_methods,       /* struct PyMethodDef *tp_methods; */
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



PyObject *TimeLine_CreatePyObject (BPySceneObject *bpysce) {
	/* Check the scenes valid */
	BPyTimeLineObject *bl_tl;
	MARKER_DEL_CHECK_PY(bpysce->scene); /* just checks for null */
	
	bl_tl = (BPyTimeLineObject *) PyObject_NEW (BPyTimeLineObject, &BPyTimeLine_Type);
	
	bl_tl->bpysce = bpysce;
	Py_INCREF(bpysce);
	bl_tl->iter = 0;
	
	return ( PyObject * ) bl_tl;
}

/* return {frame1:['mark1', merk2], frame2:[mark3, mark4], ...}*/
static PyObject *TimeLine_dict__internal( BPyTimeLineObject *self )
{
	ListBase *m = Marker_get_pointer(self);
	
	PyObject *marker_dict= NULL;
	TimeMarker *marker_it= NULL;
	PyObject *pyo= NULL, *tmpstr;

	MARKER_DEL_CHECK_PY(m);
	
	marker_dict= PyDict_New ();
	for (marker_it= m->first; marker_it; marker_it= marker_it->next) {
		pyo=PyDict_GetItem ((PyObject*)marker_dict, PyInt_FromLong ((long int)marker_it->frame));
		tmpstr = PyString_FromString(marker_it->name);
		if (pyo) {
			PyList_Append (pyo, tmpstr);
			Py_INCREF (pyo);
		}else{ 
			pyo= PyList_New (0);
			PyList_Append (pyo, tmpstr);
		}
		Py_DECREF(tmpstr);
		
		PyDict_SetItem (marker_dict, PyInt_FromLong ((long int)marker_it->frame), pyo); 
		if (pyo) { 
			Py_DECREF (pyo); 
			pyo= NULL; 
		}
	}
	return marker_dict;
}

/* return [frame1, frame2, frame3, ...]*/
static PyObject *TimeLine_keys( BPyTimeLineObject *self )
{
	ListBase *m = Marker_get_pointer(self);
	PyObject *key_list= NULL;
	TimeMarker *marker= NULL;
	PyObject *item= NULL;

	MARKER_DEL_CHECK_PY(m);
	
	key_list= PyList_New (0);
	for (marker= m->first; marker; marker= marker->next) {
		item = PyInt_FromLong((long int)marker->frame);
		if (!PySequence_Contains( key_list, item))
			PyList_Append(key_list, item);
		
		Py_DECREF(item);
	}
	PyList_Sort(key_list);
	return key_list;
}

static PyObject *TimeLine_items( BPyTimeLineObject *self )
{
	PyObject *dict = TimeLine_dict__internal(self);
	PyObject *ret, *newarg;
	if (!dict)
		return NULL;

	newarg = PyTuple_Pack(1, dict);
	Py_DECREF(dict);
	ret = PyObject_Call(PyObject_GetAttrString( ((PyObject *)&PyDict_Type), "items"), newarg, NULL);
	Py_DECREF(newarg);
	return ret;
}

static PyObject *TimeLine_values( BPyTimeLineObject *self )
{
	PyObject *dict = TimeLine_dict__internal(self);
	PyObject *ret, *newarg;
	if (!dict)
		return NULL;

	newarg = PyTuple_Pack(1, dict);
	Py_DECREF(dict);
	ret = PyObject_Call(PyObject_GetAttrString( ((PyObject *)&PyDict_Type), "values"), newarg, NULL);
	Py_DECREF(newarg);
	return ret;
}

PyObject * TimeLineType_Init( void )
{
	PyType_Ready( &BPyTimeLine_Type );
	return (PyObject *) &BPyTimeLine_Type;
}

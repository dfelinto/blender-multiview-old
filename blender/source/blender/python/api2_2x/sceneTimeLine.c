/* $Id$
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

// static PyObject *TimeLine_New (PyObject *self);
static PyObject *V24_M_TimeLine_Get (PyObject *self, PyObject *args);

static char V24_M_TimeLine_Get_doc[]= "Return the Scene.TimeLine.";

//----------------------Scene.TimeMarker subsubmodule method def----------------------------
struct PyMethodDef V24_M_TimeLine_methods[]= {
//	{"New", (PyCFunction) M_TimeMarker_New, METH_NOVAR,
//	 M_TimeLine_New_doc},
	{"Get", (PyCFunction) V24_M_TimeLine_Get, METH_VARARGS,
	 V24_M_TimeLine_Get_doc},
	{NULL, NULL, 0, NULL}
};

static PyObject *V24_TimeLine_addMarker (V24_BPy_TimeLine *self, PyObject *args);
static PyObject *V24_TimeLine_delMarker (V24_BPy_TimeLine *self, PyObject *args);
static PyObject *V24_TimeLine_setNameMarker (V24_BPy_TimeLine *self, PyObject *args);
static PyObject *V24_TimeLine_getNameMarker (V24_BPy_TimeLine *self, PyObject *args);
static PyObject *V24_TimeLine_getFramesMarked (V24_BPy_TimeLine *self, PyObject *args);

static PyObject *V24_TimeLine_repr (V24_BPy_TimeLine *self) {

	return PyString_FromFormat ("[TimeLine]");
}

static PyMethodDef V24_BPy_TimeLine_methods[] = {
	{"add", (PyCFunction) V24_TimeLine_addMarker,
	 METH_VARARGS,
	 "() - Add timemarker"},
	{"delete", (PyCFunction) V24_TimeLine_delMarker,
	 METH_VARARGS,
	 "() - delete timemarker"},
	{"setName", (PyCFunction) V24_TimeLine_setNameMarker,
	 METH_VARARGS,
	 "() - Get timemarker name"},
	{"getName", (PyCFunction) V24_TimeLine_getNameMarker,
	 METH_VARARGS,
	 "() - Set timemarker name"},
	{"getMarked", (PyCFunction) V24_TimeLine_getFramesMarked,
	 METH_VARARGS,
	 "() - Get frames timemarked"},
	{NULL, NULL, 0, NULL}
};

/*-----------------------dealloc----------------------------------------*/
static void V24_TimeLine_dealloc( V24_BPy_TimeLine * self )
{
	PyObject_DEL( self );
}

/*-----------------------getattr----------------------------------------*/
static PyObject *V24_TimeLine_getattr (V24_BPy_TimeLine *self, char *name) {
	return Py_FindMethod( V24_BPy_TimeLine_methods, ( PyObject * ) self, name );
}

/*-----------------------setattr----------------------------------------*/
static int V24_TimeLine_setattr (V24_BPy_TimeLine *self, char *name, PyObject *value) {
	PyObject *valtuple;
	PyObject *error= NULL;

	valtuple= Py_BuildValue ("(O)", value);

	if (!valtuple)
		return V24_EXPP_ReturnIntError( PyExc_MemoryError,
	    		"TimeLineSetAttr: couldn't create tuple" );
	if( strcmp( name, "name" ) == 0 )
		error = V24_TimeLine_setNameMarker (self, valtuple);
	Py_DECREF (valtuple);
	if (error != Py_None)
		return -1;

	Py_DECREF (Py_None);
	return 0;	
}

//-----------------------V24_BPy_Scene method def------------------------------
PyTypeObject V24_TimeLine_Type = {
	PyObject_HEAD_INIT (NULL) 0,	/* ob_size */
	"TimeLine",			/* tp_name */
	sizeof (V24_BPy_TimeLine),	/* tp_basicsize */
	0,			/* tp_itemsize */
	/* methods */
	( destructor ) V24_TimeLine_dealloc,	/* tp_dealloc */
	(printfunc) 0,	/* tp_print */
	(getattrfunc) V24_TimeLine_getattr,	/* tp_getattr */
	(setattrfunc) V24_TimeLine_setattr,	/* tp_setattr */
	0,
	(reprfunc) V24_TimeLine_repr,	/* tp_repr */
	0,			/* tp_as_number */
	0,			/* tp_as_sequence */
	0,			/* tp_as_mapping */
	0,			/* tp_hash */
	0,			/* tp_as_number */
	0,			/* tp_as_sequence */
	0,			/* tp_as_mapping */
	0,			/* tp_hash */
	0,0,0,0,0,0,0,0,0,
	V24_BPy_TimeLine_methods,
	0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

PyObject *V24_TimeLine_Init (void) 
{
	PyObject *V24_submodule;

	if (PyType_Ready (&V24_TimeLine_Type) < 0)
		return NULL;
	V24_submodule= Py_InitModule3 ("Blender.Scene.TimeLine", V24_M_TimeLine_methods,
			"The Blender TimeLine subsubmodule");

	return V24_submodule;
}

PyObject *V24_TimeLine_CreatePyObject (V24_BPy_TimeLine *tl) {
	V24_BPy_TimeLine *bl_tl;

	bl_tl= (V24_BPy_TimeLine *) PyObject_NEW (V24_BPy_TimeLine, &V24_TimeLine_Type);

	return (( PyObject * ) bl_tl);
}


PyObject *V24_M_TimeLine_Get (PyObject *self, PyObject *args) {

	return V24_EXPP_incr_ret (Py_None);
}

static PyObject *V24_TimeLine_getFramesMarked (V24_BPy_TimeLine *self, PyObject *args) {

	PyObject *marker_dict= NULL;
	TimeMarker *marker_it= NULL;
	PyObject *tmarker= NULL, *pyo= NULL, *tmpstr;

	if (!PyArg_ParseTuple (args, "|O", &tmarker))
		return V24_EXPP_ReturnPyObjError (PyExc_AttributeError,
					      "expected nothing, string or int as arguments.");
	if (tmarker) {
		char s[64];
		int frm= 0;

		if (PyString_Check (tmarker) && (BLI_strncpy(s, PyString_AsString (tmarker), 64)) ) {
			for (marker_it= self->marker_list->first; marker_it; marker_it= marker_it->next)
				if (!strcmp (marker_it->name, s)) {
					frm= (int)marker_it->frame;
					break;
				}
		}
		else if (PyInt_Check (tmarker))
			frm= (int)PyInt_AS_LONG (tmarker);
		else
			return V24_EXPP_ReturnPyObjError (PyExc_AttributeError,
					      "expected nothing, string or int as arguments.");
		if (frm>0) {
			marker_dict= PyDict_New ();
			for (marker_it= self->marker_list->first; marker_it; marker_it= marker_it->next){
				if (marker_it->frame==frm) {
					pyo= PyDict_GetItem ((PyObject*)marker_dict, PyInt_FromLong ((long int)marker_it->frame));
					tmpstr = PyString_FromString(marker_it->name);
					if (pyo) {
						PyList_Append (pyo, tmpstr);
						Py_INCREF(pyo);
					}else{
						pyo = PyList_New(0);
						PyList_Append (pyo, tmpstr);
					}
					Py_DECREF(tmpstr);
					
					PyDict_SetItem (marker_dict, PyInt_FromLong ((long int)marker_it->frame), pyo); 
					if (pyo) { 
						Py_DECREF (pyo); 
						pyo= NULL; 
					}
				}
			}
		}

	}else {
		marker_dict= PyDict_New ();
		for (marker_it= self->marker_list->first; marker_it; marker_it= marker_it->next) {
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
	}

	return marker_dict;
}

static PyObject *V24_TimeLine_addMarker (V24_BPy_TimeLine *self, PyObject *args) {
	int frame= 0;
	TimeMarker *marker= NULL, *marker_it= NULL;

	if (!PyArg_ParseTuple( args, "i", &frame ))
		return V24_EXPP_ReturnPyObjError (PyExc_TypeError,
		      "expected int as argument.");
	/* two markers can't be at the same place */
	for (marker_it= self->marker_list->first; marker_it; marker_it= marker_it->next) {
		if (marker_it->frame==frame)
			return V24_EXPP_incr_ret (Py_None);
	}
	if (frame<self->sfra || frame>self->efra)
		return V24_EXPP_ReturnPyObjError (PyExc_TypeError, "frame out of range.");
	marker= MEM_callocN (sizeof(TimeMarker), "TimeMarker");
	if (!marker) return V24_EXPP_incr_ret (Py_None); 
	marker->frame= frame;
	BLI_addtail (self->marker_list, marker);
	return V24_EXPP_incr_ret (Py_None);
}

static PyObject *V24_TimeLine_delMarker (V24_BPy_TimeLine *self, PyObject *args) {
	int frame= 0;
	TimeMarker *marker= NULL;
	
	if (!PyArg_ParseTuple (args, "|i", &frame))
		return V24_EXPP_ReturnPyObjError (PyExc_TypeError,
				"expected int as argument.");

	for (marker= self->marker_list->first; marker; marker= marker->next) {
		if (!frame)
			BLI_freelinkN (self->marker_list, marker);
		else if (marker->frame == frame) {
			BLI_freelinkN (self->marker_list, marker);
			return V24_EXPP_incr_ret (Py_None);
		}
	}

	return V24_EXPP_incr_ret (Py_None);
}

static PyObject *V24_TimeLine_setNameMarker (V24_BPy_TimeLine *self, PyObject *args) {
	char *buf;
	char name[64];
	int frame= 0;
	TimeMarker *marker= NULL;
	
	if (!PyArg_ParseTuple( args, "is", &frame, &buf))
		return V24_EXPP_ReturnPyObjError (PyExc_TypeError,
					      "expected int as argument.");
	PyOS_snprintf (name, sizeof (name), "%s", buf);
	for (marker= self->marker_list->first; marker; marker= marker->next) {
		if (marker->frame == frame) {
			BLI_strncpy(marker->name, name, sizeof(marker->name));
			return V24_EXPP_incr_ret (Py_None);
		}
	}

	return V24_EXPP_ReturnPyObjError (PyExc_TypeError, "frame not marked.");
}

static PyObject *V24_TimeLine_getNameMarker (V24_BPy_TimeLine *self, PyObject *args) {
	int frame= 0;
	TimeMarker *marker;

	if (!PyArg_ParseTuple (args, "i", &frame))
		return V24_EXPP_ReturnPyObjError (PyExc_TypeError, "expected int as argument.");
	
	for (marker= self->marker_list->first; marker; marker= marker->next) {
		if (marker->frame == frame)
			return PyString_FromString (marker->name);
	}

	return V24_EXPP_ReturnPyObjError (PyExc_TypeError, "frame not marked.");
}



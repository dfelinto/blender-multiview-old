/*
 * $Id: bpy_float.c 10235 2007-03-10 11:47:24Z campbellbarton $
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
 * Contributor(s): Campbell Barton
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */
/* BPyFloatObject are for accessing settings in blender that can have keyframes added */
#include "bpy_float.h"
#include "gen_utils.h"
#include "BKE_global.h"
#include "DNA_scene_types.h"
#include "BKE_utildefines.h"
#include "BSE_editipo.h"
#include "Ipocurve.h"

static int BPyFloat_init(BPyFloatObject *self, PyObject *args, PyObject *kwds)
{
	if (PyFloat_Type.tp_init((PyObject *)self, args, kwds) < 0)
		return -1;
	self->genlib = NULL;
	return 0;
}

/* Can be accessed directly from a getset */
PyObject *BPyFloat_CreatePyObject( float value, PyObject *self, int adrcode )
{
	BPyFloatObject *obj;
	PyObject *args = PyTuple_New(0); 
	
	/* passing NULL for keywords is fine*/
	obj = (BPyFloatObject *)BPyFloat_Type.tp_new(&BPyFloat_Type, args, NULL);
	BPyFloat_Type.tp_init((PyObject *)obj, args, NULL);
	obj->pyfloat.ob_fval = (double)value;
	obj->genlib = (BPyGenericLibObject *)self;
	obj->adrcode = adrcode;
	if (self) /* this is a genlib user */
		Py_INCREF(self); 
	
	Py_DECREF(args);
	return (PyObject *)obj;
}

PyObject * BPyFloat_keyframe(BPyFloatObject *self)
{	
	if (!self->genlib || !self->genlib->id)
		return ( EXPP_ReturnPyObjError( PyExc_AttributeError,
						"this blender float does not reference any keyable data or the data has been removed." ) );
	
	insertkey(self->genlib->id, GS(self->genlib->id), NULL, NULL, self->adrcode, 0);
	Py_RETURN_NONE;
}

PyObject * BPyFloat_getCurve(BPyFloatObject *self)
{	/* verify_ipocurve - can be NULL -> None */
	return IpoCurve_CreatePyObject(
			verify_ipocurve(self->genlib->id, GS(self->genlib->id), NULL, NULL, NULL, self->adrcode) );
}

static PyMethodDef BPyFloat_methods[] = {
	/* name, method, flags, doc */
	{"keyframe", ( PyCFunction ) BPyFloat_keyframe, METH_NOARGS, ""},  
	{NULL}
};

static PyGetSetDef BPyFloat_getset[] = {
	{"curve",
	 (getter)BPyFloat_getCurve, (setter)NULL,
	 "the ipo curve for this value",
	 NULL},
	{NULL,NULL,NULL,NULL,NULL}  /* Sentinel */
};

static void BPyFloat_dealloc( BPyFloatObject * self )
{
	if (self->genlib) {
		Py_DECREF((PyObject *)(self->genlib));
	}
	PyFloat_Type.tp_dealloc((PyObject *)self);
}


PyTypeObject BPyFloat_Type = {
	PyObject_HEAD_INIT(NULL)
	0,
	"BPyFloat",
	sizeof(BPyFloatObject),
	0,
	(destructor)BPyFloat_dealloc,	/* tp_dealloc */
	0,					/* tp_print */
	0,					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	0,					/* tp_repr */
	0,					/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0,					/* tp_hash */
	0,					/* tp_call */
	0,					/* tp_str */
	0,					/* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
	0,					/* tp_doc */
	0,					/* tp_traverse */
	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,					/* tp_weaklistoffset */
	0,					/* tp_iter */
	0,					/* tp_iternext */
	BPyFloat_methods,	/* tp_methods */
	0,					/* tp_members */
	BPyFloat_getset,	/* tp_getset */
	0,					/* tp_base */
	0,					/* tp_dict */
	0,					/* tp_descr_get */
	0,					/* tp_descr_set */ 
	0,					/* tp_dictoffset */
	(initproc)BPyFloat_init, /* tp_init */
	0,					/* tp_alloc */
	0,					/* tp_new */
};

PyObject *BPyFloatType_Init( void )
{
	PyFloat_Type.tp_base = &PyFloat_Type;
	PyType_Ready( &BPyFloat_Type );
	return (PyObject *) &BPyFloat_Type;
}


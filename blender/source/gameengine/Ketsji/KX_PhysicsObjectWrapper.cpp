/**
 * $Id$
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * The contents of this file may be used under the terms of either the GNU
 * General Public License Version 2 or later (the "GPL", see
 * http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
 * later (the "BL", see http://www.blender.org/BL/ ) which has to be
 * bought from the Blender Foundation to become active, in which case the
 * above mentioned GPL option does not apply.
 *
 * The Original Code is Copyright (C) 2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */
#include <Python.h>
#include "KX_PhysicsObjectWrapper.h"
#include "PHY_IPhysicsEnvironment.h"
#include "PHY_IPhysicsController.h"

KX_PhysicsObjectWrapper::KX_PhysicsObjectWrapper(
						PHY_IPhysicsController* ctrl,
						PHY_IPhysicsEnvironment* physenv,PyTypeObject *T)
: m_ctrl(ctrl),m_physenv(physenv),PyObjectPlus(T)
{
}

KX_PhysicsObjectWrapper::~KX_PhysicsObjectWrapper()
{
}


PyObject* KX_PhysicsObjectWrapper::PySetPosition(PyObject* self, 
											PyObject* args, 
											PyObject* kwds)
{
	float x,y,z;
	if (PyArg_ParseTuple(args,"fff",&x,&y,&z))
	{
		m_ctrl->setPosition(x,y,z);
	}
	Py_INCREF(Py_None); return Py_None;
}


PyObject* KX_PhysicsObjectWrapper::PySetLinearVelocity(PyObject* self, 
											PyObject* args, 
											PyObject* kwds)
{
	float x,y,z;
	int local;
	if (PyArg_ParseTuple(args,"fffi",&x,&y,&z,&local))
	{
		m_ctrl->SetLinearVelocity(x,y,z,local != 0);
	}
	Py_INCREF(Py_None); return Py_None;
}

PyObject* KX_PhysicsObjectWrapper::PySetAngularVelocity(PyObject* self, 
											PyObject* args, 
											PyObject* kwds)
{
	float x,y,z;
	int local;
	if (PyArg_ParseTuple(args,"fffi",&x,&y,&z,&local))
	{
		m_ctrl->SetAngularVelocity(x,y,z,local != 0);
	}
	Py_INCREF(Py_None); return Py_None;
}

PyObject*	KX_PhysicsObjectWrapper::PySetActive(PyObject* self, 
											PyObject* args, 
											PyObject* kwds)
{
	int active;
	if (PyArg_ParseTuple(args,"i",&active))
	{
		m_ctrl->SetActive(active!=0);
	}
	Py_INCREF(Py_None); return Py_None;
}




//python specific stuff
PyTypeObject KX_PhysicsObjectWrapper::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
		0,
		"KX_PhysicsObjectWrapper",
		sizeof(KX_PhysicsObjectWrapper),
		0,
		PyDestructor,
		0,
		__getattr,
		__setattr,
		0, //&MyPyCompare,
		__repr,
		0, //&cvalue_as_number,
		0,
		0,
		0,
		0
};

PyParentObject KX_PhysicsObjectWrapper::Parents[] = {
	&KX_PhysicsObjectWrapper::Type,
	NULL
};

PyObject*	KX_PhysicsObjectWrapper::_getattr(char* attr)
{
	_getattr_up(PyObjectPlus);
}


int	KX_PhysicsObjectWrapper::_setattr(char* attr,PyObject* pyobj)
{
	
	PyTypeObject* type = pyobj->ob_type;
	int result = 1;


	if (type == &PyInt_Type)
	{
		result = 0;
	}
	if (type == &PyString_Type)
	{
		result = 0;
	}
	if (result)
		result = PyObjectPlus::_setattr(attr,pyobj);
	return result;
};


PyMethodDef KX_PhysicsObjectWrapper::Methods[] = {
	{"setPosition",(PyCFunction) KX_PhysicsObjectWrapper::sPySetPosition, METH_VARARGS},
	{"setLinearVelocity",(PyCFunction) KX_PhysicsObjectWrapper::sPySetLinearVelocity, METH_VARARGS},
	{"setAngularVelocity",(PyCFunction) KX_PhysicsObjectWrapper::sPySetAngularVelocity, METH_VARARGS},
	{"setActive",(PyCFunction) KX_PhysicsObjectWrapper::sPySetActive, METH_VARARGS},
	{NULL,NULL} //Sentinel
};

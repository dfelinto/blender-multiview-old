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

#include "KX_ConstraintWrapper.h"
#include "PHY_IPhysicsEnvironment.h"

KX_ConstraintWrapper::KX_ConstraintWrapper(
						PHY_ConstraintType ctype,
						int constraintId,
						PHY_IPhysicsEnvironment* physenv,PyTypeObject *T)
: m_constraintType(ctype),m_constraintId(constraintId),m_physenv(physenv),PyObjectPlus(T)
{
}
KX_ConstraintWrapper::~KX_ConstraintWrapper()
{
}
//python integration methods
PyObject* KX_ConstraintWrapper::PyTestMethod(PyObject* self, 
											PyObject* args, 
											PyObject* kwds)
{
	
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject* KX_ConstraintWrapper::PyGetConstraintId(PyObject* self, 
											PyObject* args, 
											PyObject* kwds)
{
	return PyInt_FromLong(m_constraintId);
}




//python specific stuff
PyTypeObject KX_ConstraintWrapper::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
		0,
		"KX_ConstraintWrapper",
		sizeof(KX_ConstraintWrapper),
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

PyParentObject KX_ConstraintWrapper::Parents[] = {
	&KX_ConstraintWrapper::Type,
	NULL
};

PyObject*	KX_ConstraintWrapper::_getattr(char* attr)
{
	//here you can search for existing data members (like mass,friction etc.)
	_getattr_up(PyObjectPlus);
}

int	KX_ConstraintWrapper::_setattr(char* attr,PyObject* pyobj)
{
	
	PyTypeObject* type = pyobj->ob_type;
	int result = 1;

	if (type == &PyList_Type)
	{
		result = 0;
	}
	if (type == &PyFloat_Type)
	{
		result = 0;

	}
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


PyMethodDef KX_ConstraintWrapper::Methods[] = {
	{"testMethod",(PyCFunction) KX_ConstraintWrapper::sPyTestMethod, METH_VARARGS},
	{"getConstraintId",(PyCFunction) KX_ConstraintWrapper::sPyGetConstraintId, METH_VARARGS},
	{NULL,NULL} //Sentinel
};

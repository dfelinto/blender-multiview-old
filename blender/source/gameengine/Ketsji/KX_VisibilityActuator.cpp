/*
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
 * Actuator to toggle visibility/invisibility of objects
 */

#include "KX_VisibilityActuator.h"
#include "KX_GameObject.h"

KX_VisibilityActuator::KX_VisibilityActuator(
	SCA_IObject* gameobj,
	bool visible,
	PyTypeObject* T
	) 
	: SCA_IActuator(gameobj,T),
	  m_visible(visible)
{
	// intentionally empty
}

KX_VisibilityActuator::~KX_VisibilityActuator(
	void
	)
{
	// intentionally empty
}

CValue*
KX_VisibilityActuator::GetReplica(
	void
	)
{
	KX_VisibilityActuator* replica = new KX_VisibilityActuator(*this);
	replica->ProcessReplica();
	// this will copy properties and so on...
	CValue::AddDataToReplica(replica);
	return replica;
}

bool
KX_VisibilityActuator::Update(
	double curtime,
	double deltatime
	)
{
	bool bNegativeEvent = IsNegativeEvent();
	
	RemoveAllEvents();
	if (bNegativeEvent) return false;

	KX_GameObject *obj = (KX_GameObject*) GetParent();
	
	obj->SetVisible(m_visible);
	obj->MarkVisible();

	return true;
}

/* ------------------------------------------------------------------------- */
/* Python functions                                                          */
/* ------------------------------------------------------------------------- */



/* Integration hooks ------------------------------------------------------- */
PyTypeObject 
KX_VisibilityActuator::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"KX_VisibilityActuator",
	sizeof(KX_VisibilityActuator),
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

PyParentObject 
KX_VisibilityActuator::Parents[] = {
	&KX_VisibilityActuator::Type,
	&SCA_IActuator::Type,
	&SCA_ILogicBrick::Type,
	&CValue::Type,
	NULL
};

PyMethodDef 
KX_VisibilityActuator::Methods[] = {
	{"set", (PyCFunction) KX_VisibilityActuator::sPySetVisible, 
	 METH_VARARGS, SetVisible_doc},
	{NULL,NULL} //Sentinel
};

PyObject* 
KX_VisibilityActuator::_getattr(
	char* attr
	) 
{
	_getattr_up(SCA_IActuator);
};



/* set visibility ---------------------------------------------------------- */
char 
KX_VisibilityActuator::SetVisible_doc[] = 
"setVisible(visible?)\n"
"\t - visible? : Make the object visible? (KX_TRUE, KX_FALSE)"
"\tSet the properties of the actuator.\n";
PyObject* 

KX_VisibilityActuator::PySetVisible(PyObject* self, 
				    PyObject* args, 
				    PyObject* kwds) {
	int vis;

	if(!PyArg_ParseTuple(args, "i", &vis)) {
		return NULL;
	}

	m_visible = PyArgToBool(vis);

	Py_Return;
}



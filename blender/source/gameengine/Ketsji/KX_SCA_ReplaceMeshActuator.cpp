//
// Replace the mesh for this actuator's parent
//
// $Id$
//
// ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
//
// The contents of this file may be used under the terms of either the GNU
// General Public License Version 2 or later (the "GPL", see
// http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
// later (the "BL", see http://www.blender.org/BL/ ) which has to be
// bought from the Blender Foundation to become active, in which case the
// above mentioned GPL option does not apply.
//
// The Original Code is Copyright (C) 2002 by NaN Holding BV.
// All rights reserved.
//
// The Original Code is: all of this file.
//
// Contributor(s): none yet.
//
// ***** END GPL/BL DUAL LICENSE BLOCK *****

//
// Previously existed as:

// \source\gameengine\GameLogic\SCA_ReplaceMeshActuator.cpp

// Please look here for revision history.


#include "KX_SCA_ReplaceMeshActuator.h"

/* ------------------------------------------------------------------------- */
/* Python functions                                                          */
/* ------------------------------------------------------------------------- */

/* Integration hooks ------------------------------------------------------- */

	PyTypeObject 

KX_SCA_ReplaceMeshActuator::

Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"KX_SCA_ReplaceMeshActuator",
	sizeof(KX_SCA_ReplaceMeshActuator),
	0,
	PyDestructor,
	0,
	__getattr,
	__setattr,
	0, 
	__repr,
	0, 
	0,
	0,
	0,
	0
};

PyParentObject KX_SCA_ReplaceMeshActuator::Parents[] = {
	&SCA_IActuator::Type,
	&SCA_ILogicBrick::Type,
	&CValue::Type,
	NULL
};



PyMethodDef KX_SCA_ReplaceMeshActuator::Methods[] = {
  {"setMesh", (PyCFunction) KX_SCA_ReplaceMeshActuator::sPySetMesh, METH_VARARGS, SetMesh_doc},
  {NULL,NULL} //Sentinel
};



PyObject* KX_SCA_ReplaceMeshActuator::_getattr(char* attr)
{
  _getattr_up(SCA_IActuator);
}



/* 1. setMesh */
char KX_SCA_ReplaceMeshActuator::SetMesh_doc[] = 
	"setMesh(name)\n"
	"\t- name: string\n"
	"\tSet the mesh that will be substituted for the current one.\n";

PyObject* KX_SCA_ReplaceMeshActuator::PySetMesh(PyObject* self,
									  PyObject* args, 
									  PyObject* kwds)
{
	char* meshname;
	
	if (!PyArg_ParseTuple(args, "s", &meshname))
	{
		return NULL;	
	}

	void* mesh = SCA_ILogicBrick::m_sCurrentLogicManager->GetMeshByName(STR_String(meshname));
	
	if (m_mesh) {
		m_mesh= (class RAS_MeshObject*)mesh;
		Py_Return;
	}
	
	return NULL;
}



/* ------------------------------------------------------------------------- */
/* Native functions                                                          */
/* ------------------------------------------------------------------------- */

KX_SCA_ReplaceMeshActuator::KX_SCA_ReplaceMeshActuator(SCA_IObject *gameobj,
													   class RAS_MeshObject *mesh,
													   SCA_IScene* scene,
													   PyTypeObject* T) : 

	SCA_IActuator(gameobj, T),
	m_scene(scene),
	m_mesh(mesh)
{
} /* End of constructor */



KX_SCA_ReplaceMeshActuator::~KX_SCA_ReplaceMeshActuator()
{ 
	// there's nothing to be done here, really....
} /* end of destructor */



bool KX_SCA_ReplaceMeshActuator::Update(double curtime,
										double deltatime)
{
	bool result = false;	
	bool bNegativeEvent = IsNegativeEvent();
	RemoveAllEvents();

	if (bNegativeEvent)
		return false; // do nothing on negative events

	if (m_mesh) m_scene->ReplaceMesh(GetParent(),m_mesh);

	return false;
}



CValue* KX_SCA_ReplaceMeshActuator::GetReplica()
{
	KX_SCA_ReplaceMeshActuator* replica = 
		new KX_SCA_ReplaceMeshActuator(*this);

	if (replica == NULL)
		return NULL;

	replica->ProcessReplica();

	// this will copy properties and so on...
	CValue::AddDataToReplica(replica);

	return replica;
};

/* eof */

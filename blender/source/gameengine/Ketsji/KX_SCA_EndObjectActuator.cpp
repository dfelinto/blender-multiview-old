/**
 * $Id$
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

//

// Remove the actuator's parent when triggered
//
// Previously existed as:
// \source\gameengine\GameLogic\SCA_EndObjectActuator.cpp
// Please look here for revision history.

#include "SCA_IActuator.h"
#include "KX_SCA_EndObjectActuator.h"
#include "SCA_IScene.h"

KX_SCA_EndObjectActuator::KX_SCA_EndObjectActuator(SCA_IObject *gameobj,
												   SCA_IScene* scene,
												   PyTypeObject* T): 
	SCA_IActuator(gameobj, T),
	m_scene(scene)
{
    // intentionally empty 
} /* End of constructor */



KX_SCA_EndObjectActuator::~KX_SCA_EndObjectActuator()
{ 
	// there's nothing to be done here, really....
} /* end of destructor */



bool KX_SCA_EndObjectActuator::Update(double curtime,
									  double deltatime
									  )
{
	bool result = false;	
	bool bNegativeEvent = IsNegativeEvent();
	RemoveAllEvents();

	if (bNegativeEvent)
		return false; // do nothing on negative events
	m_scene->DelayedRemoveObject(GetParent());
	
	return false;
}



CValue* KX_SCA_EndObjectActuator::GetReplica()
{
	KX_SCA_EndObjectActuator* replica = 
		new KX_SCA_EndObjectActuator(*this);
	if (replica == NULL) return NULL;

	replica->ProcessReplica();
	// this will copy properties and so on...
	CValue::AddDataToReplica(replica);
	return replica;
};



/* ------------------------------------------------------------------------- */
/* Python functions : integration hooks                                      */
/* ------------------------------------------------------------------------- */

PyTypeObject KX_SCA_EndObjectActuator::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"KX_SCA_EndObjectActuator",
	sizeof(KX_SCA_EndObjectActuator),
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


PyParentObject KX_SCA_EndObjectActuator::Parents[] = {
	&KX_SCA_EndObjectActuator::Type,
	&SCA_IActuator::Type,
	&SCA_ILogicBrick::Type,
	&CValue::Type,
	NULL
};



PyMethodDef KX_SCA_EndObjectActuator::Methods[] = {
  {NULL,NULL} //Sentinel
};


PyObject* KX_SCA_EndObjectActuator::_getattr(char* attr)
{
  _getattr_up(SCA_IActuator);
}

/* eof */

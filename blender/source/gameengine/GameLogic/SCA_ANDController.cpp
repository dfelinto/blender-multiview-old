/**
 * 'And' together all inputs
 *
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

#include "SCA_ANDController.h"
#include "SCA_ISensor.h"
#include "SCA_LogicManager.h"
#include "BoolValue.h"

/* ------------------------------------------------------------------------- */
/* Native functions                                                          */
/* ------------------------------------------------------------------------- */

SCA_ANDController::SCA_ANDController(SCA_IObject* gameobj,
									 PyTypeObject* T)
	:
	SCA_IController(gameobj,T)
{
}



SCA_ANDController::~SCA_ANDController()
{
}



void SCA_ANDController::Trigger(SCA_LogicManager* logicmgr)
{

	bool sensorresult = true;

	for (vector<SCA_ISensor*>::const_iterator is=m_linkedsensors.begin();
	!(is==m_linkedsensors.end());is++)
	{
		SCA_ISensor* sensor = *is;
		if (!sensor->IsPositiveTrigger())
		{
			sensorresult = false;
			break;
		}
	}
	
	CValue* newevent = new CBoolValue(sensorresult);

	for (vector<SCA_IActuator*>::const_iterator i=m_linkedactuators.begin();
	!(i==m_linkedactuators.end());i++)
	{
		SCA_IActuator* actua = *i;//m_linkedactuators.at(i);
		logicmgr->AddActiveActuator(actua,newevent);
	}

	// every actuator that needs the event, has a it's own reference to it now so
	// release it (so to be clear: if there is no actuator, it's deleted right now)
	newevent->Release();

}



CValue* SCA_ANDController::GetReplica()
{
	CValue* replica = new SCA_ANDController(*this);
	// this will copy properties and so on...
	CValue::AddDataToReplica(replica);

	return replica;
}



/* ------------------------------------------------------------------------- */
/* Python functions                                                          */
/* ------------------------------------------------------------------------- */

/* Integration hooks ------------------------------------------------------- */
PyTypeObject SCA_ANDController::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"SCA_ANDController",
	sizeof(SCA_ANDController),
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

PyParentObject SCA_ANDController::Parents[] = {
	&SCA_ANDController::Type,
	&SCA_IController::Type,
	&SCA_ILogicBrick::Type,
	&CValue::Type,
	NULL
};

PyMethodDef SCA_ANDController::Methods[] = {
	{NULL,NULL} //Sentinel
};

PyObject* SCA_ANDController::_getattr(char* attr) {
	_getattr_up(SCA_IController);
}

/* eof */

/**
 * 'Or' together all inputs
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

#include "SCA_ORController.h"
#include "SCA_ISensor.h"
#include "SCA_LogicManager.h"
#include "BoolValue.h"

/* ------------------------------------------------------------------------- */
/* Native functions                                                          */
/* ------------------------------------------------------------------------- */

SCA_ORController::SCA_ORController(SCA_IObject* gameobj,
								   PyTypeObject* T)
		:SCA_IController(gameobj, T)
{
}



SCA_ORController::~SCA_ORController()
{
}



CValue* SCA_ORController::GetReplica()
{
	CValue* replica = new SCA_ORController(*this);
	// this will copy properties and so on...
	CValue::AddDataToReplica(replica);

	return replica;
}


void SCA_ORController::Trigger(SCA_LogicManager* logicmgr)
{

	bool sensorresult = false;
	SCA_ISensor* sensor;

	vector<SCA_ISensor*>::const_iterator is=m_linkedsensors.begin();
	while ( (!sensorresult) && (!(is==m_linkedsensors.end())) )
	{
		sensor = *is;
		if (sensor->IsPositiveTrigger()) sensorresult = true;
		is++;
	}
	
	CValue* newevent = new CBoolValue(sensorresult);

	for (vector<SCA_IActuator*>::const_iterator i=m_linkedactuators.begin();
	!(i==m_linkedactuators.end());i++)
	{
		SCA_IActuator* actua = *i;//m_linkedactuators.at(i);
		logicmgr->AddActiveActuator(actua,newevent);
	}


	newevent->Release();
}

/* ------------------------------------------------------------------------- */
/* Python functions                                                          */
/* ------------------------------------------------------------------------- */

/* Integration hooks ------------------------------------------------------- */
PyTypeObject SCA_ORController::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"SCA_ORController",
	sizeof(SCA_ORController),
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

PyParentObject SCA_ORController::Parents[] = {
	&SCA_ORController::Type,
	&SCA_IController::Type,
	&SCA_ILogicBrick::Type,
	&CValue::Type,
	NULL
};

PyMethodDef SCA_ORController::Methods[] = {
	{NULL,NULL} //Sentinel
};

PyObject* SCA_ORController::_getattr(char* attr) {
	_getattr_up(SCA_IController);
}

/* eof */

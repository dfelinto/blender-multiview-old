/**
 * Always trigger
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

#ifdef WIN32
// This warning tells us about truncation of __long__ stl-generated names.
// It can occasionally cause DevStudio to have internal compiler warnings.
#pragma warning( disable : 4786 )     
#endif

#include "SCA_AlwaysSensor.h"
#include "SCA_LogicManager.h"
#include "SCA_EventManager.h"

/* ------------------------------------------------------------------------- */
/* Native functions                                                          */
/* ------------------------------------------------------------------------- */

SCA_AlwaysSensor::SCA_AlwaysSensor(class SCA_EventManager* eventmgr,
								 SCA_IObject* gameobj,
								 PyTypeObject* T)
	: SCA_ISensor(gameobj,eventmgr, T)
{
	//SetDrawColor(255,0,0);
	m_alwaysresult = true;
}



SCA_AlwaysSensor::~SCA_AlwaysSensor()
{
	/* intentionally empty */
}



CValue* SCA_AlwaysSensor::GetReplica()
{
	CValue* replica = new SCA_AlwaysSensor(*this);//m_float,GetName());
	// this will copy properties and so on...
	CValue::AddDataToReplica(replica);

	return replica;
}



bool SCA_AlwaysSensor::IsPositiveTrigger()
{ 
	return (m_invert ? false : true);
}



bool SCA_AlwaysSensor::Evaluate(CValue* event)
{
	/* Nice! :) */
		//return true;
	/* even nicer ;) */
		//return false;
	
	/* nicest ! */
	bool result = m_alwaysresult;
	m_alwaysresult = false;
	return result;
}

/* ------------------------------------------------------------------------- */
/* Python functions                                                          */
/* ------------------------------------------------------------------------- */

/* Integration hooks ------------------------------------------------------- */
PyTypeObject SCA_AlwaysSensor::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"SCA_AlwaysSensor",
	sizeof(SCA_AlwaysSensor),
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

PyParentObject SCA_AlwaysSensor::Parents[] = {
	&SCA_AlwaysSensor::Type,
	&SCA_ISensor::Type,
	&SCA_ILogicBrick::Type,
	&CValue::Type,
	NULL
};

PyMethodDef SCA_AlwaysSensor::Methods[] = {
	{NULL,NULL} //Sentinel
};

PyObject* SCA_AlwaysSensor::_getattr(char* attr) {
	_getattr_up(SCA_ISensor);
}

/* eof */

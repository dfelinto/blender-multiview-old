/**
 * Manager for keyboard events
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

#include <iostream.h>
#include "BoolValue.h"
#include "SCA_KeyboardManager.h"
#include "SCA_KeyboardSensor.h"
#include "IntValue.h"
#include <vector>

SCA_KeyboardManager::SCA_KeyboardManager(SCA_LogicManager* logicmgr,
										 SCA_IInputDevice* inputdev)
	:	SCA_EventManager(KEYBOARD_EVENTMGR),
		m_logicmanager(logicmgr),
		m_inputDevice(inputdev)
{
}



SCA_KeyboardManager::~SCA_KeyboardManager()
{
}



SCA_IInputDevice* SCA_KeyboardManager::GetInputDevice()
{
	return m_inputDevice;
}



void SCA_KeyboardManager::NextFrame(double curtime,double deltatime)
{
	//const SCA_InputEvent& event =	GetEventValue(SCA_IInputDevice::KX_EnumInputs inputcode)=0;
  //	cerr << "SCA_KeyboardManager::NextFrame"<< endl;
	for (int i=0;i<m_sensors.size();i++)
	{
		SCA_KeyboardSensor* keysensor = (SCA_KeyboardSensor*)m_sensors[i];
		keysensor->Activate(m_logicmanager,NULL);
	}

}



void  SCA_KeyboardManager::RegisterSensor(SCA_ISensor* keysensor)
{
	m_sensors.push_back(keysensor);
}



bool SCA_KeyboardManager::IsPressed(SCA_IInputDevice::KX_EnumInputs inputcode)
{
	return false;
	//return m_kxsystem->IsPressed(inputcode);
}


/**
 * Manager for mouse events
 *
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

#include "BoolValue.h"
#include "SCA_MouseManager.h"
#include "SCA_MouseSensor.h"
#include "IntValue.h"


SCA_MouseManager::SCA_MouseManager(SCA_LogicManager* logicmgr,
								   SCA_IInputDevice* mousedev)
	:	SCA_EventManager(MOUSE_EVENTMGR),
		m_logicmanager(logicmgr),
		m_mousedevice (mousedev)
{
	m_xpos = 0;
	m_ypos = 0;
}



SCA_MouseManager::~SCA_MouseManager()
{
}



SCA_IInputDevice* SCA_MouseManager::GetInputDevice()
{
	return m_mousedevice;
}



void SCA_MouseManager::NextFrame(double curtime,double deltatime)
{
	if (m_mousedevice)
	{
		for (int i = 0; i < m_sensors.size(); i++)
		{
			SCA_MouseSensor* mousesensor = (SCA_MouseSensor*) m_sensors[i];
			// (0,0) is the Upper Left corner in our local window
			// coordinates
			if (!mousesensor->IsSuspended())
			{
				const SCA_InputEvent& event = 
					m_mousedevice->GetEventValue(SCA_IInputDevice::KX_MOUSEX);
				int mx = event.m_eventval;
				const SCA_InputEvent& event2 = 
					m_mousedevice->GetEventValue(SCA_IInputDevice::KX_MOUSEY);
				int my = event2.m_eventval;
				
				mousesensor->setX(mx);
				mousesensor->setY(my);
				
				mousesensor->Activate(m_logicmanager,NULL);
			}
		}
	}
}



void  SCA_MouseManager::RegisterSensor(SCA_ISensor* keysensor)
{
	m_sensors.push_back(keysensor);
}



bool SCA_MouseManager::IsPressed(SCA_IInputDevice::KX_EnumInputs inputcode)
{
	/* We should guard for non-mouse events maybe? A rather silly side       */
	/* effect here is that position-change events are considered presses as  */
	/* well.                                                                 */
	
	return m_mousedevice->IsPressed(inputcode);
}

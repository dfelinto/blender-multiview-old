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

#include <iostream.h>

#include "SCA_IInputDevice.h"



SCA_IInputDevice::SCA_IInputDevice()
	:
	m_currentTable(0)
{
	ClearStatusTable(0);
	ClearStatusTable(1);
}



SCA_IInputDevice::~SCA_IInputDevice()
{
}	



void SCA_IInputDevice::ClearStatusTable(int tableid)
{
	for (int i=0;i<SCA_IInputDevice::KX_MAX_KEYS;i++)
		m_eventStatusTables[tableid][i]=SCA_InputEvent(SCA_InputEvent::KX_NO_INPUTSTATUS,0);
}



const SCA_InputEvent& SCA_IInputDevice::GetEventValue(SCA_IInputDevice::KX_EnumInputs inputcode)
{
  //	cerr << "SCA_IInputDevice::GetEventValue" << endl;
	return m_eventStatusTables[m_currentTable][inputcode];
}



int SCA_IInputDevice::GetNumActiveEvents()
{
	int num = 0;

	//	cerr << "SCA_IInputDevice::GetNumActiveEvents" << endl;

	for (int i=0;i<SCA_IInputDevice::KX_MAX_KEYS;i++)
	{
		const SCA_InputEvent& event = m_eventStatusTables[m_currentTable][i];
		if ((event.m_status == SCA_InputEvent::KX_JUSTACTIVATED)
			|| (event.m_status == SCA_InputEvent::KX_ACTIVE))
			num++;
	}

	return num;
}



int SCA_IInputDevice::GetNumJustEvents()
{
	int num = 0;

	//	cerr << "SCA_IInputDevice::GetNumJustEvents" << endl;

	for (int i=0;i<SCA_IInputDevice::KX_MAX_KEYS;i++)
	{
		const SCA_InputEvent& event = m_eventStatusTables[m_currentTable][i];
		if ((event.m_status == SCA_InputEvent::KX_JUSTACTIVATED)
			|| (event.m_status == SCA_InputEvent::KX_JUSTRELEASED))
			num++;
	}

	return num;
}



void SCA_IInputDevice::NextFrame()
{
	m_currentTable = 1 - m_currentTable;

	//	cerr << "SCA_IInputDevice::NextFrame " << GetNumActiveEvents() << endl;
	
	for (int i=0;i<SCA_IInputDevice::KX_MAX_KEYS;i++)
	{
		switch (m_eventStatusTables[1 - m_currentTable][i].m_status)
		{
		case SCA_InputEvent::KX_NO_INPUTSTATUS:
			m_eventStatusTables[m_currentTable][i]
				= SCA_InputEvent(SCA_InputEvent::KX_NO_INPUTSTATUS, 1);
			break;
		case SCA_InputEvent::KX_JUSTACTIVATED:
			m_eventStatusTables[m_currentTable][i]
				= SCA_InputEvent(SCA_InputEvent::KX_ACTIVE, 1);
			break;
		case SCA_InputEvent::KX_ACTIVE:
			m_eventStatusTables[m_currentTable][i]
				= SCA_InputEvent(SCA_InputEvent::KX_ACTIVE, 1);
			break;
		case SCA_InputEvent::KX_JUSTRELEASED:
			m_eventStatusTables[m_currentTable][i]
				= SCA_InputEvent(SCA_InputEvent::KX_NO_INPUTSTATUS, 1);
			break;
		default:
			; /* error */
		}	
	}
}

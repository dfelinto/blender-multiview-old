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

#include "GPC_MouseDevice.h"


GPC_MouseDevice::GPC_MouseDevice()
{

}
GPC_MouseDevice::~GPC_MouseDevice()
{

}

/**
 * IsPressed gives boolean information about mouse status, true if pressed, false if not.
 */
bool GPC_MouseDevice::IsPressed(SCA_IInputDevice::KX_EnumInputs inputcode)
{
	const SCA_InputEvent & inevent =  m_eventStatusTables[m_currentTable][inputcode];
	bool pressed = (inevent.m_status == SCA_InputEvent::KX_JUSTACTIVATED || 
		inevent.m_status == SCA_InputEvent::KX_ACTIVE);
	return pressed;
}


/** 
 * NextFrame toggles currentTable with previousTable,
 * and copies relevant event information from previous to current table
 * (pressed keys need to be remembered).
 */
void GPC_MouseDevice::NextFrame()
{
	SCA_IInputDevice::NextFrame();
	
	// Convert just pressed events into regular (active) events
	int previousTable = 1-m_currentTable;
	for (int mouseevent= KX_BEGINMOUSE; mouseevent< KX_ENDMOUSEBUTTONS; mouseevent++) {
		SCA_InputEvent& oldevent = m_eventStatusTables[previousTable][mouseevent];
		if (oldevent.m_status == SCA_InputEvent::KX_JUSTACTIVATED ||
			oldevent.m_status == SCA_InputEvent::KX_ACTIVE) {
			m_eventStatusTables[m_currentTable][mouseevent] = oldevent;
			m_eventStatusTables[m_currentTable][mouseevent].m_status = SCA_InputEvent::KX_ACTIVE;
		}
	}
	for (int mousemove= KX_ENDMOUSEBUTTONS; mousemove< KX_ENDMOUSE; mousemove++) {
		SCA_InputEvent& oldevent = m_eventStatusTables[previousTable][mousemove];
		m_eventStatusTables[m_currentTable][mousemove] = oldevent;
		if (oldevent.m_status == SCA_InputEvent::KX_JUSTACTIVATED ||
			oldevent.m_status == SCA_InputEvent::KX_ACTIVE) {	
			m_eventStatusTables[m_currentTable][mousemove].m_status = SCA_InputEvent::KX_JUSTRELEASED;
		}
		else {
			if (oldevent.m_status == SCA_InputEvent::KX_JUSTRELEASED) {
				m_eventStatusTables[m_currentTable][mousemove].m_status = SCA_InputEvent::KX_NO_INPUTSTATUS ;
			}
		}
	}
}


bool GPC_MouseDevice::ConvertButtonEvent(TButtonId button, bool isDown)
{
	bool result = false;

	switch (button)
	{
	case buttonLeft:
		result = ConvertEvent(KX_LEFTMOUSE, isDown);
		break;
	case buttonMiddle:
		result = ConvertEvent(KX_MIDDLEMOUSE, isDown);
		break;
	case buttonRight:
		result = ConvertEvent(KX_RIGHTMOUSE, isDown);
		break;
	default:
		// Should not happen!
		break;
	}

	return result;
}

/**
 * Splits combined button and x,y cursor move events into separate Ketsji
 * x and y move and button events.
 */
bool GPC_MouseDevice::ConvertButtonEvent(TButtonId button, bool isDown, int x, int y)
{
	// First update state tables for cursor move.
	bool result = ConvertMoveEvent(x, y);

	// Now update for button state.
	if (result) {
		result = ConvertButtonEvent(button, isDown);
	}

	return result;
}

/**
 * Splits combined x,y move into separate Ketsji x and y move events.
 */
bool GPC_MouseDevice::ConvertMoveEvent(int x, int y)
{
	bool result;

	// Convert to local coordinates?
	result = ConvertEvent(KX_MOUSEX, x);
	if (result) {
		result = ConvertEvent(KX_MOUSEY, y);
	}

	return result;
}


bool GPC_MouseDevice::ConvertEvent(KX_EnumInputs kxevent, int eventval)
{
	bool result = true;
	
	// Only process it, if it's a mouse event
	if (kxevent > KX_BEGINMOUSE && kxevent < KX_ENDMOUSE) {
		int previousTable = 1-m_currentTable;

		if (eventval > 0) {
			m_eventStatusTables[m_currentTable][kxevent].m_eventval = eventval;

			switch (m_eventStatusTables[previousTable][kxevent].m_status)
			{
			case SCA_InputEvent::KX_ACTIVE:
			case SCA_InputEvent::KX_JUSTACTIVATED:
				{
					m_eventStatusTables[m_currentTable][kxevent].m_status = SCA_InputEvent::KX_ACTIVE;
					break;
				}
			case SCA_InputEvent::KX_JUSTRELEASED:
				{
					if ( kxevent > KX_BEGINMOUSEBUTTONS && kxevent < KX_ENDMOUSEBUTTONS)
					{
						m_eventStatusTables[m_currentTable][kxevent].m_status = SCA_InputEvent::KX_JUSTACTIVATED;
					} else
					{
						m_eventStatusTables[m_currentTable][kxevent].m_status = SCA_InputEvent::KX_ACTIVE;
						
					}
					break;
				}
			default:
				{
					m_eventStatusTables[m_currentTable][kxevent].m_status = SCA_InputEvent::KX_JUSTACTIVATED;
				}
			}
			
		} 
		else {
			switch (m_eventStatusTables[previousTable][kxevent].m_status)
			{
			case SCA_InputEvent::KX_JUSTACTIVATED:
			case SCA_InputEvent::KX_ACTIVE:
				{
					m_eventStatusTables[m_currentTable][kxevent].m_status = SCA_InputEvent::KX_JUSTRELEASED;
					break;
				}
			default:
				{
					m_eventStatusTables[m_currentTable][kxevent].m_status = SCA_InputEvent::KX_NO_INPUTSTATUS;
				}
			}
		}
	}
	else {
		result = false;
	}
	return result;
}

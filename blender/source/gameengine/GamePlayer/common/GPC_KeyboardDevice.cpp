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

#include <iostream.h>
#include "GPC_KeyboardDevice.h"


/** 
 * NextFrame toggles currentTable with previousTable,
 * and copies relevant event information from previous to current table
 * (pressed keys need to be remembered).
 */
void GPC_KeyboardDevice::NextFrame()
{
	SCA_IInputDevice::NextFrame();

	// Now convert justpressed key events into regular (active) keyevents
	int previousTable = 1-m_currentTable;
	for (int keyevent= KX_BEGINKEY; keyevent< KX_ENDKEY;keyevent++)
	{
		SCA_InputEvent& oldevent = m_eventStatusTables[previousTable][keyevent];
		if (oldevent.m_status == SCA_InputEvent::KX_JUSTACTIVATED ||
			oldevent.m_status == SCA_InputEvent::KX_ACTIVE	)
		{
			m_eventStatusTables[m_currentTable][keyevent] = oldevent;
			m_eventStatusTables[m_currentTable][keyevent].m_status = SCA_InputEvent::KX_ACTIVE;
			//m_eventStatusTables[m_currentTable][keyevent].m_status = SCA_InputEvent::KX_JUSTACTIVATED ;
		}
	}
}



/** 
 * ConvertBPEvent translates Windows keyboard events into ketsji kbd events.
 * Extra event information is stored, like ramp-mode (just released/pressed)
 */
bool GPC_KeyboardDevice::ConvertEvent(int incode, int val)
{
	bool result = false;

	// convert event
	KX_EnumInputs kxevent = this->ToNative(incode);

	// only process it, if it's a key
	if (kxevent >= KX_BEGINKEY && kxevent < KX_ENDKEY)
	{
		int previousTable = 1-m_currentTable;

		if (val > 0)
		{
			if (kxevent == SCA_IInputDevice::KX_ESCKEY && val != 0)
				result = true;

			// todo: convert val ??
			m_eventStatusTables[m_currentTable][kxevent].m_eventval = val ; //???

			switch (m_eventStatusTables[previousTable][kxevent].m_status)
			{
			case SCA_InputEvent::KX_JUSTACTIVATED:
			case SCA_InputEvent::KX_ACTIVE:
				{
					m_eventStatusTables[m_currentTable][kxevent].m_status = SCA_InputEvent::KX_ACTIVE;
					break;
				}

			case SCA_InputEvent::KX_NO_INPUTSTATUS:
			default:
				{
					m_eventStatusTables[m_currentTable][kxevent].m_status = SCA_InputEvent::KX_JUSTACTIVATED;
				}
			}
			
		} else
		{

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
	} else {
		exit(1);
	}	
	return result;
}

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

/**

 * $Id$
 * Copyright (C) 2001 NaN Technologies B.V.
 * @author	Maarten Gribnau
 * @date	May 11, 2001
 */

#ifndef _GHOST_EVENT_BUTTON_H_
#define _GHOST_EVENT_BUTTON_H_

#include "GHOST_Event.h"

/**
 * Mouse button event.
 * @author	Maarten Gribnau
 * @date	May 11, 2001
 */
class GHOST_EventButton : public GHOST_Event
{
public:
	/**
	 * Constructor.
	 * @param time		The time this event was generated.
	 * @param type		The type of this event.
	 * @param x			The x-coordinate of the location the cursor was at at the time of the event.
	 * @param y			The y-coordinate of the location the cursor was at at the time of the event.
	 * @param buttons	The state of the buttons  was at at the time of the event.
	 */
	GHOST_EventButton(GHOST_TUns64 time, GHOST_TEventType type, GHOST_IWindow* window, GHOST_TButtonMask button)
		: GHOST_Event(time, type, window)
	{
		m_buttonEventData.button = button;
		m_data = &m_buttonEventData;
	}

protected:
	/** The button event data. */
	GHOST_TEventButtonData m_buttonEventData;
};

#endif // _GHOST_EVENT_BUTTON_H_

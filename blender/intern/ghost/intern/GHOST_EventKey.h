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

#ifndef _GHOST_EVENT_KEY_H_
#define _GHOST_EVENT_KEY_H_

#include "GHOST_Event.h"

/**
 * Key event.
 * @author	Maarten Gribnau
 * @date	May 11, 2001
 */
class GHOST_EventKey : public GHOST_Event
{
public:
	/**
	 * Constructor.
	 * @param msec	The time this event was generated.
	 * @param type	The type of key event.
	 * @param key	The key code of the key.
	 */
	GHOST_EventKey(GHOST_TUns64 msec, GHOST_TEventType type, GHOST_IWindow* window, GHOST_TKey key)
		: GHOST_Event(msec, type, window)
	{
		m_keyEventData.key = key;
		m_keyEventData.ascii = '\0';
		m_data = &m_keyEventData;
	}
	
	/**
	 * Constructor.
	 * @param msec	The time this event was generated.
	 * @param type	The type of key event.
	 * @param key	The key code of the key.
	 * @param ascii The ascii code for the key event.
	 */
	GHOST_EventKey(GHOST_TUns64 msec, GHOST_TEventType type, GHOST_IWindow* window, GHOST_TKey key, char ascii)
		: GHOST_Event(msec, type, window)
	{
		m_keyEventData.key = key;
		m_keyEventData.ascii = ascii;
		m_data = &m_keyEventData;
	}
		
protected:
	/** The key event data. */
	GHOST_TEventKeyData m_keyEventData;
};

#endif // _GHOST_EVENT_KEY_H_

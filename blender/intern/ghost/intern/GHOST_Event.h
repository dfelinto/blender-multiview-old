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

#ifndef _GHOST_EVENT_H_
#define _GHOST_EVENT_H_

#include "GHOST_IEvent.h"


/**
 * Base class for events received the operating system.
 * @author	Maarten Gribnau
 * @date	May 11, 2001
 */

class GHOST_Event : public GHOST_IEvent
{
public:
	/**
	 * Constructor.
	 * @param msec	The time this event was generated.
	 * @param type	The type of this event.
	 * @param window The generating window (or NULL if system event).
	 */
	GHOST_Event(GHOST_TUns64 msec, GHOST_TEventType type, GHOST_IWindow* window)
		: m_type(type), m_time(msec), m_window(window), m_data(0)
	{
	}

	/**
	 * Returns the event type.
	 * @return The event type.
	 */
	virtual GHOST_TEventType getType()
	{ 
		return m_type;
	}

	/**
	 * Returns the time this event was generated.
	 * @return The event generation time.
	 */
	virtual GHOST_TUns64 getTime()
	{
		return m_time;
	}

	/**
	 * Returns the window this event was generated on, 
	 * or NULL if it is a 'system' event.
	 * @return The generating window.
	 */
	virtual GHOST_IWindow* getWindow()
	{
		return m_window;
	}

	/**
	 * Returns the event data.
	 * @return The event data.
	 */
	virtual GHOST_TEventDataPtr getData()
	{
		return m_data;
	}

protected:
	/** Type of this event. */
	GHOST_TEventType m_type;
	/** The time this event was generated. */
	GHOST_TUns64 m_time;
	/** Pointer to the generating window. */
	GHOST_IWindow* m_window;
	/** Pointer to the event data. */
	GHOST_TEventDataPtr m_data;
};

#endif // _GHOST_EVENT_H_

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
 * @date	May 31, 2001
 */

#ifndef _GHOST_IEVENT_H_
#define _GHOST_IEVENT_H_

#include "GHOST_Types.h"

class GHOST_IWindow;

/**
 * Interface class for events received the operating system.
 * @author	Maarten Gribnau
 * @date	May 31, 2001
 */
class GHOST_IEvent
{
public:
	/**
	 * Destructor.
	 */
	virtual ~GHOST_IEvent()
	{
	}

	/**
	 * Returns the event type.
	 * @return The event type.
	 */
	virtual GHOST_TEventType getType() = 0;

	/**
	 * Returns the time this event was generated.
	 * @return The event generation time.
	 */
	virtual GHOST_TUns64 getTime() = 0;

	/**
	 * Returns the window this event was generated on, 
	 * or NULL if it is a 'system' event.
	 * @return The generating window.
	 */
	virtual GHOST_IWindow* getWindow() = 0;
	
	/**
	 * Returns the event data.
	 * @return The event data.
	 */
	virtual GHOST_TEventDataPtr getData() = 0;
};

#endif // _GHOST_IEVENT_H_

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
 * @date	May 14, 2001
 */

#ifndef _GHOST_IEVENT_CONSUMER_H_
#define _GHOST_IEVENT_CONSUMER_H_

#include "GHOST_IEvent.h"

/**
 * Interface class for objects interested in receiving events.
 */
class GHOST_IEventConsumer
{
public:
	/**
	 * Destructor.
	 */
	virtual ~GHOST_IEventConsumer()
	{
	}

	/**
	 * This method is called by an event producer when an event is available.
	 * @param event	The event that can be handled or ignored.
	 * @return Indication as to whether the event was handled.
	 */
	virtual	bool processEvent(GHOST_IEvent* event) = 0;
};

#endif // _GHOST_EVENT_CONSUMER_H_

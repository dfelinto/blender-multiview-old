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

#ifndef _GHOST_EVENT_PRINTER_H_
#define _GHOST_EVENT_PRINTER_H_

#include "GHOST_IEventConsumer.h"

#include "STR_String.h"

/**
 * An Event consumer that prints all the events to standard out.
 * Really useful when debugging.
 */
class GHOST_EventPrinter : public GHOST_IEventConsumer
{
public:
	/**
	 * Prints all the events received to std out.
	 * @param event	The event that can be handled or not.
	 * @return Indication as to whether the event was handled.
	 */
	virtual	bool processEvent(GHOST_IEvent* event);

protected:
	/**
	 * Converts GHOST key code to a readable string.
	 * @param key The GHOST key code to convert.
	 * @param str The GHOST key code converted to a readable string.
	 */
	void	getKeyString(GHOST_TKey key, STR_String& str) const;
};

#endif // _GHOST_EVENT_PRINTER_H_

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

#ifndef _GHOST_ITIMER_TASK_H_
#define _GHOST_ITIMER_TASK_H_


#include "GHOST_Types.h"


/**
 * Interface for a timer task.
 * @author	Maarten Gribnau
 * @date	May 31, 2001
 */
class GHOST_ITimerTask
{
public:
	/**
	 * Destructor.
	 */
	virtual ~GHOST_ITimerTask()
	{
	}

	/**
	 * Returns the timer callback.
	 * @return The timer callback.
	 */
	inline virtual GHOST_TimerProcPtr getTimerProc() const = 0;

	/**
	 * Changes the timer callback.
	 * @param timerProc The timer callback.
	 */
	inline virtual void setTimerProc(const GHOST_TimerProcPtr timerProc) = 0;

	/**
	 * Returns the timer user data.
	 * @return The timer user data.
	 */
	inline virtual GHOST_TUserDataPtr getUserData() const = 0;
	
	/**
	 * Changes the time user data.
	 * @param data The timer user data.
	 */
	virtual void setUserData(const GHOST_TUserDataPtr userData) = 0;
};


#endif // _GHOST_ITIMER_TASK_H_

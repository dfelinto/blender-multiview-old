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
#ifndef __KX_EVENTMANAGER
#define __KX_EVENTMANAGER

#include <vector>
#include <algorithm>

class SCA_EventManager
{
protected:
	std::vector <class SCA_ISensor*>				m_sensors;

public:
	enum EVENT_MANAGER_TYPE {
		KEYBOARD_EVENTMGR = 0,
		MOUSE_EVENTMGR,
		ALWAYS_EVENTMGR, 
		TOUCH_EVENTMGR, 
		PROPERTY_EVENTMGR,
		TIME_EVENTMGR,
		RANDOM_EVENTMGR,
		RAY_EVENTMGR,
		RADAR_EVENTMGR,
		NETWORK_EVENTMGR
	};

	SCA_EventManager(EVENT_MANAGER_TYPE mgrtype);
	virtual ~SCA_EventManager();
	
	virtual void	RemoveSensor(class SCA_ISensor* sensor);
	virtual void	NextFrame(double curtime,double deltatime)=0;
	virtual void	EndFrame();
	virtual void	RegisterSensor(class SCA_ISensor* sensor)=0;
	int		GetType();

protected:
	EVENT_MANAGER_TYPE		m_mgrtype;
};
#endif

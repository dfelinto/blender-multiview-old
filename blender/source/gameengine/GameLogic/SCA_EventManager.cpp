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


#include "SCA_EventManager.h"



SCA_EventManager::SCA_EventManager(EVENT_MANAGER_TYPE mgrtype)
	:m_mgrtype(mgrtype)
{
}



SCA_EventManager::~SCA_EventManager()
{
}



void SCA_EventManager::RemoveSensor(class SCA_ISensor* sensor)
{
	std::vector<SCA_ISensor*>::iterator i =
	std::find(m_sensors.begin(), m_sensors.end(), sensor);
	if (!(i == m_sensors.end()))
	{
		std::swap(*i, m_sensors.back());
		m_sensors.pop_back();
	}
}


void SCA_EventManager::EndFrame()
{
}



int SCA_EventManager::GetType()
{
	return (int) m_mgrtype;
}

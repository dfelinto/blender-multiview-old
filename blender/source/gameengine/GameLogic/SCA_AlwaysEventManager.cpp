/**
 * Manager for 'always' events. Since always sensors can operate in pulse
 * mode, they need to be activated.
 *
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

#include "SCA_AlwaysEventManager.h"
#include "SCA_LogicManager.h"
#include <vector>
#include "SCA_ISensor.h"

using namespace std;



SCA_AlwaysEventManager::SCA_AlwaysEventManager(class SCA_LogicManager* logicmgr)
	: m_logicmgr(logicmgr),
	SCA_EventManager(ALWAYS_EVENTMGR)
{
}



void SCA_AlwaysEventManager::NextFrame(double curtime,double deltatime)
{
	for (vector<class SCA_ISensor*>::const_iterator i= m_sensors.begin();!(i==m_sensors.end());i++)
	{
		SCA_ISensor* sensor = *i;
		sensor->Activate(m_logicmgr, NULL);
	}
}



void SCA_AlwaysEventManager::RegisterSensor(SCA_ISensor* sensor)
{
	m_sensors.push_back(sensor);
}

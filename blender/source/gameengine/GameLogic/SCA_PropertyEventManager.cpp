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

#include "SCA_ISensor.h"

#include "SCA_PropertyEventManager.h"



SCA_PropertyEventManager::SCA_PropertyEventManager(class SCA_LogicManager* logicmgr)
	: SCA_EventManager(PROPERTY_EVENTMGR),
	m_logicmgr(logicmgr)
{
}



SCA_PropertyEventManager::~SCA_PropertyEventManager()
{

}



void SCA_PropertyEventManager::RegisterSensor(SCA_ISensor* sensor)
{
	m_sensors.push_back(sensor);
}



void SCA_PropertyEventManager::NextFrame(double curtime,double deltatime)
{
	// check for changed properties
	for (vector<SCA_ISensor*>::const_iterator it = m_sensors.begin();!(it==m_sensors.end());it++)
	{
		(*it)->Activate(m_logicmgr,NULL);
	}
}

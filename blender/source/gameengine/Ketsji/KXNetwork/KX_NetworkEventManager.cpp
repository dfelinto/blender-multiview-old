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
 * Ketsji Logic Extenstion: Network Event Manager generic implementation
 */

// Ketsji specific sensor part
#include "SCA_ISensor.h"

// Ketsji specific network part
#include "KX_NetworkEventManager.h"

// Network module specific
#include "NG_NetworkDeviceInterface.h"
#include "NG_NetworkMessage.h"
#include "NG_NetworkObject.h"

KX_NetworkEventManager::KX_NetworkEventManager(class SCA_LogicManager*
logicmgr, class NG_NetworkDeviceInterface *ndi) :
SCA_EventManager(NETWORK_EVENTMGR), m_logicmgr(logicmgr), m_ndi(ndi)
{
	//printf("KX_NetworkEventManager constructor\n");
}

KX_NetworkEventManager::~KX_NetworkEventManager()
{
	//printf("KX_NetworkEventManager destructor\n");
}

void KX_NetworkEventManager::RegisterSensor(class SCA_ISensor* sensor)
{
	//printf("KX_NetworkEventManager RegisterSensor\n");
	m_sensors.push_back(sensor);
}

void KX_NetworkEventManager::RemoveSensor(class SCA_ISensor* sensor)
{
	//printf("KX_NetworkEventManager RemoveSensor\n");
	// Network specific RemoveSensor stuff goes here

	// parent
	SCA_EventManager::RemoveSensor(sensor);
}

void KX_NetworkEventManager::NextFrame(double curtime, double deltatime)
{
// printf("KX_NetworkEventManager::proceed %.2f - %.2f\n", curtime, deltatime);
	// each frame, the logicmanager will call the network
	// eventmanager to look for network events, and process it's
	// 'network' sensors
	vector<class SCA_ISensor*>::iterator it;

	for (it = m_sensors.begin(); !(it==m_sensors.end()); it++) {
//	    printf("KX_NetworkEventManager::proceed sensor %.2f\n", curtime);
	    // process queue
	    (*it)->Activate(m_logicmgr, NULL);
	}

	// now a list of triggerer sensors has been built
}

void KX_NetworkEventManager::EndFrame()
{
}


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

#include "KX_TouchEventManager.h"

#include "SCA_ISensor.h"
#include "KX_TouchSensor.h"
#include "KX_GameObject.h"

#ifdef PHYSICS_NOT_YET

KX_TouchEventManager::KX_TouchEventManager(class SCA_LogicManager* logicmgr,
										   DT_RespTableHandle resphandle,
										   DT_SceneHandle scenehandle)
	: SCA_EventManager(TOUCH_EVENTMGR),
	  m_resphandle(resphandle),
	  m_scenehandle(scenehandle),
	  m_logicmgr(logicmgr) {}

void KX_TouchEventManager::RegisterSensor(SCA_ISensor* sensor)
{


	KX_TouchSensor* touchsensor = static_cast<KX_TouchSensor*>(sensor);
	m_sensors.push_back(touchsensor);

	touchsensor->RegisterSumo();//this,m_resphandle);

	//KX_GameObject* gameobj = ((KX_GameObject*)sensor->GetParent());
//	SM_Object* smobj = touchsensor->GetSumoObject();//gameobj->GetSumoObject();
//	if (smobj)
//	{
//		smobj->calcXform();
//		DT_AddObject(m_scenehandle,
//				 smobj->getObjectHandle()); 
//	}
}



void KX_TouchEventManager::EndFrame()
{
	vector<SCA_ISensor*>::iterator it;
	for ( it = m_sensors.begin();
	!(it==m_sensors.end());it++)
	{
		((KX_TouchSensor*)*it)->EndFrame();

	}
}



void KX_TouchEventManager::NextFrame(double curtime,double deltatime)
{
	if (m_sensors.size() > 0)
	{
		vector<SCA_ISensor*>::iterator it;
		
		for (it = m_sensors.begin();!(it==m_sensors.end());it++)
			((KX_TouchSensor*)*it)->SynchronizeTransform();
		
		if (DT_Test(m_scenehandle,m_resphandle))
			int i = 0;
		
		for (it = m_sensors.begin();!(it==m_sensors.end());it++)
			(*it)->Activate(m_logicmgr,NULL);
	}
}



void KX_TouchEventManager::RemoveSensor(class SCA_ISensor* sensor)
{
	std::vector<SCA_ISensor*>::iterator i =
	std::find(m_sensors.begin(), m_sensors.end(), sensor);
	if (!(i == m_sensors.end()))
	{
		//std::swap(*i, m_sensors.back());
		//m_sensors.pop_back();
		//SM_Object* smobj = ((KX_TouchSensor*)*i)->GetSumoObject();
		//DT_RemoveObject(m_scenehandle,
		//		 smobj->getObjectHandle()); 
	}
	// remove the sensor forever :)
	SCA_EventManager::RemoveSensor(sensor);
}

#endif

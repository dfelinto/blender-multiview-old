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

#include "SCA_IController.h"

#include "SCA_LogicManager.h"
#include "SCA_IActuator.h"

SCA_IController::SCA_IController(SCA_IObject* gameobj,
								 PyTypeObject* T)
	:
	SCA_ILogicBrick(gameobj,T)
{
}
	

	
SCA_IController::~SCA_IController()
{
}



const std::vector<class SCA_ISensor*>& SCA_IController::GetLinkedSensors()
{
	return m_linkedsensors;
}



const std::vector<class SCA_IActuator*>& SCA_IController::GetLinkedActuators()
{
	return m_linkedactuators;
}



void SCA_IController::UnlinkAllSensors()
{
	m_linkedsensors.clear();
}



void SCA_IController::UnlinkAllActuators()
{
	m_linkedactuators.clear();
}



/*
void SCA_IController::Trigger(SCA_LogicManager* logicmgr)
{
	//for (int i=0;i<m_linkedactuators.size();i++)
	for (vector<SCA_IActuator*>::const_iterator i=m_linkedactuators.begin();
	!(i==m_linkedactuators.end());i++)
	{
		SCA_IActuator* actua = *i;//m_linkedactuators.at(i);
		
		logicmgr->AddActiveActuator(actua);
	}

}
*/

void SCA_IController::LinkToActuator(SCA_IActuator* actua)
{
	m_linkedactuators.push_back(actua);
}

void	SCA_IController::UnlinkActuator(class SCA_IActuator* actua)
{
	std::vector<class SCA_IActuator*>::iterator actit;
	std::vector<class SCA_IActuator*>::iterator actfound = m_linkedactuators.end();
	for (actit = m_linkedactuators.begin();!(actit==m_linkedactuators.end());++actit)
	{
		if ((*actit) == actua)
			actfound = actit;
		
	}
	if (!(actfound==m_linkedactuators.end()))
	{
		m_linkedactuators.erase(actfound);
	}
	
}

void SCA_IController::LinkToSensor(SCA_ISensor* sensor)
{
	m_linkedsensors.push_back(sensor);
}

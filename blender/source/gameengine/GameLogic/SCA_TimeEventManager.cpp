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

#ifdef WIN32

// This warning tells us about truncation of __long__ stl-generated names.
// It can occasionally cause DevStudio to have internal compiler warnings.
#pragma warning( disable : 4786 )     
#endif

#include "SCA_TimeEventManager.h"

#include "SCA_LogicManager.h"
#include "FloatValue.h"

SCA_TimeEventManager::SCA_TimeEventManager(SCA_LogicManager* logicmgr)
: SCA_EventManager(TIME_EVENTMGR)
{
}



SCA_TimeEventManager::~SCA_TimeEventManager()
{
	for (vector<CValue*>::iterator it = m_timevalues.begin();
			!(it == m_timevalues.end()); it++)
	{
		(*it)->Release();
	}	
}



void SCA_TimeEventManager::RegisterSensor(SCA_ISensor* sensor)
{
	// not yet
}



void SCA_TimeEventManager::NextFrame(double curtime,double deltatime)
{
	if (m_timevalues.size() > 0)
	{
		CFloatValue* floatval = new CFloatValue(curtime);
		
		// update sensors, but ... need deltatime !
		for (vector<CValue*>::iterator it = m_timevalues.begin();
		!(it == m_timevalues.end()); it++)
		{
			float newtime = (*it)->GetNumber() + deltatime;
			floatval->SetFloat(newtime);
			(*it)->SetValue(floatval);
		}
		
		floatval->Release();
	}
}



void SCA_TimeEventManager::AddTimeProperty(CValue* timeval)
{
	timeval->AddRef();
	m_timevalues.push_back(timeval);
}



void SCA_TimeEventManager::RemoveTimeProperty(CValue* timeval)
{
	for (vector<CValue*>::iterator it = m_timevalues.begin();
			!(it == m_timevalues.end()); it++)
	{
		if ((*it) == timeval)
		{
			this->m_timevalues.erase(it);
			timeval->Release();
			break;
		}
	}
}

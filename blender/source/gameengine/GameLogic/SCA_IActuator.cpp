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

#include "SCA_IActuator.h"


using namespace std;

SCA_IActuator::SCA_IActuator(SCA_IObject* gameobj,
							 PyTypeObject* T) :
	SCA_ILogicBrick(gameobj,T) 
{
	// nothing to do
}



void SCA_IActuator::AddEvent(CValue* event)
{
	m_events.push_back(event);
}



void SCA_IActuator::RemoveAllEvents()
{	// remove event queue!
	for (vector<CValue*>::iterator i=m_events.begin(); !(i==m_events.end());i++)
	{
		(*i)->Release();
	}
	m_events.clear();
}



bool SCA_IActuator::Update(double curtime,double deltatime)
{
	return true;
}



bool SCA_IActuator::IsNegativeEvent() const
{
	bool bPositiveEvent(false);
	bool bNegativeEvent(false);

	for (vector<CValue*>::const_iterator i=m_events.begin(); i!=m_events.end();++i)
	{
		if ((*i)->GetNumber() == 0.0f)
		{
			bNegativeEvent = true;
		} else {
			bPositiveEvent = true;
		}
	}

	// if at least 1 positive event, return false
	
	return !bPositiveEvent && bNegativeEvent;
}



void SCA_IActuator::ProcessReplica()
{
	m_events.clear();
}



SCA_IActuator::~SCA_IActuator()
{
	RemoveAllEvents();
}


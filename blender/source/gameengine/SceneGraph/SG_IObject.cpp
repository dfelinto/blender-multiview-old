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

#include "SG_IObject.h"

#include "SG_Controller.h"

SG_IObject::
SG_IObject(
	void* clientobj,
	void* clientinfo,
	SG_Callbacks callbacks
): 
	m_SGclientObject(clientobj),
	m_SGclientInfo(clientinfo),
	m_callbacks(callbacks) 
{
	//nothing to do
}

SG_IObject::
SG_IObject(
	const SG_IObject &other
) :
	m_SGclientObject(other.m_SGclientObject),
	m_SGclientInfo(other.m_SGclientInfo),
	m_callbacks(other.m_callbacks) 
{
	//nothing to do
}

	void 
SG_IObject::
AddSGController(
	SG_Controller* cont
){
	m_SGcontrollers.push_back(cont);
}

	void				
SG_IObject::
RemoveAllControllers(
) { 
	m_SGcontrollers.clear(); 
}

/// Needed for replication
	SGControllerList&	
SG_IObject::
GetSGControllerList(
){ 
	return m_SGcontrollers; 
}

	void*				
SG_IObject::
GetSGClientObject(
){ 
	return m_SGclientObject;
}

const 
	void*			
SG_IObject::
GetSGClientObject(
) const	{
	return m_SGclientObject;
}

	void	
SG_IObject::
SetSGClientObject(
	void* clientObject
){
	m_SGclientObject = clientObject;
}


	void
SG_IObject::
ActivateReplicationCallback(
	SG_IObject *replica
){
	if (m_callbacks.m_replicafunc)
	{
		// Call client provided replication func
		m_callbacks.m_replicafunc(replica,m_SGclientObject,m_SGclientInfo);
	}
};	

	void
SG_IObject::
ActivateDestructionCallback(
){
	if (m_callbacks.m_destructionfunc)
	{
		// Call client provided destruction function on this!
		m_callbacks.m_destructionfunc(this,m_SGclientObject,m_SGclientInfo);
	}
}

	void 
SG_IObject::
SetControllerTime(
	double time
){
	SGControllerList::iterator contit;

	for (contit = m_SGcontrollers.begin();contit!=m_SGcontrollers.end();++contit)
	{
		(*contit)->SetSimulatedTime(time);
	}
}


SG_IObject::
~SG_IObject()
{
	SGControllerList::iterator contit;

	for (contit = m_SGcontrollers.begin();contit!=m_SGcontrollers.end();++contit)
	{
		delete (*contit);
	}
}

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
#ifndef __KX_ICONTROLLER
#define __KX_ICONTROLLER

#include "SCA_ILogicBrick.h"

class SCA_IController : public SCA_ILogicBrick
{
protected:
	std::vector<class SCA_ISensor*>		m_linkedsensors;
	std::vector<class SCA_IActuator*>	m_linkedactuators;
public:
	SCA_IController(SCA_IObject* gameobj,PyTypeObject* T);
	virtual ~SCA_IController();
	virtual void Trigger(class SCA_LogicManager* logicmgr)=0;
	void	LinkToSensor(SCA_ISensor* sensor);
	void	LinkToActuator(SCA_IActuator*);
	const std::vector<class SCA_ISensor*>&		GetLinkedSensors();
	const std::vector<class SCA_IActuator*>&	GetLinkedActuators();
	void	UnlinkAllSensors();
	void	UnlinkAllActuators();
	void	UnlinkActuator(class SCA_IActuator* actua);


};
#endif

/**
 * Manager for mouse events
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


#ifndef __KX_MOUSEMANAGER
#define __KX_MOUSEMANAGER


#include "SCA_EventManager.h"

#include <vector>

using namespace std;

#include "SCA_IInputDevice.h"


class SCA_MouseManager : public SCA_EventManager
{

	class 	SCA_IInputDevice*				m_mousedevice;
	class 	SCA_LogicManager*				m_logicmanager;
	
	unsigned short m_xpos; // Cached location of the mouse pointer
	unsigned short m_ypos;
	
public:
	SCA_MouseManager(class SCA_LogicManager* logicmgr,class SCA_IInputDevice* mousedev);
	virtual ~SCA_MouseManager();

	/**
	 * Checks whether a mouse button is depressed. Ignores requests on non-
	 * mouse related evenst. Can also flag mouse movement.
	 */
	bool IsPressed(SCA_IInputDevice::KX_EnumInputs inputcode);
	virtual void 	NextFrame(double curtime,double deltatime);	
	virtual void	RegisterSensor(class SCA_ISensor* sensor);
	SCA_IInputDevice* GetInputDevice();
};

#endif //__KX_MOUSEMANAGER

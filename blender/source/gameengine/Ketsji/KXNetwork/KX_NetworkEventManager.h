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
 * Ketsji Logic Extenstion: Network Event Manager class
 */
#ifndef KX_NETWORK_EVENTMANAGER_H
#define KX_NETWORK_EVENTMANAGER_H

#include "SCA_EventManager.h"

class KX_NetworkEventManager : public SCA_EventManager
{
	class SCA_LogicManager* m_logicmgr;
	class NG_NetworkDeviceInterface* m_ndi;

public:
	KX_NetworkEventManager(class SCA_LogicManager* logicmgr,
			       class NG_NetworkDeviceInterface *ndi);
	virtual ~KX_NetworkEventManager ();

	virtual void RegisterSensor(class SCA_ISensor* sensor);
	virtual void RemoveSensor(class SCA_ISensor* sensor);

	virtual void NextFrame(double curtime, double deltatime);
	virtual void EndFrame();

	SCA_LogicManager* GetLogicManager() { return m_logicmgr; }
	class NG_NetworkDeviceInterface* GetNetworkDevice() {
	    return m_ndi; }
};

#endif //KX_NETWORK_EVENTMANAGER_H

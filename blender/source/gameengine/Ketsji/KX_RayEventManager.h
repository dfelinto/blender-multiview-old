/**
 * Manager for ray events
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

#ifndef __KX_RAYEVENTMGR
#define __KX_RAYEVENTMGR
#include "SCA_EventManager.h"
#include <vector>
using namespace std;
class KX_RayEventManager : public SCA_EventManager
{

	class SCA_LogicManager* m_logicmgr;
public:
	KX_RayEventManager(class SCA_LogicManager* logicmgr)
		: m_logicmgr(logicmgr),
		SCA_EventManager(RAY_EVENTMGR)
	{}
	virtual void NextFrame(double curtime,double deltatime);
	virtual void	RegisterSensor(SCA_ISensor* sensor);
};
#endif //__KX_RAYEVENTMGR

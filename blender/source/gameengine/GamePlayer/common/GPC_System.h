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

#ifndef __GPC_SYSTEM_H
#define __GPC_SYSTEM_H

#if defined(WIN32)
#pragma warning (disable:4786) // suppress stl-MSVC debug info warning
#endif /* WIN32 */

#include "KX_ISystem.h"

//class NG_NetworkDeviceInterface;

class GPC_System : public KX_ISystem
{
public:
	GPC_System();

//	virtual void NextFrame();
//	virtual void StartMainLoop();
	virtual double GetTimeInSeconds() = 0;
//	virtual void Sleep(int millisec);
	//virtual bool IsPressed(SCA_IInputDevice::KX_EnumInputs inputcode);
//	void AddKey(unsigned char key, bool down);

//	virtual void SetNetworkDevice(NG_NetworkDeviceInterface* ndi);
//	virtual NG_NetworkDeviceInterface* GetNetworkDevice() const;

//protected:
//	NG_NetworkDeviceInterface* m_ndi;
};


#endif  // __GPC_SYSTEM_H

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

#include "GPC_System.h"

#include "GPC_KeyboardDevice.h"
#include "NG_NetworkDeviceInterface.h"


GPC_System::GPC_System()
//		: m_ndi(0)
{
}

/*
void GPC_System::NextFrame()
{
	// Have the imput devices proceed
	std::vector<SCA_IInputDevice*>::iterator idev;
	for (idev = m_inputDevices.begin(); !(idev == m_inputDevices.end()); idev++) {
		(*idev)->NextFrame();
	}

	// Have the network device proceed
	if (m_ndi) {
		m_ndi->NextFrame();
	}
}

void GPC_System::StartMainLoop()
{
}


void GPC_System::Sleep(int millisec)
{
	// do nothing for now ;)
}


void GPC_System::AddKey(unsigned char key, bool down)
{
	GPC_KeyboardDevice* keydev = (GPC_KeyboardDevice*) this->GetKeyboardDevice();
	if (keydev) {
		//SCA_IInputDevice::KX_EnumInputs inp = keydev->ToNative(key);
		keydev->ConvertEvent(key, down);
	}
}


void GPC_System::SetNetworkDevice(NG_NetworkDeviceInterface* ndi)
{
	m_ndi = ndi;
}


NG_NetworkDeviceInterface* GPC_System::GetNetworkDevice() const
{
	return m_ndi;
}
*/
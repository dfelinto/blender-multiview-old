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

#ifndef __GPC_KEYBOARDDEVICE_H
#define __GPC_KEYBOARDDEVICE_H

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // WIN32

#include "SCA_IInputDevice.h"

#include <map>


/**
 * System independent implementation of SCA_IInputDevice.
 * System dependent keyboard devices need only to inherit this class
 * and fill the m_reverseKeyTranslateTable key translation map.
 * @see SCA_IInputDevice
 */

class GPC_KeyboardDevice : public SCA_IInputDevice
{
protected:

	/**
	 * This map converts system dependent keyboard codes into Ketsji codes.
	 * System dependent keyboard codes are stored as ints.
	 */
	std::map<int, KX_EnumInputs> m_reverseKeyTranslateTable;

public:
	GPC_KeyboardDevice()
	{
	}

	virtual ~GPC_KeyboardDevice(void)
	{
	}

	virtual bool IsPressed(SCA_IInputDevice::KX_EnumInputs inputcode)
	{
		return false;
	}

	virtual void NextFrame();
	
	virtual KX_EnumInputs ToNative(int incode)
	{
		return m_reverseKeyTranslateTable[incode];
	}

	virtual bool ConvertEvent(int incode, int val);
};


#endif  // _GPC_KEYBOARDDEVICE_H

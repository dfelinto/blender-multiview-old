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

#ifndef _GPW_KEYBOARDDEVICE_H_
#define _GPW_KEYBOARDDEVICE_H_

#pragma warning (disable : 4786)

#include <windows.h>

#include "GPC_KeyboardDevice.h"

/**
 * Win32 implementation of GPC_KeyboardDevice.
 * The contructor fills the keyboard code translation map.
 * Base class GPC_KeyboardDevice does the rest.
 * @see SCA_IInputDevice
 */
class GPW_KeyboardDevice : public GPC_KeyboardDevice
{
public:
	GPW_KeyboardDevice(void);

	virtual ~GPW_KeyboardDevice(void);

	void ConvertWinEvent(WPARAM wParam, bool isDown);

protected:

	void ConvertModifierKey(WPARAM wParam, bool isDown);

	void CheckForSeperateLeftRight(WPARAM wParam);

	/** Stores the capability of this system to distinguish left and right modifier keys. */
	bool m_seperateLeftRight;
	/** Stores the initialization state of the member m_leftRightDistinguishable. */
	bool m_seperateLeftRightInitialized;
};

#endif //_GPW_KEYBOARDDEVICE_H_

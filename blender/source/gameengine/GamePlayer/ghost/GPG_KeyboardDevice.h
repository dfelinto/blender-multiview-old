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
 * GHOST Blender Player keyboard device.
 */

#ifndef _GPG_KEYBOARDDEVICE_H_
#define _GPG_KEYBOARDDEVICE_H_

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // WIN32

#include "GHOST_Types.h"
#include "GPC_KeyboardDevice.h"

/**
 * GHOST implementation of GPC_KeyboardDevice.
 * The contructor fills the keyboard code translation map.
 * Base class GPC_KeyboardDevice does the rest.
 * @see SCA_IInputDevice
 */
class GPG_KeyboardDevice : public GPC_KeyboardDevice
{
public:
	GPG_KeyboardDevice(void);
	virtual ~GPG_KeyboardDevice(void);
};

#endif //_GPG_KEYBOARDDEVICE_H_

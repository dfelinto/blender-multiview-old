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

#ifndef __GPU_KEYBOARDDEVICE_H
#define __GPU_KEYBOARDDEVICE_H

#include <X11/keysym.h>
#include <X11/X.h> // Brilliant name, eh? Stupid !@#!$!@#@@% This is
 // actually needed so as not to get name clashes between Object from
 // blender and Object from X11... The proper include would be
 // Intrinsic.h . Yes, we are a bunch of sado-masochists. Let's hurt
 // ourselves!

#include "GPC_KeyboardDevice.h"

class GPU_KeyboardDevice : public GPC_KeyboardDevice
{
public:

	void register_X_key_down_event(KeySym k);
	void register_X_key_up_event(KeySym k);
	
	GPU_KeyboardDevice(void);
	virtual ~GPU_KeyboardDevice()
	{
		/* intentionally empty */
	}
	
 private: 
	SCA_IInputDevice::KX_EnumInputs
		convert_x_keycode_to_kx_keycode(unsigned int key);
};


#endif  // _GPU_KEYBOARDDEVICE_H

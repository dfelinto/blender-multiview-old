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
#ifndef __KX_BLENDERMOUSEDEVICE
#define __KX_BLENDERMOUSEDEVICE

#include "KX_BlenderInputDevice.h"

class KX_BlenderMouseDevice : public BL_BlenderInputDevice
{
public:
	KX_BlenderMouseDevice();
	virtual ~KX_BlenderMouseDevice();

	virtual bool	IsPressed(SCA_IInputDevice::KX_EnumInputs inputcode);
//	virtual const SCA_InputEvent&	GetEventValue(SCA_IInputDevice::KX_EnumInputs inputcode);
	virtual bool	ConvertBlenderEvent(unsigned short incode,short val);
	virtual void	NextFrame();
};
#endif //__KX_BLENDERMOUSEDEVICE

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

#ifndef __GPC_MOUSE_DEVICE_H
#define __GPC_MOUSE_DEVICE_H

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // WIN32

#include "SCA_IInputDevice.h"


/**
 * Generic Ketsji mouse device.
 * @see SCA_IInputDevice
 */
class GPC_MouseDevice : public SCA_IInputDevice
{
public:
	/**
	 * Button identifier.
	 */
	typedef enum {
		buttonLeft,
		buttonMiddle,
		buttonRight
	} TButtonId;

	GPC_MouseDevice();
	virtual ~GPC_MouseDevice(void);

	virtual bool IsPressed(SCA_IInputDevice::KX_EnumInputs inputcode);
	virtual void NextFrame();

	/**
	 * Call this routine to update the mouse device when a button state changes.
	 * @param button	Which button state changes.
	 * @param isDown	The new state of the button.
	 * @param x			Position x-coordinate of the cursor at the time of the state change.
	 * @param y			Position y-coordinate of the cursor at the time of the state change.
	 * @return Indication as to whether the event was processed.
	 */
	virtual bool ConvertButtonEvent(TButtonId button, bool isDown);

	/**
	 * Call this routine to update the mouse device when a button state and
	 * cursor position changes at the same time (e.g. in Win32 messages).
	 * @param button	Which button state changes.
	 * @param isDown	The new state of the button.
	 * @param x			Position x-coordinate of the cursor at the time of the state change.
	 * @param y			Position y-coordinate of the cursor at the time of the state change.
	 * @return Indication as to whether the event was processed.
	 */
	virtual bool ConvertButtonEvent(TButtonId button, bool isDown, int x, int y);

	/**
	 * Call this routine to update the mouse device when the cursor has moved.
	 * @param x			Position x-coordinate of the cursor.
	 * @param y			Position y-coordinate of the cursor.
	 * @return Indication as to whether the event was processed.
	 */
	virtual bool ConvertMoveEvent(int x, int y);

protected:
	/**
	 * This routine converts a single mouse event to a Ketsji mouse event.
	 * @param kxevent	Ketsji event code.
	 * @param eventval	Value for this event.
	 * @return Indication as to whether the event was processed.
	 */
	virtual bool ConvertEvent(KX_EnumInputs kxevent, int eventval);
};


#endif  // __GPC_MOUSE_DEVICE_H

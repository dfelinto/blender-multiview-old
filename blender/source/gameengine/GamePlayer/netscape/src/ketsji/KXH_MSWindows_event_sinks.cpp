/**
 * $Id$
 *
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 * MSWindows event handling for the browser plugin
 */

#ifdef _WIN32

#include "GPC_MouseDevice.h"
#include "GPW_KeyboardDevice.h"
#include "GPC_System.h"

#include "KXH_MSWindows_event_sinks.h"
#include "KXH_engine_data_wraps.h"
#include "PLG_MSWindows_windowing.h"

static ketsji_engine_data*
handle_to_data(
	APH_application_handle handle
	);

// For logging: 
//#define GENERATE_LOG


/* --------------------------------------------------------------------- */
/* Handlers: connected to the MSWindows windowing                        */
/* --------------------------------------------------------------------- */

void
APH_mouse_movement(
	APH_application_handle e, 
	int x,
	int y
	)
{
	ketsji_engine_data* ked = handle_to_data(e);
	GPC_System* sys = ked->logic_system;
	GPC_MouseDevice* m = NULL;

	if((!ked) || (!ked->kx_initialized)) return;

	if (sys) {
		m = (GPC_MouseDevice*) ked->mousedevice;	
	}
	if (m) {
		m->ConvertMoveEvent(x, y);
	}
}

void
APH_keyboard_press(
	APH_application_handle e, 
	UINT nChar
	) 
{
	ketsji_engine_data* ked = handle_to_data(e);
	GPC_System* sys = ked->logic_system;
	GPW_KeyboardDevice* kb = NULL;

	if((!ked) || (!ked->kx_initialized)) return;

	if (sys) kb = (GPW_KeyboardDevice*) ked->keyboarddevice;
	if (kb) {
	  kb->ConvertWinEvent(nChar,
			      false);
	}
}

void
APH_keyboard_release(
	APH_application_handle e, 
	UINT nChar
	)
{
	ketsji_engine_data* ked = handle_to_data(e);
	GPC_System* sys = ked->logic_system;
	GPW_KeyboardDevice* kb = NULL;
	
	if((!ked) || (!ked->kx_initialized)) return;

	if (sys) kb = (GPW_KeyboardDevice*) ked->keyboarddevice;
	if (kb) {
	  kb->ConvertWinEvent(nChar,
			      false);
	}
}

/* Stupido! We keep converting the mouse buttons until doomsday!!!
 * left = 1, middle = 2, right = 3 */

#define KXH_MOUSE_CONVERT(x) ((x == MSWINDOWS_RIGHT_MOUSE_BUTTON) ? GPC_MouseDevice::buttonRight: ((x == MSWINDOWS_MIDDLE_MOUSE_BUTTON) ? GPC_MouseDevice::buttonMiddle : GPC_MouseDevice::buttonLeft) )

void
APH_mousekey_press(
	APH_application_handle e, 
	int i
	) 
{
	ketsji_engine_data* ked = handle_to_data(e);
	GPC_System* sys = ked->logic_system;
	GPC_MouseDevice* m = NULL;

	if((!ked) || (!ked->kx_initialized)) return;

	if (sys) m = (GPC_MouseDevice*) ked->mousedevice;	
	if (m) {
	  m->ConvertButtonEvent(KXH_MOUSE_CONVERT(i), 
				true);
	}
}

void
APH_mousekey_release(
	APH_application_handle e, 
	int i
	)
{
	ketsji_engine_data* ked = handle_to_data(e);
	GPC_System* sys = ked->logic_system;
	GPC_MouseDevice* m = NULL;

	if((!ked) || (!ked->kx_initialized)) return;

	if (sys) m = (GPC_MouseDevice*) ked->mousedevice;	
	if (m) {
	  m->ConvertButtonEvent(KXH_MOUSE_CONVERT(i), 
				true);
	}
}

/* --------------------------------------------------------------------- */

static ketsji_engine_data*
handle_to_data(
	APH_application_handle handle
	)
{
	return (ketsji_engine_data*) handle;
}

#endif


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
 * X11 event handling for the browser plugin
 */

#if !defined(_WIN32) && !defined(__APPLE__)

#include "GPC_MouseDevice.h"
#include "GPU_KeyboardDevice.h"
#include "GPC_System.h"

#include "KXH_unix_event_sinks.h"
#include "KXH_engine_data_wraps.h"

/** x mouse button codes to gpc codes */
static bool
convert_x_mousebutton_code_to_tbuttonid(
	int xbut, 
	GPC_MouseDevice::TButtonId* tbut
	);

static ketsji_engine_data*
handle_to_data(
	APH_application_handle handle
	);

// For logging: 
//#define GENERATE_LOG


/* --------------------------------------------------------------------- */
/* Handlers: connected to the X11 windowing                              */
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
	KeySym key
	) 
{
	ketsji_engine_data* ked = handle_to_data(e);
	GPC_System* sys = ked->logic_system;
	GPU_KeyboardDevice* kb = NULL;

	if((!ked) || (!ked->kx_initialized)) return;

	if (sys) kb = (GPU_KeyboardDevice*) ked->keyboarddevice;
	if (kb) {
		kb->register_X_key_down_event(key);
	}
}

void
APH_keyboard_release(
	APH_application_handle e, 
	KeySym key
	)
{
	ketsji_engine_data* ked = handle_to_data(e);
	GPC_System* sys = ked->logic_system;
	GPU_KeyboardDevice* kb = NULL;
	
	if((!ked) || (!ked->kx_initialized)) return;

	if (sys) kb = (GPU_KeyboardDevice*) ked->keyboarddevice;
	if (kb) {
		kb->register_X_key_up_event(key);
	}
}

void
APH_mousekey_press(
	APH_application_handle e, 
	int xbut
	) 
{
	GPC_MouseDevice::TButtonId b;
	ketsji_engine_data* ked = handle_to_data(e);
	GPC_System* sys = ked->logic_system;
	GPC_MouseDevice* m = NULL;

	if((!ked) || (!ked->kx_initialized)) return;

	if (convert_x_mousebutton_code_to_tbuttonid(xbut, &b)) {
		if (sys) m = (GPC_MouseDevice*) ked->mousedevice;	
		if (m) {
			m->ConvertButtonEvent(b, true);
		}
	}
}

void
APH_mousekey_release(
	APH_application_handle e, 
	int xbut
	)
{
	GPC_MouseDevice::TButtonId b;
	ketsji_engine_data* ked = handle_to_data(e);
	GPC_System* sys = ked->logic_system;
	GPC_MouseDevice* m = NULL;

	if((!ked) || (!ked->kx_initialized)) return;

	if (convert_x_mousebutton_code_to_tbuttonid(xbut, &b)) {
		if (sys) m = (GPC_MouseDevice*) ked->mousedevice;	
		if (m) {
			m->ConvertButtonEvent(b, false);
		}
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


static bool
convert_x_mousebutton_code_to_tbuttonid(
	int xbut, 
	GPC_MouseDevice::TButtonId* tbut
	)
{
	bool result = true;

	switch (xbut) {
	case Button1 : *tbut = GPC_MouseDevice::buttonLeft; break;
	case Button2 : *tbut = GPC_MouseDevice::buttonMiddle; break;
	case Button3 : *tbut = GPC_MouseDevice::buttonRight; break;
	default :
		result = false; // nada. 
	}
	
	return result;
}

#endif


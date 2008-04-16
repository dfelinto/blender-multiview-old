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
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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

#ifndef KXH_MSWINDOWS_EVENT_SINKS_H
#define KXH_MSWINDOWS_EVENT_SINKS_H

#include "PLG_plugin_handles.h"

#ifdef __cplusplus
extern "C" {
#endif

    void
    APH_mouse_movement(
	APH_application_handle e, 
	int x,
	int y
	);

    void
    APH_keyboard_press(
	APH_application_handle e, 
	UINT nChar
	);

    void
    APH_keyboard_release(
	APH_application_handle e, 
	UINT nChar
	);

    void
    APH_mousekey_press(
	APH_application_handle e, 
	int b
	);

    void
    APH_mousekey_release(
	APH_application_handle e, 
	int b
	);

#ifdef __cplusplus
}
#endif

#endif


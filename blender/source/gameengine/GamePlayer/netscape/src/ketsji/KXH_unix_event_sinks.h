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
 * X11 event handlingfor the browser plugin
 */

#ifndef KXH_UNIX_EVENT_SINKS_H
#define KXH_UNIX_EVENT_SINKS_H

#include "PLG_plugin_handles.h"
#include <X11/Xlib.h>

#ifdef __cplusplus
extern "C" {
#endif
       
	
	/** Accept an X key press event */
	void
	APH_keyboard_press(
		APH_application_handle h, 
		KeySym key
		);

	/** Accept an X key release event */
	void
	APH_keyboard_release(
		APH_application_handle h,
		KeySym key
		);

	/** Accept an X mousebutton press event */
	void
	APH_mousekey_press(
		APH_application_handle h,
		int xbut
		);

	/** Accept an X mousebutton release event */
	void
	APH_mousekey_release(
		APH_application_handle h,
		int xbut
		);

	/** Accept an X mouse movement event */
	void
	APH_mouse_movement(
		APH_application_handle h, 
		int x, 
		int y
		);

#ifdef __cplusplus
}
#endif

#endif


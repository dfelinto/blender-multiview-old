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


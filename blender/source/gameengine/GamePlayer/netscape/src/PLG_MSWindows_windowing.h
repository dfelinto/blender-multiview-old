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
 * MS Windows windowing for the plugin
 */


#ifndef PLG_MSWINDOWS_WINDOWING_H
#define PLG_MSWINDOWS_WINDOWING_H

#include "blender_plugin_types.h"

#define MSWINDOWS_LEFT_MOUSE_BUTTON 1
#define MSWINDOWS_MIDDLE_MOUSE_BUTTON 2
#define MSWINDOWS_RIGHT_MOUSE_BUTTON 3


#ifdef __cplusplus
extern "C" {
#endif

	LRESULT CALLBACK
	PluginWindowProc(
		HWND hWnd,
		UINT Msg,
		WPARAM wParam,
		LPARAM lParam
		);

	/** Platform independant windowing init */
	void
	PLG_global_instance_init(
		void
		);

	void
	PLG_plugin_new_instance_init(
		BlenderPluginInstance * inst
		);


	NPError 
	PLG_set_platform_window(
		NPP instance,
		NPWindow* window,
		BlenderPluginInstance * inst
		);

	HWND
	PLA_get_HWND(
	    PLA_plugin_handle p
	    );

	HDC
	PLA_get_HDC(
	    PLA_plugin_handle p
	    );

#ifdef __cplusplus
}
#endif


#endif

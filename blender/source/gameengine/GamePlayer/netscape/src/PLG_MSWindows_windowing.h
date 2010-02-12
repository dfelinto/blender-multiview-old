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


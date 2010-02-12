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
 * X11 windowing for the browser plugin
 */

#ifndef PLG_X11_WINDOWING_H
#define PLG_X11_WINDOWING_H

#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include "blender_plugin_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif


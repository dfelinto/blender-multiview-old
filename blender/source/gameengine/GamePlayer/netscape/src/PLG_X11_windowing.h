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

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
 * Hooks from gameengine to plugin. These are used by the GPU canvas.
 */

#ifndef KXH_KETSJI_HOOKS_H
#define KXH_KETSJI_HOOKS_H 

#define KXH_DECLARE_HANDLE(name) typedef struct name##__ { int unused; } *name

#ifndef __KXH_PLUGIN_HANDLE
#define __KXH_PLUGIN_HANDLE
	KXH_DECLARE_HANDLE(KXH_plugin_handle);
#endif

#ifdef __cplusplus
extern "C" {
#endif

	/** Acquire drawing context. */
	bool
	KXH_begin_draw(KXH_plugin_handle h);

	/** Swap buffers in the drawing context. This does not release the
	 * context! */
	void
	KXH_swap_buffers(KXH_plugin_handle h);

	/** Release drawing context. */
	void
	KXH_end_draw(KXH_plugin_handle h);
	

#ifdef __cplusplus
}
#endif

#endif

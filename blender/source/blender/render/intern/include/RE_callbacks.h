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
 * Callbacks to make the renderer interact with calling modules.
 */

#ifndef RE_CALLBACKS_H
#define RE_CALLBACKS_H

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * Test whether operation should be prematurely terminated.
	 *
	 * @returns 0 to continue, any other value to break.
	 */
	int RE_local_test_break(void);

	/**
	 * Set a red square with the argument as text as cursor.
	 */
	void RE_local_timecursor(int i);

	/**
	 * Render these lines from the renderbuffer on screen (needs better spec) 
	 */
	void RE_local_render_display(int i, int j, int k, int l, unsigned int *m);

	/**
	 * Initialise a render display (needs better spec)
	 */
	void RE_local_init_render_display(void);

	/**
	 * Clear/close a render display (needs better spec)
	 */
	void RE_local_clear_render_display(short);

	/**
	 * Print render statistics.
	 */
	void RE_local_printrenderinfo(double time, int i);

	/** Get the data for the scene to render. */
	void RE_local_get_renderdata(void);
	
	/** Release the data for the scene that was rendered. */
	void RE_local_free_renderdata(void);

	
#ifdef __cplusplus
}
#endif

#endif

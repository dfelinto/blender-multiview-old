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
 * Available functions for the application.
 */

#ifndef PLA_APPLICATION_CALLBACK_H
#define PLA_APPLICATION_CALLBACK_H

#include "PLG_plugin_handles.h"

#ifdef __cplusplus
extern "C" {
#endif

	int
	PLA_create_gl_context(
		PLA_plugin_handle i
		);

	void 
	PLA_delete_gl_context(
		PLA_plugin_handle i
		);

	/** Claim access to the rendering context. Return 0 if it fails, 1
	 * if it succeeds. */
	int
	PLA_acquire_gl_context(
		PLA_plugin_handle inst
		);

	/** Swap buffers, but do not let go of the context resource. */
	void
	PLA_swap_gl_context(
		PLA_plugin_handle i
		);
		
	/** Release the context resource. This doesn't swap buffers. */
	void
	PLA_release_gl_context(
		PLA_plugin_handle inst
		);

	/** Get the width of the display window. */
	int
	PLA_get_display_width(
		PLA_plugin_handle inst
		);

	/** Get the height of the display window. */
	int
	PLA_get_display_height(
		PLA_plugin_handle inst
		);

	/** Get a ref to the loading animation file URL. */
	char*
	PLA_get_loading_animation_url(
		PLA_plugin_handle i
		);

	/** Get the number of bytes retrieved from the main file
	 * stream. */
	int
	PLA_stream_retrieved(
		PLA_plugin_handle i
		);

	/** Get the number of bytes expected from the main stream. */
	int
	PLA_stream_total(
		PLA_plugin_handle i
		);

	/** Retrieve a ref to the foreground colour vector. */
	float*
	PLA_get_foreground_colour(
		PLA_plugin_handle i
		);

	/** Retrieve a ref to the background colour vector. */
	float*
	PLA_get_background_colour(
		PLA_plugin_handle i
		);

	/** Retrieve the desired framerate. */
	float
	PLA_requested_framerate(
		PLA_plugin_handle i
		);
	
	/** Check with the platform whether we can proceed one frame. */
	void
	PLA_request_application_progress(
		PLA_plugin_handle i
		);

	/** post a request for url retrieval. I should wire all request
	    through the same function*/
	int
	PLA_request_replacing_url(
		PLA_plugin_handle i,
		char* url
		);

	void 
	PLA_await_window(
		PLA_plugin_handle i
		);

	void 
	PLA_lock_events(
		PLA_plugin_handle i
		);

	void 
	PLA_unlock_events(
		PLA_plugin_handle i
		);
		
#ifdef __cplusplus
}
#endif

#endif

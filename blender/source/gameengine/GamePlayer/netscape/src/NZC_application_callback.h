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

	/** Retrieve a ref to the foreground color vector. */
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


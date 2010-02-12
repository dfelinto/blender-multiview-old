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
 * Wrapper to communicate with a threaded application from the plugin.
 */

#ifndef APH_APPLICATION_HOOKS_H
#define APH_APPLICATION_HOOKS_H

#include "npapi.h"
#include "prthread.h"
#include "prlock.h"
#include "PLG_plugin_handles.h"

#ifdef __cplusplus
extern "C" {
#endif

	/* ----------------------------------------------------------------- */
	/* Initialization and stuff                                          */
	/* ----------------------------------------------------------------- */

	/** Create a new application instance. */
	APH_application_handle 
	APH_create_application(
		PLA_plugin_handle h
		);

	/** Reset the viewport because of changes in geometry etc. */
	void
	APH_reset_viewport(
		APH_application_handle h,
		int x,
		int y
		);

	/** Redraw the application. */
	void
	APH_redraw(
		APH_application_handle h
		);

	/** Terminate the application. */
	void
	APH_terminate_application(
		APH_application_handle h
		);

	/** Pass the result of the loading of the main file. */
	void
	APH_main_file_loaded(
		APH_application_handle h,
		unsigned char * buffer,
		int size
		);

	/** Pass the result of the loading of the loading anim
	 * file. */
	void
	APH_loading_anim_loaded(
		APH_application_handle h,
		unsigned char * buffer,
		int size
		);
	
	/** Signal failure during loading of the main blend file. */
	void
	APH_main_file_failed(
		APH_application_handle h
		);

	/** Signal failure during loading of the loading animation
	 * blend file.*/
	void
	APH_loading_anim_failed(
		APH_application_handle h
		);

	/* ----------------------------------------------------------------- */
	/* Hooks for event handling                                          */
	/* ----------------------------------------------------------------- */

	/** send a message to the current gameworld*/
	void
	APH_send_message(
		APH_application_handle h, 
		char* to, 
		char* from, 
		char* subject, 
		char* body
		);

	/** Prepare to restart with a new url. Streaming is started
	 * elsewhere. */
	void
	APH_restart_with_url(
		APH_application_handle h
		);
  
#ifdef __cplusplus
}
#endif

# endif


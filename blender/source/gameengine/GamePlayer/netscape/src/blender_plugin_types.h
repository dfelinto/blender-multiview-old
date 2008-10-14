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
 * Plugin-instance related data
 */

#ifndef BLENDER_PLUGIN_TYPES_H
#define BLENDER_PLUGIN_TYPES_H

#include <stdio.h>

#ifdef _WIN32
/* Windows stuff goes here: */
#include <windows.h>

#elif defined(__APPLE__)
/* Apple stuff goes here: */

#else 
/* Unix stuff goes here: */
#include <GL/glx.h>
#include <X11/Intrinsic.h>
#endif

#include "npapi.h"              /* NS related types*/
#include "prlock.h"             /* NSPR locking */
#include "PLG_plugin_handles.h" /* Handles */

#ifdef __cplusplus
extern "C" {
#endif

	struct netscape_plugin_Plugin;
	
	typedef struct _BlenderPluginInstance
	{
#ifdef _WIN32

		/** Win default proc? */
		WNDPROC    fDefaultWindowProc;

		/** Window handle */
		HWND       fhWnd;
		/** Drawing context: */
		HDC        fhDC;

		NPWindow*  fWindow;
		uint16     fMode;


#elif defined(__APPLE__)

#else
		/* Unix: X and glx: */

		/* position */
		uint32 x, y;

		/** The Xt application context. The browser _should_
		 * create one, and it should never change. This is
		 * needed for correct locking of the app. */
		XtAppContext appctx;

		/** Reference to the most recently passed, valid
		 * display handle */		
		Display* display_ref;
		
		/** Reference to the X window from Netscape. This is
		 * the area in which the plugin sits, as defined by
		 * the embedding html. */
		Window window_ref;

		/** The gl context that is used for drawing */
		GLXContext ctx;
		/** the desired visual info of the glx context */
		XVisualInfo *vi;
		/** the acquired visual info of the glx context */
		XVisualInfo *def_vi;
		
		/* My guess: a ref? */
		Widget ns_widget;

		/** Set to 1 if the X event handlers need to be
		 * reattached. */
		int reset_event_handlers;

		/** 1 if there is GLX, 0 if not */
		int have_glx;

		/* GLX version, minor */
		int gl_version_minor;
		
		/* GLX version, major */
		int gl_version_major;

		/* just the colormap id */
		Colormap cmap;

#endif		

		/* size */
		uint32 width, height;

		
		/** This lock ensures that the event handlers do not enter data into
		 * the keyboard/mouse handler, when the gameengine is messing with
		 * them. Otherwise, you can lose events.
		 */
		PRLock* event_lock;

		/** This lock does handshaking between the KXH thread and the plugin
		 * thread, to make sure glx isn't used before sufficient information
		 * is present to do something with it. */
		PRLock* window_ready_lock;

		/** Window/GLX handshake lock: only needs to be done
		 * once, at startup. Set to 1 when it's done. */
		int wrl_locked;

		/** 0 if the window ref is valid, non-0 if it is
		 * invalidated */
		int window_invalidated;

		/** 0 if the window ref is valid, non-0 if it is
		 * invalidated */
		int display_invalidated;

		/** Has the plugin been initialized?*/
		int initialized;
		

		/** Flag to see whether we rely on jri or XPCOM for
		 * Javascript connections. 0 = jri, 1 = XPCOM */
		int jri_or_xp;
		
		/** Reference to the java peer for JRI coupling. */
		struct netscape_plugin_Plugin* java_peer;

		/** Reference to the XPCOM peer for XPConnect
		 * coupling.  */
		void* xpcom_peer;
		
		char* new_url;
		
		/** Handle to the application. */
		APH_application_handle application;

		/** reach back to the browser: needed for file
		 * streaming */
		NPP browser_instance;

		/** The default stream.... */
		NPStream* main_file_stream;

		/** Total bytes expected for main file */
		int stream_total;

		/** Total bytes retrieved for mail file */
		int stream_retrieved;

		/** Mem chunk for the main file */
		void* main_file_store;
		
		/** The stream we request for the loading anim, if
		 * needed. */
		NPStream* loading_anim_stream;

		/** Total bytes expected for loading anim */
		int loading_anim_total;

		/** Total bytes retrieved for loading anim */
		int loading_anim_retrieved;

		/** Mem chunk for the loading anim */
		void* loading_anim_store;

		/** diagnostics: set if a url was requested */
		int url_request_posted;

		/** Desired framerate. */
		int framerate;

		/** URL of the .blend to use as a loading animation,
		 * NULL if the default should be used. */
		char* loading_animation;

		/** URL of the .blend you want to show. Must be set. */
		char* blend_file;

		/** Foreground colour to use. */
		float foreground_colour[3];

		/** Foreground colour to use. */
		float background_colour[3];

		/** Use file background colour for padding areas? (0 = no) */
		int use_file_background_colour;


	} BlenderPluginInstance;
	
#ifdef __cplusplus
}
#endif

# endif


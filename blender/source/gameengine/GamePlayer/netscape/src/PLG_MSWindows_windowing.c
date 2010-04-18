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

#ifdef _WIN32

#include "prthread.h"

#include "PLG_MSWindows_windowing.h"
#include "KXH_MSWindows_event_sinks.h"
#include "NZC_application_hooks.h"

#define GENERATE_LOG

/* --------------------------------------------------------------------- */
/* Needed for finding plugin-specific data.  */
const char* gInstanceLookupString = "instance->pdata";

/* Logging: */
static void
log_entry(char* msg);

/* --------------------------------------------------------------------- */

/* I don't use it, so just stub it. Seems to give stupid link
 * errors.*/
void    NP_LOADDS
NPP_URLNotify(
	NPP instance,
	const char* url,
	NPReason reason,
	void* notifyData
	)
{
	/* Intentionally empty */
	log_entry("NPP_URLNotify");
}



/* stuff for making this dll-able*/
HINSTANCE g_hDllInstance = NULL;

BOOL WINAPI
DllMain(
	HINSTANCE  hinstDLL,
	DWORD  fdwReason,
	LPVOID  lpvReserved
	)
{
	log_entry("DllMain");

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hDllInstance = hinstDLL;
		break;
/* Eeek! What now? */		
	case DLL_THREAD_ATTACH:
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

/* --------------------------------------------------------------------- */
/* Only the stub is needed here. */
int16
NPP_HandleEvent(
	NPP instance,
	void* ev
	)
{
	int16 eventHandled = FALSE;
 	log_entry("NPP_HandleEvent");
	return eventHandled;
}

/* --------------------------------------------------------------------- */

void
PLG_global_instance_init(
	void
	)
{
	/* Shouldn't need anything... X does Xt init here. */
 	log_entry("PLG_global_instance_init");
}

void
PLG_plugin_new_instance_init(
	BlenderPluginInstance * inst
	)
{
	log_entry("PLG_plugin_new_instance_init");

	inst->fMode              = 0;
	inst->fWindow            = NULL;
	inst->fhWnd              = NULL;
	inst->fDefaultWindowProc = NULL;
	
}


NPError
PLG_set_platform_window(
	NPP instance,
	NPWindow* window,
	BlenderPluginInstance * inst
	)
{
	log_entry("PLG_set_platform_window");
	
	/* register! Without this, the windows main function cannot
	 * find the correct plugin instance data to operate on. */
/* 	SetProp(inst->fhWnd, */
/* 		gInstanceLookupString, */
/* 		inst); */


	inst->width  = window->width;
	inst->height = window->height;

	if (inst->width == 0 || inst->height == 0 ) {
	log_entry("PLG_set_platform_window:: draw area does not exist!");
	}
	
	if( inst->fWindow != NULL ) // If we already have a window, clean
		// it up before trying to subclass
		// the new window.
	{
		if( (window == NULL) || ( window->window == NULL ) ) {
			// There is now no window to use. get rid of the old
			// one and exit.
			SetWindowLong(
				inst->fhWnd,
				GWL_WNDPROC,
				(LONG)inst->fDefaultWindowProc
				);
			inst->fDefaultWindowProc = NULL;
			inst->fhWnd = NULL;
			return NPERR_NO_ERROR;
		}
		
		else if ( inst->fWindow->window == window->window ) {
			// The new window is the same as the old one. Exit now.
			return NPERR_NO_ERROR;
		}
		else {
			// Clean up the old window, so that we can subclass the new
			// one later.
			SetWindowLong( inst->fhWnd,
				       GWL_WNDPROC,
				       (LONG)inst->fDefaultWindowProc);
			inst->fDefaultWindowProc = NULL;
			inst->fhWnd = NULL;
		}
	}
	else if( (window == NULL) || ( window->window == NULL ) ) {
		// We can just get out of here if there is no current
		// window and there is no new window to use.
		return NPERR_NO_ERROR;
	}
	
	// At this point, we will subclass
	// window->window so that we can begin drawing and
	// receiving window messages.
  	log_entry("PLG_set_platform_window:: register event loop function"); 
	
	inst->fDefaultWindowProc =
		(WNDPROC)SetWindowLong(
			(HWND)window->window,
			GWL_WNDPROC,
			(LONG)PluginWindowProc
			);
	
	inst->fhWnd = (HWND) window->window;

	inst->fhDC = GetDC(inst->fhWnd);
	
	// register "this" with the window structure
	SetProp(
		inst->fhWnd,
		gInstanceLookupString,
		(HANDLE)inst
		);
	
	InvalidateRect(
		inst->fhWnd,
		NULL,
		TRUE
		);
	
	UpdateWindow(
		inst->fhWnd
		);

	return NPERR_NO_ERROR;
	
}


/* This is the event loop function, which is primed in
 * PLG_set_platform_window() */
LRESULT CALLBACK
PluginWindowProc(
	HWND hWnd,
	UINT Msg,
	WPARAM wParam,
	LPARAM lParam
	)
{
	/* This needs to registered! */
	BlenderPluginInstance* plug =
		(BlenderPluginInstance*) GetProp(hWnd, gInstanceLookupString);
	int x, y;
	NPRect rect;

	APH_application_handle This = plug->application;

/*   	log_entry("PluginWindowProc");  */
	
	switch( Msg ) {
	case WM_KEYUP:
		APH_keyboard_release(This,
				     wParam);
		break;
	case WM_KEYDOWN:
		log_entry("PluginWindowProc:: key pressed");
		APH_keyboard_press(This,
				   wParam);
		break;
	case WM_LBUTTONUP: 
		APH_mousekey_release(This,
				     MSWINDOWS_LEFT_MOUSE_BUTTON);
		break; 
	case WM_LBUTTONDOWN: 
		APH_mousekey_press(This,
				   MSWINDOWS_LEFT_MOUSE_BUTTON);
		break; 
	case WM_MBUTTONUP: 
		APH_mousekey_release(This,
				     MSWINDOWS_MIDDLE_MOUSE_BUTTON);
		break; 
	case WM_MBUTTONDOWN:
		APH_mousekey_press(This,
				   MSWINDOWS_MIDDLE_MOUSE_BUTTON);
		break; 
	case WM_RBUTTONUP:
		APH_mousekey_release(This,
				     MSWINDOWS_RIGHT_MOUSE_BUTTON);
		break; 
	case WM_RBUTTONDOWN: 
		APH_mousekey_press(This,
				   MSWINDOWS_RIGHT_MOUSE_BUTTON);
		break; 
	case WM_MOUSEMOVE: 
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		APH_mouse_movement(This,
				   x,
				   y);
		break;
	case WM_PALETTECHANGED:
		/* ? Doesn't seem to require actions... */
		break;
	case WM_PAINT:

		APH_redraw(This);
		
		/* origin ... */
/* 		rect.top = 0; */
/* 		rect.left = 0; */
		/* ... to lower corner */
/* 	rect.bottom = inst->height; */
/* 	rect.right = inst->width; */
		
		/* These calls cause callbacks on NPP_HandleEvent, so they are
		 * only useful for win/mac */

		/* wtf? not there? */
/* 		NPN_InvalidateRect(plug->browser_instance, */
/* 				   &rect); */
/* 		NPN_ForceRedraw(plug->browser_instance); */
		break;
	default:
/* 		This->fDefaultWindowProc( hWnd, Msg, wParam, lParam); */
		;
	}
	return 0;
}


HWND
PLA_get_HWND(
	PLA_plugin_handle i
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	return inst->fhWnd;
}

HDC
PLA_get_HDC(
	PLA_plugin_handle i
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	return inst->fhDC;
}



/* All these are done in the canvas. Maybe do the same for the X
 * plug? */
void
PLA_acquire_gl_context(
	PLA_plugin_handle i
	)
{
	
}

void
PLA_swap_gl_context(
	PLA_plugin_handle i
	)
{
	
}
void
PLA_release_gl_context(
	PLA_plugin_handle i
	)
{
	
}

void
PLA_delete_gl_context(
	PLA_plugin_handle i
	)
{
	
}

void
PLA_create_gl_context(
	BlenderPluginInstance* inst
	)
{
	
}

/** Get the width of the display window. */
int
PLA_get_display_width(
	PLA_plugin_handle i
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	return inst->width;
}

/** Get the height of the display window. */
int
PLA_get_display_height(
	PLA_plugin_handle i
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	return inst->height;
}

void
PLA_request_application_progress(
	PLA_plugin_handle i
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
/* 	APH_redraw(inst->application); */

	InvalidateRect(
		inst->fhWnd,
		NULL,
		TRUE
		);

	
}


static void
log_entry(char* msg)
{
#ifdef GENERATE_LOG
	FILE* fp = fopen("/tmp/plugin_log","a");
	if (!fp) return;
  	fprintf(fp, "--> PLG_MSWindows_windowing::(%p) %s\n", 
		PR_GetCurrentThread(),
		msg); 
	fflush(fp);
	fclose (fp);
#endif
}

#endif /* _WIN32 */




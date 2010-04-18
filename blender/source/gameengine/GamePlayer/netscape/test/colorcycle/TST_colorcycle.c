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
 * Colorcycle the canvas.
 */


#include <X11/Intrinsic.h>
#include <stdio.h>
#include "npapi.h"
#include <GL/gl.h>
#include <GL/glx.h>

#include "NZC_application_hooks.h"
#include "NZC_application_callback.h"

/* If defined: report to the logfile if a function is called */
/*  #define TST_GENERATE_LOG */


/** Log entry into this function */
static void
TST_log_entry(char* msg);
static void
TST_log_pointer(void* p);

/* ----------------------------------------------------------------- */
/* Initialization and stuff                                          */
/* ----------------------------------------------------------------- */
static float myfrand(void){
	return drand48();
}

typedef struct _cycle_data {
	PLA_plugin_handle plugin;
} cycle_data;

/* ----------------------------------------------------------------- */
/* Initialization and stuff                                          */
/* ----------------------------------------------------------------- */

/** Create a game engine instance, and set up devices.  */
APH_application_handle
APH_create_application(PLA_plugin_handle h)
{
	APH_application_handle retval = NULL;
	cycle_data* c = NULL;

	TST_log_entry("NZC_create_application");	
	
	c = malloc(sizeof(cycle_data));
	if(!c) return NULL;

	c->plugin = h;
	
	return (APH_application_handle) c;
}

void
APH_reset_viewport(APH_application_handle h, int x, int y)
{
}


void
APH_redraw(APH_application_handle h)
{
	Bool result;
	cycle_data* c = (cycle_data*) h;

	TST_log_entry("NZC_redraw");
	TST_log_pointer((void*)h);
	TST_log_pointer((void*)c);
	TST_log_pointer((void*)c->plugin);
	
	if (!PLA_acquire_glx_context(c->plugin)) return;	

	glClearColor(myfrand(), myfrand(), myfrand(), 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	PLA_swap_glx_context(c->plugin);
	PLA_release_glx_context(c->plugin);
	
}

void
APH_terminate_application(APH_application_handle handle)
{
	cycle_data* h = (cycle_data*) handle;
	free(h);
}

/* ----------------------------------------------------------------- */
/*  Hooks for event handling                                         */
/* ----------------------------------------------------------------- */

/** Accept an X key press event */
void APH_keyboard_press(APH_application_handle h, KeySym key)
{
	cycle_data* c = (cycle_data*) h;
/*	PLA_post_redraw_notify(c->plugin);*/
}

/** Accept an X key release event */
void APH_keyboard_release(APH_application_handle h, KeySym key)
{
}

/** Accept an X mousebutton press event */
void APH_mousekey_press(APH_application_handle h, int xbut)
{
	cycle_data* c = (cycle_data*) h;
/* 	PLA_post_redraw_notify(c->plugin); */
}

/** Accept an X mousebutton release event */
void APH_mousekey_release(APH_application_handle h, int xbut)
{
}

/** Accept an X mouse movement event */
void APH_mouse_movement(APH_application_handle h, int x, int y)
{
	cycle_data* c = (cycle_data*) h;
/* 	PLA_post_redraw_notify(c->plugin); */
}
	
static void
TST_log_entry(char* msg)
{
#ifdef TST_GENERATE_LOG
	FILE* fp = fopen("plugin_log","a");
	if (!fp) return;
  	fprintf(fp, "--> NZC_implementation::%s\n", msg); 
	fflush(fp);
	fclose (fp);
#endif
}

static void
TST_log_pointer(void* p)
{
#ifdef TST_GENERATE_LOG
	FILE* fp = fopen("plugin_log","a");
	if (!fp) return;
  	fprintf(fp, "--> pointer::%p\n", p); 
	fflush(fp);
	fclose (fp);
#endif
}

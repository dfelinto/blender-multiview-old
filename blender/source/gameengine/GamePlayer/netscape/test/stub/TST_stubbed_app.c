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
 * Empty implementation of the application interface.
 */


#include <X11/Intrinsic.h>
#include <stdio.h>
#include "npapi.h"

#include "NZC_application_hooks.h"

/* ----------------------------------------------------------------- */
/* Initialization and stuff                                          */
/* ----------------------------------------------------------------- */

/** Create a game engine instance, and set up devices.  */
APH_application_handle
APH_create_application(PLA_plugin_handle h)
{
	return NULL;
}

void
APH_reset_viewport(APH_application_handle h, int x, int y)
{
}

void
APH_redraw(APH_application_handle h)
{
}

void
APH_terminate_application(APH_application_handle h)
{
}

/* ----------------------------------------------------------------- */
/*  Hooks for event handling                                         */
/* ----------------------------------------------------------------- */

/** Accept an X key press event */
void APH_keyboard_press(APH_application_handle h, KeySym key)
{
}

/** Accept an X key release event */
void APH_keyboard_release(APH_application_handle h, KeySym key)
{
}

/** Accept an X mousebutton press event */
void APH_mousekey_press(APH_application_handle h, int xbut)
{
}

/** Accept an X mousebutton release event */
void APH_mousekey_release(APH_application_handle h, int xbut)
{
}

/** Accept an X mouse movement event */
void APH_mouse_movement(APH_application_handle h, int x, int y)
{
}
	

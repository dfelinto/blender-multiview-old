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
 * Empty implementation of the application interface.
 */


#include <X11/Intrinsic.h>
#include <stdio.h>
#include "npapi.h"

#include "NZC_application_hooks.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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
	

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
 */
/* Abstract window operations */
	
typedef struct _Window Window;
typedef void	(*WindowHandlerFP)	(Window *win, void *user_data, short evt, short val, char ascii);

Window*	window_open			(char *title, int x, int y, int width, int height, int start_maximized);
void	window_set_handler	(Window *win, WindowHandlerFP handler, void *user_data);
void	window_destroy		(Window *win);

void	window_set_timer	(Window *win, int delay_ms, int event);

void	window_make_active	(Window *win);
void	window_swap_buffers	(Window *win);

void	window_raise		(Window *win);
void	window_lower		(Window *win);

short	window_get_qual		(Window *win);
short	window_get_mbut		(Window *win);
void	window_get_mouse	(Window *win, short *mval);

void	window_get_position	(Window *win, int *posx_r, int *poxy_r);

void	window_get_size		(Window *win, int *width_r, int *height_r);
void	window_set_size		(Window *win, int width, int height);

char*	window_get_title	(Window *win);
void	window_set_title	(Window *win, char *title);

void	window_set_cursor	(Window *win, int cursor);
void	window_set_custom_cursor	(Window *win, unsigned char mask[16][2], unsigned char bitmap[16][2]);

void	window_warp_pointer	(Window *win, int x, int y);

void	window_queue_redraw	(Window *win);

	/* Global windowing operations */

Window*	winlay_get_active_window(void);
	
void	winlay_process_events	(int wait_for_event);

void	winlay_get_screensize	(int *width_r, int *height_r);

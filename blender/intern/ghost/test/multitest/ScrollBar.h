/**
 * $Id$
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

typedef struct _ScrollBar ScrollBar;


	/***/
	
ScrollBar*	scrollbar_new				(int inset, int minthumb);

int			scrollbar_is_scrolling		(ScrollBar *sb);
int			scrollbar_contains_pt		(ScrollBar *sb, int pt[2]);

void		scrollbar_start_scrolling	(ScrollBar *sb, int yco);
void		scrollbar_keep_scrolling	(ScrollBar *sb, int yco);
void		scrollbar_stop_scrolling	(ScrollBar *sb);

void		scrollbar_set_thumbpct		(ScrollBar *sb, float pct);
void		scrollbar_set_thumbpos		(ScrollBar *sb, float pos);
void		scrollbar_set_rect			(ScrollBar *sb, int rect[2][2]);

float		scrollbar_get_thumbpct		(ScrollBar *sb);
float		scrollbar_get_thumbpos		(ScrollBar *sb);
void		scrollbar_get_rect			(ScrollBar *sb, int rect_r[2][2]);

void		scrollbar_get_thumb			(ScrollBar *sb, int thumb_r[2][2]);

void		scrollbar_free				(ScrollBar *sb);

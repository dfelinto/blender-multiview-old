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

#ifndef BIF_GRAPHICS_H
#define BIF_GRAPHICS_H

	/* XXX, should move somewhere else, with collected windowing
	 * stuff, to be done once the proper windowing stuff has
	 * been formed.
	 */
	
enum {
	CURSOR_VPAINT, 
	CURSOR_FACESEL, 
	CURSOR_WAIT, 
	CURSOR_EDIT, 
	CURSOR_X_MOVE, 
	CURSOR_Y_MOVE, 
	CURSOR_HELP, 
	CURSOR_STD, 
	CURSOR_NONE
};

void set_cursor(int curs);
int get_cursor(void);

#endif /* BIF_GRAPHICS_H */

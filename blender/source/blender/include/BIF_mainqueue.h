/*
 * Central queue handling functions.
 * 
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

#ifndef BIF_MAINQUEUE_H
#define BIF_MAINQUEUE_H

#define MAXQUEUE 256

unsigned short	mainqtest		(void);
unsigned short	mainqread		(short *val, char *ascii);
void			mainqenter		(unsigned short event, short val);
void			mainqenter_ext	(unsigned short event, short val, char ascii);
void			mainqpushback	(unsigned short event, short val, char ascii);

#endif /* BIF_MAINQUEUE_H */

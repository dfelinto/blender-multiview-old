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
 * os dependent include locations of gl.h
 */

#ifndef BIF_GL_H
#define BIF_GL_H

	/* XXX, fixme zr... what is the deal here,
	 * is there a reason unices don't use the
	 * <> include? (they should)
	 */
#if !defined(__APPLE__) && !defined(WIN32)
#include "GL/gl.h"
#include "GL/glu.h"
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

	/*
	 * these should be phased out. cpack should be replaced in
	 * code with calls to glColor3ub, lrectwrite probably should
	 * change to a function. - zr
	 */
	 
/* Mij afspraak is cpack een getal dat als 0xFFaa66 of zo kan worden
 * uitgedrukt. Is dus gevoelig voor endian.  Met deze define wordt het
 * altijd goed afgebeeld 
 *
 * This define converts a numerical value to the equivalent 24-bit
 * colour, while not being endian-sensitive. On little-endians, this
 * is the same as doing a 'naive'indexing, on big-endian, it is not!
 * */
#define cpack(x)	glColor3ub( ((x)&0xFF), (((x)>>8)&0xFF), (((x)>>16)&0xFF) )

#define glMultMatrixf(x)		glMultMatrixf( (float *)(x))
#define glLoadMatrixf(x)		glLoadMatrixf( (float *)(x))

#define lrectwrite(a, b, c, d, rect)	{glRasterPos2i(a,  b);glDrawPixels((c)-(a)+1, (d)-(b)+1, GL_RGBA, GL_UNSIGNED_BYTE,  rect);}

#endif /* #ifdef BIF_GL_H */

/**
 * blenlib/DNA_view2d_types.h (mar-2001 nzc)
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
#ifndef DNA_VIEW2D_TYPES_H
#define DNA_VIEW2D_TYPES_H

#include "DNA_vec_types.h"

typedef struct View2D {
	rctf tot, cur;
	rcti vert, hor, mask;
	float min[2], max[2];
	float minzoom, maxzoom;
	short scroll, keeptot;
	short keepaspect, keepzoom;
} View2D;

#define V2D_KEEPZOOM	0x0001	
#define V2D_LOCKZOOM_X	0x0100
#define V2D_LOCKZOOM_Y	0x0200

#endif

/**
 * blenlib/BKE_bmfont_types.h (mar-2001 nzc)
 *
 *
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
#ifndef BKE_BMFONT_TYPES_H
#define BKE_BMFONT_TYPES_H

#define is_power_of_two(N) ((N ^ (N - 1)) == (2 * N - 1))
/*
Moved to IMB_imbuf_types.h where it will live close to the ImBuf type.
It is used as a userflag bit mask.
#define IB_BITMAPFONT 1
*/
typedef struct bmGlyph {
	unsigned short unicode;
	short locx, locy;
	signed char ofsx, ofsy;
	unsigned char sizex, sizey;
	unsigned char advance, reserved;
} bmGlyph;

typedef struct bmFont {
	char magic[4];
	short version;
	short glyphcount;
	short xsize, ysize;
	bmGlyph glyphs[1];
} bmFont;

#endif

/**
 * blenlib/BKE_bmfont.h (mar-2001 nzc)
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
#ifndef BKE_BMFONT_H
#define BKE_BMFONT_H

#ifdef __cplusplus
extern "C" {
#endif

struct bmGlyph;
struct ImBuf;
struct bmFont;

void printfGlyph(struct bmGlyph * glyph);
void calcAlpha(struct ImBuf * ibuf);
void readBitmapFontVersion0(struct ImBuf * ibuf,
								   unsigned char * rect,
								   int step);
void detectBitmapFont(struct ImBuf *ibuf);
int locateGlyph(struct bmFont *bmfont, unsigned short unicode);
void matrixGlyph(struct ImBuf * ibuf, unsigned short unicode,
				 float *centerx, float *centery,
				 float *sizex,   float *sizey,
				 float *transx,  float *transy,
				 float *movex,   float *movey, float *advance); 

#ifdef __cplusplus
}
#endif
	
#endif

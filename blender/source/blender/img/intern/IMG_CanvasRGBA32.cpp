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

#include "IMG_CanvasRGBA32.h"



IMG_CanvasRGBA32::IMG_CanvasRGBA32(TUns32 width, TUns32 height)
	: IMG_PixmapRGBA32(width, height)
{
}

IMG_CanvasRGBA32::IMG_CanvasRGBA32(void* image, TUns32 width, TUns32 height, TUns32 rowBytes)
	: IMG_PixmapRGBA32(image, width, height, rowBytes)
{
}


void IMG_CanvasRGBA32::blendPixmap(
	TUns32 xStart, TUns32 yStart, TUns32 xEnd, TUns32 yEnd,
	const IMG_PixmapRGBA32& pixmap)
{
	// Determine visibility of the line
	IMG_Line l (xStart, yStart, xEnd, yEnd);	// Line used for blending
	IMG_Rect bnds (0, 0, m_width, m_height);	// Bounds of this pixmap
	TVisibility v = bnds.getVisibility(l);
	if (v == kNotVisible) return;
	if (v == kPartiallyVisible) {
		bnds.clip(l);
	}

	float numSteps = (((float)l.getLength()) / ((float)pixmap.getWidth() / 4));
	//numSteps *= 4;
	numSteps = numSteps ? numSteps : 1;
	float step = 0.f, stepSize = 1.f / ((float)numSteps);
	TInt32 x, y;
    for (TUns32 s = 0; s < numSteps; s++) {
		l.getPoint(step, x, y);
		IMG_PixmapRGBA32::blendPixmap((TUns32)x, (TUns32)y, pixmap);
		step += stepSize;
	}
}


void IMG_CanvasRGBA32::blendPixmap(
	float uStart, float vStart, float uEnd, float vEnd,
	const IMG_PixmapRGBA32& pixmap)
{
	TUns32 xStart, yStart, xEnd, yEnd;
	getPixelAddress(uStart, vStart, xStart, yStart);
	getPixelAddress(uEnd, vEnd, xEnd, yEnd);
	blendPixmap(xStart, yStart, xEnd, yEnd, pixmap);
}


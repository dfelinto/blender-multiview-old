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
 * @author	Maarten Gribnau
 * @date	March 7, 2001
 */
#ifndef _H_IMG_API
#define _H_IMG_API

#include <stddef.h>


typedef void* IMG_BrushPtr;
typedef void* IMG_CanvasPtr;

#ifdef __cplusplus
extern "C" {
#endif


extern IMG_BrushPtr	IMG_BrushCreate(unsigned int width, unsigned int height, float red, float green, float blue, float alpha);
extern void			IMG_BrushDispose(IMG_BrushPtr brush);

extern IMG_CanvasPtr	IMG_CanvasCreate(unsigned int width, unsigned int height);
extern IMG_CanvasPtr	IMG_CanvasCreateFromPtr(void* imagePtr, unsigned int width, unsigned int height, size_t rowBytes);
extern void				IMG_CanvasDispose(IMG_CanvasPtr canvas);
extern void				IMG_CanvasDraw(IMG_CanvasPtr canvas, IMG_BrushPtr brush, unsigned int x, unsigned int y);
extern void				IMG_CanvasDrawUV(IMG_CanvasPtr canvas, IMG_BrushPtr brush, float u, float v);
extern void				IMG_CanvasDrawLine(IMG_CanvasPtr canvas, IMG_BrushPtr brush, unsigned int xStart, unsigned int yStart, unsigned int xEns, unsigned int yEnd);
extern void				IMG_CanvasDrawLineUV(IMG_CanvasPtr canvas, IMG_BrushPtr brush, float uStart, float vStart, float uEnd, float vEnd);

#ifdef __cplusplus
}
#endif

#endif // _H_IMG_API

/*
 * pixelshading.h
 *
 * These functions determine what actual colour a pixel will have.
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

#ifndef PIXELSHADING_H
#define PIXELSHADING_H

/* ------------------------------------------------------------------------- */

#include "render.h"
#include "vanillaRenderPipe_types.h"
/* ------------------------------------------------------------------------- */

/**
 * Render the pixel at (x,y) for object ap. Apply the jitter mask. 
 * Output is given in float collector[4]. The type vector:
 * t[0] - min. distance
 * t[1] - face/halo index
 * t[2] - jitter mask                     
 * t[3] - type ZB_POLY or ZB_HALO
 * t[4] - max. distance
 * @return pointer to the object
 */
void *renderPixel(float x, float y, int *t);

/**
 * Spothalos on otherwise empty pixels.
 */
void renderSpotHaloPixel(float x, float y, float* colbuf);

/**
 * Set the sky blending to the indicated type.
 */
void setSkyBlendingMode(enum RE_SkyAlphaBlendingType mode);

/**
 * Get the sky blending mode.
 */
enum RE_SkyAlphaBlendingType getSkyBlendingMode(void);
/**
 * Render the sky at pixel (x, y).
 */
void renderSkyPixelFloat(float x, float y);

/* ------------------------------------------------------------------------- */
/* All these are supposed to be internal. I should move these to a separate  */
/* header.                                                                   */

/**
 * Determine colour for pixel at SCS x,y for face <vlaknr>. Result end up in
 * <collector>
 * @return pointer to this object's VlakRen
 */
void *renderFacePixel(float x, float y, int vlaknr);

/**
 * Render this pixel for halo haloNr. Leave result in <collector>.
 * @return pointer to this object's HaloRen
 */
void *renderHaloPixel(float x, float y, int haloNr);

/**
 * Shade the halo at the given location
 */
void shadeHaloFloat(HaloRen *har, float *col, unsigned int zz, 
					float dist, float xn, float yn, short flarec);

/**
 * Shade a sky pixel on a certain line, into collector[4]
 * The x-coordinate (y as well, actually) are communicated through
 * R.view[3]
 */
void shadeSkyPixel(float x, float y);

void shadeSpotHaloPixelFloat(float *col);
void spotHaloFloat(struct LampRen *lar, float *view, float *intens);
void shadeLampLusFloat(void);

/* this should be replaced by shadeSpotHaloPixelFloat(), but there's         */
/* something completely fucked up here with the arith.                       */
/*  void renderspothaloFix(unsigned short *col); */
void renderspothaloFix(float *col); 

/* used by shadeSkyPixel: */
void shadeSkyPixelFloat(float y);
void fillBackgroundImage(float x, float y);

/* ------------------------------------------------------------------------- */

#endif

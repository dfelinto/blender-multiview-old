/*
 * vanillaRenderPipe_types.h
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

#ifndef VANILLARENDERPIPE_TYPES_H
#define VANILLARENDERPIPE_TYPES_H

/* Render defines */
#define  RE_MAX_OSA_COUNT 16 /* The max. number of possible oversamples     */
#define  RE_MAX_FACES_PER_PIXEL 500 /* max. nr of faces rendered behind one */
                             /* pixel                                       */

enum RE_SkyAlphaBlendingType {
	RE_ALPHA_NODEF = 0,
	RE_ALPHA_PREMUL,
	RE_ALPHA_KEY,
	RE_ALPHA_SKY,
	RE_ALPHA_MAX
};


/* Render typedefs */
typedef float RE_COLBUFTYPE; /* datatype for the colour buffer              */


/**
 * Threshold for add-blending for faces
 */
#define RE_FACE_ADD_THRESHOLD 0.001

/**
   For oversampling 
   
   New stack: the old stack limits our freedom to do all kinds of
   manipulation, so we rewrite it.
   
   A stacked face needs:
   - a face type
   - a colour
   - a conflict count
   - a data pointer (void*)
  - a mask
  
  The stack starts at index 0, with the closest face, and stacks up.
  
*/

struct RE_faceField {
	int faceType;
	float colour[4];
	int conflictCount;
	void *data;
	int mask;
};



#endif /* VANILLARENDERPIPE_TYPES_H */



/**
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
 * allocimbuf.c
 *
 * $Id$
 */

#ifdef WIN32
#include "BLI_winstuff.h"
#endif
#include "BLI_blenlib.h"

#include "imbuf.h"
#include "imbuf_patch.h"
#include "IMB_imbuf_types.h"
#include "IMB_imbuf.h"

#include "IMB_allocimbuf.h"

void IMB_rectcpy(unsigned int *drect, unsigned int *srect, int x, int dummy)
{
	memcpy(drect,srect, x * sizeof(int));
}


void IMB_rectfill(unsigned int *drect, unsigned int *srect, int x, int value)
{
	for (;x > 0; x--) *drect++ = value;
}

void IMB_rectop(struct ImBuf *dbuf,
			struct ImBuf *sbuf,
			int destx,
			int desty,
			int srcx,
			int srcy,
			int width,
			int height,
			void (*operation)(unsigned int *, unsigned int*, int, int),
			int value)
{
	unsigned int *drect,*srect;

	if (dbuf == 0) return;
	if (operation == 0) return;

	if (destx < 0){
		srcx -= destx ;
		width += destx ;
		destx = 0;
	}
	if (srcx < 0){
		destx -= srcx ;
		width += destx ;
		srcx = 0;
	}
	if (desty < 0){
		srcy -= desty ;
		height += desty ;
		desty = 0;
	}
	if (srcy < 0){
		desty -= srcy ;
		height += desty ;
		srcy = 0;
	}

	if (width > dbuf->x - destx) width = dbuf->x - destx;
	if (height > dbuf->y - desty) height = dbuf->y - desty;
	if (sbuf){
		if (width > sbuf->x - srcx) width = sbuf->x - srcx;
		if (height > sbuf->y - srcy) height = sbuf->y - srcy;
	}

	if (width <= 0) return;
	if (height <= 0) return;

	drect = dbuf->rect;
	if (sbuf) srect = sbuf->rect;

	drect += desty * dbuf->x;
	drect += destx;
	destx = dbuf->x;

	if (sbuf) {
		srect += srcy * sbuf->x;
		srect += srcx;
		srcx = sbuf->x;
	} else{
		srect = drect;
		srcx = destx;
	}

	for (;height > 0; height--){
		operation(drect,srect,width, value);
		drect += destx;
		srect += srcx;
	}
}


void IMB_rectoptot(struct ImBuf *dbuf,
			   struct ImBuf *sbuf,
			   void (*operation)(unsigned int *, unsigned int*, int, int),
			   int value)
{
	IMB_rectop(dbuf,sbuf,0,0,0,0,32767,32767,operation, value);
}

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
 * rotate.c
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

void IMB_flipy(struct ImBuf * ibuf)
{
	short x,y,backx;
	unsigned int *top,*bottom,temp;

	if (ibuf == 0) return;
	if (ibuf->rect == 0) return;

	x = ibuf->x;
	y = ibuf->y;
	backx = x<<1;

	top = ibuf->rect;
	bottom = top + ((y-1) * x);
	y >>= 1;

	for(;y>0;y--){
		for(x = ibuf->x; x > 0; x--){
			temp = *top;
			*(top++) = *bottom;
			*(bottom++) = temp;
		}
		bottom -= backx;
	}
}

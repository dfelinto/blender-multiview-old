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

/**

 * $Id$
 * Copyright (C) 2001 NaN Technologies B.V.
 */

#ifndef __BMF_FONTDATA_H__
#define __BMF_FONTDATA_H__

typedef struct {
	signed char width, height;
	signed char xorig, yorig;
	signed char advance;
	
	short		data_offset;
} BMF_CharData;

typedef struct {
	int				xmin, ymin;
	int				xmax, ymax;

	BMF_CharData	chars[256];
	unsigned char*	bitmap_data;
} BMF_FontData;

#endif

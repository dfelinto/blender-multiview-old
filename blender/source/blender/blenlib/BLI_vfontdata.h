/**
 * @file BLI_vfontdata.h
 * 
 * A structure to represent vector fonts, 
 * and to load them from PostScript fonts.
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
 
#ifndef BLI_VFONTDATA_H
#define BLI_VFONTDATA_H

#include "DNA_listBase.h"

struct PackedFile;

#define MAX_VF_CHARS 256

typedef struct VFontData {
	ListBase nurbsbase[MAX_VF_CHARS];
	float	    resol[MAX_VF_CHARS];
	float	    width[MAX_VF_CHARS];
	float	    *points[MAX_VF_CHARS];
} VFontData;

/**
 * Construct a new VFontData structure from 
 * PostScript font data in a PackedFile.
 * 
 * @param pf The font data.
 * @retval A new VFontData structure, or NULL
 * if unable to load.
 */
	VFontData*
BLI_vfontdata_from_psfont(
	struct PackedFile *pf);

#endif

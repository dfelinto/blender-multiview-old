/**
 * blenlib/BKE_osa_types.h (mar-2001 nzc)
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
#ifndef BKE_OSA_TYPES_H
#define BKE_OSA_TYPES_H

/*  typedef struct Osa */
typedef struct RE_Osa
{
	float dxco[3], dyco[3];
	float dxlo[3], dylo[3], dxgl[3], dygl[3], dxuv[3], dyuv[3];
	float dxref[3], dyref[3], dxorn[3], dyorn[3];
	float dxno[3], dyno[3], dxview, dyview;
	float dxlv[3], dylv[3];
	float dxwin[3], dywin[3];
	float dxsticky[3], dysticky[3];
} Osa;

/*  extern Osa O;  */
/* This one used to be done in render/extern/include/render.h, because
    memory was allocated in that module. (nzc)*/

#endif

/**
 * blenlib/DNA_camera_types.h (mar-2001 nzc)
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
#ifndef DNA_CAMERA_TYPES_H
#define DNA_CAMERA_TYPES_H

#include "DNA_ID.h"
#include "DNA_scriptlink_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Ipo;

typedef struct Camera {
	ID id;
	
	short type, flag, drawzoom, hold;
	float clipsta, clipend;
	float netsta, netend;			/* network camera */
	float lens, drawsize;
	float hololen, hololen1;
	
	struct Ipo *ipo;
	
	ScriptLink scriptlink;
} Camera;

/* **************** CAMERA ********************* */

/* type */
#define CAM_PERSP		0
#define CAM_ORTHO		1

/* flag */
#define CAM_SHOWLIMITS	1
#define CAM_SHOWMIST	2

#define CAM_HOLO1		16
#define CAM_HOLO2		32

#ifdef __cplusplus
}
#endif

#endif

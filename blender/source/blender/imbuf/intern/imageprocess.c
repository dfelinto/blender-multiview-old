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
 * This file was moved here from the src/ directory. It is meant to
 * deal with endianness. It resided in a general blending lib. The
 * other functions were only used during rendering. This single
 * function remained. It should probably move to imbuf/intern/util.c,
 * but we'll keep it here for the time being. (nzc)*/

/*  imageprocess.c        MIXED MODEL
 * 
 *  april 95
 * 
 * $Id$
 */

#include "IMB_imbuf.h"

/* Only this one is used liberally here, and in imbuf */
void IMB_convert_rgba_to_abgr(int size, unsigned int *rect)
{
	char *cp= (char *)rect, rt;
	
	while(size-- > 0) {
		rt= cp[0];
		cp[0]= cp[3];
		cp[3]= rt;
		rt= cp[1];
		cp[1]= cp[2];
		cp[2]= rt;
		cp+= 4;
	}
}

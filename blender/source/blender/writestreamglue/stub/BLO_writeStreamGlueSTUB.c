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
 * streamGlueRead stubs
 */

#include <stdio.h>

#include "GEN_messaging.h"
#include "BLO_writeStreamGlue.h"

	int
writeStreamGlue(
	struct streamGlueControlStruct *streamGlueControl,
	struct writeStreamGlueStruct **streamGlue,
	unsigned char *data,
	unsigned int dataIn,
	int finishUp)
{
	int err = 0;
	streamGlueControl = NULL;
	*streamGlue = NULL;
	data = NULL;
	dataIn = 0;
	finishUp = 0;
#ifndef NDEBUG
	fprintf(GEN_errorstream,
			"Error writeStreamGlue is a stub\n");
#endif
	err = BWS_SETFUNCTION(BWS_WRITESTREAMGLUE) |
		  BWS_SETGENERR(BWS_STUB);
	return err;
}


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
 * streamglue loopback. Needed at start of Read stream.
 */

#include <stdlib.h> // TODO use blender's

#include "BLO_readStreamGlue.h"
#include "BLO_readStreamGlueLoopBack.h"

	struct readStreamGlueLoopBackStruct *
readStreamGlueLoopBack_begin(
	void *endControl)
{
	struct readStreamGlueLoopBackStruct *control;
	control = malloc(sizeof(struct readStreamGlueLoopBackStruct));
	if (control == NULL) {
		return NULL;
	}

	control->streamGlue = NULL;
	control->endControl = endControl;

	return(control);
}

	int
readStreamGlueLoopBack_process(
	struct readStreamGlueLoopBackStruct *control,
	unsigned char *data,
	unsigned int dataIn)
{
	int err = 0;
	/* Is there really new data available ? */
	if (dataIn > 0) {
		err = readStreamGlue(
			control->endControl,
			&(control->streamGlue),
			data,
			dataIn);
	}
	return err;
}

	int
readStreamGlueLoopBack_end(
	struct readStreamGlueLoopBackStruct *control)
{
	free(control);
	return 0;
}


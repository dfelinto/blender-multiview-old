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
 * define what actions a write stream should do 
 */

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "BLO_writeStreamGlue.h"

	struct streamGlueControlStruct *
streamGlueControlConstructor(
	void)
{
	struct streamGlueControlStruct *control;
	control = malloc(sizeof(struct streamGlueControlStruct));
	assert(control);
	// TODO handle malloc errors
	control->actions = 0;
	control->actionsDone = 0;
	memset(control->action, 0, MAXSTREAMLENGTH);
	return(control);
}

	void
streamGlueControlDestructor(
	struct streamGlueControlStruct *streamControl)
{
	free(streamControl);
}

	int
streamGlueControlAppendAction(
	struct streamGlueControlStruct *streamControl,
	unsigned char nextAction)
{
	assert(streamControl);
	assert(streamControl->actions < MAXSTREAMLENGTH);
	streamControl->action[streamControl->actions] = nextAction;
	streamControl->actions++;
	return(streamControl->actions);
}

	unsigned char
streamGlueControlGetNextAction(
	struct streamGlueControlStruct *streamControl)
{
	unsigned char nextAction;
	assert(streamControl);
	assert(streamControl->actionsDone < streamControl->actions);
	if (streamControl->actionsDone >= streamControl->actions) {
		// the stream should have been terminated by a data
		// processor, but instead streamGlue is called again ...
		nextAction = UNKNOWN;	// best guess ...
	} else {
		nextAction = streamControl->action[streamControl->actionsDone];
		streamControl->actionsDone++;
	}
	return(nextAction);
}


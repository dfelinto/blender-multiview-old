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
 * struct/function that connects the data stream processors
 */

#ifndef BLO_WRITESTREAMGLUE_H
#define BLO_WRITESTREAMGLUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "BLO_readStreamGlue.h"
#include "BLO_writeStreamErrors.h"

/******************** start BLO_streamGlueControl.c part *****************/
struct streamGlueControlStruct {
	int actions;
	int actionsDone;
	unsigned char action[MAXSTREAMLENGTH];
};

	struct streamGlueControlStruct *
streamGlueControlConstructor(
	void);

	void
streamGlueControlDestructor(
	struct streamGlueControlStruct *streamControl);

	int
streamGlueControlAppendAction(
	struct streamGlueControlStruct *streamControl,
	unsigned char nextAction);

	unsigned char
streamGlueControlGetNextAction(
	struct streamGlueControlStruct *streamControl);

// TODO avoid this global variable
extern struct streamGlueControlStruct *Global_streamGlueControl;
/******************** end BLO_streamGlueControl.c part *****************/

struct writeStreamGlueStruct {
	int dataProcessorType;
	unsigned int streamBufferCount;
	unsigned char *streamBuffer;
};

	int
writeStreamGlue(
	struct streamGlueControlStruct *streamGlueControl,
	struct writeStreamGlueStruct **streamGlue,
	unsigned char *data,
	unsigned int dataIn,
	int finishUp);

#ifdef __cplusplus
}
#endif

#endif /* BLO_WRITESTREAMGLUE_H */

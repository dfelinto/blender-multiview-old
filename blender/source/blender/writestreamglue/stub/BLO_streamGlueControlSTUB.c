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
 * 
 */

#include <stdio.h>

#include "GEN_messaging.h"

#include "BLO_writeStreamGlue.h"

	struct streamGlueControlStruct *
streamGlueControlConstructor(
	void)
{
#ifdef NDEBUG
	fprintf(GEN_errorstream,
			"Error streamGlueControlConstructor is a stub\n");
#endif
	return(NULL);
}

	void
streamGlueControlDestructor(
	struct streamGlueControlStruct *streamControl)
{
	streamControl =  NULL;
#ifdef NDEBUG
	fprintf(GEN_errorstream,
			"Error streamGlueControlDestructor is a stub\n");
#endif
}

	int
streamGlueControlAppendAction(
	struct streamGlueControlStruct *streamControl,
	unsigned char nextAction)
{
	streamControl =  NULL;
	nextAction = 0;
#ifdef NDEBUG
	fprintf(GEN_errorstream,
			"Error streamGlueControlAppendAction is a stub\n");
#endif
	return(0);
}

	unsigned char
streamGlueControlGetNextAction(
	struct streamGlueControlStruct *streamControl)
{
	streamControl =  NULL;
#ifdef NDEBUG
	fprintf(GEN_errorstream,
			"Error streamGlueControlGetNextAction is a stub\n");
#endif
	return(0);
}


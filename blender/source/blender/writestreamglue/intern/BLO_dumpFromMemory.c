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
 * streamglue loopback adds a streamGlueHeader to start of the write stream
 */

#include <stdio.h>
#include <stdlib.h>

#include "GEN_messaging.h"

#include "zlib.h"

#include "BLO_writeStreamGlue.h"

#include "BLO_dumpFromMemory.h"

	int
BLO_dumpFromMemory(
	unsigned char *data,
	unsigned int dataIn,
	struct streamGlueHeaderStruct *streamGlueHeader)
{
	struct writeStreamGlueStruct *streamGlue = NULL;
	int err = 0;

#ifndef NDEBUG
	fprintf(GEN_errorstream,
			"BLO_dumpFromMemory: %u streamGlueHeader + %u data = %u\n",
			STREAMGLUEHEADERSIZE,
			dataIn,
			STREAMGLUEHEADERSIZE + dataIn);
#endif
	
	// all data is in. set size in streamGlueHeader and write it out
	streamGlueHeader->totalStreamLength = htonl(dataIn);
	streamGlueHeader->crc = htonl(crc32(0L, (const Bytef *) streamGlueHeader,
										STREAMGLUEHEADERSIZE - 4));
	err = writeStreamGlue(
		Global_streamGlueControl,
		&streamGlue,
		(unsigned char *)streamGlueHeader,
		STREAMGLUEHEADERSIZE,
		0);
	if (err) return err;

	// write out data
	err = writeStreamGlue(
		Global_streamGlueControl,
		&streamGlue,
		data,
		dataIn,
		1);

	return err;
}


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
 * connect the read stream data processors
 */

#ifndef BLO_READSTREAMGLUE_H
#define BLO_READSTREAMGLUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "BLO_sys_types.h"
#include "BLO_readStreamErrors.h"

#define UNKNOWN			0
#define DUMPTOMEMORY	1
#define DUMPFROMMEMORY	2
#define READBLENFILE	3
#define WRITEBLENFILE	4
#define INFLATE			5
#define DEFLATE			6
#define DECRYPT			7
#define ENCRYPT			8
#define VERIFY			9
#define SIGN			10

#define MAXSTREAMLENGTH 10

#define STREAMGLUEHEADERSIZE sizeof(struct streamGlueHeaderStruct)

struct streamGlueHeaderStruct {
	uint8_t magic;					/* poor mans header recognize check */
	uint32_t totalStreamLength;		/* how much data is there */
	uint32_t dataProcessorType;		/* next data processing action */
	uint32_t crc;					/* header minus crc itself checksum */
};

struct readStreamGlueStruct {
	/* my control structure elements */
	unsigned int totalStreamLength;
	unsigned int streamDone;
	int dataProcessorType;
	void *ProcessorTypeControlStruct;

	unsigned char headerbuffer[STREAMGLUEHEADERSIZE];

	void *(*begin)(void *);
	int (*process)(void *, unsigned char *, unsigned int);
	int (*end)(void *);
};

        unsigned int
correctByteOrder(
        unsigned int x);

	int
readStreamGlue(
	void *endControl,
	struct readStreamGlueStruct **control,
	unsigned char *data,
	unsigned int dataIn);

#ifdef __cplusplus
}
#endif

#endif /* BLO_READSTREAMGLUE_H */

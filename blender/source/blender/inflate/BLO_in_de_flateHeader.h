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
 * zlib inflate and deflate stream header
 */

#ifndef BLO_IN_DE_FLATE_H
#define BLO_IN_DE_FLATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "BLO_sys_types.h"

#define IN_DE_FLATEHEADERSTRUCTSIZE sizeof(struct BLO_in_de_flateHeaderStruct)

/* POSIX datatypes, use BYTEORDER(3) */
struct BLO_in_de_flateHeaderStruct {
	uint8_t magic;					/* poor mans header recognize check */
	uint32_t compressedLength;		/* how much compressed data is there */
	uint32_t uncompressedLength;	/* how much uncompressed data there is */
	uint32_t dictionary_id;			/* which dictionary are we using */
	uint32_t dictId;				/* Adler32 value of the dictionary */
	uint32_t crc;					/* header minus crc itself checksum */
};

#ifdef __cplusplus
}
#endif

#endif /* BLO_IN_DE_FLATE_H */

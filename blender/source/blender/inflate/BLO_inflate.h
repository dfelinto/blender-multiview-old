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
 * zlib inflate decompression wrapper library interface
 */

#ifndef BLO_INFLATE_H
#define BLO_INFLATE_H

#ifdef __cplusplus
extern "C" {
#endif

#define INFLATE_DECLARE_HANDLE(name) typedef struct name##__ { int unused; } *name

INFLATE_DECLARE_HANDLE(BLO_inflateStructHandle);

/**
 * zlib inflate decompression initializer
 * @retval pointer to inflate control structure
 */

	BLO_inflateStructHandle
BLO_inflate_begin(
	void *endControl);

/**
 * zlib inflate dataprocessor wrapper
 * @param BLO_inflate Pointer to inflate control structure
 * @param data Pointer to new data
 * @param dataIn New data amount
 * @retval streamGlueRead return value
 */
	int
BLO_inflate_process(
	BLO_inflateStructHandle BLO_inflate_handle,
	unsigned char *data,
	unsigned int dataIn);

/**
 * zlib inflate final call and cleanup
 * @param BLO_inflate Pointer to inflate control structure
 * @retval streamGlueRead return value
 */
	int
BLO_inflate_end(
	BLO_inflateStructHandle BLO_inflate_handle);

#ifdef __cplusplus
}
#endif

#endif /* BLO_INFLATE_H */

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
 */

#ifndef BLO_VERIFY_H
#define BLO_VERIFY_H

#ifdef __cplusplus
extern "C" {
#endif

#define VERIFY_DECLARE_HANDLE(name) typedef struct name##__ { int unused; } *name

VERIFY_DECLARE_HANDLE(BLO_verifyStructHandle);

/**
 * openssl verify initializer
 * @retval pointer to verify control structure
 */
	BLO_verifyStructHandle
BLO_verify_begin(
	void *endControl);

/**
 * openssl verify dataprocessor wrapper
 * @param BLO_verify Pointer to verify control structure
 * @param data Pointer to new data
 * @param dataIn New data amount
 * @retval streamGlueRead return value
 */
	int
BLO_verify_process(
	BLO_verifyStructHandle BLO_verifyHandle,
	unsigned char *data,
	unsigned int dataIn);

/**
 * openssl verify final call and cleanup
 * @param BLO_verify Pointer to verify control structure
 * @retval streamGlueRead return value
 */
	int
BLO_verify_end(
	BLO_verifyStructHandle BLO_verifyHandle);

#ifdef __cplusplus
}
#endif

#endif /* BLO_VERIFY_H */

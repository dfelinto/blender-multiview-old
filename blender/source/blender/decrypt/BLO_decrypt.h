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

#ifndef BLO_DECRYPT_H
#define BLO_DECRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

#define DECRYPT_DECLARE_HANDLE(name) typedef struct name##__ { int unused; } *name
		 
DECRYPT_DECLARE_HANDLE(BLO_decryptStructHandle);

/**
 * openssl decrypt decompression initializer
 * @retval pointer to decrypt control structure
 */
	BLO_decryptStructHandle
BLO_decrypt_begin(
	void *endControl);

/**
 * openssl decrypt dataprocessor wrapper
 * @param BLO_decrypt Pointer to decrypt control structure
 * @param data Pointer to new data
 * @param dataIn New data amount
 * @retval streamGlueRead return value
 */
	int
BLO_decrypt_process(
	BLO_decryptStructHandle BLO_decryptHandle,
	unsigned char *data,
	unsigned int dataIn);

/**
 * openssl decrypt final call and cleanup
 * @param BLO_decrypt Pointer to decrypt control structure
 * @retval streamGlueRead return value
 */
	int
BLO_decrypt_end(
	BLO_decryptStructHandle BLO_decryptHandle);

#ifdef __cplusplus
}
#endif

#endif /* BLO_DECRYPT_H */

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

#ifndef BLO_EN_DE_CRYPT_H
#define BLO_EN_DE_CRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "BLO_sys_types.h"

#define EN_DE_CRYPTHEADERSTRUCTSIZE sizeof(struct BLO_en_de_cryptHeaderStruct)

// Tests showed: pubKeyLen 64, cryptedKeyLen 64 bytes
// So we pick 2*64 bytes + 2 bytes dummy tail for now :
#define MAXPUBKEYLEN 130
#define MAXCRYPTKEYLEN 130

struct BLO_en_de_cryptHeaderStruct {
	uint8_t  magic;				// poor mans header recognize check
	uint32_t length;			// how much crypted data is there
	uint8_t  pubKey[MAXPUBKEYLEN];
	uint32_t pubKeyLen;			// the actual pubKey length
	uint8_t  cryptedKey[MAXCRYPTKEYLEN];
	int32_t  cryptedKeyLen;		// the actual cryptedKey length (NOTE: signed)
	uint32_t datacrc;			// crypted data checksum
	uint32_t headercrc;			// header minus crc itself checksum
};

#ifdef __cplusplus
}
#endif

#endif /* BLO_EN_DE_CRYPT_H */

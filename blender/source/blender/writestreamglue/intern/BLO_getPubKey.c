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
 * Publisher only: get the public key from the .BPkeyfile
 */

#include <stdlib.h>

#include "BLO_keyStore.h"
#include "BLO_getPubKey.h"

	int
getPubKey(byte *dataStreamPubKey,
		  int dataStreamPubKeyLen,
		  byte **publisherPubKey,
		  int *publisherPubKeyLen)
{
	int err = 0;

	*publisherPubKeyLen = keyStoreGetPubKey(publisherPubKey);

	if (*publisherPubKeyLen == 0) {
		// we're a publisher without .BPkey
		*publisherPubKey = NULL;
		return 1;
	}

	if (dataStreamPubKeyLen != *publisherPubKeyLen) {
		// different keys
		*publisherPubKeyLen = 0;
		*publisherPubKey = NULL;
		return 2;
	}

	if (memcmp(dataStreamPubKey, *publisherPubKey, *publisherPubKeyLen)
		!= 0) {
		// different keys
		*publisherPubKeyLen = 0;
		*publisherPubKey = NULL;
		return 3;
	}

	return err;
}


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
 * make all key elements available through functions
 */

#include <stdlib.h>
#include <assert.h>

#include "BLO_keyStore.h"
#include "BLO_keyStorePrivate.h"

// our ugly but private global pointer
static struct keyStoreStruct *keyStore = NULL;

	void
keyStoreConstructor(
	UserStruct *keyUserStruct,
	char *privHexKey,
	char *pubHexKey,
	byte *ByteChecks,
	char *HexPython)
{
	assert(keyStore == NULL);
	keyStore = malloc(sizeof(struct keyStoreStruct));
	assert(keyStore);
	// TODO check for malloc errors

	keyStore->keyUserStruct = *keyUserStruct;

	keyStore->privKey = DeHexify(privHexKey);
	keyStore->privKeyLen = strlen(privHexKey) / 2;

	keyStore->pubKey = DeHexify(pubHexKey);
	keyStore->pubKeyLen = strlen(pubHexKey) / 2;

	memcpy(keyStore->ByteChecks, ByteChecks, 1000);

	keyStore->PythonCode = DeHexify(HexPython);
	keyStore->PythonCodeLen = strlen(HexPython) / 2;
}

	void
keyStoreDestructor(
	void)
{
	assert(keyStore);
	if (!keyStore) {
		return;
	}
	free(keyStore->privKey);
	free(keyStore->pubKey);
	free(keyStore->PythonCode);
	free(keyStore);
	keyStore = NULL;
}

	int
keyStoreGetPubKey(
	byte **PubKey)
{
	if (!keyStore) {
		*PubKey = NULL;
		return 0;
	}
	*PubKey = keyStore->pubKey;
	return(keyStore->pubKeyLen);
}

	int
keyStoreGetPrivKey(
	byte **PrivKey)
{
	if (!keyStore) {
		*PrivKey = NULL;
		return 0;
	}
	*PrivKey = keyStore->privKey;
	return(keyStore->privKeyLen);
}

	char *
keyStoreGetUserName(
	void)
{
	if (!keyStore) {
		return NULL;
	}
	return(keyStore->keyUserStruct.name);
}

	char *
keyStoreGetEmail(
	void)
{
	if (!keyStore) {
		return NULL;
	}
	return(keyStore->keyUserStruct.email);
}


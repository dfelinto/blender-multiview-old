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
 * decrypt STUB
 */

#include <stdio.h>

#include "GEN_messaging.h"

#include "BLO_readStreamErrors.h"
#include "BLO_decrypt.h"

	BLO_decryptStructHandle
BLO_decrypt_begin(
	void *endControl)
{
#ifndef NDEBUG
	fprintf(GEN_errorstream,
			"Error BLO_decrypt_begin is a stub\n");
#endif
	return (NULL);
}

	int
BLO_decrypt_process(
	BLO_decryptStructHandle BLO_decryptHandle,
	unsigned char *data,
	unsigned int dataIn)
{
	int err = 0;
	BLO_decryptHandle = NULL;
	data = NULL;
	dataIn = 0;
#ifndef NDEBUG
	fprintf(GEN_errorstream,
			"Error BLO_decrypt_process is a stub\n");
#endif
	err = BRS_SETFUNCTION(BRS_DECRYPT) |
		  BRS_SETGENERR(BRS_STUB);
	return (err);
}

	int
BLO_decrypt_end(
	BLO_decryptStructHandle BLO_decryptHandle)
{
	int err = 0;
	BLO_decryptHandle = NULL;
#ifndef NDEBUG
	fprintf(GEN_errorstream,
			"Error BLO_decrypt_end is a stub\n");
#endif	
	err = BRS_SETFUNCTION(BRS_DECRYPT) |
		  BRS_SETGENERR(BRS_STUB);
	return (err);
}


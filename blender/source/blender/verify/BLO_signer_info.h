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

#include "BLO_sign_verify_Header.h"

/* external struct for signer info */

struct BLO_SignerInfo {
	char name[MAXSIGNERLEN];
	char email[MAXSIGNERLEN];
	char homeUrl[MAXSIGNERLEN];
	// more to come...
};

struct BLO_SignerInfo *BLO_getSignerInfo(void);
int BLO_isValidSignerInfo(struct BLO_SignerInfo *info);
void BLO_clrSignerInfo(struct BLO_SignerInfo *info);

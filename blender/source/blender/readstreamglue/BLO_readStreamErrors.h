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
 * all Blender Read Stream errors
 */

#ifndef BLO_READSTREAMERRORS_H
#define BLO_READSTREAMERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

#define BRS_SETFUNCTION(x)	(  (int)(x) << 1)
#define BRS_GETFUNCTION(x)	(( (int)(x) >> 1) & 7)
#define BRS_SETGENERR(x)	(  (int)(x) << 4)
#define BRS_GETGENERR(x)	(( (int)(x) >> 4) & 7)
#define BRS_SETSPECERR(x)	(  (int)(x) << 7)
#define BRS_GETSPECERR(x)	(( (int)(x) >> 7) & 7)

// FUNCTION
#define BRS_READSTREAMGLUE	1
#define BRS_READSTREAMLOOP	2
#define BRS_KEYSTORE		3
#define BRS_READSTREAMFILE	4
#define BRS_INFLATE			5
#define BRS_DECRYPT			6
#define BRS_VERIFY			7

// GENeric errors
#define BRS_MALLOC			1
#define BRS_NULL			2
#define BRS_MAGIC			3
#define BRS_CRCHEADER		4
#define BRS_CRCDATA			5
#define BRS_DATALEN			6
#define BRS_STUB			7

// READSTREAMGLUE specific
#define BRS_UNKNOWN			1

// READSTREAMFILE specific
#define BRS_NOTABLEND		1
#define BRS_READERROR		2

// INFLATE specific
#define BRS_INFLATEERROR	1

// DECRYPT specific
#define BRS_RSANEWERROR		1
#define BRS_DECRYPTERROR	2
#define BRS_NOTOURPUBKEY	3

// VERIFY specific
#define BRS_RSANEWERROR		1
#define BRS_SIGFAILED		2

#ifdef __cplusplus
}
#endif

#endif /* BLO_READSTREAMERRORS_H */

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
 * all Blender Write Stream errors
 * NOTE: adding stuff here means you also have to update the error
 * messages in writefile.c
 */

#ifndef BLO_WRITESTREAMERRORS_H
#define BLO_WRITESTREAMERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

#define BWS_SETFUNCTION(x)  (  (int)(x) << 1)
#define BWS_GETFUNCTION(x)  (( (int)(x) >> 1) & 7)
#define BWS_SETGENERR(x)    (  (int)(x) << 4)
#define BWS_GETGENERR(x)    (( (int)(x) >> 4) & 7)
#define BWS_SETSPECERR(x)   (  (int)(x) << 7)
#define BWS_GETSPECERR(x)   (( (int)(x) >> 7) & 7)

// FUNCTION
#define BWS_WRITESTREAMGLUE	1
#define BWS_DEFLATE			2
#define BWS_ENCRYPT			3
#define BWS_SIGN			4
#define BWS_WRITEBLENFILE	5

// GENeric errors
#define BWS_MALLOC			1
#define BWS_STUB			2
#define BWS_RSA				3

// WRITESTREAMGLUE specific
#define BWS_UNKNOWN			1

// DEFLATE specific
#define BWS_DEFLATEERROR	1

// ENCRYPT specific
#define BWS_RSANEWERROR		1
#define BWS_ENCRYPTERROR	2

// SIGN specific
#define BWS_RSANEWERROR		1
#define BWS_SIGNERROR		2

// WRITEBLENFILE specific
#define BWS_FILEDES			1
#define BWS_WRITE			2
#define BWS_PARAM			3

#ifdef __cplusplus
}
#endif

#endif /* BLO_WRITESTREAMERRORS_H */

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
 * A platform-independent definition of [u]intXX_t
 * Plus the accompanying header include for htonl/ntohl
 *
 * This file includes <sys/types.h> to define [u]intXX_t types, where
 * XX can be 8, 16, 32 or 64. Unfortunately, not all systems have this
 * file.
 * - Windows uses __intXX compiler-builtin types. These are signed,
 *   so we have to flip the signs.
 * For these rogue platforms, we make the typedefs ourselves.
 *
 */

#ifndef BLO_SYS_TYPES_H
#define BLO_SYS_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32

/* The __intXX are built-in types of the visual complier! So we don't
 * need to include anything else here. */

typedef signed __int8  int8_t;
typedef signed __int16 int16_t;
typedef signed __int32 int32_t;
typedef signed __int64 int64_t;

typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

#elif defined(__linux__)

	/* Linux-i386, Linux-Alpha, Linux-ppc */
#include <stdint.h>

#elif defined (__APPLE__)

#include <inttypes.h>

#else

	/* FreeBSD, Irix, Solaris */
#include <sys/types.h>

#endif /* ifdef platform for types */

#ifdef _WIN32
#define htonl(x) correctByteOrder(x)
#define ntohl(x) correctByteOrder(x)
#elif defined __FreeBSD__
#include <sys/param.h>
#elif defined (__APPLE__)
#include <sys/types.h>
#else  /* irix sun linux */
#include <netinet/in.h>
#endif /* ifdef platform for htonl/ntohl */

#ifdef __cplusplus 
}
#endif

#endif 
/* eof */

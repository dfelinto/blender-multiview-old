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
 * zlib deflate compression wrapper library interface
 */

#ifndef BLO_DEFLATE_H
#define BLO_DEFLATE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * zlib deflate dataprocessor wrapper
 * @param data Pointer to raw input data
 * @param dataIn Raw data input amount
 * @param streamGlueHeader Our streamGlueHeaderStruct
 * @retval streamGlueWrite return value
 */
	int
BLO_deflate(
	unsigned char *data,
	unsigned int dataIn,
	struct streamGlueHeaderStruct *streamGlueHeader);

#ifdef __cplusplus
}
#endif

#endif /* BLO_DEFLATE_H */

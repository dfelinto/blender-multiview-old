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

#ifndef BLO_WRITEBLENFILE_H
#define BLO_WRITEBLENFILE_H

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * Writes a file from memory to disk. The filedescriptor of the
	 * targetfile is taken from mywfile.
	 *
	 * @param data   pointer to the datachunk to write
	 * @param dataIn size of the datachunk to write (bytes)
	 * @param streamGlueHeader the eader describing this datachunk
	 * @returns 0 if the file was written correctly, non-0 if an error
	 * occured.  */
	int
BLO_writeblenfile(
	unsigned char *data,
	unsigned int dataIn,
	struct streamGlueHeaderStruct *streamGlueHeader);

#ifdef __cplusplus
}
#endif

#endif /* BLO_WRITEBLENFILE_H */

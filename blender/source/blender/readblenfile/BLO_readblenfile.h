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

#ifndef BLO_READBLENFILE_H
#define BLO_READBLENFILE_H

#ifdef __cplusplus
extern "C" {
#endif

	BlendFileData *
BLO_readblenfilename(
	char *fileName, 
	BlendReadError *error_r);

	BlendFileData *
BLO_readblenfilehandle(
	int fileHandle, 
	BlendReadError *error_r);

	BlendFileData *
BLO_readblenfilememory(
	char *fromBuffer,
	int fromBufferSize, 
	BlendReadError *error_r);


	void
BLO_setcurrentversionnumber(
	char array[4]);

	void
BLO_setversionnumber(
	char array[4],
	int version);

	int
blo_is_a_runtime(
	char *file);

	BlendFileData *
blo_read_runtime(
	char *file, 
	BlendReadError *error_r);

#define BLO_RESERVEDSIZE 12
extern char *headerMagic;

#ifdef __cplusplus
}
#endif

#endif /* BLO_READBLENFILE_H */

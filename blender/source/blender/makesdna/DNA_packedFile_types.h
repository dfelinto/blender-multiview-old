/* DNA_packedFile_types.h 
 * 
 * 12-oct-2000 nzc
 * 
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
 */

#ifndef DNA_PACKEDFILE_TYPES_H
#define DNA_PACKEDFILE_TYPES_H

typedef struct PackedFile {
	int size;
	int seek;
	int flags;
	int pad;
	void * data;
} PackedFile;

enum PF_FileStatus
{
	PF_EQUAL = 0,
	PF_DIFFERS,
	PF_NOFILE,
			
	PF_WRITE_ORIGINAL,
	PF_WRITE_LOCAL,
	PF_USE_LOCAL,
	PF_USE_ORIGINAL,
	PF_KEEP,
	PF_NOOP,
			
	PF_ASK
};

#endif /* PACKEDFILE_TYPES_H */

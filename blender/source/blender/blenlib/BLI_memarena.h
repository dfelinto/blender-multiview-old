/* 
 * Memory arena ADT
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
 * 
 * Memory arena's are commonly used when the program
 * needs to quickly allocate lots of little bits of
 * data, which are all freed at the same moment.
 * 
 */

#ifndef BLI_MEMARENA_H
#define BLI_MEMARENA_H

	/* A reasonable standard buffer size, big
	 * enough to not cause much internal fragmentation, 
	 * small enough not to waste resources
	 */
#define BLI_MEMARENA_STD_BUFSIZE	4096

struct MemArena;
typedef struct MemArena MemArena;


struct MemArena*	BLI_memarena_new	(int bufsize);
void				BLI_memarena_free	(struct MemArena *ma);

void*				BLI_memarena_alloc	(struct MemArena *ma, int size);

#endif

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
 * Efficient memory allocation for lots of similar small chunks.
 */

#include "MEM_guardedalloc.h"

#include "BLI_blenlib.h"
#include "BLI_memarena.h"
#include "BLI_linklist.h"

struct MemArena {
	unsigned char *curbuf;
	int bufsize, cursize;
	
	LinkNode *bufs;
};

MemArena *BLI_memarena_new(int bufsize) {
	MemArena *ma= MEM_callocN(sizeof(*ma), "memarena");
	ma->bufsize= bufsize;
	
	return ma;
}
void BLI_memarena_free(MemArena *ma) {
	BLI_linklist_free(ma->bufs, MEM_freeN);
	MEM_freeN(ma);
}

	/* amt must be power of two */
#define PADUP(num, amt)	((num+(amt-1))&~(amt-1))

void *BLI_memarena_alloc(MemArena *ma, int size) {
	void *ptr;

		/* ensure proper alignment by rounding
		 * size up to multiple of 8 */	
	size= PADUP(size, 8);
	
	if (size>=ma->cursize) {
		ma->cursize= (size>ma->bufsize)?size:ma->bufsize;
		ma->curbuf= MEM_mallocN(ma->cursize, "ma->curbuf");
		
		BLI_linklist_prepend(&ma->bufs, ma->curbuf);
	}
	
	ptr= ma->curbuf;
	ma->curbuf+= size;
	ma->cursize-= size;
	
	return ptr;
}

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
 * Support for linked lists.
 */

#include "MEM_guardedalloc.h"
#include "BLI_blenlib.h"
#include "BLI_linklist.h"
#include "BLI_memarena.h"

int BLI_linklist_length(LinkNode *list) {
	if (0) {
		return list?(1+BLI_linklist_length(list->next)):0;
	} else {
		int len;

		for (len=0; list; list= list->next)
			len++;
	
		return len;
	}
}

void BLI_linklist_reverse(LinkNode **listp) {
	LinkNode *rhead= NULL, *cur= *listp;
	
	while (cur) {
		LinkNode *next= cur->next;
		
		cur->next= rhead;
		rhead= cur;
		
		cur= next;
	}
	
	*listp= rhead;
}

void BLI_linklist_prepend(LinkNode **listp, void *ptr) {
	LinkNode *nlink= MEM_mallocN(sizeof(*nlink), "nlink");
	nlink->link= ptr;
	
	nlink->next= *listp;
	*listp= nlink;
}

void BLI_linklist_prepend_arena(LinkNode **listp, void *ptr, MemArena *ma) {
	LinkNode *nlink= BLI_memarena_alloc(ma, sizeof(*nlink));
	nlink->link= ptr;
	
	nlink->next= *listp;
	*listp= nlink;
}

void BLI_linklist_free(LinkNode *list, LinkNodeFreeFP freefunc) {
	while (list) {
		LinkNode *next= list->next;
		
		if (freefunc)
			freefunc(list->link);
		MEM_freeN(list);
		
		list= next;
	}
}

void BLI_linklist_apply(LinkNode *list, LinkNodeApplyFP applyfunc) {
	for (; list; list= list->next)
		applyfunc(list->link);
}

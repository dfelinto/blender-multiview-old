/*
 * Routines for working with singly linked lists
 * of 'links' - pointers to other data.
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
 
#ifndef BLI_LINKLIST_H
#define BLI_LINKLIST_H

struct MemArena;

typedef void (*LinkNodeFreeFP)(void *link);
typedef void (*LinkNodeApplyFP)(void *link);

struct LinkNode;
typedef struct LinkNode {
	struct LinkNode *next;
	void *link;
} LinkNode;

int		BLI_linklist_length		(struct LinkNode *list);

void	BLI_linklist_reverse	(struct LinkNode **listp);

void	BLI_linklist_prepend		(struct LinkNode **listp, void *ptr);
void	BLI_linklist_prepend_arena	(struct LinkNode **listp, void *ptr, struct MemArena *ma);

void	BLI_linklist_free		(struct LinkNode *list, LinkNodeFreeFP freefunc);
void	BLI_linklist_apply		(struct LinkNode *list, LinkNodeApplyFP applyfunc);

#endif

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
 */
#include <string.h>

#include "MEM_guardedalloc.h"

#include "BLI_gsqueue.h"

typedef struct _GSQueueElem GSQueueElem;
struct _GSQueueElem {
	GSQueueElem *next;
};

struct _GSQueue {
	GSQueueElem	*head;
	GSQueueElem	*tail;
	int			elem_size;
};

GSQueue *BLI_gsqueue_new(int elem_size)
{
	GSQueue *gq= MEM_mallocN(sizeof(*gq), "gqueue_new");
	gq->head= gq->tail= NULL;
	gq->elem_size= elem_size;
	
	return gq;
}

int BLI_gsqueue_is_empty(GSQueue *gq)
{
	return (gq->head==NULL);
}

void BLI_gsqueue_peek(GSQueue *gq, void *item_r)
{
	memcpy(item_r, &gq->head[1], gq->elem_size);
}
void BLI_gsqueue_pop(GSQueue *gq, void *item_r)
{
	GSQueueElem *elem= gq->head;
	if (elem==gq->tail) {
		gq->head= gq->tail= NULL;
	} else {
		gq->head= gq->head->next;
	}
	
	if (item_r) memcpy(item_r, &elem[1], gq->elem_size);
	MEM_freeN(elem);
}
void BLI_gsqueue_push(GSQueue *gq, void *item)
{
	GSQueueElem *elem= MEM_mallocN(sizeof(*elem)+gq->elem_size, "gqueue_push");
	memcpy(&elem[1], item, gq->elem_size);
	elem->next= NULL;
	
	if (BLI_gsqueue_is_empty(gq)) {
		gq->tail= gq->head= elem;
	} else {
		gq->tail= gq->tail->next= elem;
	}
}
void BLI_gsqueue_pushback(GSQueue *gq, void *item)
{
	GSQueueElem *elem= MEM_mallocN(sizeof(*elem)+gq->elem_size, "gqueue_push");
	memcpy(&elem[1], item, gq->elem_size);
	elem->next= gq->head;

	if (BLI_gsqueue_is_empty(gq)) {
		gq->head= gq->tail= elem;
	} else {
		gq->head= elem;
	}
}

void BLI_gsqueue_free(GSQueue *gq)
{
	while (gq->head) {
		BLI_gsqueue_pop(gq, NULL);
	}
	MEM_freeN(gq);
}



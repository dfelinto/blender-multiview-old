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
 * Dynamically sized string ADT
 */

#include <stdlib.h>
#include <string.h>

#include "MEM_guardedalloc.h"

#include "BLI_blenlib.h"
#include "BLI_dynstr.h"

/***/

typedef struct DynStrElem DynStrElem;
struct DynStrElem {
	DynStrElem *next;
	
	char *str;
};

struct DynStr {
	DynStrElem *elems, *last;
	int curlen;
};

/***/

DynStr *BLI_dynstr_new(void) {
	DynStr *ds= MEM_mallocN(sizeof(*ds), "DynStr");
	ds->elems= ds->last= NULL;
	ds->curlen= 0;
	
	return ds;
}

void BLI_dynstr_append(DynStr *ds, char *cstr) {
	DynStrElem *dse= malloc(sizeof(*dse));
	int cstrlen= strlen(cstr);
	
	dse->str= malloc(cstrlen+1);
	memcpy(dse->str, cstr, cstrlen+1);
	dse->next= NULL;
	
	if (!ds->last)
		ds->last= ds->elems= dse;
	else
		ds->last= ds->last->next= dse;

	ds->curlen+= cstrlen;
}

int BLI_dynstr_get_len(DynStr *ds) {
	return ds->curlen;
}

char *BLI_dynstr_get_cstring(DynStr *ds) {
	char *s, *rets= MEM_mallocN(ds->curlen+1, "dynstr_cstring");
	DynStrElem *dse;
	
	for (s= rets, dse= ds->elems; dse; dse= dse->next) {
		int slen= strlen(dse->str);

		memcpy(s, dse->str, slen);

		s+= slen;
	}
	rets[ds->curlen]= '\0';
	
	return rets;
}

void BLI_dynstr_free(DynStr *ds) {
	DynStrElem *dse;
	
	for (dse= ds->elems; dse; ) {
		DynStrElem *n= dse->next;
		
		free(dse->str);
		free(dse);
		
		dse= n;
	}
	
	MEM_freeN(ds);
}

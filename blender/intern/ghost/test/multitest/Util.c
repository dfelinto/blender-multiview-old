/**
 * $Id$
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

#include <stdlib.h>

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "MEM_guardedalloc.h"

#include "Util.h"

void* memdbl(void *mem, int *size_pr, int item_size) {
	int cur_size= *size_pr;
	int new_size= cur_size?(cur_size*2):1;
	void *nmem= MEM_mallocN(new_size*item_size, "memdbl");
	
	memcpy(nmem, mem, cur_size*item_size);
	MEM_freeN(mem);
		
	*size_pr= new_size;
	return nmem;
}

char* string_dup(char *str) {
	int len= strlen(str);
	char *nstr= MEM_mallocN(len + 1, "string_dup");

	memcpy(nstr, str, len+1);
	
	return nstr;
}

void fatal(char *fmt, ...) {
	va_list ap;
	
	fprintf(stderr, "FATAL: ");
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	
	exit(1);
}

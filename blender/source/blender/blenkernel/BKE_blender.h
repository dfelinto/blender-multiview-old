/**
 * blenlib/BKE_blender.h (mar-2001 nzc)
 *	
 * Blender util stuff?
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
#ifndef BKE_BLENDER_H
#define BKE_BLENDER_H

#ifdef __cplusplus
extern "C" {
#endif

struct ListBase;

#define BLENDER_VERSION		225

int	BKE_read_file(char *dir, void *type_r);
int BKE_read_file_from_memory(char* filebuf, int filelength, void *type_r);

void duplicatelist(struct ListBase *list1, struct ListBase *list2);
void free_blender(void);
void initglobals(void);

void pushdata(void *data, int len);
void popfirst(void *data);
void poplast(void *data);
void free_pushpop(void);
void pushpop_test(void);

#ifdef __cplusplus
}
#endif

#endif

/**
 * blenlib/BKE_library.h (mar-2001 nzc)
 *	
 * Library
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
#ifndef BKE_LIBRARY_TYPES_H
#define BKE_LIBRARY_TYPES_H

struct ListBase;
struct ID;
struct Main;
struct Library;

void *alloc_libblock(struct ListBase *lb, short type, char *name);
void *copy_libblock(void *rt);

void id_lib_extern(struct ID *id);
void id_us_plus(struct ID *id);

int new_id(struct ListBase *lb, struct ID *id, char *name);

struct ListBase *wich_libbase(struct Main *mainlib, short type);
int set_listbasepointers(struct Main *main, struct ListBase **lb);

void free_libblock(struct ListBase *lb, void *idv);
void free_libblock_us(struct ListBase *lb, void *idv);
void free_main(struct Main *mainvar);

void splitIDname(char *name, char *left, int *nr);
void rename_id(struct ID *id, char *name);
void test_idbutton(char *name);
void all_local(void);
struct ID *find_id(char *type, char *name);
void clear_id_newpoins(void);

void IDnames_to_pupstring(char **str, char *title, char *extraops, struct ListBase *lb,struct ID* link, short *nr);
void IPOnames_to_pupstring(char **str, char *title, char *extraops, struct ListBase *lb, struct ID* link, short *nr, int blocktype);

#endif

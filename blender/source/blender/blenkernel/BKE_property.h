/**
 * blenkernel/BKE_property.h (mar-2001 nzc)
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
#ifndef BKE_PROPERTY_H
#define BKE_PROPERTY_H

struct bProperty;
struct ListBase;
struct Object;

void free_property(struct bProperty *prop);
void free_properties(struct ListBase *lb);
struct bProperty *copy_property(struct bProperty *prop);
void copy_properties(struct ListBase *lbn, struct ListBase *lbo);
void init_property(struct bProperty *prop);
struct bProperty *new_property(int type);
struct bProperty *get_property(struct Object *ob, char *name);
int compare_property(struct bProperty *prop, char *str);
void set_property(struct bProperty *prop, char *str);
void add_property(struct bProperty *prop, char *str);
void set_property_valstr(struct bProperty *prop, char *str);
void cp_property(struct bProperty *prop1, struct bProperty *prop2);
	
#endif

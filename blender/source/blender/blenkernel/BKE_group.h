/**
 * blenlib/BKE_group.h (mar-2001 nzc)
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
#ifndef BKE_GROUP_H
#define BKE_GROUP_H
struct Group;
struct GroupKey;
struct GroupObject;
struct ObjectKey;
struct Object;

void free_object_key(struct ObjectKey *ok);
void free_group_object(struct GroupObject *go);
void free_group(struct Group *group);
struct Group *add_group(void);
void object_to_obkey(struct Object *ob, struct ObjectKey *ok);
void obkey_to_object(struct ObjectKey *ok, struct Object *ob);
void add_object_key(struct GroupObject *go, struct GroupKey *gk);
void add_to_group(struct Group *group, struct Object *ob);
void rem_from_group(struct Group *group, struct Object *ob);
void add_group_key(struct Group *group);
void set_object_key(struct Object *ob, struct ObjectKey *ok);
void set_group_key(struct Group *group);
struct Group *find_group(struct Object *ob);
void set_group_key_name(struct Group *group, char *name);
void set_group_key_frame(struct Group *group, float frame);

#endif

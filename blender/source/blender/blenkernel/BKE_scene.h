/**
 * blenlib/BKE_scene.h (mar-2001 nzc)
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
#ifndef BKE_SCENE_H
#define BKE_SCENE_H

struct Scene;
struct Object;
struct Base;
struct AviCodecData;

void free_avicodecdata(struct AviCodecData *acd);
void free_scene(struct Scene *me);
struct Scene *add_scene(char *name);
int object_in_scene(struct Object *ob, struct Scene *sce);
void sort_baselist(struct Scene *sce);
void set_scene_bg(struct Scene *sce);
void set_scene_name(char *name);
int next_object(int val, struct Base **base, struct Object **ob);
struct Object *scene_find_camera(struct Scene *sc);

struct Base *scene_add_base(struct Scene *sce, struct Object *ob);
void scene_deselect_all(struct Scene *sce);
void scene_select_base(struct Scene *sce, struct Base *selbase);

#endif

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
 * General operations, lookup, etc. for materials.
 */

#ifndef BKE_MATERIAL_H
#define BKE_MATERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

struct Material;
struct ID;
struct Object;

void free_material(struct Material *sc); 
void test_object_materials(struct ID *id);
void init_material(struct Material *ma);
struct Material *add_material(char *name);
struct Material *copy_material(struct Material *ma);
void make_local_material(struct Material *ma);
struct Material ***give_matarar(struct Object *ob);
short *give_totcolp(struct Object *ob);
struct Material *give_current_material(struct Object *ob, int act);
ID *material_from(struct Object *ob, int act);
void assign_material(struct Object *ob, struct Material *ma, int act);
void new_material_to_objectdata(struct Object *ob);
void init_render_material(struct Material *ma);
void init_render_materials(void);
void end_render_material(struct Material *ma);
void end_render_materials(void);
void automatname(struct Material *ma);
void delete_material_index(void);            

#ifdef __cplusplus
}
#endif

#endif

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
 * General operations, lookup, etc. for blender objects.
 */

#ifndef BKE_OBJECT_H
#define BKE_OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

struct Base;
struct Object;
struct Camera;
struct BoundBox;
struct View3D;

void clear_workob(void);
void copy_baseflags(void);
void copy_objectflags(void);
void update_base_layer(struct Object *ob);
void free_object(struct Object *ob);
void unlink_object(struct Object *ob);
int exist_object(struct Object *obtest);
void *add_camera(void);
struct Camera *copy_camera(struct Camera *cam);
void make_local_camera(struct Camera *cam);
void *add_lamp(void);
struct Lamp *copy_lamp(struct Lamp *la);
void make_local_lamp(struct Lamp *la);
void free_camera(struct Camera *ca);
void free_lamp(struct Lamp *la);
void *add_wave(void);

struct Object *add_object(int type);
void base_init_from_view3d(struct Base *base, struct View3D *v3d);

struct Object *copy_object(struct Object *ob);
void expand_local_object(struct Object *ob);
void make_local_object(struct Object *ob);
void set_mblur_offs(int blur);
void disable_speed_curve(int val);
float bsystem_time(struct Object *ob, struct Object *par, float cfra, float ofs);
void object_to_mat3(struct Object *ob, float mat[][3]);
void object_to_mat4(struct Object *ob, float mat[][4]);
void ob_parcurve(struct Object *ob, struct Object *par, float mat[][4]);
void ob_parlimb(struct Object *ob, struct Object *par, float mat[][4]);
void ob_parbone(struct Object *ob, struct Object *par, float mat[][4]);	/* __NLA */
void give_parvert(struct Object *par, int nr, float *vec);
void ob_parvert3(struct Object *ob, struct Object *par, float mat[][4]);
void set_no_parent_ipo(int val);
void set_dtime(int dtime);
void disable_where_script(short on);
int during_script(void);
void where_is_object_time(struct Object *ob, float ctime);
void where_is_object(struct Object *ob);
void where_is_object_simul(struct Object *ob);
void what_does_parent1(struct Object *par, int partype, int par1, int par2, int par3);
void what_does_parent(struct Object *ob);
struct BoundBox *unit_boundbox(void);
void minmax_object(struct Object *ob, float *min, float *max);
void solve_tracking (struct Object *ob, float targetmat[][4]);
void solve_constraints (struct Object *ob, short obtype, void *obdata, float ctime);

	
#ifdef __cplusplus
}
#endif

#endif

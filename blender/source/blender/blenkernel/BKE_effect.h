/**
 * blenlib/BKE_effect.h (mar-2001 nzc)
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
#ifndef BKE_EFFECT_H
#define BKE_EFFECT_H

struct Effect;
struct ListBase;
struct Object;
struct PartEff;
struct MTex;
struct Mesh;
struct WaveEff;
struct Particle;

struct Effect *add_effect(int type);
void free_effect(struct Effect *eff);
void free_effects(struct ListBase *lb);
struct Effect *copy_effect(struct Effect *eff);
void copy_act_effect(struct Object *ob);
void copy_effects(struct ListBase *lbn, struct ListBase *lb);
void deselectall_eff(struct Object *ob);
void set_buildvars(struct Object *ob, int *start, int *end);
struct Particle *new_particle(struct PartEff *paf);
struct PartEff *give_parteff(struct Object *ob);
void where_is_particle(struct PartEff *paf, struct Particle *pa, float ctime, float *vec);
void particle_tex(struct MTex *mtex, struct PartEff *paf, float *co, float *no);
void make_particle_keys(int depth, int nr, struct PartEff *paf, struct Particle *part, float *force, int deform, struct MTex *mtex);
void init_mv_jit(float *jit, int num);
void give_mesh_mvert(struct Mesh *me, int nr, float *co, short *no);
void build_particle_system(struct Object *ob);
void calc_wave_deform(struct WaveEff *wav, float ctime, float *co);
void object_wave(struct Object *ob);                            

#endif

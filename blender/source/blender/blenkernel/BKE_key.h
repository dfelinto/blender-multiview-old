/**
 * blenlib/BKE_key.h (mar-2001 nzc)
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
#ifndef BKE_KEY_H
#define BKE_KEY_H

struct Key;
struct KeyBlock;
struct ID;
struct ListBase;
struct Curve;
struct Object;
struct Lattice;
struct Mesh;

void free_key(struct Key *sc); 
struct Key *add_key(struct ID *id);
struct Key *copy_key(struct Key *key);
void make_local_key(struct Key *key);
void sort_keys(struct Key *key);
void set_four_ipo(float d, float *data, int type);
void set_afgeleide_four_ipo(float d, float *data, int type);
int setkeys(float fac, struct ListBase *lb, struct KeyBlock *k[], float *t, int cycl);
void flerp(int aantal, float *in, float *f0, float *f1, float *f2, float *f3, float *t);
void cp_key(int start, int end, int tot, char *poin, struct Key *key, struct KeyBlock *k, int mode);
void cp_cu_key(struct Curve *cu, struct KeyBlock *kb, int start, int end);
void rel_flerp(int aantal, float *in, float *ref, float *out, float fac);
void do_rel_key(int start, int end, int tot, char *basispoin, struct Key *key, float ctime, int mode);
void do_key(int start, int end, int tot, char *poin, struct Key *key, struct KeyBlock **k, float *t, int mode);
void do_mesh_key(struct Mesh *me);
void do_cu_key(struct Curve *cu, struct KeyBlock **k, float *t);
void do_rel_cu_key(struct Curve *cu, float ctime);
void do_curve_key(struct Curve *cu);
void do_latt_key(struct Lattice *lt);
void do_all_keys(void);
void do_ob_key(struct Object *ob);
void do_spec_key(struct Key *key);
	
#endif

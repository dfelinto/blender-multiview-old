/**
 * blenlib/BKE_ika.h (mar-2001 nzc)
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
#ifndef BKE_IKA_H
#define BKE_IKA_H

struct Ika;
struct Limb;
struct Object;

void unlink_ika(struct Ika *ika);
void free_ika(struct Ika *ika);
struct Ika *add_ika(void);
struct Ika *copy_ika(struct Ika *ika);
void make_local_ika(struct Ika *ika);
int count_limbs(struct Object *ob);
void calc_limb(struct Limb *li);
void calc_ika(struct Ika *ika, struct Limb *li);
void init_defstate_ika(struct Object *ob);
void itterate_limb(struct Ika *ika, struct Limb *li);
void rotate_ika(struct Object *ob, struct Ika *ika);
void rotate_ika_xy(struct Object *ob, struct Ika *ika);
void itterate_ika(struct Object *ob);
void do_all_ikas(void);
void do_all_visible_ikas(void);
void init_skel_deform(struct Object *par, struct Object *ob);
void calc_skel_deform(struct Ika *ika, float *co);

#endif

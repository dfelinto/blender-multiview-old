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
 * june-2001 ton
 */

#ifndef BKE_LATTICE_H
#define BKE_LATTICE_H

struct Lattice;
struct Object;
struct BPoint;

extern struct Lattice *editLatt;
extern int lt_applyflag;


void resizelattice(struct Lattice *lt);
struct Lattice *add_lattice(void);
struct Lattice *copy_lattice(struct Lattice *lt);
void free_lattice(struct Lattice *lt);
void make_local_lattice(struct Lattice *lt);
void calc_lat_fudu(int flag, int res, float *fu, float *du);
void init_latt_deform(struct Object *oblatt, struct Object *ob);
void calc_latt_deform(float *co);
void end_latt_deform(void);
int object_deform(struct Object *ob);
struct BPoint *latt_bp(struct Lattice *lt, int u, int v, int w);
void outside_lattice(struct Lattice *lt);


#endif

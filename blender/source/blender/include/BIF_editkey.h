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
 */

#ifndef BIF_EDITKEY_H
#define BIF_EDITKEY_H

struct Key;
struct KeyBlock;
struct Mesh;
struct Object;
struct Lattice;
struct Curve;

void mesh_to_key(struct Mesh *me, struct KeyBlock *kb);
void key_to_mesh(struct KeyBlock *kb, struct Mesh *me);
void insert_meshkey(struct Mesh *me);

void latt_to_key(struct Lattice *lt, struct KeyBlock *kb);
void key_to_latt(struct KeyBlock *kb, struct Lattice *lt);
void insert_lattkey(struct Lattice *lt);

void curve_to_key(struct Curve *cu, struct KeyBlock *kb, ListBase *nurb);
void key_to_curve(struct KeyBlock *kb, struct Curve  *cu, ListBase *nurb);
void insert_curvekey(struct Curve *cu);

struct Key *give_current_key(struct Object *ob);
void showkeypos(struct Key *key, struct KeyBlock *kb);

void deselectall_key(void);
void delete_key(void);
void move_keys(void);

#endif

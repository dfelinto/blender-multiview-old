/**
 * blenlib/BKE_mball.h (mar-2001 nzc)
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
#ifndef BKE_MBALL_H
#define BKE_MBALL_H

struct MetaBall;
struct Object;
struct MetaElem;
struct VERTICES;
struct VERTEX;
struct MB_POINT;
struct PROCESS;
struct CUBE;
struct PROCESS;

void unlink_mball(struct MetaBall *mb);
void free_mball(struct MetaBall *mb);
struct MetaBall *add_mball(void);
struct MetaBall *copy_mball(struct MetaBall *mb);
void make_local_mball(struct MetaBall *mb);
void tex_space_mball( struct Object *ob);
void make_orco_mball( struct Object *ob);
struct Object *find_basis_mball( struct Object *ob);
void metaball_polygonize(struct Object *ob);
void calc_mballco(struct MetaElem *ml, float *vec);
float densfunc(struct MetaElem *ball, float x, float y, float z);
float metaball(float x, float y, float z);
void accum_mballfaces(int i1, int i2, int i3, int i4);
void *new_pgn_element(int size);
int nextcwedge (int edge, int face);
void BKE_freecubetable(void);

#endif

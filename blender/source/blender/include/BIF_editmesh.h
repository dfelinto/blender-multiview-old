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

#ifndef BIF_EDITMESH_H
#define BIF_EDITMESH_H

struct EditVlak;
struct EditEdge;
struct EditVert;
struct Mesh;
struct bDeformGroup;
struct View3D;

void free_hashedgetab(void);
void fasterdraw(void);
void slowerdraw(void);
void vertexnoise(void);
void vertexsmooth(void);
void make_sticky(void);
void deselectall_mesh(void);

	/** Aligns the selected TFace's of @a me to the @a v3d,
	 * using the given axis (0-2). Can give a user error.
	 */
void faceselect_align_view_to_selected(struct View3D *v3d, struct Mesh *me, int axis);
	/** Aligns the selected faces or vertices of @a me to the @a v3d,
	 * using the given axis (0-2). Can give a user error.
	 */
void editmesh_align_view_to_selected(struct View3D *v3d, int axis);

struct EditVert *addvertlist(float *vec);
struct EditEdge *addedgelist(struct EditVert *v1, struct EditVert *v2);
struct EditVlak *addvlaklist(struct EditVert *v1, struct EditVert *v2, struct EditVert *v3, struct EditVert *v4, struct EditVlak *example);
struct EditEdge *findedgelist(struct EditVert *v1, struct EditVert *v2);

void remedge(struct EditEdge *eed);

int vlakselectedAND(struct EditVlak *evl, int flag);

void recalc_editnormals(void);
void flip_editnormals(void);
void vertexnormals(int testflip);
/* this is currently only used by the python NMesh module: */
void vertexnormals_mesh(struct Mesh *me, float *extverts);

void make_editMesh(void);
void load_editMesh(void);
void free_editMesh(void);
void remake_editMesh(void);

void convert_to_triface(int all);

void righthandfaces(int select);

void mouse_mesh(void);

void selectconnected_mesh(void);
short extrudeflag(short flag,short type);
void rotateflag(short flag, float *cent, float rotmat[][3]);
void translateflag(short flag, float *vec);
short removedoublesflag(short flag, float limit);
void xsortvert_flag(int flag);
void hashvert_flag(int flag);
void subdivideflag(int flag, float rad, int beauty);
void adduplicateflag(int flag);
void extrude_mesh(void);
void adduplicate_mesh(void);
void split_mesh(void);
void separate_mesh(void);
void extrude_repeat_mesh(int steps, float offs);
void spin_mesh(int steps,int degr,float *dvec, int mode);
void screw_mesh(int steps,int turns);
void selectswap_mesh(void);
void addvert_mesh(void);
void addedgevlak_mesh(void);
void delete_mesh(void);
void add_primitiveMesh(int type);
void hide_mesh(int swap);
void reveal_mesh(void);
void beauty_fill(void);
void join_triangles(void);
void edge_flip(void);
void join_mesh(void);
void clever_numbuts_mesh(void);
void sort_faces(void);
void vertices_to_sphere(void);
void fill_mesh(void);

#endif

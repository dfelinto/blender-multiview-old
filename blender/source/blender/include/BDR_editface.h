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

#ifndef BDR_EDITFACE_H
#define BDR_EDITFACE_H

struct TFace;
struct Mesh;

void set_lasttface(void);
void default_uv(float uv[][2], float size);
void default_tface(struct TFace *tface);
void make_tfaces(struct Mesh *me);
void reveal_tface(void);
void hide_tface(void);
void select_linked_tfaces(void);
void deselectall_tface(void);
void rotate_uv_tface(void);
struct TFace* face_pick(struct Mesh *me, short x, short y);
void face_select(void);
void face_borderselect(void);
float CalcNormUV(float *a, float *b, float *c);
void uv_autocalc_tface(void);
void set_faceselect(void);
void face_draw(void);   

#endif /* BDR_EDITFACE_H */

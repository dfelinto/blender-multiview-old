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

#ifndef BDR_DRAWOBJECT_H
#define BDR_DRAWOBJECT_H

#ifdef __cplusplus
extern "C" { 
#endif


struct Object;
struct Nurb;
struct Lamp;
struct ListBase;
struct BoundBox;
struct Base;

void init_draw_rects(void);
void helpline(float *vec);
void drawaxes(float size);
void drawcamera(struct Object *ob);
void calc_lattverts_ext(void);
void calc_meshverts(void);
void calc_meshverts_ext(void);
void calc_nurbverts_ext(void);
void tekenvertices(short sel);
void tekenvertices_ext(int mode);
void drawcircball(float *cent, float rad, float tmat[][4]);
void get_local_bounds(struct Object *ob, float *centre, float *size);
void draw_object(struct Base *base);
void draw_object_ext(struct Base *base);

#ifdef __cplusplus
}
#endif

#endif  /*  BDR_DRAWOBJECT_H */

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
 * protos for view.c  -- not complete
 */

#ifndef BSE_VIEW_H
#define BSE_VIEW_H

struct Object;
struct BoundBox;
struct View3D;
struct ScrArea;

void persp3d(struct View3D *v3d, int a);
void persp_general(int a);
void persp(int a);
void initgrabz(float x, float y, float z);
void window_to_3d(float *vec, short mx, short my);
void project_short(float *vec, short *adr);
void project_short_noclip(float *vec, short *adr);
int boundbox_clip(float obmat[][4], struct BoundBox *bb);
void fdrawline(float x1, float y1, float x2, float y2);
void fdrawbox(float x1, float y1, float x2, float y2);
void sdrawline(short x1, short y1, short x2, short y2);
void sdrawbox(short x1, short y1, short x2, short y2);
void calctrackballvecfirst(struct rcti *area, short *mval, float *vec);
void calctrackballvec(struct rcti *area, short *mval, float *vec);
void viewmove(int mode);
void setwinmatrixview3d(struct rctf *rect);
void obmat_to_viewmat(struct Object *ob);
void setviewmatrixview3d(void);
float *give_cursor(void);
unsigned int free_localbit(void);
void initlocalview(void);
void centreview(void);
void restore_localviewdata(struct View3D *vd);
void endlocalview(struct ScrArea *sa);
void view3d_home(int centre);
short selectprojektie(unsigned int *buffer, short x1, short y1, short x2, short y2);
void view3d_align_axis_to_vector(struct View3D *v3d, int axisidx, float vec[3]);

#endif

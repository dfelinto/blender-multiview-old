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

#ifndef BSE_DRAWVIEW_H
#define BSE_DRAWVIEW_H

struct Object;
struct BGpic;
struct rcti;

void setalpha_bgpic(struct BGpic *bgpic);
void default_gl_light(void);
void init_gl_stuff(void);
void two_sided(int val);
void circf(float x, float y, float rad);
void circ(float x, float y, float rad);
void backdrawview3d(int test);
void drawview3d(void);
void drawview3d_render(struct View3D *v3d);
int update_time(void);
void calc_viewborder(struct View3D *v3d, struct rcti *viewborder_r);
void view3d_set_1_to_1_viewborder(struct View3D *v3d);
void timestr(double time, char *str);
double speed_to_swaptime(int speed);
double key_to_swaptime(int key);

void sumo_callback(void *obp);
void init_anim_sumo(void);
void update_anim_sumo(void);
void end_anim_sumo(void);

void inner_play_anim_loop(int init, int mode);
int play_anim(int mode);

#endif /* BSE_DRAWVIEW_H */

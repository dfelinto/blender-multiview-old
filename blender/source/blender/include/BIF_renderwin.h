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
struct ScrArea;

void calc_renderwin_rectangle(int posmask, int renderpos_r[2], int rendersize_r[2]);

void BIF_close_render_display(void);

void BIF_do_render(int anim);

/**
 * @param v3d The View3D space to render.
 */
void BIF_do_ogl_render(struct View3D *v3d, int anim);

void BIF_renderwin_set_for_ogl_render(void);
void BIF_renderwin_set_custom_cursor(unsigned char mask[16][2], unsigned char bitmap[16][2]);

void BIF_swap_render_rects(void);
void BIF_toggle_render_display(void);

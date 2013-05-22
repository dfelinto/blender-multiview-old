/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version. 
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2007 Blender Foundation.
 * All rights reserved.
 *
 * 
 * Contributor(s): Blender Foundation
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file blender/windowmanager/intern/wm_draw.c
 *  \ingroup wm
 */


#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>

#include "DNA_listBase.h"
#include "DNA_screen_types.h"
#include "DNA_windowmanager_types.h"
#include "DNA_userdef_types.h"
#include "DNA_view3d_types.h"

#include "MEM_guardedalloc.h"

#include "BLI_blenlib.h"
#include "BLI_utildefines.h"
#include "BLI_math_base.h"

#include "BIF_gl.h"

#include "BKE_context.h"
#include "BKE_global.h"
#include "BKE_screen.h"

#include "GHOST_C-api.h"

#include "ED_view3d.h"
#include "ED_screen.h"

#include "GPU_draw.h"
#include "GPU_extensions.h"

#include "RE_engine.h"

#include "WM_api.h"
#include "WM_types.h"
#include "wm.h"
#include "wm_stereo.h"
#include "wm_draw.h" /* wmDrawTriple */
#include "wm_window.h"
#include "wm_event_system.h"

/* swap */
#if 0
#define WIN_NONE_OK     0
#define WIN_BACK_OK     1
#define WIN_FRONT_OK    2
#define WIN_BOTH_OK     3
#endif

/* XXX not tested */
static void wm_method_draw_stereo_pageflip(wmWindow *win)
{
	int view = 0;
	wmDrawTriple *triple;

	for (view=0; view < 2; view ++) {
		if (view == STEREO_LEFT_ID) {
			triple = win->drawdata;
			glDrawBuffer(GL_BACK_LEFT);
		}
		else {
			triple = win->drawdatastereo;
			glDrawBuffer(GL_BACK_RIGHT);
		}

		wm_triple_draw_textures(win, triple, 1.0);
	}
}

static void wm_method_draw_stereo_epilepsy(wmWindow *win)
{
	wmDrawTriple *triple;
	static int view = 0;

	if (view++ %2)
		triple = win->drawdata;
	else
		triple = win->drawdatastereo;

	wm_triple_draw_textures(win, triple, 1.0);
}

static GLuint hinterlace_mask[33];
static int hinterlace_started = FALSE;

static void wm_method_draw_stereo_interlace(wmWindow *win)
{
	int view = 0;
	wmDrawTriple *triple;

	if (hinterlace_started == FALSE)
	{
		int i;
		for (i = 0; i < 32; i++)
			hinterlace_mask[i] = (i&1)*0xFFFFFFFF;
		hinterlace_mask[32] = 0;
		hinterlace_started = TRUE;
	}

	for (view=0; view < 2; view ++) {
		if (view == STEREO_LEFT_ID)
			triple = win->drawdata;
		else
			triple = win->drawdatastereo;

		glEnable(GL_POLYGON_STIPPLE);
		glPolygonStipple((const GLubyte*) &hinterlace_mask[view]);

		wm_triple_draw_textures(win, triple, 1.0);
		glDisable(GL_POLYGON_STIPPLE);
	}

}

static void wm_method_draw_stereo_anaglyph(wmWindow *win)
{
	int view, bit;
	wmDrawTriple *triple;

	for (view = 0; view < 2; view++) {
		if (view == STEREO_LEFT_ID)
			triple = win->drawdata;
		else
			triple = win->drawdatastereo;

		bit = view + 1;
		switch(U.anaglyph_type) {
			case USER_ANAGLYPH_TYPE_RED_CYAN:
				glColorMask(1&bit, 2&bit, 2&bit, FALSE);
				break;
			case USER_ANAGLYPH_TYPE_GREEN_MAGENTA:
				glColorMask(2&bit, 1&bit, 2&bit, FALSE);
				break;
			case USER_ANAGLYPH_TYPE_YELLOW_BLUE:
				glColorMask(1&bit, 1&bit, 2&bit, FALSE);
				break;
		}

		wm_triple_draw_textures(win, triple, 1.0);

		glColorMask(TRUE, TRUE, TRUE, TRUE);
	}
}

static void wm_method_draw_stereo_sidebyside(wmWindow *win)
{
	float halfx, halfy, ratiox, ratioy;
	int x, y, sizex, sizey, offx, offy;
	float alpha = 1.0f;
	wmDrawTriple *triple;
	int view;
	int soffx;

	for (view=0; view < 2; view ++) {
		if (view == STEREO_LEFT_ID) {
			triple = win->drawdata;
			soffx = 0;
		}
		else {
			triple = win->drawdatastereo;
			soffx = WM_window_pixels_x(win) * 0.5;
		}

		glEnable(triple->target);

		for (y = 0, offy = 0; y < triple->ny; offy += triple->y[y], y++) {
			for (x = 0, offx = 0; x < triple->nx; offx += triple->x[x], x++) {
				sizex = (x == triple->nx - 1) ? WM_window_pixels_x(win) - offx : triple->x[x];
				sizey = (y == triple->ny - 1) ? WM_window_pixels_y(win) - offy : triple->y[y];

				/* wmOrtho for the screen has this same offset */
				ratiox = sizex;
				ratioy = sizey;
				halfx = GLA_PIXEL_OFS;
				halfy = GLA_PIXEL_OFS;

				/* texture rectangle has unnormalized coordinates */
				if (triple->target == GL_TEXTURE_2D) {
					ratiox /= triple->x[x];
					ratioy /= triple->y[y];
					halfx /= triple->x[x];
					halfy /= triple->y[y];
				}

				glBindTexture(triple->target, triple->bind[x + y * triple->nx]);

				glColor4f(1.0f, 1.0f, 1.0f, alpha);
				glBegin(GL_QUADS);
				glTexCoord2f(halfx, halfy);
				glVertex2f(soffx + (offx * 0.5), offy);

				glTexCoord2f(ratiox + halfx, halfy);
				glVertex2f(soffx + ((offx + sizex) * 0.5), offy);

				glTexCoord2f(ratiox + halfx, ratioy + halfy);
				glVertex2f(soffx + ((offx + sizex) * 0.5), offy + sizey);

				glTexCoord2f(halfx, ratioy + halfy);
				glVertex2f(soffx + (offx * 0.5), offy + sizey);
				glEnd();
			}
		}

		glBindTexture(triple->target, 0);
		glDisable(triple->target);
		glColorMask(TRUE, TRUE, TRUE, TRUE);
	}
}

static void wm_method_draw_stereo_topbottom(wmWindow *win)
{
	float halfx, halfy, ratiox, ratioy;
	int x, y, sizex, sizey, offx, offy;
	float alpha = 1.0f;
	wmDrawTriple *triple;
	int view;
	int soffy;

	for (view=0; view < 2; view ++) {
		if (view == STEREO_LEFT_ID) {
			triple = win->drawdata;
			soffy = 0;
		}
		else {
			triple = win->drawdatastereo;
			soffy = WM_window_pixels_y(win) * 0.5;
		}

		glEnable(triple->target);

		for (y = 0, offy = 0; y < triple->ny; offy += triple->y[y], y++) {
			for (x = 0, offx = 0; x < triple->nx; offx += triple->x[x], x++) {
				sizex = (x == triple->nx - 1) ? WM_window_pixels_x(win) - offx : triple->x[x];
				sizey = (y == triple->ny - 1) ? WM_window_pixels_y(win) - offy : triple->y[y];

				/* wmOrtho for the screen has this same offset */
				ratiox = sizex;
				ratioy = sizey;
				halfx = GLA_PIXEL_OFS;
				halfy = GLA_PIXEL_OFS;

				/* texture rectangle has unnormalized coordinates */
				if (triple->target == GL_TEXTURE_2D) {
					ratiox /= triple->x[x];
					ratioy /= triple->y[y];
					halfx /= triple->x[x];
					halfy /= triple->y[y];
				}

				glBindTexture(triple->target, triple->bind[x + y * triple->nx]);

				glColor4f(1.0f, 1.0f, 1.0f, alpha);
				glBegin(GL_QUADS);
				glTexCoord2f(halfx, halfy);
				glVertex2f(offx, soffy + (offy * 0.5));

				glTexCoord2f(ratiox + halfx, halfy);
				glVertex2f(offx + sizex, soffy + (offy * 0.5));

				glTexCoord2f(ratiox + halfx, ratioy + halfy);
				glVertex2f(offx + sizex, soffy + ((offy + sizey) * 0.5));

				glTexCoord2f(halfx, ratioy + halfy);
				glVertex2f(offx, soffy + ((offy + sizey) * 0.5));
				glEnd();
			}
		}

		glBindTexture(triple->target, 0);
		glDisable(triple->target);
		glColorMask(TRUE, TRUE, TRUE, TRUE);
	}
}

void wm_method_draw_stereo(bContext *UNUSED(C), wmWindow *win)
{
	switch (U.stereo_display)
	{
		case USER_STEREO_DISPLAY_ANAGLYPH:
			wm_method_draw_stereo_anaglyph(win);
			break;
		case USER_STEREO_DISPLAY_EPILEPSY:
			wm_method_draw_stereo_epilepsy(win);
			break;
		case USER_STEREO_DISPLAY_INTERLACE:
			wm_method_draw_stereo_interlace(win);
			break;
		case USER_STEREO_DISPLAY_PAGEFLIP:
			wm_method_draw_stereo_pageflip(win);
			break;
		case USER_STEREO_DISPLAY_SIDEBYSIDE:
			wm_method_draw_stereo_sidebyside(win);
			break;
		case USER_STEREO_DISPLAY_TOPBOTTOM:
			wm_method_draw_stereo_topbottom(win);
			break;
		default:
			break;
	}
}

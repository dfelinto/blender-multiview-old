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

#ifndef BIF_DRAWIMAGE_H
#define BIF_DRAWIMAGE_H

struct SpaceImage;

void calc_image_view(struct SpaceImage *sima, char mode);
void drawimagespace(void);
void draw_tfaces(void);
void image_changed(struct SpaceImage *sima, int dotile);
void image_home(void);
void image_viewmove(void);
void rectwrite_part(int winxmin, int winymin, int winxmax, int winymax, int x1, int y1, int xim, int yim, float zoomx, float zoomy, unsigned int *rect);
void uvco_to_areaco(float *vec, short *mval);
void uvco_to_areaco_noclip(float *vec, short *mval);
void what_image(struct SpaceImage *sima);

#endif

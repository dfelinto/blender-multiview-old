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

#ifndef BSE_DRAWOOPS_H
#define BSE_DRAWOOPS_H

struct Oops;
struct uiBlock;

void boundbox_oops(void);
void give_oopslink_line(struct Oops *oops, struct OopsLink *ol, float *v1, float *v2);
void draw_oopslink(struct Oops *oops);
void draw_icon_oops(float *co, short type);
void mysbox(float x1, float y1, float x2, float y2);
unsigned int give_oops_color(short type, short sel, unsigned int *border);
void calc_oopstext(char *str, float *v1);
void draw_oops(struct Oops *oops, struct uiBlock *block);
void drawoopsspace(void);

#endif /* BSE_DRAWOOPS */

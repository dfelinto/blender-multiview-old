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

#ifndef BSE_DRAWIPO_H
#define BSE_DRAWIPO_H

struct EditIpo;
struct View2D;
struct rctf;


void calc_ipogrid(void);
void draw_ipogrid(void);

void areamouseco_to_ipoco	(struct View2D *v2d, short *mval, float *x, float *y);
void ipoco_to_areaco		(struct View2D *v2d, float *vec, short *mval);
void ipoco_to_areaco_noclip	(struct View2D *v2d, float *vec, short *mval);

void test_view2d			(struct View2D *v2d, int winx, int winy);
void calc_scrollrcts		(struct View2D *v2d, int winx, int winy);

int in_ipo_buttons(void);
void drawscroll(int disptype);
void drawipo(void);

void scroll_ipobuts(void);
void view2dzoom(void);
int view2dmove(void); 
void view2dborder(void);

struct EditIpo *select_proj_ipo(struct rctf *rectf, int event);


#endif  /*  BSE_DRAWIPO_H */

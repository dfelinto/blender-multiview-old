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

#ifndef BDR_DRAWACTION_H
#define BDR_DRAWACTION_H

struct BezTriple;
struct Ipo;
struct gla2DDrawInfo;
struct bAction;
struct Object;

void draw_cfra_action(void);
void draw_bevel_but(int x, int y, int w, int h, int sel);
void draw_ipo_channel(struct gla2DDrawInfo *di, struct Ipo *ipo, int flags, float ypos);
void draw_action_channel(struct gla2DDrawInfo *di, struct bAction *act, int flags, float ypos);
void draw_object_channel(struct gla2DDrawInfo *di, struct Object *ob, int flags, float ypos);

#endif  /*  BDR_DRAWACTION_H */

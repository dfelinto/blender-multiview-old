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

#ifndef BSE_DRAWIMASEL_H
#define BSE_DRAWIMASEL_H

struct SpaceImaSel;

void viewgate(short sx, short sy, short ex, short ey);
void areaview (void);
void calc_hilite(struct SpaceImaSel *simasel);
void make_sima_area(struct SpaceImaSel *simasel);
void draw_sima_area(struct SpaceImaSel *simasel);
void select_ima_files(struct SpaceImaSel *simasel);
void move_imadir_sli(struct SpaceImaSel *simasel);
void move_imafile_sli(struct SpaceImaSel *simasel);
void ima_select_all(struct SpaceImaSel *simasel);
void pibplay(struct SpaceImaSel *simasel);
void drawimasel(void);   

/*  void calc_hilite(SpaceImaSel *simasel); */
/*  void ima_select_all(SpaceImaSel *simasel); */
/*  void move_imadir_sli(SpaceImaSel *simasel); */
/*  void move_imafile_sli(SpaceImaSel *simasel); */
/*  void pibplay(SpaceImaSel *simasel); */
/*  void select_ima_files(SpaceImaSel *simasel); */

#endif  /*  BSE_DRAWIMASEL_H */

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
#ifdef __cplusplus
extern "C" {
#endif

struct ScrArea;

	/**
	 * Finds the first spacedata of @a type within
	 * the scrarea.
	 */
void *scrarea_find_space_of_type(ScrArea *sa, int type);

int		scrarea_get_win_x		(struct ScrArea *sa);
int		scrarea_get_win_y		(struct ScrArea *sa);
int		scrarea_get_win_width	(struct ScrArea *sa);
int		scrarea_get_win_height	(struct ScrArea *sa);

#ifdef __cplusplus
}
#endif

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
#include "DNA_screen_types.h"
#include "DNA_space_types.h"

#include "BIF_scrarea.h"

void *scrarea_find_space_of_type(ScrArea *sa, int type)
{
	SpaceLink *sl;

	for (sl= sa->spacedata.first; sl; sl= sl->next)
		if (sl->spacetype==type)
			return (void*) sl;

	return 0;
}

int scrarea_get_win_x(ScrArea *sa) 
{
	return sa->winrct.xmin;	
}

int scrarea_get_win_y(ScrArea *sa) 
{
	return sa->winrct.ymin;	
}

int scrarea_get_win_width(ScrArea *sa)
{
	return sa->winx;
}

int scrarea_get_win_height(ScrArea *sa)
{
	return sa->winy;
}

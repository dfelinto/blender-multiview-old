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

#include <stdlib.h>

#ifdef WIN32
#include "BLI_winstuff.h"
#endif

#include "BLI_blenlib.h"

#include "DNA_screen_types.h"

#include "BIF_gl.h"
#include "BIF_mywindow.h"
#include "BIF_screen.h"
#include "BIF_spacetypes.h"

/***/

struct _SpaceType {
	char			name[32];
	
	SpaceDrawFP		windraw;
	SpaceChangeFP	winchange;
	SpaceHandleFP	winhandle;
};


SpaceType *spacetype_new(char *name)
{
	SpaceType *st= calloc(1, sizeof(*st));
	BLI_strncpy(st->name, name, sizeof(st->name));

	return st;
}

void spacetype_set_winfuncs(SpaceType *st, SpaceDrawFP draw, SpaceChangeFP change, SpaceHandleFP handle) 
{
	st->windraw= draw;
	st->winchange= change;
	st->winhandle= handle;
}

	/***/

SpaceType *spacetype_from_code(int spacecode)
{
	switch (spacecode) {
	case SPACE_ACTION:	return spaceaction_get_type();
	case SPACE_BUTS:	return spacebuts_get_type();
	case SPACE_FILE:	return spacefile_get_type();
	case SPACE_IMAGE:	return spaceimage_get_type();
	case SPACE_IMASEL:	return spaceimasel_get_type();
	case SPACE_INFO:	return spaceinfo_get_type();
	case SPACE_IPO:		return spaceipo_get_type();
	case SPACE_NLA:		return spacenla_get_type();
	case SPACE_OOPS:	return spaceoops_get_type();
	case SPACE_SEQ:		return spaceseq_get_type();
	case SPACE_SOUND:	return spacesound_get_type();
	case SPACE_TEXT:	return spacetext_get_type();
	case SPACE_VIEW3D:	return spaceview3d_get_type();
	default:
		return NULL;
	}
}

void scrarea_do_windraw(ScrArea *area)
{
	SpaceType *st= spacetype_from_code(area->spacetype);
	
	areawinset(area->win);

	if(area->win && st->windraw) {
		st->windraw();
	}
	else {
		glClearColor(0.4375, 0.4375, 0.4375, 0.0); 
		glClear(GL_COLOR_BUFFER_BIT);
	}
	
	area->win_swap= WIN_BACK_OK;
}
void scrarea_do_winchange(ScrArea *area)
{
	SpaceType *st= spacetype_from_code(area->spacetype);

	areawinset(area->win);

	if (st->winchange) {
		st->winchange();
	} else {
		if (!BLI_rcti_is_empty(&area->winrct)) {
			bwin_ortho2(area->win, -0.5, area->winrct.xmax-area->winrct.xmin-0.5, -0.5, area->winrct.ymax-area->winrct.ymin-0.5);
			glLoadIdentity();
		}
	}
}
void scrarea_do_winhandle(ScrArea *area, unsigned short event, short val, char ascii)
{
	SpaceType *st= spacetype_from_code(area->spacetype);

	areawinset(area->win);
	
	if (st->winhandle) {
		st->winhandle(event, val, ascii);
	}
}


/* screen.c  dec/jan 93/94		GRAPHICS
 * 
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

#include <string.h>
#include <stdio.h>
#include <math.h>

#include "DNA_screen_types.h"

#include "BKE_bad_level_calls.h"

#include "BLI_blenlib.h"

/* don't free screen itself */
void free_screen(bScreen *sc)
{
	unlink_screen(sc);
	
	BLI_freelistN(&sc->vertbase);
	BLI_freelistN(&sc->edgebase);
	BLI_freelistN(&sc->areabase);
}

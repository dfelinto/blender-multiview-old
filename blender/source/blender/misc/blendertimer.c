/*
 * blendertimer.c
 * 
 * A system-independent timer
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

#ifdef WIN32
#include "BLI_winstuff.h"
#else
#include <unistd.h>
#include <sys/times.h>
#include <sys/time.h>	/* struct timeval */
#include <sys/resource.h>	/* struct rusage */
#endif

#include "BLI_blenlib.h"
#include "BKE_global.h"
#include "BIF_screen.h" /* qtest(), extern_qread() */
#include "mydevice.h"

#include "blendertimer.h"

#include "PIL_time.h"

int MISC_test_break(void)
{
	if (!G.background) {
		static double ltime= 0;
		double curtime= PIL_check_seconds_timer();

			/* only check for breaks every 10 milliseconds
			 * if we get called more often.
			 */
		if ((curtime-ltime)>.001) {
			ltime= curtime;

			while(qtest()) {
				short val;
				if (extern_qread(&val) == ESCKEY) {
					G.afbreek= 1;
				}
			}
		}
	}

	return (G.afbreek==1);
}

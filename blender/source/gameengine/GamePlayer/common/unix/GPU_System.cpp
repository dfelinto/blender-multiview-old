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
   
#include <sys/time.h>

#include "GPU_System.h"


static struct timeval startTime;
static int startTimeDone = 0;


double GPU_System::GetTimeInSeconds()
{
	if(!startTimeDone)
	{
		gettimeofday(&startTime, NULL);
		startTimeDone = 1;
	}

	struct timeval now;
	gettimeofday(&now, NULL);
	// next '1000' are used for precision
	long ticks = (now.tv_sec - startTime.tv_sec) * 1000 + (now.tv_usec - startTime.tv_usec) / 1000;
	double secs = (double)ticks / 1000.0;
	return secs;
}

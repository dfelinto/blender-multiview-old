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
#include "PIL_time.h"

#ifdef WIN32

#include <windows.h>

double PIL_check_seconds_timer(void) 
{
	static int hasperfcounter= -1; /* -1==unknown */
	static double perffreq;

	if (hasperfcounter==-1) {
		__int64 ifreq;
		hasperfcounter= QueryPerformanceFrequency((LARGE_INTEGER*) &ifreq);
		perffreq= (double) ifreq;
	} 

	if (hasperfcounter) {
		__int64 count;

		QueryPerformanceCounter((LARGE_INTEGER*) &count);

		return count/perffreq;
	} else {
		static double accum= 0.0;
		static int ltick= 0;
		int ntick= GetTickCount();

		if (ntick<ltick) {
			accum+= (0xFFFFFFFF-ltick+ntick)/1000.0;
		} else {
			accum+= (ntick-ltick)/1000.0;
		}

		ltick= ntick;
		return accum;
	}
}

void PIL_sleep_ms(int ms)
{
	Sleep(ms);
}

#else

#include <unistd.h>
#include <sys/time.h>

double PIL_check_seconds_timer(void) 
{
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);

	return ((double) tv.tv_sec + tv.tv_usec/1000000.0);
}

void PIL_sleep_ms(int ms)
{
	if (ms>=1000) {
		sleep(ms/1000);
		ms= (ms%1000);
	}
	
	usleep(ms*1000);
}

#endif

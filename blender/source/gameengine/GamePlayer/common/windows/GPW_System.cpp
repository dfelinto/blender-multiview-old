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
 * Timing routine taken and modified from KX_BlenderSystem.cpp
 */

#include <windows.h>

#include "GPW_System.h"


GPW_System::GPW_System(void)
{
	m_freq = 0;
	m_lastCount = 0;
	m_lastRest = 0;
	m_lastTime = 0;
}


double GPW_System::GetTimeInSeconds()
{
#if 0
	double secs = ::GetTickCount();
	secs /= 1000.;
	return secs;
#else

	// 03/20/1999 Thomas Hieber: completely redone to get true Millisecond 
	// accuracy instead of very rough ticks. This routine will also provide
	// correct wrap around at the end of "long"

	// m_freq was set to -1, if the current Hardware does not support 
	// high resolution timers. We will use GetTickCount instead then.
	if (m_freq < 0) {
		return ::GetTickCount();
	}

	// m_freq is 0, the first time this function is being called.
	if (m_freq == 0) {
		// Try to determine the frequency of the high resulution timer
		if (!::QueryPerformanceFrequency((LARGE_INTEGER*)&m_freq)) {
			// There is no such timer....
			m_freq = -1;
			return 0;
		}
	}

	// Retrieve current count
	__int64 count = 0;
	::QueryPerformanceCounter((LARGE_INTEGER*)&count);

	// Calculate the time passed since last call, and add the rest of
	// those tics that didn't make it into the last reported time.
	__int64 delta = 1000*(count-m_lastCount) + m_lastRest;

	m_lastTime += (long)(delta/m_freq);	// Save the new value
	m_lastRest  = delta%m_freq;			// Save those ticks not being counted
	m_lastCount = count;				// Save last count

	// Return a high quality measurement of time
	return m_lastTime/1000.0; 
#endif
}



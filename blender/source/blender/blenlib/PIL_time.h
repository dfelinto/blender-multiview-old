/**
 * @file PIL_time.h
 * 
 * Platform independant time functions.
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
 
#ifndef PIL_TIME_H
#define PIL_TIME_H

#ifdef __cplusplus
extern "C" { 
#endif

extern 
	/** Return an indication of time, expressed	as
	 * seconds since some fixed point. Successive calls
	 * are guarenteed to generate values greator than or 
	 * equal to the last call.
	 */
double	PIL_check_seconds_timer		(void);

	/**
	 * Platform-independant sleep function.
	 * @param ms Number of milliseconds to sleep
	 */
void	PIL_sleep_ms				(int ms);

#ifdef __cplusplus
}
#endif

#endif

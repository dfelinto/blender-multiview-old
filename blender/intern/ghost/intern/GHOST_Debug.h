/**
 * $Id$
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

/**

 * $Id$
 * Copyright (C) 2001 NaN Technologies B.V.
 * @author	Maarten Gribnau
 * @date	June 1, 2001
 */

#ifndef _GHOST_DEBUG_H_
#define _GHOST_DEBUG_H_

#ifdef WIN32
	#ifdef _DEBUG
		#pragma warning (disable:4786) // suppress stl-MSVC debug info warning
		#define GHOST_DEBUG
	#endif // _DEBUG
#else // WIN32
	#ifndef NDEBUG
		#define GHOST_DEBUG
	#endif // DEBUG
#endif // WIN32

#ifdef GHOST_DEBUG
	#include <iostream>
#endif // GHOST_DEBUG


#ifdef GHOST_DEBUG
	#define GHOST_PRINT(x) { std::cout << x; }
	//#define GHOST_PRINTF(x) { printf(x); }
#else  // GHOST_DEBUG
	#define GHOST_PRINT(x)
	//#define GHOST_PRINTF(x)
#endif // GHOST_DEBUG


#ifdef GHOST_DEBUG
	#define GHOST_ASSERT(x, info) { if (!(x)) {GHOST_PRINT("assertion failed: "); GHOST_PRINT(info); GHOST_PRINT("\n"); } }
#else  // GHOST_DEBUG
	#define GHOST_ASSERT(x, info)
#endif // GHOST_DEBUG

#endif // _GHOST_DEBUG_H_

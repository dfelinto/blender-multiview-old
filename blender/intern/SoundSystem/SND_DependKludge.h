/*
 * SND_DependKludge.h
 *
 * who needs what?
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


#if defined (_WIN32)
#define USE_FMOD
#else
#	if defined (__linux__)
#		if defined (__i386__)
#			define USE_OPENAL
#		endif
#	else
#		if defined (__FreeBSD__)
#			define USE_OPENAL
#		endif
#		ifdef USE_OPENAL
#			undef USE_OPENAL
#		endif
#		ifdef USE_FMOD
#			undef USE_FMOD
#		endif
#	endif
#endif

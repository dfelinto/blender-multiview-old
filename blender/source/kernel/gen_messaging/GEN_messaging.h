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
#ifndef GEN_MESSAGING_H
#define GEN_MESSAGING_H

#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * Stream for error messages.
	 */
	extern FILE* GEN_errorstream;

	/**
	 * Stream for notices to the user.
	 */
	extern FILE* GEN_userstream;

	/**
	 * Initialise the messaging system. If the system is not
	 * initialised, the streams cannot be used. */
	void GEN_init_messaging_system(void);
	
#ifdef __cplusplus
}
#endif


#endif /* GEN_MESSAGING_H */

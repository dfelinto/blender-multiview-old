/**
 * blenlib/DNA_object_types.h (mar-2001 nzc)
 *	
 * Scriptlink is hard-coded in object for some reason.
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
#ifndef DNA_SCRIPTLINK_TYPES_H
#define DNA_SCRIPTLINK_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

struct ID;

typedef struct ScriptLink {
	struct ID **scripts;
	short *flag;
	
	short actscript, totscript;
	int pad;
} ScriptLink;

/* **************** SCRIPTLINKS ********************* */

#define SCRIPT_FRAMECHANGED	1
#define SCRIPT_ONLOAD		2
#define SCRIPT_REDRAW		4

#ifdef __cplusplus
}
#endif

#endif

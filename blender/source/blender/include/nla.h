/* nla.h   May 2001
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
 *  
 *	Use this to turn experimental options on
 *	or off with the #define flags.  Please do not
 *  put other includes, typdefs etc in this file.
 *	===========================================
 *
 *	__NLA
 *	This encompasses the new armature object, the
 *	action datablock and the action window-type.
 *
 *	__CON_IPO
 *	Support for constraint ipo keys
 *
 *	__NLA_BAKE
 *	Allow users to bake constraints into keyframes
 *
 *	__NLA_ACTION_BY_MOTION_ACTUATOR
 *	New action actuator playback type
 *
 * $Id$
 */

#ifndef NLA_H
#define NLA_H

#define __NLA			

#define __NLA_BAKE							//	Not for release: Not yet fully implemented
#define __CON_IPO							//	Not for Release: Not yet fully implemented
//#define __NLA_ACTION_BY_MOTION_ACTUATOR	//	Not for release: Not yet fully implemented

#endif


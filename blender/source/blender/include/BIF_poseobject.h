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

#ifndef BIF_POSEOBJECT
#define BIF_POSEOBJECT

/**
 * Activates posemode
 */
void enter_posemode(void);

/**
 * Provides the current object the opportunity to specify
 * which channels to key in the current pose (if any).
 * If an object provides its own filter, it must clear
 * then POSE_KEY flags of unwanted channels, as well as
 * setting the flags for desired channels.
 *
 * Default behaviour is to key all channels.
 */
void filter_pose_keys(void);

/**
 * Deactivates posemode
 * @param freedata 0 or 1 value indicating that posedata should be deleted
 */
void exit_posemode(int freedata);

/**
 * Removes unreferenced pose channels from an object 
 * @param ob Object to check
 */
void collect_pose_garbage(struct Object *ob);

#endif


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

#ifndef NAN_INCLUDED_NonCopyable_h

#define NAN_INCLUDED_NonCopyable_h

/**
 * Simple class that makes sure sub classes cannot
 * generate standard copy constructors.
 * If you want to make sure that your class does
 * not have any of these cheesy hidden constructors
 * inherit from this class.
 */

class MEM_NonCopyable {
protected :

	MEM_NonCopyable(
	) {
	};

private :

	MEM_NonCopyable (const MEM_NonCopyable *);
	MEM_NonCopyable (const MEM_NonCopyable &);
};

#endif



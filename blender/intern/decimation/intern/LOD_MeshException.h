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

#ifndef NAN_INCLUDED_MeshExceptions_h

#define NAN_INCLUDED_MeshExceptions_h


class LOD_MeshException {

public :

	// stick in more error types as you think of them

	enum ExceptionType{
		e_non_manifold,
		e_search_error
	} m_e_type;

	LOD_MeshException (
		ExceptionType type
	) : m_e_type (type)
	{
	}
};

#endif

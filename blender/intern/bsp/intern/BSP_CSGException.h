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

#ifndef NAN_INCLUDED_CSGException_h

#define NAN_INCLUDED_CSGException_h

// stick in more error types as you think of them

enum BSP_ExceptionType{
	e_split_error,
	e_mesh_error,
	e_mesh_input_error,
	e_param_error,
	e_tree_build_error
};


class BSP_CSGException {
public :
	BSP_ExceptionType m_e_type;

	BSP_CSGException (
		BSP_ExceptionType type
	) : m_e_type (type)
	{
	}
};

#endif

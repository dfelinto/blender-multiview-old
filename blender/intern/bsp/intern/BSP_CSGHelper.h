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

#ifndef BSP_CSGHELPER_H

#define BSP_CSGHELPER_H

class BSP_CSGMesh;
class BSP_MeshFragment;

#include "../extern/CSG_BooleanOps.h"
#include "BSP_MeshPrimitives.h"

enum BSP_OperationType{
	e_intern_csg_union,
	e_intern_csg_intersection,
	e_intern_csg_difference
};

class BSP_CSGHelper {
public :

	BSP_CSGHelper(
	);

		bool
	ComputeOp(
		BSP_CSGMesh * obA,
		BSP_CSGMesh * obB,
		BSP_OperationType op_type, 
		BSP_CSGMesh & output,
		CSG_InterpolateUserFaceVertexDataFunc fv_func
	);

	
	~BSP_CSGHelper(
	);

private:

	// Iterate through the fragment,
	// add new vertices to output,
	// map polygons to new vertices.

		void
	DuplicateMesh(
		const BSP_MeshFragment & frag,
		BSP_CSGMesh & output
	);

		void
	TranslateSplitFragments(
		const BSP_MeshFragment & in_frag,
		const BSP_MeshFragment & out_frag,
		const BSP_MeshFragment & on_frag,
		BSP_Classification keep,
		BSP_MeshFragment & spanning_frag,
		BSP_MeshFragment & output
	);

		void
	MergeFrags(
		const BSP_MeshFragment & in,
		BSP_MeshFragment & out
	);

	

};

#endif

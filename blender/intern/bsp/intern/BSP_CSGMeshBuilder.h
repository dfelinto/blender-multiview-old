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

#ifndef BSP_CSGMeshBuilder_h

#define BSP_CSGMeshBuilder_h

#include "../extern/CSG_BooleanOps.h"
#include "BSP_CSGMesh.h"
#include "MEM_NonCopyable.h"
#include "MEM_SmartPtr.h"

/**
 * This class helps you to build a mesh from 2 seperate vertex/face
 * iterators defined in the external interface of the bsp module.
 * This code should really become party of a generic C++ mesh interface
 * but later...
 */

class BSP_CSGMeshBuilder : public MEM_NonCopyable{

public :

	/**
	 * Return a new BSP_CSGMesh with the desired props
	 * built from the given face and vertex iterators.
	 * The iterators are exhausted by this action.
	 */

	static 
		MEM_SmartPtr<BSP_CSGMesh> 
	NewMesh(
		CSG_MeshPropertyDescriptor &props,
		CSG_FaceIteratorDescriptor &face_it,
		CSG_VertexIteratorDescriptor &vertex_it
	); 

private :

	BSP_CSGMeshBuilder(
	);

};


#endif

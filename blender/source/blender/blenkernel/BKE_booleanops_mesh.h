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
#ifndef BKE_PyBooleanOps_h
#define BKE_PyBooleanOps_h

#include "CSG_BooleanOps.h"

/**
 * Internal mesh structure.
 * Safe to copy by value... hopefully.
 */

struct Base;
struct Object;
struct CSG_MeshDescriptor;

typedef void (*CSG_DestroyMeshFunc)(struct CSG_MeshDescriptor *);

typedef struct CSG_MeshDescriptor {
	struct Base *base; // Ptr to base of original blender object - used in creating a new object
	CSG_MeshPropertyDescriptor m_descriptor;
	CSG_FaceIteratorDescriptor m_face_iterator;
	CSG_VertexIteratorDescriptor m_vertex_iterator;
	CSG_DestroyMeshFunc m_destroy_func;
} CSG_MeshDescriptor;


extern
	int
CSG_LoadBlenderMesh(
	struct Object * obj,
	CSG_MeshDescriptor *output
);

/**
 * Destroy the contents of a mesh descriptor.
 * If the internal descriptor refers to a blender
 * mesh, no action is performed apart from freeing
 * internal memory in the desriptor.
 */

extern
	void
CSG_DestroyMeshDescriptor(
	CSG_MeshDescriptor *mesh
);

/**
 * Perform a boolean operation between 2 meshes and return the 
 * result as a new mesh descriptor.
 * op_type is an integer code of the boolean operation type.
 * 1 = intersection,
 * 2 = union,
 * 3 = difference.
 */

extern
	int 
CSG_PerformOp(
	CSG_MeshDescriptor *mesh1,
	CSG_MeshDescriptor *mesh2,
	int op_type,
	CSG_MeshDescriptor *output
);



/**
 * Add a mesh to blender as a new object.
 */

extern
	int
CSG_AddMeshToBlender(
	CSG_MeshDescriptor *mesh
);

/**
 * Test functionality.
 */

extern
	int
NewBooleanMeshTest(
	struct Base * base,
	struct Base * base_select,
	int op_type
);




#endif

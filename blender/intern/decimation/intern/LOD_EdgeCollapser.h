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

#ifndef NAN_INCLDUED_EgdeCollapser_h

#define NAN_INCLDUED_EgdeCollapser_h

// This is a helper class that collapses edges of a 2 - manifold mesh.

#include "LOD_MeshPrimitives.h"
#include "MEM_NonCopyable.h"
#include <vector>
#include <functional>

class LOD_ManMesh2;

class LOD_EdgeCollapser 
: public  MEM_NonCopyable
{

public :
			
	static
		LOD_EdgeCollapser * 
	New(
	);

	// returns via arguments the set of modified
	// verts,edges and faces.

		bool
	CollapseEdge(
		LOD_EdgeInd ei,
		LOD_ManMesh2 &mesh,
		std::vector<LOD_EdgeInd> &	degenerate_edges,
		std::vector<LOD_FaceInd> &	degenerate_faces,
		std::vector<LOD_VertexInd> & degenerate_vertices,
		std::vector<LOD_EdgeInd> &	new_edges,
		std::vector<LOD_FaceInd> &	update_faces,
		std::vector<LOD_VertexInd> & update_vertices
	);

private :

	LOD_EdgeCollapser(
	);

	// Test to see if the result of collapsing the
	// edge produces 2 junctions in the mesh i.e. where
	// an edge is shared by more than 2 polygons

	// We count the number of coincedent edge pairs that
	// result from the collapse of collapse_edge.

	// If collapse edge is a boundary edge then the number of
	// coincedent pairs should be 1
	// else it should be 2.

		bool
	TJunctionTest(
		LOD_ManMesh2 &mesh,
		std::vector<LOD_EdgeInd> &e_v0v1,
		LOD_EdgeInd collapse_edge
	);

	// here's the definition of the sort function
	// we use to determine coincedent edges

	// assumes the edges are normalized i.e. m_verts[0] <= m_verts[1]

	struct less : std::binary_function<LOD_Edge, LOD_Edge, bool> {
			bool 
		operator()(
			const LOD_Edge& a,
			const LOD_Edge& b
		) const {
				
			if (int(a.m_verts[0]) == int(b.m_verts[0])) {
				return (int(a.m_verts[1]) < int(b.m_verts[1]));
			} else {
				return (int(a.m_verts[0]) < int(b.m_verts[0]));
			}
		}
	};

};


#endif


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

#ifndef NAN_INCLUDED_LOD_QuadricEditor_h

#define NAN_INCLUDED_LOD_QuadricEditor_h

#include "MEM_NonCopyable.h"
#include "LOD_ManMesh2.h"
#include "MT_Vector3.h"
#include "LOD_Quadric.h"

class LOD_ExternNormalEditor;


class LOD_QuadricEditor : public MEM_NonCopyable
{

public : 

	// Creation
	///////////

	static
		LOD_QuadricEditor *
	New(
		LOD_ManMesh2 &mesh
	); 

	// Property editor interface
	////////////////////////////

		void
	Remove(
		std::vector<LOD_VertexInd> &sorted_vertices
	);

		void
	Update(
		std::vector<LOD_FaceInd> &sorted_vertices
	);


		std::vector<LOD_Quadric> &
	Quadrics(
	) const {
		return *m_quadrics;
	};


	// Editor specific methods
	//////////////////////////

		bool
	BuildQuadrics(
		LOD_ExternNormalEditor& normal_editor,
		bool preserve_boundaries
	);	


		void
	ComputeEdgeCosts(
		std::vector<LOD_EdgeInd> &edges
	); 	

		MT_Vector3 
	TargetVertex(
		LOD_Edge &e
	);

	~LOD_QuadricEditor(
	 ){
		delete(m_quadrics);
	};

		
private :

	std::vector<LOD_Quadric> * m_quadrics;

	LOD_ManMesh2 &m_mesh;

private :
	
	LOD_QuadricEditor(LOD_ManMesh2 &mesh);



};

#endif

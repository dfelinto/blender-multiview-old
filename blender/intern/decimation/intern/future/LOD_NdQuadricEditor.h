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

#ifndef NAN_INCLUDED_LOD_NdQuadricEditor_h

#define NAN_INCLUDED_LOD_NdQuadricEditor_h

#include "common/NonCopyable.h"
#include "LOD_ManMesh2.h"
#include "MT_Vector3.h"
#include "LOD_NdQuadric.h"

class LOD_ExternNormalEditor;
class LOD_ExternVColorEditor;


class LOD_NdQuadricEditor
{

public : 

	// Creation
	///////////

	static
		LOD_NdQuadricEditor *
	New(
		LOD_ManMesh2 &mesh
	); 

	// Property editor interface
	////////////////////////////

		void
	Remove(
		const std::vector<LOD_VertexInd> &sorted_vertices
	);

		std::vector<LOD_NdQuadric> &
	Quadrics(
	) const {
		return *m_quadrics;
	};


	// Editor specific methods
	//////////////////////////

		bool
	BuildQuadrics(
		const LOD_ExternNormalEditor& normal_editor,
		bool preserve_boundaries
	);	

		void
	InitializeHeapKeys(
		const LOD_ExternVColorEditor & color_editor
	);


		void
	ComputeEdgeCosts(
		const std::vector<LOD_EdgeInd> &edges,
		const LOD_ExternVColorEditor & color_editor
	); 	

		void 
	TargetVertex(
		const LOD_Edge & e,
		TNT::Vector<MT_Scalar> &result,
		const LOD_ExternVColorEditor & color_editor
	) const ;

	~LOD_NdQuadricEditor(
	 ){
		delete(m_quadrics);
	};

		
private :

	std::vector<LOD_NdQuadric> * m_quadrics;

	LOD_ManMesh2 &m_mesh;

private :
	
	LOD_NdQuadricEditor(LOD_ManMesh2 &mesh);



};

#endif

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

#ifndef NAN_INCLUDED_FaceNormalEditor_h

#define NAN_INCLUDED_FaceNormalEditor_h

#include "MEM_NonCopyable.h"
#include "LOD_ManMesh2.h"
#include "MT_Vector3.h"


class LOD_FaceNormalEditor : public MEM_NonCopyable
{

public : 

	// Creation
	///////////

	static
		LOD_FaceNormalEditor *
	New(
		LOD_ManMesh2 &mesh
	); 

	// Property editor interface
	////////////////////////////


	// Faces
	////////
		void
	Remove(
		std::vector<LOD_FaceInd> &sorted_faces
	);

		void
	Add(
	); 	

		void
	Update(
		std::vector<LOD_FaceInd> &sorted_faces
	);

	
	// vertex normals
	/////////////////

		void
	RemoveVertexNormals(
		std::vector<LOD_VertexInd> &sorted_verts
	);


		void
	UpdateVertexNormals(
		std::vector<LOD_VertexInd> &sorted_verts
	);



	const 
		std::vector<MT_Vector3> &
	Normals(
	) const {
		return m_normals.Ref();
	};


	const 
		std::vector<MT_Vector3> &
	VertexNormals(
	) const {
		return m_vertex_normals.Ref();
	};

	// Editor specific methods
	//////////////////////////

		void
	BuildNormals(
	);	


private :

	MEM_SmartPtr<std::vector<MT_Vector3> > m_normals;
	MEM_SmartPtr<std::vector<MT_Vector3> > m_vertex_normals;

	LOD_ManMesh2 &m_mesh;

private :
	
		
	LOD_FaceNormalEditor(LOD_ManMesh2 &mesh);

	const 
		MT_Vector3 
	ComputeNormal(
		const LOD_TriFace &face
	) const ;

	const 
		MT_Vector3
	ComputeVertexNormal (
		const LOD_VertexInd vi
	) const;



};

#endif

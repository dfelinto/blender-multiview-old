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

#ifndef NAN_INCLUDED_LOD_QSDecimator_H

#define NAN_INCLUDED_LOD_QSDecimator_H

#include "MEM_NonCopyable.h"
#include "LOD_ManMesh2.h"
#include "LOD_ExternNormalEditor.h"
#include "LOD_EdgeCollapser.h"
#include "LOD_QuadricEditor.h"

class LOD_ExternBufferEditor;

class LOD_QSDecimator : public MEM_NonCopyable {

public :

	static
		LOD_QSDecimator *
	New(
		LOD_ManMesh2 &mesh,
		LOD_ExternNormalEditor &face_editor,
		LOD_ExternBufferEditor &extern_editor
	);


		bool
	Arm(
	);


		bool
	Step(
	);

private :

	LOD_QSDecimator(
		LOD_ManMesh2 &mesh,
		LOD_ExternNormalEditor &face_editor,
		LOD_ExternBufferEditor &extern_editor
	);

		bool
	CollapseEdge(
	);

		bool
	BuildHeap(
	);

		void
	UpdateHeap(
		std::vector<LOD_EdgeInd> &deg_edges,
		std::vector<LOD_EdgeInd> &new_edges
	);	

		void
	DeletePrimitives(
		const std::vector<LOD_EdgeInd> & degenerate_edges,
		const std::vector<LOD_FaceInd> & degenerate_faces,
		const std::vector<LOD_VertexInd> & degenerate_vertices
	);


private :	

	// owned by this class
	//////////////////////

	MEM_SmartPtr<LOD_EdgeCollapser> m_collapser;
	MEM_SmartPtr<CTR_UHeap<LOD_Edge> > m_heap;
	MEM_SmartPtr<LOD_QuadricEditor> m_quadric_editor;

	bool m_is_armed;

	// arguments to New(...)
	////////////////////////

	LOD_ManMesh2 & m_mesh;
	LOD_ExternNormalEditor &m_face_editor;
	LOD_ExternBufferEditor & m_extern_editor;	

	// temporary buffers
	////////////////////

	std::vector<LOD_FaceInd> m_deg_faces;
	std::vector<LOD_EdgeInd> m_deg_edges;
	std::vector<LOD_VertexInd> m_deg_vertices;

	std::vector<LOD_FaceInd> m_update_faces;
	std::vector<LOD_EdgeInd> m_new_edges;
	std::vector<LOD_VertexInd> m_update_vertices;


};
#endif


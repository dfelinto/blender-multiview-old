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

#ifndef BSP_TMesh_h

#define BSP_TMesh_h

#include "MT_Point3.h"
#include "MT_Vector3.h"
#include "MT_Transform.h"

#include "MEM_SmartPtr.h"

#include <vector>

#include "CSG_BooleanOps.h"

/**
 * A very basic test mesh.
 */

struct BSP_TVertex {
	MT_Point3 m_pos;
};

struct BSP_TFace {
	int m_verts[3];
	MT_Vector3 m_normal;
};


class BSP_TMesh {
public :

	std::vector<BSP_TVertex> m_verts;
	std::vector<BSP_TFace> m_faces;
	
	MT_Vector3 m_min,m_max;

		std::vector<BSP_TVertex> &
	VertexSet(
	){
		return m_verts;
	}

		std::vector<BSP_TFace> &
	FaceSet(
	) {
		return m_faces;
	}

		void
	AddFace(
		int *verts,
		int num_verts
	);

		void
	BuildNormal(
		BSP_TFace & f
	) const ;
		
};



/**
 *  some iterator functions to describe the mesh to the BSP module.
 */

/**
 * This class defines 2 C style iterators over a CSG mesh, one for
 * vertices and 1 for faces. They conform to the iterator interface
 * defined in CSG_BooleanOps.h
 */

struct VertexIt {
	BSP_TMesh * mesh;
	BSP_TVertex * pos;
	MT_Transform trans;
};


static
	void
VertexIt_Destruct(
	CSG_VertexIteratorDescriptor * iterator
) {
	delete ((VertexIt *)(iterator->it));
	iterator->it = NULL;
	delete(iterator);
};


static
	int
VertexIt_Done(
	CSG_IteratorPtr it
) {
	// assume CSG_IteratorPtr is of the correct type.
	VertexIt * vertex_it = (VertexIt *)it;

	if (vertex_it->pos < vertex_it->mesh->VertexSet().end()) return 0;
	return 1;
};

static
	void
VertexIt_Fill(
	CSG_IteratorPtr it,
	CSG_IVertex *vert
) {
	// assume CSG_IteratorPtr is of the correct type.
	VertexIt * vertex_it = (VertexIt *)it;
			
	MT_Point3 p = vertex_it->pos->m_pos;
	p = vertex_it->trans * p;

	p.getValue(vert->position);
};

static
	void
VertexIt_Step(
	CSG_IteratorPtr it
) {
	// assume CSG_IteratorPtr is of the correct type.
	VertexIt * vertex_it = (VertexIt *)it;

	++(vertex_it->pos);
};

static
	CSG_VertexIteratorDescriptor * 
VertexIt_Construct(
	BSP_TMesh *mesh,
	MT_Transform trans
){
	// user should have insured mesh is not equal to NULL.
	
	CSG_VertexIteratorDescriptor * output = new CSG_VertexIteratorDescriptor;
	if (output == NULL) return NULL;
	output->Done = VertexIt_Done;
	output->Fill = VertexIt_Fill;
	output->Step = VertexIt_Step;
	output->num_elements = mesh->VertexSet().size();
	
	VertexIt * v_it = new VertexIt;
	v_it->mesh = mesh;
	v_it->pos = mesh->VertexSet().begin();
	v_it->trans = trans;
	output->it = v_it;
	return output;
};			


/**
 * Face iterator.
 */

struct FaceIt {
	BSP_TMesh * mesh;
	BSP_TFace *pos;
};


static
	void
FaceIt_Destruct(
	CSG_FaceIteratorDescriptor * iterator
) {
	delete ((FaceIt *)(iterator->it));
	iterator->it = NULL;
	delete(iterator);
};


static
	int
FaceIt_Done(
	CSG_IteratorPtr it
) {
	// assume CSG_IteratorPtr is of the correct type.
	FaceIt * face_it = (FaceIt *)it;

	if (face_it->pos < face_it->mesh->FaceSet().end()) {
		return 0;
	}
	return 1;
};

static
	void
FaceIt_Fill(
	CSG_IteratorPtr it,
	CSG_IFace *face
){
	// assume CSG_IteratorPtr is of the correct type.
	FaceIt * face_it = (FaceIt *)it;		
	// essentially iterating through a triangle fan here.

	face->vertex_index[0] = int(face_it->pos->m_verts[0]);
	face->vertex_index[1] = int(face_it->pos->m_verts[1]);
	face->vertex_index[2] = int(face_it->pos->m_verts[2]);

	face->vertex_number = 3;
};

static
	void
FaceIt_Step(
	CSG_IteratorPtr it
) {
	// assume CSG_IteratorPtr is of the correct type.
	FaceIt * face_it = (FaceIt *)it;		

	face_it->pos ++;
};

static
	CSG_FaceIteratorDescriptor * 
FaceIt_Construct(
	BSP_TMesh * mesh
) {
	CSG_FaceIteratorDescriptor * output = new CSG_FaceIteratorDescriptor;
	if (output == NULL) return NULL;

	output->Done = FaceIt_Done;
	output->Fill = FaceIt_Fill;
	output->Step = FaceIt_Step;

	output->num_elements = mesh->FaceSet().size();
	
	FaceIt * f_it = new FaceIt;
	f_it->mesh = mesh;
	f_it->pos = mesh->FaceSet().begin();

	output->it = f_it;

	return output;
};

/**
 * Some Build functions.
 */

static
	MEM_SmartPtr<BSP_TMesh>
BuildMesh(
	CSG_MeshPropertyDescriptor &props,
	CSG_FaceIteratorDescriptor &face_it,
	CSG_VertexIteratorDescriptor &vertex_it
) {
	MEM_SmartPtr<BSP_TMesh> mesh = new BSP_TMesh();

	CSG_IVertex vert;

	while (!vertex_it.Done(vertex_it.it)) {

		vertex_it.Fill(vertex_it.it,&vert);
	
		BSP_TVertex v;
		v.m_pos = MT_Point3(vert.position);
		mesh->VertexSet().push_back(v);

		vertex_it.Step(vertex_it.it);
	}

	
	CSG_IFace face;

	while (!face_it.Done(face_it.it)) {
		face_it.Fill(face_it.it,&face);

		BSP_TFace f;

		f.m_verts[0] = face.vertex_index[0],
		f.m_verts[1] = face.vertex_index[1],
		f.m_verts[2] = face.vertex_index[2],

		mesh->BuildNormal(f);

		mesh->FaceSet().push_back(f);

		face_it.Step(face_it.it);
	}

	return mesh;
};



	

	






	



	



















#endif
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

#include "BSP_MeshPrimitives.h"

#include "MT_assert.h"
#include "BSP_CSGException.h"
#include <algorithm>

using namespace std;

BSP_MVertex::
BSP_MVertex(
) :
	m_pos (MT_Point3()),
	m_select_tag (false),
	m_open_tag (0)
{
};

BSP_MVertex::
BSP_MVertex(
	const MT_Point3 & pos
) :
	m_pos(pos),
	m_select_tag (false),
	m_open_tag (0)
{
};


	bool
BSP_MVertex::
RemoveEdge(
	BSP_EdgeInd e
){
	vector<BSP_EdgeInd>::iterator result = find(m_edges.begin(),m_edges.end(),e);
	if (result == m_edges.end()) {
		return false;
	}
	BSP_EdgeInd last = m_edges.back();
	m_edges.pop_back();
	if (m_edges.empty()) return true;

	*result = last;
	return true;	
}	

	void
BSP_MVertex::
AddEdge(
	BSP_EdgeInd e
){
	m_edges.push_back(e);
}

	void
BSP_MVertex::
SwapEdge(
	BSP_EdgeInd e_old,
	BSP_EdgeInd e_new
){
	vector<BSP_EdgeInd>::iterator result = 
		find(m_edges.begin(),m_edges.end(),e_old);
	if (result == m_edges.end()) {
		BSP_CSGException e(e_mesh_error);
		throw(e);
		MT_assert(false);
	}
	
	*result = e_new;
}

	bool
BSP_MVertex::
SelectTag(
) const{
	return m_select_tag;
}

	void
BSP_MVertex::
SetSelectTag(
	bool tag	
){
	m_select_tag = tag;
}

	int
BSP_MVertex::
OpenTag(
) const {
	return m_open_tag;
}

	void
BSP_MVertex::
SetOpenTag(
	int tag
){
	m_open_tag = tag;
}


/**
 * Edge Primitive Methods.
 */

BSP_MEdge::
BSP_MEdge(
){
	m_verts[0] = m_verts[1] = BSP_VertexInd::Empty();
}
	
	bool 
BSP_MEdge::
operator == (
	BSP_MEdge & rhs
){
	// edges are the same if their vertex indices are the 
	// same!!! Other properties are not checked 

	int matches = 0;

	if (this->m_verts[0] == rhs.m_verts[0]) {
		++matches;
	}
	if (this->m_verts[1] == rhs.m_verts[0]) {
		++matches;
	}
	if (this->m_verts[0] == rhs.m_verts[1]) {
		++matches;
	}
	if (this->m_verts[1] == rhs.m_verts[1]) {
		++matches;
	}
	
	if (matches >= 2) {
		return true;
	}
	return false;
}

	void
BSP_MEdge::
SwapFace(
	BSP_FaceInd old_f,
	BSP_FaceInd new_f
){
	vector<BSP_FaceInd>::iterator result = 
		find(m_faces.begin(),m_faces.end(),old_f);
	if (result == m_faces.end()) {
		BSP_CSGException e(e_mesh_error);
		throw(e);
		MT_assert(false);
	}
	
	*result = new_f;
}

    BSP_VertexInd
BSP_MEdge::
OpVertex(
	BSP_VertexInd vi
) const {
	if (vi == m_verts[0]) return m_verts[1];
	if (vi == m_verts[1]) return m_verts[0];
	MT_assert(false);
	BSP_CSGException e(e_mesh_error);
	throw(e);

	return BSP_VertexInd::Empty();
}

	bool
BSP_MEdge::
SelectTag(
) const {
	return bool(m_verts[1].Tag() & 0x1);
}
	void
BSP_MEdge::
SetSelectTag(
	bool tag	
){
	m_verts[1].SetTag(int(tag));
}

	int
BSP_MEdge::
OpenTag(
) const {
	return m_verts[0].Tag();
}

	void
BSP_MEdge::
SetOpenTag(
	int tag
) {
	// Note conversion from int to unsigned int!!!!!
	m_verts[0].SetTag(tag);
}
	

/**
 * Face primitive methods
 */


BSP_MFace::
BSP_MFace(
):
	m_open_tag(-1)
{
	// nothing to do
}

	void
BSP_MFace::
Invert(
){

	// TODO replace reverse as I think some compilers
	// do not support the STL routines employed.

	reverse(
		m_verts.begin(),
		m_verts.end()
	);

	reverse(
		m_fv_data.begin(),
		m_fv_data.end()
	);

	// invert the normal
	m_plane.Invert();
}

	bool
BSP_MFace::
SelectTag(
) const {
	return bool(m_verts[1].Tag() & 0x1);
}	

	void
BSP_MFace::
SetSelectTag(
	bool tag	
){
	m_verts[1].SetTag(int(tag));
};	

	int
BSP_MFace::
OpenTag(
) const {
	return m_open_tag;
}

	void
BSP_MFace::
SetOpenTag(
	int tag
){
	// Note conversion from int to unsigned int!!!!!
	m_open_tag = tag;
}




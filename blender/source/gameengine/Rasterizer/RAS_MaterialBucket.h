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
#ifndef __RAS_MATERIALBUCKET
#define __RAS_MATERIALBUCKET

#include "RAS_TexVert.h"
#include "GEN_Map.h"
#include "STR_HashedString.h"

#include "MT_Transform.h"
#include "RAS_IPolygonMaterial.h"
#include "RAS_Deformer.h"	// __NLA
#include <vector>
#include <map>
#include <set>
using namespace std;

typedef vector< vector<class RAS_TexVert>* >  vecVertexArray;
typedef vector<unsigned int> KX_IndexArray;
typedef vector< KX_IndexArray* > vecIndexArrays;

typedef vector<RAS_TexVert> KX_VertexArray;



struct KX_VertexIndex {
public:
	KX_VertexIndex(int size);
	void	SetIndex(short loc,short index);
	short	m_vtxarray;
	short	m_indexarray[4];
	short	m_size;
};



class KX_MeshSlot
{
public:
	void*					m_clientObj;
	RAS_Deformer*			m_pDeformer;	//	__NLA
	double*					m_OpenGLMatrix;
	class RAS_MeshObject*	m_mesh;
	mutable bool			m_bVisible; // for visibility
	mutable bool  		    m_bObjectColor;
	mutable MT_Vector4		m_RGBAcolor;
	
	KX_MeshSlot() :m_bVisible(true), m_pDeformer(NULL) {}
	//	KX_MeshSlot() :m_bVisible(true) {}
	
	bool					Less(const KX_MeshSlot& lhs) const;
};


inline bool operator <( const KX_MeshSlot& rhs,const KX_MeshSlot& lhs)
{
	return ( rhs.Less(lhs));
}


class RAS_MaterialBucket
{
	typedef std::set<KX_MeshSlot> T_MeshSlotList;
	
	T_MeshSlotList				m_meshSlots;
	bool						m_bScheduled;
	bool						m_bModified;
	RAS_IPolyMaterial*			m_material;
	double*						m_pOGLMatrix;
	
public:
	RAS_MaterialBucket(RAS_IPolyMaterial* mat);
	virtual ~RAS_MaterialBucket() {}
	
	void	Render(const MT_Transform& cameratrans,
					   class RAS_IRasterizer* rasty,
					   class RAS_IRenderTools* rendertools);
	
	void	SchedulePolygons(int drawingmode);
	void	ClearScheduledPolygons();
	
	RAS_IPolyMaterial*		GetPolyMaterial();
	bool	IsTransparant();
		
	static void	StartFrame();
	static void EndFrame();

	void	SetMeshSlot(KX_MeshSlot& ms);
	void	RemoveMeshSlot(KX_MeshSlot& ms);
	void	MarkVisibleMeshSlot(KX_MeshSlot& ms,
								bool visible,
								bool color,
								const MT_Vector4& rgbavec);
};


#endif //__KX_BUCKET

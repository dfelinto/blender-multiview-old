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
#ifndef __RAS_POLYGON
#define __RAS_POLYGON

#include "RAS_TexVert.h"
//#include "KX_BoundingVolumes.h"
#include "RAS_MaterialBucket.h"

#include <vector>
using namespace std;


//
// Bitfield that stores the flags for each CValue derived class
//
struct PolygonFlags {
	PolygonFlags() :
		Visible(true),
		Collider(true)
	{
	}
	unsigned char Visible : 1;
	unsigned char Collider : 1;
	//int Visible : 1;
	//int Collider : 1;
};

class RAS_Polygon
{
	RAS_MaterialBucket*			m_bucket;
	KX_VertexIndex				m_vertexindexbase;
	int							m_numverts;
	int							m_edgecode;
	PolygonFlags				m_polyFlags;
	

public:
	RAS_Polygon(RAS_MaterialBucket* bucket,
				bool visible,
				int numverts,
				int vtxarrayindex) ;
	virtual ~RAS_Polygon() {};
	
//	RAS_TexVert* GetVertex(int index);
	int					VertexCount();
	void				SetVertex(int i,int vertexindex); //const MT_Point3& xyz,const MT_Point2& uv,const unsigned int rgbacolor,const MT_Vector3& normal)
	
	const KX_VertexIndex& GetIndexBase();

	void				SetVisibleWireframeEdges(int edgecode);
	// each bit is for a visible edge, starting with bit 1 for the first edge, bit 2 for second etc.
	int					GetEdgeCode();
	
	bool				IsVisible();
	bool				IsCollider();
	void				SetCollider(bool col);

	KX_VertexIndex&		GetVertexIndexBase();
	RAS_MaterialBucket*	GetMaterial();

};
#endif

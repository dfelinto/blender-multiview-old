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

#ifdef WIN32

#pragma warning (disable:4786)
#endif

#include "RAS_Polygon.h"

/*
RAS_TexVert*	RAS_Polygon::GetVertex(int index)
{ 
		if (m_bucket)
			return m_bucket->GetVertex(m_vertexindexbase.m_vtxarray, m_vertexindexbase.m_indexarray[index]);
		else
			return NULL;
}
*/

/*void RAS_Polygon::Bucketize(double* oglmatrix)
{
		//Transform(trans);
		if (m_bucket)
			m_bucket->AddPolygon(this,oglmatrix);
}
*/

RAS_Polygon::RAS_Polygon(RAS_MaterialBucket* bucket,
				bool visible,
				int numverts,
				int vtxarrayindex) 
		:m_bucket(bucket),
		m_vertexindexbase(numverts),
		m_numverts(numverts),
		m_edgecode(0)
{
	int size = sizeof(RAS_Polygon);
	m_vertexindexbase.m_vtxarray = vtxarrayindex ;//m_bucket->FindVertexArray(numverts);
	m_polyFlags.Visible = visible;
}



int RAS_Polygon::VertexCount()
{
	return m_numverts;
}



void RAS_Polygon::SetVertex(int i,
							int vertexindex ) //const MT_Point3& xyz,const MT_Point2& uv,const unsigned int rgbacolor,const MT_Vector3& normal)
{
	m_vertexindexbase.SetIndex(i,vertexindex); //m_bucket->FindOrAddVertex(m_vertexindexbase.m_vtxarray,
	//xyz,uv,rgbacolor,normal));
}



const KX_VertexIndex& RAS_Polygon::GetIndexBase()
{
	return m_vertexindexbase;
}



void RAS_Polygon::SetVisibleWireframeEdges(int edgecode)
{
	m_edgecode = edgecode;
}



// each bit is for a visible edge, starting with bit 1 for the first edge, bit 2 for second etc.
int RAS_Polygon::GetEdgeCode()
{
	return m_edgecode;
}


	
bool RAS_Polygon::IsVisible()
{
	return m_polyFlags.Visible;
}



bool RAS_Polygon::IsCollider()
{
	return m_polyFlags.Collider;
}



void RAS_Polygon::SetCollider(bool col)
{
	m_polyFlags.Collider = col;
}



KX_VertexIndex& RAS_Polygon::GetVertexIndexBase()
{
	return m_vertexindexbase;
}



RAS_MaterialBucket*	RAS_Polygon::GetMaterial()
{
	return m_bucket;
}

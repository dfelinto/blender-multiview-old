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
 * Deformer that supports armature skinning
 */

#ifdef WIN32
#pragma warning (disable:4786) // get rid of stupid stl-visual compiler debug warning
#endif //WIN32
#include "RAS_IPolygonMaterial.h"
#include "BL_SkinMeshObject.h"
#include "BL_DeformableGameObject.h"
#include "DNA_mesh_types.h"
#include "KX_GameObject.h"
#include "RAS_BucketManager.h"

void BL_SkinMeshObject::AddPolygon(RAS_Polygon* poly)
{
	/* We're overriding this so that we can eventually associate faces with verts somehow */

	//	For vertIndex in poly:
	//		find the appropriate normal

	RAS_MeshObject::AddPolygon(poly);
}

#ifdef __NLA_OLDDEFORM
int BL_SkinMeshObject::FindOrAddDeform(int vtxarray, struct MVert *mv, struct MDeformVert *dv, RAS_IPolyMaterial* mat)
#else
int BL_SkinMeshObject::FindOrAddDeform(int vtxarray, int mv, struct MDeformVert *dv, RAS_IPolyMaterial* mat)
#endif
{
	BL_SkinArrayOptimizer* ao = (BL_SkinArrayOptimizer*)GetArrayOptimizer(mat);//*(m_matVertexArrays[*mat]);
	int numvert = ao->m_MvertArrayCache1[vtxarray]->size();
	
	/* Check to see if this has already been pushed */
	for (int i=0; i<ao->m_MvertArrayCache1[vtxarray]->size(); i++){
		if (mv == (*ao->m_MvertArrayCache1[vtxarray])[i])
			return i;
	}

	ao->m_MvertArrayCache1[vtxarray]->push_back(mv);
	ao->m_DvertArrayCache1[vtxarray]->push_back(dv);
	
	return numvert;
};

int	BL_SkinMeshObject::FindVertexArray(int numverts,RAS_IPolyMaterial* polymat)
{
	int array=-1;
	
	BL_SkinArrayOptimizer* ao = (BL_SkinArrayOptimizer*)GetArrayOptimizer(polymat);


	for (int i=0;i<ao->m_VertexArrayCache1.size();i++)
	{
		if ( (ao->m_TriangleArrayCount[i] + (numverts-2)) < BUCKET_MAX_TRIANGLES) 
		{
			 if((ao->m_VertexArrayCache1[i]->size()+numverts < BUCKET_MAX_INDICES))
				{
					array = i;
					ao->m_TriangleArrayCount[array]+=numverts-2;
					break;
				}
		}
	}
	

	if (array == -1)
	{
		array = ao->m_VertexArrayCache1.size();
		
		vector<RAS_TexVert>* va = new vector<RAS_TexVert>;
		ao->m_VertexArrayCache1.push_back(va);
		
		KX_IndexArray *ia = new KX_IndexArray();
		ao->m_IndexArrayCache1.push_back(ia);

#ifdef __NLA_OLDDEFORM
		BL_MVertArray *bva = new BL_MVertArray();
#else
		KX_IndexArray *bva = new KX_IndexArray();
#endif
		ao->m_MvertArrayCache1.push_back(bva);

		BL_DeformVertArray *dva = new BL_DeformVertArray();
		ao->m_DvertArrayCache1.push_back(dva);

		KX_IndexArray *da = new KX_IndexArray();
		ao->m_DIndexArrayCache1.push_back(da);

		ao->m_TriangleArrayCount.push_back(numverts-2);

	}

		
	return array;
}


//void BL_SkinMeshObject::Bucketize(double* oglmatrix,void* clientobj,bool useObjectColor,const MT_Vector4& rgbavec,RAS_BucketManager* bucketmgr)
void BL_SkinMeshObject::Bucketize(double* oglmatrix,void* clientobj,bool useObjectColor,const MT_Vector4& rgbavec)
{

	KX_MeshSlot ms;
	ms.m_clientObj = clientobj;
	ms.m_mesh = this;
	ms.m_OpenGLMatrix = oglmatrix;
	ms.m_bObjectColor = useObjectColor;
	ms.m_RGBAcolor = rgbavec;
	ms.m_pDeformer = ((BL_DeformableGameObject*)clientobj)->m_pDeformer;
	
	for (BucketMaterialSet::iterator it = m_materials.begin();it!=m_materials.end();it++)
	{

		RAS_MaterialBucket* materialbucket = (*it);

		KX_ArrayOptimizer* oa = GetArrayOptimizer(materialbucket->GetPolyMaterial());
		materialbucket->SetMeshSlot(ms);
	}

}




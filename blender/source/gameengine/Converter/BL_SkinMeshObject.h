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

#ifndef __BL_SKINMESHOBJECT
#define __BL_SKINMESHOBJECT

#ifdef WIN32
#pragma warning (disable:4786) // get rid of stupid stl-visual compiler debug warning
#endif //WIN32

#include "RAS_MeshObject.h"
#include "RAS_Deformer.h"
#include "RAS_IPolygonMaterial.h"

#include "BL_MeshDeformer.h"

#include "DNA_mesh_types.h"

typedef vector<struct MVert*> BL_MVertArray;
typedef vector<struct MDeformVert*> BL_DeformVertArray;
typedef vector<class BL_TexVert> BL_VertexArray;


typedef vector<vector<struct MDeformVert*>*> vecMDVertArray;
typedef vector<vector<class BL_TexVert>*> vecBVertexArray;

class BL_SkinArrayOptimizer : public KX_ArrayOptimizer  
{
public:
	BL_SkinArrayOptimizer(int index)
		:KX_ArrayOptimizer (index) {};
	virtual ~BL_SkinArrayOptimizer(){

		for (vector<KX_IndexArray*>::iterator itv = m_MvertArrayCache1.begin();
		!(itv == m_MvertArrayCache1.end());itv++)
		{
			delete (*itv);
		}
		for (vector<BL_DeformVertArray*>::iterator itd = m_DvertArrayCache1.begin();
		!(itd == m_DvertArrayCache1.end());itd++)
		{
			delete (*itd);
		}
		for (vector<KX_IndexArray*>::iterator iti = m_DIndexArrayCache1.begin();
		!(iti == m_DIndexArrayCache1.end());iti++)
		{
			delete (*iti);
		}
		
		m_MvertArrayCache1.clear();
		m_DvertArrayCache1.clear();
		m_DIndexArrayCache1.clear();
	};

	vector<KX_IndexArray*>		m_MvertArrayCache1;
	vector<BL_DeformVertArray*>	m_DvertArrayCache1;
	vector<KX_IndexArray*>		m_DIndexArrayCache1;

};

class BL_SkinMeshObject : public RAS_MeshObject
{

	enum	{	BUCKET_MAX_INDICES = 2048};//2048};//8192};
	enum	{	BUCKET_MAX_TRIANGLES = 1024};

	KX_ArrayOptimizer*		GetArrayOptimizer(RAS_IPolyMaterial* polymat)
	{
		KX_ArrayOptimizer** aop = (m_matVertexArrayS[*polymat]);
		if (aop)
			return *aop;
		int numelements = m_matVertexArrayS.size();
		m_sortedMaterials.push_back(polymat);
		
		BL_SkinArrayOptimizer* ao = new BL_SkinArrayOptimizer(numelements);
		m_matVertexArrayS.insert(*polymat,ao);
		return ao;
	}

protected:
public:
	void Bucketize(double* oglmatrix,void* clientobj,bool useObjectColor,const MT_Vector4& rgbavec);
//	void Bucketize(double* oglmatrix,void* clientobj,bool useObjectColor,const MT_Vector4& rgbavec,class RAS_BucketManager* bucketmgr);

	int FindVertexArray(int numverts,RAS_IPolyMaterial* polymat);
	BL_SkinMeshObject(int lightlayer) : RAS_MeshObject (lightlayer)
	{};

	virtual ~BL_SkinMeshObject(){
	};

	const vecIndexArrays& GetDIndexCache (RAS_IPolyMaterial* mat)
	{
		BL_SkinArrayOptimizer* ao = (BL_SkinArrayOptimizer*)GetArrayOptimizer(mat);//*(m_matVertexArrays[*mat]);
		return ao->m_DIndexArrayCache1;
	}
	const vecMDVertArray&	GetDVertCache (RAS_IPolyMaterial* mat)
	{
		BL_SkinArrayOptimizer* ao = (BL_SkinArrayOptimizer*)GetArrayOptimizer(mat);//*(m_matVertexArrays[*mat]);
		return ao->m_DvertArrayCache1;
	}
	const vecIndexArrays&	GetMVertCache (RAS_IPolyMaterial* mat)
	{
		BL_SkinArrayOptimizer* ao = (BL_SkinArrayOptimizer*)GetArrayOptimizer(mat);//*(m_matVertexArrays[*mat]);
		return ao->m_MvertArrayCache1;
	}
	
	void AddPolygon(RAS_Polygon* poly);
	int FindOrAddDeform(int vtxarray, int mv, struct MDeformVert *dv, RAS_IPolyMaterial* mat);
	int FindOrAddVertex(int vtxarray,const MT_Point3& xyz,
		const MT_Point2& uv,
		const unsigned int rgbacolor,
		const MT_Vector3& normal, int defnr, bool flat, RAS_IPolyMaterial* mat)
	{
		short newnormal[3];
		newnormal[0]=(short)(normal[0] * 32767.0);
		newnormal[1]=(short)(normal[1] * 32767.0);
		newnormal[2]=(short)(normal[2] * 32767.0);

		RAS_TexVert tempvert(xyz,uv,rgbacolor,newnormal,flat ? TV_CALCFACENORMAL : 0);
		
		//		KX_ArrayOptimizer* ao = GetArrayOptimizer(mat);//*(m_matVertexArrays[*mat]);
		BL_SkinArrayOptimizer* ao = (BL_SkinArrayOptimizer*)GetArrayOptimizer(mat);//*(m_matVertexArrays[*mat]);
		
		int numverts = ao->m_VertexArrayCache1[vtxarray]->size();//m_VertexArrayCount[vtxarray];
		
		int index=-1;
		
		for (int i=0;i<numverts;i++)
		{
			const RAS_TexVert&  vtx = (*ao->m_VertexArrayCache1[vtxarray])[i];
			if (tempvert.closeTo(&vtx))
			{
				index = i;
				break;
			}
			
		}
		if (index >= 0)
			return index;
		
		// no vertex found, add one
		ao->m_VertexArrayCache1[vtxarray]->push_back(tempvert);
		ao->m_DIndexArrayCache1[vtxarray]->push_back(defnr);
		
		return numverts;
		
		
	}

};


#endif


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
 * Simple deformation controller that restores a mesh to its rest position
 */

#ifdef WIN32
// This warning tells us about truncation of __long__ stl-generated names.
// It can occasionally cause DevStudio to have internal compiler warnings.
#pragma warning( disable : 4786 )     
#endif

#include "RAS_IPolygonMaterial.h"
#include "BL_MeshDeformer.h"
#include "BL_SkinMeshObject.h"
#include "DNA_mesh_types.h"
#include "BLI_arithb.h"

#include "GEN_Map.h"
#include "STR_HashedString.h"


bool BL_MeshDeformer::Apply(RAS_IPolyMaterial *mat)
{
	int			i, j, index;
	vecVertexArray	array;
	vecIndexArrays	mvarray;
	vecIndexArrays	diarray;
	float co[3];
	
	RAS_TexVert *tv;
	MVert	*mvert;
	
	// For each material
	array = m_pMeshObject->GetVertexCache(mat);
	mvarray = m_pMeshObject->GetMVertCache(mat);
	diarray = m_pMeshObject->GetDIndexCache(mat);
	
	// For each array
	for (i=0; i<array.size(); i++){
		//	For each vertex
		for (j=0; j<array[i]->size(); j++){
			tv = &((*array[i])[j]);
			MT_Point3 pt = tv->xyz();
			
			index = ((*diarray[i])[j]);

			mvert = &(m_bmesh->mvert[((*mvarray[i])[index])]);
			//	Do the nasty (in this case, copy the untransformed data from the blender mesh)
			co[0]=mvert->co[0];
			co[1]=mvert->co[1];
			co[2]=mvert->co[2];
			
			pt[0] = co[0];
			pt[1] = co[1];
			pt[2] = co[2];
			
			tv->SetXYZ(pt);
		}
	}
	return true;
}

BL_MeshDeformer::~BL_MeshDeformer()
{	
	if (m_transverts)
		delete []m_transverts;
	if (m_transnors)
		delete []m_transnors;
};

void BL_MeshDeformer::RecalcNormals()
{
	int v, f;
	float fnor[3], co1[3], co2[3], co3[3], co4[3];

	/* Clear all vertex normal accumulators */
	for (v =0; v<m_bmesh->totvert; v++){
		m_transnors[v]=MT_Point3(0,0,0);
	}
	
	/* Find the face normals */
	for (f = 0; f<m_bmesh->totface; f++){
		// Make new face normal based on the transverts
		MFace *mf= &((MFace*)m_bmesh->mface)[f];
		
		if (mf->v3) {
			for (int vl=0; vl<3; vl++){
				co1[vl]=m_transverts[mf->v1][vl];
				co2[vl]=m_transverts[mf->v2][vl];
				co3[vl]=m_transverts[mf->v3][vl];
				if (mf->v4)
					co4[vl]=m_transverts[mf->v4][vl];
			}

			/* FIXME: Use moto */
			if (mf->v4)
				CalcNormFloat4(co1, co2, co3, co4, fnor);
			else
				CalcNormFloat(co1, co2, co3, fnor);
	
			/* Decide which normals are affected by this face's normal */
			m_transnors[mf->v1]+=MT_Point3(fnor);
			m_transnors[mf->v2]+=MT_Point3(fnor);
			m_transnors[mf->v3]+=MT_Point3(fnor);
			if (mf->v4)
				m_transnors[mf->v4]+=MT_Point3(fnor);

		}
		
	}
	
	for (v =0; v<m_bmesh->totvert; v++){
		float nor[3];

		m_transnors[v]=m_transnors[v].safe_normalized();
		nor[0]=m_transnors[v][0];
		nor[1]=m_transnors[v][1];
		nor[2]=m_transnors[v][2];
		
	};
}

void BL_MeshDeformer::VerifyStorage()
{
	/* Ensure that we have the right number of verts assigned */
	if (m_tvtot!=m_bmesh->totvert+m_bmesh->totface){
		if (m_transverts)
			delete []m_transverts;
		if (m_transnors)
			delete []m_transnors;
		
		m_transnors =new MT_Point3[m_bmesh->totvert+m_bmesh->totface];
		m_transverts=new MT_Point3[m_bmesh->totvert];
		m_tvtot = m_bmesh->totvert;
	}
}
 
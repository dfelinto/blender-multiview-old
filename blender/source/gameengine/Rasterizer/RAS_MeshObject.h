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
#ifndef __RAS_MESHOBJECT
#define __RAS_MESHOBJECT


#ifdef WIN32
// disable the STL warnings ("debug information length > 255")
#pragma warning (disable:4786)
#endif

#include <vector>
#include <set>

#include "RAS_Polygon.h"
#include "MT_Transform.h"

#include "GEN_HashedPtr.h"

class KX_ArrayOptimizer
{
public:
	KX_ArrayOptimizer(int index) 
		: m_index1(index)
	{};
	virtual ~KX_ArrayOptimizer();
	
	vector<KX_VertexArray*>		m_VertexArrayCache1;
	vector<int>					m_TriangleArrayCount;
	vector<KX_IndexArray*>		m_IndexArrayCache1;

	/**
		order in which they are stored into the mesh
	*/
	int							m_index1;
};

struct	RAS_TriangleIndex
{
public:
	int		m_index[3];
	bool	m_collider;
};

class	RAS_MatArrayIndex
{
public:

	int	m_arrayindex1;
	int	m_matid;
	int	m_array;
	int	m_index;

	inline bool	Less(const RAS_MatArrayIndex& lhs) const {
		bool result = 
			(	(m_matid < lhs.m_matid)		||
				((m_matid == lhs.m_matid)&&(m_array < lhs.m_array)) ||
				((m_matid == lhs.m_matid) && (m_array == lhs.m_array) &&
				(m_index < lhs.m_index))
					
			);
		return result;
			
	}

	
};
inline  bool operator <( const RAS_MatArrayIndex& rhs,const RAS_MatArrayIndex& lhs)
{
	return ( rhs.Less(lhs));
}


class RAS_MeshObject
{
	
	enum { BUCKET_MAX_INDICES = 2048 };//2048};//8192};
	enum { BUCKET_MAX_TRIANGLES = 1024 };
	
	//	GEN_Map<class RAS_IPolyMaterial,KX_ArrayOptimizer*> m_matVertexArrayS;
	//vector<class RAS_IPolyMaterial*,KX_ArrayOptimizer> m_vertexArrays;
	virtual KX_ArrayOptimizer* GetArrayOptimizer(RAS_IPolyMaterial* polymat);
	//vector<RAS_Polygon*>		m_polygons;
	
	unsigned int				m_debugcolor;
	bool						m_bModified;
	int							m_lightlayer;
	
	vector<class RAS_Polygon*> 	m_Polygons;
	STR_String					m_name;
	static STR_String			s_emptyname;
	
protected:
	GEN_Map<class RAS_IPolyMaterial,KX_ArrayOptimizer*> m_matVertexArrayS;
	typedef set<class RAS_MaterialBucket*> BucketMaterialSet;
	
	BucketMaterialSet			m_materials;
public:
	// for now, meshes need to be in a certain layer (to avoid sorting on lights in realtime)
	RAS_MeshObject(int lightlayer);
	virtual ~RAS_MeshObject();

	vector<RAS_IPolyMaterial*>				m_sortedMaterials;
	vector<vector<RAS_MatArrayIndex> >		m_xyz_index_to_vertex_index_mapping;
	vector<RAS_TriangleIndex >				m_triangle_indices;

	int					GetLightLayer();
	int					NumMaterials();
	const STR_String&	GetMaterialName(int matid);
	RAS_MaterialBucket*	GetMaterialBucket(int matid);
	const STR_String&	GetTextureName(int matid);
	virtual void		AddPolygon(RAS_Polygon* poly);
	void				UpdateMaterialList();
	
	int					NumPolygons();
	RAS_Polygon*		GetPolygon(int num);
	
	virtual void		Bucketize(
							double* oglmatrix,
							void* clientobj,
							bool useObjectColor,
							const MT_Vector4& rgbavec
						);

	void				RemoveFromBuckets(
							double* oglmatrix,
							void* clientobj
						);

	void				MarkVisible(
							double* oglmatrix,
							void* clientobj,
							bool visible,
							bool useObjectColor,
							const MT_Vector4& rgbavec
						);

	void				DebugColor(unsigned int abgr);
	
	void				SchedulePolygons(
							int drawingmode,
							class RAS_IRasterizer* rasty
						);

	void				ClearArrayData();
	
	set<RAS_MaterialBucket*>::iterator GetFirstMaterial();
	set<RAS_MaterialBucket*>::iterator GetLastMaterial();
	
	virtual RAS_TexVert*	GetVertex(
								short array,
								short index,
								RAS_IPolyMaterial* polymat
							);
	
	virtual int			FindVertexArray(
							int numverts,
							RAS_IPolyMaterial* polymat
						);
	
	void				SchedulePoly(
							const KX_VertexIndex& idx,
							int numverts,
							RAS_IPolyMaterial* mat
						);

	void				ScheduleWireframePoly(
							const KX_VertexIndex& idx,
							int numverts,
							int edgecode,
							RAS_IPolyMaterial* mat
						);
	
	// find (and share) or add vertices
	// for some speedup, only the last 20 added vertices are searched for equality
	
	virtual int			FindOrAddVertex(
							int vtxarray,
							const MT_Point3& xyz,
							const MT_Point2& uv,
							const unsigned int rgbacolor,
							const MT_Vector3& normal,
							RAS_IPolyMaterial* mat,
							int orgindex
						);
	
	const vecVertexArray&	GetVertexCache (RAS_IPolyMaterial* mat);
	
	int					GetVertexArrayLength(RAS_IPolyMaterial* mat);

	RAS_TexVert*		GetVertex(
							int matid,
							int index
						);

	const vecIndexArrays& GetIndexCache (RAS_IPolyMaterial* mat);
	void				SetName(STR_String name);
	const STR_String&	GetName();
};

#endif //__RAS_MESHOBJECT

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
#ifndef __RAS_IPOLYGONMATERIAL
#define __RAS_IPOLYGONMATERIAL

#include "STR_HashedString.h"


/**
 * Polygon Material on which the material buckets are sorted
 *
 */
#include "MT_Vector3.h"
#include "STR_HashedString.h"

class RAS_IRasterizer;

class RAS_IPolyMaterial
{
	//todo: remove these variables from this interface/protocol class
protected:
	STR_HashedString    m_texturename;
	STR_String          m_materialname; //also needed for touchsensor  
	int					m_tile;
	int					m_tilexrep,m_tileyrep;
	int					m_drawingmode;	// tface->mode
	int					m_transparant;
	int					m_lightlayer;
	bool				m_bIsTriangle;
	
public:

	MT_Vector3			m_diffuse;
	float				m_shininess;
	MT_Vector3			m_specular;
	float				m_specularity;
	
	/** Used to store caching information for materials. */
	typedef void* TCachingInfo;

	// care! these are taken from blender polygonflags, see file DNA_mesh_types.h for #define TF_BILLBOARD etc.
	enum MaterialFlags
	{
		BILLBOARD_SCREENALIGNED = 256,
		BILLBOARD_AXISALIGNED = 4096,
		SHADOW				  =8192
	};

	RAS_IPolyMaterial(const STR_String& texname,
					  bool ba,
					  const STR_String& matname,
					  int tile,
					  int tilexrep,
					  int tileyrep,
					  int mode,
					  int transparant,
					  int lightlayer,
					  bool bIsTriangle,
					  void* clientobject);
	virtual ~RAS_IPolyMaterial() {};
 
	/**
	 * Returns the caching information for this material,
	 * This can be used to speed up the rasterizing process.
	 * @return The caching information.
	 */
	virtual TCachingInfo GetCachingInfo(void) const { return 0; }

	/**
	 * Activates the material in the rasterizer.
	 * On entry, the cachingInfo contains info about the last activated material.
	 * On exit, the cachingInfo should contain updated info about this material.
	 * @param rasty			The rasterizer in which the material should be active.
	 * @param cachingInfo	The information about the material used to speed up rasterizing.
	 */
	virtual void Activate(RAS_IRasterizer* rasty, TCachingInfo& cachingInfo) const {}

	bool				Equals(const RAS_IPolyMaterial& lhs) const;
	int					GetLightLayer();
	bool				IsTransparant();
	bool				UsesTriangles();
	unsigned int		hash() const;
	int					GetDrawingMode();
	const STR_String&	GetMaterialName() const;
	const STR_String&	GetTextureName() const;
};

inline  bool operator ==( const RAS_IPolyMaterial & rhs,const RAS_IPolyMaterial & lhs)
{
	return ( rhs.Equals(lhs));
}


#endif //__RAS_IPOLYGONMATERIAL

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
#ifndef __KX_BLENDERPOLYMATERIAL
#define __KX_BLENDERPOLYMATERIAL

#include "RAS_MaterialBucket.h"
#include "RAS_IRasterizer.h"

struct TFace;
extern "C" int set_tpage(TFace* tface);		/* Worst hack ever */


class KX_BlenderPolyMaterial : public RAS_IPolyMaterial
{
	/** Blender texture face structure. */
	TFace* m_tface;

public:
	
	KX_BlenderPolyMaterial(const STR_String &texname,
		bool ba,
		const STR_String& matname,
		int tile,
		int tilexrep,
		int tileyrep,
		int mode,
		int transparant,
		int lightlayer,
		bool bIsTriangle,
		void* clientobject,
		struct TFace* tface);	
	
	/**
	 * Returns the caching information for this material,
	 * This can be used to speed up the rasterizing process.
	 * @return The caching information.
	 */
	virtual TCachingInfo GetCachingInfo(void) const;

	/**
	 * Activates the material in the (OpenGL) rasterizer.
	 * On entry, the cachingInfo contains info about the last activated material.
	 * On exit, the cachingInfo should contain updated info about this material.
	 * @param rasty			The rasterizer in which the material should be active.
	 * @param cachingInfo	The information about the material used to speed up rasterizing.
	 */
	virtual void Activate(RAS_IRasterizer* rasty, TCachingInfo& cachingInfo) const;

	/**
	 * Returns the Blender texture face structure that is used for this material.
	 * @return The material's texture face.
	 */
	TFace* GetTFace(void) const;
protected:
private:
};


inline TFace* KX_BlenderPolyMaterial::GetTFace(void) const
{
	return m_tface;
}

inline RAS_IPolyMaterial::TCachingInfo KX_BlenderPolyMaterial::GetCachingInfo(void) const
{
	return GetTFace();
}

#endif // __KX_BLENDERPOLYMATERIAL

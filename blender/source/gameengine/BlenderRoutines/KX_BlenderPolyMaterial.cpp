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


#include "KX_BlenderPolyMaterial.h"
#include "BKE_mesh.h"

KX_BlenderPolyMaterial::KX_BlenderPolyMaterial(const STR_String &texname,
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
											   struct TFace* tface)
		: RAS_IPolyMaterial(texname,
							false,
							matname,
							tile,
							tilexrep,
							tileyrep,
							mode,
							transparant,
							lightlayer,
							bIsTriangle,
							clientobject),
		m_tface(tface)
{
}


void KX_BlenderPolyMaterial::Activate(RAS_IRasterizer* rasty, TCachingInfo& cachingInfo) const 
{

	
	if (GetCachingInfo() != cachingInfo)
	{
		if (!cachingInfo)
		{
			set_tpage(NULL);
		}
		cachingInfo = GetCachingInfo();

		if ((m_drawingmode & 4)&& (rasty->GetDrawingMode() == RAS_IRasterizer::KX_TEXTURED))
		{
			update_realtime_texture((struct TFace*) m_tface, rasty->GetTime());
			set_tpage(m_tface);
			rasty->EnableTextures(true);
		}
		else
		{
			set_tpage(NULL);
			rasty->EnableTextures(false);
		}
		
		//TF_TWOSIDE == 512, todo, make this a ketsji enum
		if(m_drawingmode & 512)
		{
			rasty->SetCullFace(false);
		}
		else
		{
			rasty->SetCullFace(true);
		}
	}

	rasty->SetSpecularity(m_specular[0],m_specular[1],m_specular[2],m_specularity);
	rasty->SetShinyness(m_shininess);
	rasty->SetDiffuse(m_diffuse[0], m_diffuse[1],m_diffuse[2], 1.0);
}





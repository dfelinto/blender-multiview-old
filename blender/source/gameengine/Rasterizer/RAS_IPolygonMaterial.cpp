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

#include "RAS_IPolygonMaterial.h"


RAS_IPolyMaterial::RAS_IPolyMaterial(const STR_String& texname,
									 bool ba,
									 const STR_String& matname,
									 int tile,
									 int tilexrep,
									 int tileyrep,
									 int mode,
									 int transparant,
									 int lightlayer,
									 bool bIsTriangle,
									 void* clientobject=NULL) :

		m_texturename(texname),
		m_materialname(matname),
		m_tile(tile),
		m_tilexrep(tilexrep),
		m_tileyrep(tileyrep),
		m_drawingmode (mode),
		m_transparant(transparant),
		m_lightlayer(lightlayer),
		m_bIsTriangle(bIsTriangle)
{
	m_shininess = 35.0;
	m_specular = MT_Vector3(0.5,0.5,0.5);
	m_specularity = 1.0;
	m_diffuse = MT_Vector3(0.5,0.5,0.5);
}


bool RAS_IPolyMaterial::Equals(const RAS_IPolyMaterial& lhs) const
{
	return (
			this->m_texturename	==		lhs.m_texturename &&
			this->m_tile		==		lhs.m_tile &&
			this->m_tilexrep	==		lhs.m_tilexrep &&
			this->m_tileyrep	==		lhs.m_tileyrep &&
			this->m_transparant	==		lhs.m_transparant &&
			this->m_drawingmode	==		lhs.m_drawingmode &&
			this->m_bIsTriangle	==		lhs.m_bIsTriangle &&
			this->m_lightlayer  ==		lhs.m_lightlayer &&
			this->m_materialname ==		lhs.m_materialname
	);
}



int RAS_IPolyMaterial::GetLightLayer()
{
	return m_lightlayer;
}



bool RAS_IPolyMaterial::IsTransparant()
{
	return (m_transparant != 0);
}



bool RAS_IPolyMaterial::UsesTriangles()
{
	return m_bIsTriangle;
}



unsigned int RAS_IPolyMaterial::hash() const
{
	return m_texturename.hash();
}



int RAS_IPolyMaterial::GetDrawingMode()
{
	return m_drawingmode;
}



const STR_String& RAS_IPolyMaterial::GetMaterialName() const
{ 
	return m_materialname;
}


const STR_String& RAS_IPolyMaterial::GetTextureName() const
{
	return m_texturename;
}

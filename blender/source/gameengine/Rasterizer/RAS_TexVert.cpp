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

#include "RAS_TexVert.h"


RAS_TexVert::RAS_TexVert(const MT_Point3& xyz,
						 const MT_Point2& uv,
						 const unsigned int rgba,
						 const short *normal,
						 const short flag) 
{
	xyz.getValue(m_localxyz);
	uv.getValue(m_uv1);
	SetRGBA(rgba);
	m_normal[0] = normal[0];
	m_normal[1] = normal[1];
	m_normal[2] = normal[2];
	m_flag = flag;
}



const float* RAS_TexVert::getUV1 () const
{
	return m_uv1;
}



const MT_Point3& RAS_TexVert::xyz()
{
	g_pt3.setValue(m_localxyz);
	return g_pt3;
}

void RAS_TexVert::SetXYZ(const MT_Point3& xyz)
{
	xyz.getValue(m_localxyz);
}



void RAS_TexVert::SetUV(const MT_Point2& uv)
{
	uv.getValue(m_uv1);
}



void RAS_TexVert::SetRGBA(const unsigned int rgba)
{ 
	m_rgba = rgba;
}


void RAS_TexVert::SetFlag(const short flag)
{
	m_flag = flag;
}
void RAS_TexVert::SetNormal(const MT_Vector3& normal)
{
	m_normal[0] = short(normal.x()*32767.0);
	m_normal[1] = short(normal.y()*32767.0);
	m_normal[2] = short(normal.z()*32767.0);
}



// leave multiline for debugging
const short* RAS_TexVert::getNormal() const
{
	return m_normal;
}



const float* RAS_TexVert::getLocalXYZ() const
{ 
	return m_localxyz;
}
	


const unsigned int& RAS_TexVert::getRGBA() const
{
	return m_rgba;
}



// compare two vertices, and return TRUE if both are almost identical (they can be shared)
bool RAS_TexVert::closeTo(const RAS_TexVert* other)
{
	return ((MT_Vector3(m_localxyz) - MT_Vector3(other->m_localxyz)).fuzzyZero() &&
		(MT_Vector2(m_uv1) - MT_Vector2(other->m_uv1)).fuzzyZero() &&
		m_normal[0] == other->m_normal[0] &&
		m_normal[1] == other->m_normal[1] &&
		m_normal[2] == other->m_normal[2] &&
		m_flag == other->m_flag &&
		m_rgba == other->m_rgba) ;
	
}



bool RAS_TexVert::closeTo(const MT_Point3& otherxyz,
			 const MT_Point2& otheruv,
			 const unsigned int otherrgba,
			 short othernormal[3]) const
{
	return ((MT_Vector3(m_localxyz) - otherxyz).fuzzyZero() &&
		(MT_Vector2(m_uv1) - otheruv).fuzzyZero() &&
		m_normal[0] == othernormal[0] &&
		m_normal[1] == othernormal[1] &&
		m_normal[2] == othernormal[2] &&
		m_rgba == otherrgba) ;
}


short RAS_TexVert::getFlag() const
{
	return m_flag;
}

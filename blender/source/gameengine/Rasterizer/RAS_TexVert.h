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
#ifndef __RAS_TEXVERT
#define __RAS_TEXVERT


#include "MT_Point3.h"
#include "MT_Point2.h"
#include "MT_Transform.h"


static MT_Point3 g_pt3;
static MT_Point2 g_pt2;

#define TV_CALCFACENORMAL 0x0001

class RAS_TexVert
{
	
	float			m_localxyz[3];	// 3*4=12 = 24
	float			m_uv1[2];		// 2*4=8 = 24 + 16 = 40
	unsigned int	m_rgba;			//4 = 40 + 4 = 44
	short			m_normal[3];	//3*2=6 = 50 
	short			m_flag;			//32 bytes total size, fits nice = 52 = not fit nice


public:
	short getFlag() const;
	RAS_TexVert()// :m_xyz(0,0,0),m_uv(0,0),m_rgba(0)
	{}
	RAS_TexVert(const MT_Point3& xyz,
				const MT_Point2& uv,
				const unsigned int rgba,
				const short *normal,
				const short flag);
	~RAS_TexVert() {};

	// leave multiline for debugging
	const float* getUV1 () const;

	//const float* getUV1 () const { 
	//	return m_uv1;
	//};

	const MT_Point3&	xyz();

	void				SetXYZ(const MT_Point3& xyz);
	void				SetUV(const MT_Point2& uv);
	void				SetRGBA(const unsigned int rgba);
	void				SetNormal(const MT_Vector3& normal);
	void				SetFlag(const short flag);
	// leave multiline for debugging
	const short*		getNormal() const;
	//const float* getLocalXYZ() const { 
	//	return m_localxyz;
	//};

	const float*		getLocalXYZ() const;
	const unsigned int&	getRGBA() const;
	// compare two vertices, and return TRUE if both are almost identical (they can be shared)
	bool				closeTo(const RAS_TexVert* other);

	bool				closeTo(const MT_Point3& otherxyz,
								const MT_Point2& otheruv,
								const unsigned int otherrgba,
								short othernormal[3]) const;
};

#endif //__RAS_TEXVERT

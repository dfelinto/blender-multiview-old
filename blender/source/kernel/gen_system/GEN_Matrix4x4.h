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
#ifndef GEN_MATRIX4X4
#define GEN_MATRIX4X4

#include "MT_Point3.h"

class GEN_Matrix4x4
{
public:
	// creators.
	GEN_Matrix4x4();
	GEN_Matrix4x4(const float value[4][4]);
	GEN_Matrix4x4(const double value[16]);
	GEN_Matrix4x4(const GEN_Matrix4x4 & other);
	GEN_Matrix4x4(const MT_Point3& orig,
				  const MT_Vector3& dir,
				  const MT_Vector3 up);

	void Identity();
	void SetMatrix(const GEN_Matrix4x4 & other);
	double*	getPointer();
	const double* getPointer() const;
	void setElem(int pos,double newvalue);

	
	MT_Vector3	GetRight() const;
	MT_Vector3	GetUp() const;
	MT_Vector3	GetDir() const;
	MT_Point3	GetPos() const;
	void		SetPos(const MT_Vector3 & v);

	double& operator () (int row,int col)	{ return m_V[col][row]; }
	
	static GEN_Matrix4x4 Perspective(MT_Scalar inLeft,
									 MT_Scalar inRight,
									 MT_Scalar inBottom,
									 MT_Scalar inTop,
									 MT_Scalar inNear,
									 MT_Scalar inFar);
protected:
	union
	{
		double m_V[4][4];
		double m_Vflat[16];
	};
};

#endif //GEN_MATRIX4X4

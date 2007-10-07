#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "ri.h"
#include "qdVector.h"
// for mem...()
#include <cstring>

#include "QDRender.h"
__BEGIN_QDRENDER

class Transform
{
public:
	// ctors
	Transform() { identity(); }
	Transform(RtMatrix T)
	{
		m[0][0]=T[0][0];  m[0][1]=T[0][1];  m[0][2]=T[0][2];  m[0][3]=T[0][3];
		m[1][0]=T[1][0];  m[1][1]=T[1][1];  m[1][2]=T[1][2];  m[1][3]=T[1][3];
		m[2][0]=T[2][0];  m[2][1]=T[2][1];  m[2][2]=T[2][2];  m[2][3]=T[2][3];
		m[3][0]=T[3][0];  m[3][1]=T[3][1];  m[3][2]=T[3][2];  m[3][3]=T[3][3];
	}
	Transform(float m00, float m01, float m02, float m03,
						float m10, float m11, float m12, float m13,
						float m20, float m21, float m22, float m23,
						float m30, float m31, float m32, float m33)
	{
		m[0][0]=m00;  m[0][1]=m01;  m[0][2]=m02;  m[0][3]=m03;
		m[1][0]=m10;  m[1][1]=m11;  m[1][2]=m12;  m[1][3]=m13;
		m[2][0]=m20;  m[2][1]=m21;  m[2][2]=m22;  m[2][3]=m23;
		m[3][0]=m30;  m[3][1]=m31;  m[3][2]=m32;  m[3][3]=m33;
	}
	Transform(const Transform &m2)
	{
		memcpy(&m[0][0], &(m2.m[0][0]), sizeof(RtMatrix));
	}
	// dtor
	~Transform();
	// assignment op
	Transform& operator=(const Transform &m2)
	{
		memcpy(&m[0][0], &(m2.m[0][0]), sizeof(RtMatrix));
		return *this;
	}
	// mtds
	// set to identity
	void identity()
	{
		memset(&m[0][0], 0, sizeof(RtMatrix));
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
	}
	// transpose the matrix
	void transpose()
	{
		Transform m2(m);
		for (int j=0; j<4; ++j)
			for (int i=0; i<4; ++i)
				m[i][j] = m2[j][i];
	}
	// multiply matrices
	Transform operator*(const Transform& m2) const
	{
		return Transform(
		m[0][0]*m2.m[0][0] + m[0][1]*m2.m[1][0] + m[0][2]*m2.m[2][0] + m[0][3]*m2.m[3][0],
		m[0][0]*m2.m[0][1] + m[0][1]*m2.m[1][1] + m[0][2]*m2.m[2][1] + m[0][3]*m2.m[3][1],
		m[0][0]*m2.m[0][2] + m[0][1]*m2.m[1][2] + m[0][2]*m2.m[2][2] + m[0][3]*m2.m[3][2],
		m[0][0]*m2.m[0][3] + m[0][1]*m2.m[1][3] + m[0][2]*m2.m[2][3] + m[0][3]*m2.m[3][3],

		m[1][0]*m2.m[0][0] + m[1][1]*m2.m[1][0] + m[1][2]*m2.m[2][0] + m[1][3]*m2.m[3][0],
		m[1][0]*m2.m[0][1] + m[1][1]*m2.m[1][1] + m[1][2]*m2.m[2][1] + m[1][3]*m2.m[3][1],
		m[1][0]*m2.m[0][2] + m[1][1]*m2.m[1][2] + m[1][2]*m2.m[2][2] + m[1][3]*m2.m[3][2],
		m[1][0]*m2.m[0][3] + m[1][1]*m2.m[1][3] + m[1][2]*m2.m[2][3] + m[1][3]*m2.m[3][3],

		m[2][0]*m2.m[0][0] + m[2][1]*m2.m[1][0] + m[2][2]*m2.m[2][0] + m[2][3]*m2.m[3][0],
		m[2][0]*m2.m[0][1] + m[2][1]*m2.m[1][1] + m[2][2]*m2.m[2][1] + m[2][3]*m2.m[3][1],
		m[2][0]*m2.m[0][2] + m[2][1]*m2.m[1][2] + m[2][2]*m2.m[2][2] + m[2][3]*m2.m[3][2],
		m[2][0]*m2.m[0][3] + m[2][1]*m2.m[1][3] + m[2][2]*m2.m[2][3] + m[2][3]*m2.m[3][3],

		m[3][0]*m2.m[0][0] + m[3][1]*m2.m[1][0] + m[3][2]*m2.m[2][0] + m[3][3]*m2.m[3][0],
		m[3][0]*m2.m[0][1] + m[3][1]*m2.m[1][1] + m[3][2]*m2.m[2][1] + m[3][3]*m2.m[3][1],
		m[3][0]*m2.m[0][2] + m[3][1]*m2.m[1][2] + m[3][2]*m2.m[2][2] + m[3][3]*m2.m[3][2],
		m[3][0]*m2.m[0][3] + m[3][1]*m2.m[1][3] + m[3][2]*m2.m[2][3] + m[3][3]*m2.m[3][3]);
	}
	Transform& operator*=(const Transform& m2)
	{
		Transform tm(
		m[0][0]*m2.m[0][0] + m[0][1]*m2.m[1][0] + m[0][2]*m2.m[2][0] + m[0][3]*m2.m[3][0],
		m[0][0]*m2.m[0][1] + m[0][1]*m2.m[1][1] + m[0][2]*m2.m[2][1] + m[0][3]*m2.m[3][1],
		m[0][0]*m2.m[0][2] + m[0][1]*m2.m[1][2] + m[0][2]*m2.m[2][2] + m[0][3]*m2.m[3][2],
		m[0][0]*m2.m[0][3] + m[0][1]*m2.m[1][3] + m[0][2]*m2.m[2][3] + m[0][3]*m2.m[3][3],

		m[1][0]*m2.m[0][0] + m[1][1]*m2.m[1][0] + m[1][2]*m2.m[2][0] + m[1][3]*m2.m[3][0],
		m[1][0]*m2.m[0][1] + m[1][1]*m2.m[1][1] + m[1][2]*m2.m[2][1] + m[1][3]*m2.m[3][1],
		m[1][0]*m2.m[0][2] + m[1][1]*m2.m[1][2] + m[1][2]*m2.m[2][2] + m[1][3]*m2.m[3][2],
		m[1][0]*m2.m[0][3] + m[1][1]*m2.m[1][3] + m[1][2]*m2.m[2][3] + m[1][3]*m2.m[3][3],

		m[2][0]*m2.m[0][0] + m[2][1]*m2.m[1][0] + m[2][2]*m2.m[2][0] + m[2][3]*m2.m[3][0],
		m[2][0]*m2.m[0][1] + m[2][1]*m2.m[1][1] + m[2][2]*m2.m[2][1] + m[2][3]*m2.m[3][1],
		m[2][0]*m2.m[0][2] + m[2][1]*m2.m[1][2] + m[2][2]*m2.m[2][2] + m[2][3]*m2.m[3][2],
		m[2][0]*m2.m[0][3] + m[2][1]*m2.m[1][3] + m[2][2]*m2.m[2][3] + m[2][3]*m2.m[3][3],

		m[3][0]*m2.m[0][0] + m[3][1]*m2.m[1][0] + m[3][2]*m2.m[2][0] + m[3][3]*m2.m[3][0],
		m[3][0]*m2.m[0][1] + m[3][1]*m2.m[1][1] + m[3][2]*m2.m[2][1] + m[3][3]*m2.m[3][1],
		m[3][0]*m2.m[0][2] + m[3][1]*m2.m[1][2] + m[3][2]*m2.m[2][2] + m[3][3]*m2.m[3][2],
		m[3][0]*m2.m[0][3] + m[3][1]*m2.m[1][3] + m[3][2]*m2.m[2][3] + m[3][3]*m2.m[3][3]);
		*this = tm;
		return *this;
	}
	// concatenate a perspective matrix
	void perspective(float fov, float near_clip, float far_clip);
	// concatenate a translation matrix
	void translate(float dx, float dy, float dz);
	// concatenate a rotation matrix, from angle/axis
	void rotate(float angle, float dx, float dy, float dz);
	// concatenate a scaling matrix
	void scale(float dx, float dy, float dz);
	// concatenate a skew matrix
	void skew(float angle, float dx1, float dy1, float dz1,
												float dx2, float dy2, float dz2);
	//  invert matrix
	Transform& invert();

	// point mult. (w = 1)
	Point3 operator*(const Point3& p) const
	{
		const Point3 pp(m[0][0]*p.x + m[0][1]*p.y + m[0][2]*p.z + m[0][3],
		                m[1][0]*p.x + m[1][1]*p.y + m[1][2]*p.z + m[1][3],
		                m[2][0]*p.x + m[2][1]*p.y + m[2][2]*p.z + m[2][3]);
		const float w = m[3][0]*p.x + m[3][1]*p.y + m[3][2]*p.z + m[3][3];
		if (w == 1.f) return pp;
		return pp/w;
	}
	// vector mult. (w = 0)
	Vector operator*(const Vector& v) const
	{
		return Vector(m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z,
									m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z,
									m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z);
	}
	// Homogenous point mult.
	HPoint operator*(const HPoint& h) const
	{
		return HPoint(m[0][0]*h.x + m[0][1]*h.y + m[0][2]*h.z + m[0][3]*h.w,
									m[1][0]*h.x + m[1][1]*h.y + m[1][2]*h.z + m[1][3]*h.w,
									m[2][0]*h.x + m[2][1]*h.y + m[2][2]*h.z + m[2][3]*h.w,
									m[3][0]*h.x + m[3][1]*h.y + m[3][2]*h.z + m[3][3]*h.w);
	}

	float determinant() const
	{
		return ((m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])) -
						(m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])) +
						(m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0])));
	}

	// returns true if matrix righthanded (negative determinant)
	bool handedness() const
	{
		return (((m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])) -
						 (m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])) +
						 (m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]))) < 0.f);
	}

	// returns the individual axis scale factors from the matrix
	Vector getScaleFactors() const
	{
		return Vector(sqrtf(m[0][0]*m[0][0] + m[0][1]*m[0][1] + m[0][2]*m[0][2]),
		              sqrtf(m[1][0]*m[1][0] + m[1][1]*m[1][1] + m[1][2]*m[1][2]),
		              sqrtf(m[2][0]*m[2][0] + m[2][1]*m[2][1] + m[2][2]*m[2][2]));
	}

	void print() const;

	// array access
	const float* operator[](int i) const { return m[i]; }
	float* operator[](int i) { return m[i]; }

	// used by the C based functions
	const RtMatrix* getRtMatrixPtr() const { return &m; }

protected:
	RtMatrix m;
};

Transform Scale(float x, float y, float z);
Transform Translate(float x, float y, float z);
Transform Rotate(float angle, float dx, float dy, float dz);
Transform Skew(float angle, float dx1, float dy1, float dz1,
														float dx2, float dy2, float dz2);
Transform LookAt(const Point3 &pos, const Point3 &look, const Vector &up);
Transform Perspective(float fov, float n, float f);

__END_QDRENDER

#endif // TRANSFORM_H

#include "Transform.h"
#include <cmath>
#include <iostream>

__BEGIN_QDRENDER

//------------------------------------------------------------------------------

// Scaling matrix
Transform Scale(float x, float y, float z)
{
	return Transform(x,0,0,0, 0,y,0,0, 0,0,z,0, 0,0,0,1);
}

// Translation matrix
Transform Translate(float x, float y, float z)
{
	return Transform(1,0,0,x, 0,1,0,y, 0,0,1,z, 0,0,0,1);
}

// just here to normalize vec from 3 float args
inline void normalize(float &dx, float &dy, float &dz)
{
	float d = dx*dx + dy*dy + dz*dz;
	if (d>0.f) {
		d = 1.f/sqrtf(d);
		dx *= d, dy *= d, dz *= d;
	}
}

// Rotation matrix from axis & angle
Transform Rotate(float angle, float dx, float dy, float dz)
{
	angle *= (float)M_PI/180.f;
	const float s = sinf(angle), c = cosf(angle);
	const float t = 1.f - c;
	normalize(dx, dy, dz);
	return Transform(dx*dx*t + c,    dx*dy*t - s*dz, dx*dz*t + s*dy, 0.f,
	                 dy*dx*t + s*dz, dy*dy*t + c,    dy*dz*t - s*dx, 0.f,
	                 dz*dx*t - s*dy, dz*dy*t + s*dx, dz*dz*t + c,    0.f,
	                 0.f,            0.f,            0.f,            1.f);
}

// Skew matrix
Transform Skew(float angle, float dx1, float dy1, float dz1,
														float dx2, float dy2, float dz2)
{
	float th = angle*(float)M_PI/180.f;
	normalize(dx1, dy1, dz1);
	normalize(dx2, dy2, dz2);
	float maxangle = acosf(dx1*dx2 + dy1*dy2 + dz1*dz2);
	float s = sinf(maxangle - th);
	return Transform(1.f + s*dx1*dx2,       s*dy1*dx2,       s*dz1*dx2, 0.f,
	                       s*dx1*dy2, 1.f + s*dy1*dy2,       s*dz1*dy2, 0.f,
	                       s*dx1*dz2,       s*dy1*dz2, 1.f + s*dz1*dz2, 0.f,
	                             0.f,             0.f,             0.f, 1.f);
}

Transform LookAt(const Point3 &pos, const Point3 &look, const Vector &up)
{
	const Vector dir = (look - pos).normalize();
	const Vector nup = Vector(up).normalize();
	const Vector right = (dir VCROSS nup).normalize();
	const Vector newup = right VCROSS dir;
	Transform m(right.x, newup.x, dir.x, pos.x,
	            right.y, newup.y, dir.y, pos.y,
	            right.z, newup.z, dir.z, pos.z,
	            0.f,     0.f,     0.f,   1.f);
	m.invert();
	return m;
}

Transform Perspective(float fov, float n, float f)
{
	float inv_denom = 1.f/(f - n);
	Transform persp(1, 0,       0,          0,
	                0, 1,       0,          0,
	                0, 0, f*inv_denom, -f*n*inv_denom,
	                0, 0,       1,          0);
	float invTanAng = 1.f / tanf(0.5f*fov*(float)M_PI/180.f);
	return Scale(invTanAng, invTanAng, 1) * persp;
}

//------------------------------------------------------------------------------
// Transform class functions

Transform::~Transform()
{
	// nothing to do
}

Transform& Transform::invert()
{
	int indxc[4], indxr[4], ipiv[4];
	Transform minv = *this;

	ipiv[0] = ipiv[1] = ipiv[2] = ipiv[3] = 0;
	for (int i = 0; i < 4; i++) {
		int irow = 0, icol = 0;
		float big = 0.f;
		for (int j = 0; j < 4; j++) {
			if (ipiv[j] != 1) {
				for (int k = 0; k < 4; k++) {
					if (ipiv[k] == 0) {
						if (fabs(minv.m[j][k]) >= big) {
							big = float(fabs(minv.m[j][k]));
							irow = j;
							icol = k;
						}
					}
					else if (ipiv[k] > 1) return *this; // not invertable
				}
			}
		}
		++ipiv[icol];
		if (irow != icol) {
			for (int k = 0; k < 4; ++k) {
				const float tt = minv.m[irow][k];
				minv.m[irow][k] = minv.m[icol][k];
				minv.m[icol][k] = tt;
			}
		}
		indxr[i] = irow;
		indxc[i] = icol;
		if (minv.m[icol][icol] == 0.) return *this; // not invertable
		const float pivinv = 1.f / minv.m[icol][icol];
		minv.m[icol][icol] = 1.f;
		for (int j = 0; j < 4; j++)
			minv.m[icol][j] *= pivinv;
		for (int j = 0; j < 4; j++) {
			if (j != icol) {
				const float save = minv.m[j][icol];
				minv.m[j][icol] = 0;
				for (int k = 0; k < 4; k++)
					minv.m[j][k] -= minv.m[icol][k]*save;
			}
		}
	}
	for (int j = 3; j >= 0; j--) {
		if (indxr[j] != indxc[j]) {
			for (int k = 0; k < 4; k++) {
				const float tt = minv.m[k][indxr[j]];
				minv.m[k][indxr[j]] = minv.m[k][indxc[j]];
				minv.m[k][indxc[j]] = tt;
			}
		}
	}
	*this = minv;
	return *this;
}

void Transform::print() const
{
	for (int j=0; j<4; ++j)
		printf("[%g %g %g %g]\n", m[j][0], m[j][1], m[j][2], m[j][3]);
}

//------------------------------------------------------------------------------

void Transform::perspective(float angle, float near_clip, float far_clip)
{
	*this *= Perspective(angle, near_clip, far_clip);
}

void Transform::translate(float dx, float dy, float dz)
{
	*this *= Translate(dx, dy, dz);
}


void Transform::rotate(float angle, float dx, float dy, float dz)
{
	*this *= Rotate(angle, dx, dy, dz);
}

void Transform::scale(float dx, float dy, float dz)
{
	*this *= Scale(dx, dy, dz);
}

void Transform::skew(float angle, float dx1, float dy1, float dz1,
                                  float dx2, float dy2, float dz2)
{
	*this *= Skew(angle, dx1, dy1, dz1, dx2, dy2, dz2);
}

__END_QDRENDER


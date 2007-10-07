// Various math & other functions
#ifndef _MATHUTIL_H
#define _MATHUTIL_H

#include "qdVector.h"
#include "Bound.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <cassert>
// for mem...()
#include <cstring>
#include "ri.h"

#include "QDRender.h"
__BEGIN_QDRENDER

//------------------------------------------------------------------------------
// various template functions

// returns absolute value of x
template<typename T>
inline T ABS(T x) { return (x < (T)0) ? -x : x; }

// to be used with points/vectors, mostly to test ccw or cw order
// returns twice the (signed) area of triangle defined by vertices (a, b, c)
template<typename T>
inline float signedArea2X(const T &a, const T &b, const T &c)
{
	return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
}

// 2D Signed Triangle Area
template<typename T>
inline float tri_area(const T &a, const T &b, const T &c)
{
	return 0.5f*((b.x - a.x)*(c.y - a.y) - (c.x - a.x)*(b.y - a.y));
}

// 2D Signed Quad Area
template <typename T>
inline float quad_area(const T &a, const T &b, const T &c, const T &d)
{
	return 0.5f*((c.x - a.x)*(d.y - b.y) - (d.x - b.x)*(c.y - a.y));
}

// Intersection test of two 2D line segments, returns intersect time along first segment, -1 if no intersection
template<typename T>
inline float segInt(const T &s1, const T &e1, const T &s2, const T &e2)
{
	const float dx11 = e1.x - s1.x, dx22 = e2.x - s2.x;
	const float dy11 = e1.y - s1.y, dy22 = e2.y - s2.y;
	const float d = dy22*dx11 - dx22*dy11;
	if (d == 0.f) return -1.f;
	const float dx12 = s1.x - s2.x, dy12 = s1.y - s2.y;
	const float a = dx22*dy12 - dy22*dx12, b = dx11*dy12 - dy11*dx12;
	const float u1 = a / d;
	if ((u1 < 0.f) || (u1 > 1.f)) return -1.f;
	const float u2 = b / d;
	return ((u2 < 0.f) || (u2 > 1.f)) ? -1.f : u1;
}

// Area of completely general quad
template<typename T>
inline float gQuad_area(const T &a, const T &b, const T &c, const T &d)
{
	// test if any two opposite segments intersect,
	// if so, return sum of area of the two triangles connected at intersection point,
	// otherwise, do direct area computation which also works for concave quads.
	float u = segInt(a, b, c, d);
	if (u != -1.f) {
		const T ip = a + u*(b - a);
		return ABS(tri_area(a, ip, d)) + ABS(tri_area(b, ip, c));
	}
	u = segInt(a, d, b, c);
	if (u != -1.f) {
		const T ip = a + u*(d - a);
		return ABS(tri_area(a, b, ip)) + ABS(tri_area(c, d, ip));
	}
	return ABS(quad_area(a, b, c, d));
}

// returns minimum of two elements
template<typename T>
inline T MIN2(T a, T b)
{
	return (a < b) ? a : b;
}

// returns maximum of two elements
template<typename T>
inline T MAX2(T a, T b)
{
	return (a > b) ? a : b;
}

// returns minimum of four elements
template<typename T>
inline T MIN4(T a, T b, T c, T d)
{
	return MIN2(a, MIN2(b, MIN2(c, d)));
}

// returns maximum of four elements
template<typename T>
inline T MAX4(T a, T b, T c, T d)
{
	return MAX2(a, MAX2(b, MAX2(c, d)));
}

// swaps contents of two objects
template<typename T>
inline void SWAP(T &a, T &b)
{
	T t = a;
	a = b;
	b = t;
}

// clamp value to zero if negative
template<typename T>
inline T CLAMP0(T v)
{
	return (v < (T)0) ? ((T)0) : v;
}

// clamp value in range [min, max]
template<typename T>
inline T CLAMP(T a, T min, T max)
{
	return ((a < min) ? min : ((a > max) ? max : a));
}

// returns sign of x
template<typename T>
inline T SIGN(T x) { return (x==(T)0) ? 0 : ((x<(T)0) ? (T)-1 : (T)1); }

//----------------------------------------------------------------------------------
// General simple C based math routines mainly used by the shader virtual machine,
// most vector functions are also used for points/normals and colors.
// For the cases where the difference matters, explicit routines exist,
// eg. matrix vector & point multiply .etc.
// General format is func(result, var1, var2, ...)
// For all functions where operands are the same type as the result type,
// the result variable can be the same as either of the operands, eg. cross(v1, v1, v2)
//----------------------------------------------------------------------------------
// vectors
inline RtVoid addVVV(RtVector r, const RtVector v1, const RtVector v2)
{
	r[0] = v1[0] + v2[0];
	r[1] = v1[1] + v2[1];
	r[2] = v1[2] + v2[2];
}

inline RtVoid subVVV(RtVector r, const RtVector v1, const RtVector v2)
{
	r[0] = v1[0] - v2[0];
	r[1] = v1[1] - v2[1];
	r[2] = v1[2] - v2[2];
}

inline RtVoid mulVVV(RtVector r, const RtVector v1, const RtVector v2)
{
	r[0] = v1[0] * v2[0];
	r[1] = v1[1] * v2[1];
	r[2] = v1[2] * v2[2];
}

inline RtVoid maddVVV(RtVector r, const RtVector v1, const RtVector v2)
{
	r[0] += v1[0] * v2[0];
	r[1] += v1[1] * v2[1];
	r[2] += v1[2] * v2[2];
}

inline RtVoid msubVVV(RtVector r, const RtVector v1, const RtVector v2)
{
	r[0] -= v1[0] * v2[0];
	r[1] -= v1[1] * v2[1];
	r[2] -= v1[2] * v2[2];
}

inline RtVoid divVVV(RtVector r, const RtVector v1, const RtVector v2)
{
	if (v2[0] != 0.f) r[0] = v1[0] / v2[0];
	if (v2[1] != 0.f) r[1] = v1[1] / v2[1];
	if (v2[2] != 0.f) r[2] = v1[2] / v2[2];
}

inline RtVoid mulVVF(RtVector r, const RtVector v, const RtFloat f)
{
	r[0] = v[0] * f;
	r[1] = v[1] * f;
	r[2] = v[2] * f;
}

inline RtVoid maddVVF(RtVector r, const RtVector v, const RtFloat f)
{
	r[0] += v[0] * f;
	r[1] += v[1] * f;
	r[2] += v[2] * f;
}

inline RtVoid msubVVF(RtVector r, const RtVector v, const RtFloat f)
{
	r[0] -= v[0] * f;
	r[1] -= v[1] * f;
	r[2] -= v[2] * f;
}

inline RtVoid divVVF(RtVector r, const RtVector v, RtFloat f)
{
	if (f != 0.f) {
		f = 1.f/f;
		r[0] = v[0] * f;
		r[1] = v[1] * f;
		r[2] = v[2] * f;
	}
}

inline RtVoid mulVV(RtVector r, const RtVector v)
{
	r[0] *= v[0];
	r[1] *= v[1];
	r[2] *= v[2];
}

inline RtVoid vcross(RtVector r, const RtVector v1, const RtVector v2)
{
	const RtFloat x = v1[1]*v2[2] - v1[2]*v2[1];
	const RtFloat y = v1[2]*v2[0] - v1[0]*v2[2];
	r[2] = v1[0]*v2[1] - v1[1]*v2[0];
	r[0] = x;
	r[1] = y;
}

inline RtVoid vdot(RtFloat &r, const RtVector v1, const RtVector v2)
{
	r = v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

inline RtVoid vnormalize(RtVector r, const RtVector v)
{
	RtFloat d = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	if (d != 0.f) {
		d = 1.f / sqrtf(d);
		r[0] = v[0]*d;
		r[1] = v[1]*d;
		r[2] = v[2]*d;
	}
}

// sets the length of v to L
inline RtVoid vsetlength(RtVector r, const RtVector v, float L)
{
	RtFloat d = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	if (d != 0.f) {
		d = L / sqrtf(d);
		r[0] = v[0]*d;
		r[1] = v[1]*d;
		r[2] = v[2]*d;
	}
}

inline RtVoid vlength(RtFloat &r, const RtVector v)
{
	r = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

// step/smoothstep
inline RtVoid step(RtFloat &r, const RtFloat min, const RtFloat val)
{
	r = (val < min) ? 0.f : 1.f;
}

inline RtVoid smoothstep(RtFloat &r, const RtFloat min, const RtFloat max, RtFloat val)
{
	if (val < min)
		r = 0.f;
	else if (val > max)
		r = 1.f;
	else {
		val = (val - min) / (max - min);
		r = val*val*(3.f - 2.f*val);
	}
}

// faceforward
inline RtVoid faceforward(RtVector r, const RtVector v1, const RtVector v2, const RtVector v3)
{
	// assumes v1 = N, I = v2, v3 = Ng|Nref
	const RtFloat d = ((-v2[0]*v3[0] - v2[1]*v3[1] - v2[2]*v3[2]) < 0.f) ? -1.f : 1.f;
	r[0] = d*v1[0];
	r[1] = d*v1[1];
	r[2] = d*v1[2];
}

// float linear interpolation
inline RtVoid mixf(RtFloat &r, const RtFloat f0, const RtFloat f1, const RtFloat v)
{
	r = f0 + v*(f1 - f0);
}

// color/point/vector/normal linear interpolation
inline RtVoid mixv(RtVector r, const RtVector c0, const RtVector c1, const RtFloat v)
{
	r[0] = c0[0] + v*(c1[0] - c0[0]);
	r[1] = c0[1] + v*(c1[1] - c0[1]);
	r[2] = c0[2] + v*(c1[2] - c0[2]);
}

// conversion of degrees to radians
inline RtVoid radians(RtFloat &r, const RtFloat d)
{
	r = d * ((RtFloat)M_PI/180.f);
}

// conversion of radians to degrees
inline RtVoid degrees(RtFloat &r, const RtFloat d)
{
	r = d * (180.f/(RtFloat)M_PI);
}

// bilinear interpolation (other float[3] types can be used too, RtPoint/RtVector/etc)
inline void bilerp(RtColor r, RtFloat u, RtFloat v,
				const RtColor c00, const RtColor c10, const RtColor c01, const RtColor c11)
{
	const RtFloat w00=(1.f-u)*(1.f-v), w10=u*(1.f-v), w01=(1.f-u)*v, w11=u*v;
	r[0] = w00*c00[0] + w10*c10[0] + w01*c01[0] + w11*c11[0];
	r[1] = w00*c00[1] + w10*c10[1] + w01*c01[1] + w11*c11[1];
	r[2] = w00*c00[2] + w10*c10[2] + w01*c01[2] + w11*c11[2];
}

// bilinear interpolation of single floats
inline void bilerpF(RtFloat &r, RtFloat u, RtFloat v,
				RtFloat f00, RtFloat f10, RtFloat f01, RtFloat f11)
{
	r = (1.f-v)*((1.f-u)*f00 + u*f10) + v*((1.f-u)*f01 + u*f11);
}

// reflect vector
inline void reflect(RtVector r, const RtVector I, const RtVector N)
{
	const RtFloat ndi2 = 2.f*(N[0]*I[0] + N[1]*I[1] + N[2]*I[2]);
	r[0] = I[0] - ndi2*N[0];
	r[1] = I[1] - ndi2*N[1];
	r[2] = I[2] - ndi2*N[2];
}

// refract vector
inline void refract(RtVector r, const RtVector I, const RtVector N, const RtFloat eta)
{
	const RtFloat ndi = N[0]*I[0] + N[1]*I[1] + N[2]*I[2];
	const RtFloat k = 1.f - eta*eta*(1.f - ndi*ndi);
	if (k < 0.f)
		r[0] = r[1] = r[2] = 0.f;
	else {
		const RtFloat f = eta*ndi + sqrtf(k);
		r[0] = eta*I[0] - f*N[0];
		r[1] = eta*I[1] - f*N[1];
		r[2] = eta*I[2] - f*N[2];
	}
}

void fresnel(const RtVector I, const RtVector N, const float eta, float& Kr, float& Kt, RtVector R = NULL, RtVector T = NULL);

//------------------------------------------------------------------------------
// matrices

RtVoid addMMM(RtMatrix r, const RtMatrix m1, const RtMatrix m2);
RtVoid subMMM(RtMatrix r, const RtMatrix m1, const RtMatrix m2);
RtVoid mulMMM(RtMatrix r, const RtMatrix m1, const RtMatrix m2);
RtBoolean invertMatrix(RtMatrix minv, const RtMatrix m2);

inline RtVoid transposeM(RtMatrix r)
{
	SWAP(r[0][1], r[1][0]);
	SWAP(r[0][2], r[2][0]);
	SWAP(r[0][3], r[3][0]);
	SWAP(r[2][1], r[1][2]);
	SWAP(r[3][2], r[2][3]);
	SWAP(r[3][1], r[1][3]);
}

inline RtVoid divMMM(RtMatrix r, const RtMatrix m1, const RtMatrix m2)
{
	RtMatrix im2;
	assert(invertMatrix(im2, m2));
	mulMMM(r, m1, im2);
}

inline RtVoid mulVMV(RtVector r, const RtMatrix m, const RtVector v)
{
	const RtFloat x = v[0], y = v[1];
	r[0] = m[0][0]*x + m[0][1]*y + m[0][2]*v[2];
	r[1] = m[1][0]*x + m[1][1]*y + m[1][2]*v[2];
	r[2] = m[2][0]*x + m[2][1]*y + m[2][2]*v[2];
}

inline RtVoid mulNMN(RtNormal r, const RtMatrix m, const RtNormal n)
{
	// normal, multiply by inverse of transpose (could be optimized, don't need to recalculate everytime)
	RtMatrix m2;
	assert(invertMatrix(m2, m));
	const RtFloat x = n[0], y = n[1];
	r[0] = m2[0][0]*x + m2[1][0]*y + m2[2][0]*n[2];
	r[1] = m2[0][1]*x + m2[1][1]*y + m2[2][1]*n[2];
	r[2] = m2[0][2]*x + m2[1][2]*y + m2[2][2]*n[2];
}

inline RtVoid mulPMP(RtPoint r, const RtMatrix m, const RtPoint p)
{
	const RtFloat x = p[0], y = p[1], z = p[2];
	r[0] = m[0][0]*x + m[0][1]*y + m[0][2]*z + m[0][3];
	r[1] = m[1][0]*x + m[1][1]*y + m[1][2]*z + m[1][3];
	r[2] = m[2][0]*x + m[2][1]*y + m[2][2]*z + m[2][3];
	const RtFloat w = m[3][0]*x + m[3][1]*y + m[3][2]*z + m[3][3];
	if (w != 1.f) divVVF(r, r, w);
}

inline RtVoid mulPMP4(RtHpoint r, const RtMatrix m, const RtHpoint p)
{
	const RtFloat x = p[0], y = p[1], z = p[2];
	r[0] = m[0][0]*x + m[0][1]*y + m[0][2]*z + m[0][3]*p[3];
	r[1] = m[1][0]*x + m[1][1]*y + m[1][2]*z + m[1][3]*p[3];
	r[2] = m[2][0]*x + m[2][1]*y + m[2][2]*z + m[2][3]*p[3];
	r[3] = m[3][0]*x + m[3][1]*y + m[3][2]*z + m[3][3]*p[3];
}

//------------------------------------------------------------------------------
// Not sure what the purpose of the below floor/ceil/fmod functions was anymore,
// kind of superfluous, could just as well use the actual math funcs...

// returns highest integer <= x as float
inline float FLOORF(float x)
{
	const float r = float(int(x));
	return ((x >= 0.f) || (r == x)) ? r : (r - 1.f);
}

// returns lowest integer >= x as float
inline float CEILF(float x)
{
	const float r = float(int(x));
	return ((x <= 0.f) || (r == x)) ? r : (r + 1.f);
}

// returns highest integer <= x as integer
inline int FLOORI(float x)
{
	if (x >= 0.f) return int(x);
	const int r = int(x);
	return (float(r) == x) ? r : (r - 1);
}

// returns lowest integer >= x as integer
inline int CEILI(float x)
{
	if (x <= 0.f) return int(x);
	const int r = int(x);
	return (float(r) == x) ? r : (r + 1);
}

// returns float x modulo y, return value is in range (0, y)
inline float FMOD(float x, float y)
{
	float z = x / y;
	return y*(z - FLOORF(z));
}

// returns integer log2 of x (== highest set bit)
inline unsigned int ilog2(unsigned int x)
{
	unsigned int b = 0;
	while (x >>= 1) b++;
	return b;
}

// as above, but always rounds upwards
inline unsigned int ilog2_roundup(unsigned int x)
{
	unsigned int xx = x,  b = 0;
	while (xx >>= 1) b++;
	return b + (unsigned int)((unsigned int)(1 << b) < x);
}

// direct bicubic evaluation funcs
// direct Bezier eval. at value t
inline Point3 Bezier(float t, const Point3 &p0, const Point3 &p1, const Point3 &p2, const Point3 &p3)
{
	const float tm = 1.f - t;
	return (3.f*tm*p1 + 3.f*t*p2)*tm*t + tm*tm*tm*p0 + t*t*t*p3;
}

// derivative of Bezier
inline void BezierDeriv(float t, const RtPoint p0, const RtPoint p1, const RtPoint p2, const RtPoint p3, RtPoint bd)
{
	// for now the more complicated form here
	bd[0] = (-p0[0] + 3.f*p1[0] + (-3.f*p2[0]) + p3[0])*3.f*t*t + (3.f*p0[0] + (-6.f*p1[0]) + 3.f*p2[0])*2.f*t + (-3.f*p0[0] + 3.f*p1[0]);
	bd[1] = (-p0[1] + 3.f*p1[1] + (-3.f*p2[1]) + p3[1])*3.f*t*t + (3.f*p0[1] + (-6.f*p1[1]) + 3.f*p2[1])*2.f*t + (-3.f*p0[1] + 3.f*p1[1]);
	bd[2] = (-p0[2] + 3.f*p1[2] + (-3.f*p2[2]) + p3[2])*3.f*t*t + (3.f*p0[2] + (-6.f*p1[2]) + 3.f*p2[2])*2.f*t + (-3.f*p0[2] + 3.f*p1[2]);
}

// direct evaluation using Power basis
inline Point3 Power(float t, const Point3 &p0, const Point3 &p1, const Point3 &p2, const Point3 &p3)
{
	return ((p0*t + p1)*t + p2)*t + p3;
}

// direct bicubic curve eval. at value t, arbitrary basis b
inline void SplineF(RtFloat& r, const float t, const RtBasis &b, const RtFloat &f0, const RtFloat &f1, const RtFloat &f2, const RtFloat &f3)
{
  r = (((b[0][0]*f0 + b[0][1]*f1 + b[0][2]*f2 + b[0][3]*f3) *t
      + (b[1][0]*f0 + b[1][1]*f1 + b[1][2]*f2 + b[1][3]*f3))*t
      + (b[2][0]*f0 + b[2][1]*f1 + b[2][2]*f2 + b[2][3]*f3))*t
      + (b[3][0]*f0 + b[3][1]*f1 + b[3][2]*f2 + b[3][3]*f3);
}

inline void SplineV(RtVector r, const float t, const RtBasis &b, const RtVector &v0, const RtVector &v1, const RtVector &v2, const RtVector &v3)
{
	SplineF(r[0], t, b, v0[0], v1[0], v2[0], v3[0]);
	SplineF(r[1], t, b, v0[1], v1[1], v2[1], v3[1]);
	SplineF(r[2], t, b, v0[2], v1[2], v2[2], v3[2]);
}

// Bezier eval. using de Casteljau method
inline void deCasteljau_P(float t, const RtPoint* cp, RtPoint P)
{
	// osculating plane -> tangent -> point
	float d0, d1, d2;
#define CAS(a, b, c, d, r) \
	d0 = a + t*(b - a);      \
	d1 = b + t*(c - b);      \
	d2 = c + t*(d - c);      \
	d0 += t*(d1 - d0);       \
	d1 += t*(d2 - d1);       \
	r = d0 + t*(d1 - d0);
	CAS(cp[0][0], cp[1][0], cp[2][0], cp[3][0], P[0])
	CAS(cp[0][1], cp[1][1], cp[2][1], cp[3][1], P[1])
	CAS(cp[0][2], cp[1][2], cp[2][2], cp[3][2], P[2])
#undef CAS
}

// as above, but also returns v tangent in dPdv
inline void deCasteljau_P_dPdv(float t, const RtPoint* cp, RtPoint P, RtVector dPdv)
{
	float d0, d1, d2;
#define CAS(a, b, c, d, r, rt) \
	d0 = a + t*(b - a);        \
	d1 = b + t*(c - b);        \
	d2 = c + t*(d - c);        \
	d0 += t*(d1 - d0);         \
	d1 += t*(d2 - d1);         \
	r = d0 + t*(rt = d1 - d0); \
	rt *= 3.f;
	CAS(cp[0][0], cp[1][0], cp[2][0], cp[3][0], P[0], dPdv[0])
	CAS(cp[0][1], cp[1][1], cp[2][1], cp[3][1], P[1], dPdv[1])
	CAS(cp[0][2], cp[1][2], cp[2][2], cp[3][2], P[2], dPdv[2])
#undef CAS
}

// as above, but only returns u tangent vector instead
inline void deCasteljau_dPdu(float t, const RtPoint* cp, RtVector dPdu)
{
	float d0, d1, d2;
#define CAS(a, b, c, d, r) \
	d0 = a + t*(b - a);       \
	d1 = b + t*(c - b);       \
	d2 = c + t*(d - c);       \
	d0 += t*(d1 - d0);        \
	d1 += t*(d2 - d1);        \
	r = 3.f*(d1 - d0);
	CAS(cp[0][0], cp[1][0], cp[2][0], cp[3][0], dPdu[0])
	CAS(cp[0][1], cp[1][1], cp[2][1], cp[3][1], dPdu[1])
	CAS(cp[0][2], cp[1][2], cp[2][2], cp[3][2], dPdu[2])
#undef CAS
}

// version of deCasteljau_P() which can used for c.pts not in array
inline void deCasteljau_P(float t, const RtPoint cp0, const RtPoint cp1,
                          const RtPoint cp2, const RtPoint cp3, RtPoint P)
{
	float d0, d1, d2;
#define CAS(a, b, c, d, r) \
	d0 = a + t*(b - a);      \
	d1 = b + t*(c - b);      \
	d2 = c + t*(d - c);      \
	d0 += t*(d1 - d0);       \
	d1 += t*(d2 - d1);       \
	r = d0 + t*(d1 - d0);
	CAS(cp0[0], cp1[0], cp2[0], cp3[0], P[0])
	CAS(cp0[1], cp1[1], cp2[1], cp3[1], P[1])
	CAS(cp0[2], cp1[2], cp2[2], cp3[2], P[2])
#undef CAS
}

__END_QDRENDER

#endif //_MATHUTIL_H

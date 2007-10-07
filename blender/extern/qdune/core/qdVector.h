#ifndef QDVECTOR_H
#define QDVECTOR_H

#define _USE_MATH_DEFINES
#include <cmath>

#include "QDRender.h"
__BEGIN_QDRENDER

#define VCROSS ^
#define VDOT *

class Vector
{
public:
	// ctors
	Vector():x(0), y(0), z(0) {}
	explicit Vector(float a):x(a), y(a), z(a) {}
	explicit Vector(float a, float b, float c):x(a), y(b), z(c) {}
	Vector(const Vector &v):x(v.x), y(v.y), z(v.z) {}
	// dtor
	~Vector() {}
	// mtds
	void set(float a) { x=y=z=a; }
	void set(float a, float b, float c) { x=a;  y=b;  z=c; }
	void set(const Vector &v) { x=v.x;  y=v.y;  z=v.z; }
	float length() const
	{
		float L = x*x + y*y + z*z;
		if (L>0) return sqrt(L);
		return 0;
	}
	float lengthSquared() const { return x*x + y*y + z*z; }


	// dotproduct, also as operator* below
	float operator|(const Vector &v) const { return x*v.x + y*v.y + z*v.z; }
	float dot(const Vector &v) const { return x*v.x + y*v.y + z*v.z; }

	// crossproduct
	Vector operator^(const Vector &v)  const { return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x); }
	Vector cross(const Vector &v)  const { return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x); }

	Vector& normalize()
	{
		float d = x*x + y*y + z*z;
		if (d>0) {
			d = 1.f/sqrt(d);
			x *= d, y *= d, z *= d;
		}
		return *this;
	}
	float normLen()
	{
		float d = x*x + y*y + z*z;
		if (d>0) {
			d = sqrtf(d);
			const float dv = 1.f/d;
			x *= dv, y *= dv, z *= dv;
		}
		return d;
	}
	float normLenSquared()
	{
		float d = x*x + y*y + z*z;
		if (d>0) {
			const float dv = 1.f/sqrtf(d);
			x *= dv, y *= dv, z *= dv;
		}
		return d;
	}
	Vector& setLength(float L)
	{
		float d = x*x + y*y + z*z;
		if (d>0) {
			d = L/sqrtf(d);
			x *= d, y *= d, z *= d;
		}
		return *this;
	}
	// basic arith.
	Vector operator+(const Vector &v) const { return Vector(x+v.x, y+v.y, z+v.z); }
	Vector operator-(const Vector &v) const { return Vector(x-v.x, y-v.y, z-v.z); }
	Vector operator-() const { return Vector(-x, -y, -z); }
	Vector operator*(float s) const { return Vector(x*s, y*s, z*s); }
	friend Vector operator*(float s, const Vector &v) { return Vector(v.x*s, v.y*s, v.z*s); }
	float operator*(const Vector &v) const { return x*v.x + y*v.y + z*v.z; }
	Vector operator/(float s) const { if (s!=0) s=1.f/s;  return Vector(x*s, y*s, z*s);  }
	Vector& operator+=(const Vector &v) { x+=v.x;  y+=v.y;  z+=v.z;  return *this; }
	Vector& operator-=(const Vector &v) { x-=v.x;  y-=v.y;  z-=v.z;  return *this; }
	Vector& operator*=(float s) { x*=s;  y*=s;  z*=s;  return *this; }
	Vector& operator/=(float s) { if (s!=0) s=1.f/s;   x*=s;  y*=s;  z*=s;  return *this; }
	Vector reflect(const Vector &n) const
	{
		const float k = 2.f * (x*n.x + y*n.y + z*n.z);
		return Vector(x - k*n.x, y - k*n.y, z - k*n.z);
	}

	// componentwise mult.
	Vector scale(const Vector &v) const { return Vector(v.x*x, v.y*y, v.z*z); }

	// access as array
	float operator[](int i) const { return xyz[i]; }
	float& operator[](int i) { return xyz[i]; }
	// data
	union {
		struct { float x, y, z; };
		float xyz[3];
	};
};

class Point3
{
public:
	Point3():x(0), y(0), z(0) {}
	explicit Point3(float a):x(a), y(a), z(a) {}
	explicit Point3(float a, float b, float c=0):x(a), y(b), z(c) {}
	Point3(const Point3 &p):x(p.x), y(p.y), z(p.z) {}
	Point3(const Vector &v):x(v.x), y(v.y), z(v.z) {}
	void set(float ix, float iy, float iz=0) { x=ix;  y=iy;  z=iz; }
	void set(const Point3 &p) { x=p.x;  y=p.y;  z=p.z; }
	float length() const;
	float lengthSquared() const;
	float distance(const Point3 &a, const Point3 &b);
	Point3 operator-() const { return Point3(-x, -y, -z); }
	Point3& operator=(const Vector &v) { x=v.x;  y=v.y;  z=v.z;  return *this; }
	Point3& operator=(const Point3 &s) { x=s.x;  y=s.y;  z=s.z;  return *this; }
	Point3& operator+=(const Point3 &s) { x+=s.x;  y+=s.y;  z+=s.z;  return *this;}
	Point3& operator+=(const Vector &v) { x+=v.x;  y+=v.y;  z+=v.z;  return *this;}
	Point3& operator-=(const Point3 &s) { x-=s.x;  y-=s.y;  z-=s.z;  return *this;}
	Point3& operator*=(float s) { x*=s;  y*=s;  z*=s;  return *this;}
	Point3& operator/=(float s) { if (s!=0.f) s=1.f/s; x*=s;  y*=s;  z*=s;  return *this;}
	~Point3() {};
	float operator[](int i) const { return xyz[i]; }
	float& operator[](int i) { return xyz[i]; }
	// data
	union {
		struct { float x, y, z; };
		float xyz[3];
	};
};


inline Point3 operator*(float f, const Point3 &p)
{
	return Point3(f*p.x, f*p.y, f*p.z);
}

inline Point3 operator*(const Point3 &p, float f)
{
	return Point3(p.x*f, p.y*f, p.z*f);
}

inline Point3 operator/(const Point3 &p, float f)
{
	if (f!=0.f) f=1.f/f;
	return Point3(p.x*f, p.y*f, p.z*f);
}

inline Vector operator-(const Point3 &a, const Point3 &b)
{
	return Vector(a.x-b.x, a.y-b.y, a.z-b.z);
}

inline Point3 operator-(const Point3 &a, const Vector &b)
{
	return Point3(a.x-b.x, a.y-b.y, a.z-b.z);
}

inline Point3 operator+(const Point3 &a, const Point3 &b)
{
	return Point3(a.x+b.x, a.y+b.y, a.z+b.z);
}

inline Point3 operator+(const Point3 &a, const Vector &b)
{
	return Point3(a.x+b.x, a.y+b.y, a.z+b.z);
}

inline Vector operator+(const Vector &a, const Point3 &b)
{
	return Vector(a.x+b.x, a.y+b.y, a.z+b.z);
}

inline float Point3::length() const
{
	return sqrt(x*x + y*y + z*z);
}

inline float Point3::lengthSquared() const
{
	return x*x + y*y + z*z;
}

inline float distance(const Point3 &a, const Point3 &b)
{
	const float i=a.x-b.x, j=a.y-b.y, k=a.z-b.z;
	return sqrt(i*i + j*j + k*k);
}

inline Vector toVector(const Point3 &p)
{
	return Vector(p.x, p.y, p.z);
}

class Point2
{
public:
	Point2():x(0), y(0) {}
	explicit Point2(float a):x(a), y(a) {}
	explicit Point2(float a, float b):x(a), y(b) {}
	Point2(const Point3 &p):x(p.x), y(p.y) {}
	Point2(const Vector &v):x(v.x), y(v.y) {}
	Point2(const Point2 &p):x(p.x), y(p.y) {}
	~Point2() {};
	void set(const Point2& p) { x = p.x;  y = p.y; }
	void set(float a, float b) { x = a;  y = b; }
	Point2 operator-() const { return Point2(-x, -y); }
	Point2& operator=(const Point2 &p) { x=p.x;  y=p.y;  return *this; }
	Point2& operator+=(const Point2 &p) { x+=p.x;  y+=p.y;  return *this; }
	Point2& operator+=(const Point3 &p) { x+=p.x;  y+=p.y;  return *this; }
	Point2& operator+=(const Vector &v) { x+=v.x;  y+=v.y;  return *this; }
	Point2& operator-=(const Point2 &p) { x-=p.x;  y-=p.y;  return *this; }
	Point2& operator-=(const Point3 &p) { x-=p.x;  y-=p.y;  return *this; }
	Point2& operator-=(const Vector &v) { x-=v.x;  y-=v.y;  return *this; }
	Point2& operator*=(float f) { x*=f;  y*=f;  return *this; }
	friend Point2 operator*(float f, const Point2 &p) { return Point2(p.x*f, p.y*f); }
	Point2& operator/=(float s) { if (s!=0.f) s=1.f/s; x*=s;  y*=s;  return *this;}
	float operator[](int i) { return xy[i]; }
	float operator[](int i) const { return xy[i]; }
	// data
	union {
		struct { float x, y; };
		float xy[2];
	};
};

inline Point2 operator+(const Point2 &a, const Point2 &b)
{
	return Point2(a.x + b.x, a.y + b.y);
}

inline Point2 operator-(const Point2 &a, const Point2 &b)
{
	return Point2(a.x - b.x, a.y - b.y);
}

// minimal homogeneous point class
class HPoint
{
public:
	HPoint():x(0), y(0), z(0), w(0) {}
	explicit HPoint(float a):x(a), y(a), z(a), w(a) {}
	explicit HPoint(float a, float b, float c, float d):x(a), y(b), z(c), w(d) {}
	HPoint(const Point3 &p):x(p.x), y(p.y), z(p.z), w(1) {}
	HPoint(const Vector &v):x(v.x), y(v.y), z(v.z), w(0) {}
	HPoint(const HPoint &p):x(p.x), y(p.y), z(p.z), w(p.w) {}
	~HPoint() {};
	void set(float a, float b, float c, float d) { x=a;  y=b;  z=c;  w=d; }
	HPoint operator-() const { return HPoint(-x, -y, -z, -w); }
	HPoint& operator=(const HPoint &p) { x=p.x;  y=p.y;  z=p.z;  w=p.w;  return *this; }
	HPoint& operator+=(const HPoint &p) { x+=p.x;  y+=p.y;  z+=p.z;  w+=p.w;  return *this; }
	HPoint& operator+=(const Point3 &p) { x+=p.x;  y+=p.y;  z+=p.z;  return *this; }
	HPoint& operator+=(const Vector &v) { x+=v.x;  y+=v.y;  z+=v.z;  return *this; }
	HPoint& operator-=(const HPoint &p) { x-=p.x;  y-=p.y;  z-=p.z;  w-=p.w;  return *this; }
	HPoint& operator-=(const Point3 &p) { x-=p.x;  y-=p.y;  z-=p.z;  return *this; }
	HPoint& operator-=(const Vector &v) { x-=v.x;  y-=v.y;  z-=v.z;  return *this; }
	HPoint& operator*=(float f) { x*=f;  y*=f;  z*=f;  w*=f;  return *this; }
	friend HPoint operator*(float f, const HPoint &p) { return HPoint(p.x*f, p.y*f, p.z*f, p.w*f); }
	float operator[](int i) { return xyzw[i]; }
	float operator[](int i) const { return xyzw[i]; }
	// data
	union {
		struct { float x, y, z, w; };
		float xyzw[4];
	};
};

// ray (+diff.)
struct ray_t
{
	ray_t() {}
	ray_t(const Point3& o, const Vector& d, const Vector& dx, const Vector& dy) : org(o), dir(d), dfx(dx), dfy(dy) {}
	Point3 org;
	Vector dir, dfx, dfy;
};

// simple LC random number generator, good enough...
extern unsigned int _randomseed;
inline unsigned int irand()
{
	_randomseed = 1664525L * _randomseed + 1013904223L;
	return _randomseed;
}
inline float frand() { return irand()*(1.f/4294967286.f); }
inline float frandn() { return irand()*(1.f/2147483648.f)-1.f; }

void createCS(const Vector &N, Vector &u, Vector &v);
Vector RandomVectorSphere();
void shirleyDisc(float r1, float r2, float &du, float &dv);

__END_QDRENDER

#endif // QDVECTOR_H

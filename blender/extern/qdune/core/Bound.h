#ifndef _BOUND_H
#define _BOUND_H

#include "qdVector.h"
#include "ri.h"

#include "QDRender.h"
__BEGIN_QDRENDER

class Transform;

class Bound
{
public:
	enum bbAxis {X_AXIS=0, Y_AXIS, Z_AXIS};
	// ctor
	Bound()
	{
		minmax[0].set(1e10f, 1e10f, 1e10f);
		minmax[1].set(-1e10f, -1e10f, -1e10f);
	}
	Bound(const Point3 &bmin, const Point3 &bmax) { set(bmin, bmax); }
	// dtor
	~Bound() {}
	// mtds
	void set(const Point3 &bmin, const Point3 &bmax)
	{
		minmax[0] = bmin;
		minmax[1] = bmax;
	}
	void addEpsilon()
	{
		minmax[0] -= Point3(1e-5f);
		minmax[1] += Point3(1e-5f);
	}
	void transform(const Transform* m);
	void include(const Point3 &pt);
	void include(const RtPoint pt);
	void include(const Bound &b);
	int longestAxis();
	bool isInside(const Point3 &pt) const;
	Point3 midpoint() const { return 0.5f*(minmax[0] + minmax[1]); }
	float diagonalLength() const { return (minmax[1] - minmax[0]).length(); }
	//bool intersect(const Ray &r, float t0, float t1) const;
	Point3 minmax[2];
};


// 2D Bound, only used for occlusion testing,
struct Bound2D
{
	Bound2D():xmin(0), ymin(0), xmax(0), ymax(0) {}
	Bound2D(int _xmin, int _ymin, int _xmax, int _ymax)
	       : xmin(_xmin), ymin(_ymin), xmax(_xmax), ymax(_ymax) {}
	int xmin, ymin, xmax, ymax;
};

__END_QDRENDER

#endif //_BOUND_H

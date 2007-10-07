#include "Bound.h"
#include "Transform.h"

#include "QDRender.h"
__BEGIN_QDRENDER

void Bound::transform(const Transform* m)
{
	// from Graphic Gems I, by Jim Arvo
	Point3 amin(minmax[0]), amax(minmax[1]);
	// matrix4 is row major, so tranpose used here, translate is in m[][3]
	Point3 bmin((*m)[0][3], (*m)[1][3], (*m)[2][3]);
	Point3 bmax(bmin);
	for (int i=0; i<3; ++i)
		for (int j=0; j<3; ++j) {
			float a = (*m)[i][j] * amin[j];
			float b = (*m)[i][j] * amax[j];
			if (a < b) {
				bmin[i] += a;
				bmax[i] += b;
			}
			else {
				bmin[i] += b;
				bmax[i] += a;
			}
		}
	minmax[0] = bmin;
	minmax[1] = bmax;
}

void Bound::include(const Point3 &pt)
{
	if (pt.x < minmax[0].x) minmax[0].x = pt.x;
	if (pt.y < minmax[0].y) minmax[0].y = pt.y;
	if (pt.z < minmax[0].z) minmax[0].z = pt.z;
	if (pt.x > minmax[1].x) minmax[1].x = pt.x;
	if (pt.y > minmax[1].y) minmax[1].y = pt.y;
	if (pt.z > minmax[1].z) minmax[1].z = pt.z;
}

void Bound::include(const RtPoint pt)
{
	if (pt[0] < minmax[0].x) minmax[0].x = pt[0];
	if (pt[1] < minmax[0].y) minmax[0].y = pt[1];
	if (pt[2] < minmax[0].z) minmax[0].z = pt[2];
	if (pt[0] > minmax[1].x) minmax[1].x = pt[0];
	if (pt[1] > minmax[1].y) minmax[1].y = pt[1];
	if (pt[2] > minmax[1].z) minmax[1].z = pt[2];
}

void Bound::include(const Bound &b)
{
	include(b.minmax[0]);
	include(b.minmax[1]);
}

int Bound::longestAxis()
{
	const Vector bdim = minmax[1] - minmax[0];
	if ((bdim.x > bdim.y) && (bdim.x > bdim.z))
		return X_AXIS;
	if (bdim.y > bdim.z)
		return Y_AXIS;
	return Z_AXIS;
}

bool Bound::isInside(const Point3 &pt) const
{
	return ((pt.x>=minmax[0].x) && (pt.y>=minmax[0].y) && (pt.z>=minmax[0].z) &&
	        (pt.x<=minmax[1].x) && (pt.y<=minmax[1].y) && (pt.z<=minmax[1].z));
}


#if 0
bool Bound::intersect(const Ray_t &r, float t0, float t1) const
{
	float tmin = (minmax[r.sign[0]].x - r.origin.x) * r.inv_direction.x;
	float tmax = (minmax[1-r.sign[0]].x - r.origin.x) * r.inv_direction.x;
	float tymin = (minmax[r.sign[1]].y - r.origin.y) * r.inv_direction.y;
	float tymax = (minmax[1-r.sign[1]].y - r.origin.y) * r.inv_direction.y;
	if ((tmin > tymax) || (tymin > tmax)) return false;
	if (tymin > tmin) tmin = tymin;
	if (tymax < tmax) tmax = tymax;
	float tzmin = (minmax[r.sign[2]].z - r.origin.z) * r.inv_direction.z;
	float tzmax = (minmax[1-r.sign[2]].z - r.origin.z) * r.inv_direction.z;
	if ((tmin > tzmax) || (tzmin > tmax)) return false;
	if (tzmin > tmin) tmin = tzmin;
	if (tzmax < tmax) tmax = tzmax;
	return ((tmin < t1) && (tmax > t0));
}
#endif

__END_QDRENDER


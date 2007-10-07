#ifndef _HIDER_H
#define _HIDER_H

#include "qdVector.h"

#include "QDRender.h"
__BEGIN_QDRENDER

class MicroPolygonGrid;
class Color;
class Bound;
class Primitive;

// return values from inFrustum
enum vflag {NOT_VISIBLE, EYE_SPLIT, IS_VISIBLE};

class Hider
{
public:
	Hider() {}
	virtual ~Hider() {}
	virtual void worldBegin()=0;
	virtual void worldEnd()=0;
	virtual void insert(Primitive*)=0;
	virtual void remove(const Primitive*)=0;
	virtual bool bucketBegin()=0;
	virtual Primitive *firstPrim()=0;
	virtual void hide(MicroPolygonGrid &g)=0;
	virtual Color trace(const Point3 &p, const Vector &r)=0;
	virtual void bucketEnd()=0;

	// inFrustum test in hider because it uses it's own camera
	virtual int inFrustum(Primitive* p) { return IS_VISIBLE; }

	virtual unsigned int gridSize()=0;
	virtual float rasterEstimate(const Bound &b)=0;
	virtual float rasterEstimate(const Point3 &p0, const Point3 &p1)=0;

	// for Z files, no shading needed, only need to know depth data
	virtual bool noShading() const=0;
};

__END_QDRENDER

#endif //_HIDER_H

#ifndef _ZBUFFERHIDER_H
#define _ZBUFFERHIDER_H

#include "Hider.h"
#include "Camera.h"
#include "HierarchicalZbuffer.h"
#include "ri.h"
#include "Bound.h"
#include "Color.h"
#include "RenderBucket.h"

#include <vector>

#include "QDRender.h"
__BEGIN_QDRENDER

// 2D bound, cached
struct CachedBound2D
{
	CachedBound2D() {}
	CachedBound2D(const Bound2D &b, float _minz) :bound(b), minz(_minz) {}
	CachedBound2D(const CachedBound2D &b):bound(b.bound), minz(b.minz) {}
	Bound2D bound;
	float minz;
};

struct SampleCoords
{
	float jx, jy;	// jittered sample coordinates
	float lu, lv;	// lens sample offsets
	float tm;			// shutter time
};

class Attributes;
class Options;
class MicroPolygon;
class ZbufferHider : public Hider
{
public:
	ZbufferHider(const Attributes& attr, const Options& opt);
	~ZbufferHider();
	virtual void worldBegin();
	virtual void worldEnd();
	virtual void insert(Primitive* p);
	virtual void remove(const Primitive* p);
	virtual bool bucketBegin();
	virtual Primitive* firstPrim();
	virtual void hide(MicroPolygonGrid &g);
	virtual Color trace(const Point3 &p, const Vector &r) { return Color(0); }
	virtual void bucketEnd();

	virtual int inFrustum(Primitive* p);

	virtual unsigned int gridSize();
	virtual float rasterEstimate(const Bound &b);
	virtual float rasterEstimate(const Point3 &p0, const Point3 &p1);

	// for Z files, no shading needed, only need to know depth data
	virtual bool noShading() const { return no_shading; }

protected:
	// local camera because of sampling at higher resolution
	Camera lcam;

	RtSepFilterFunc pixelfilter;
	float xwidth, ywidth;
	float *filtX, *filtY, *filtbuf;
	unsigned int X_ftmax, X_xt_smp, Y_ftmax, Y_xt_smp;

	HierarchicalZbuffer* hzbuf;
	float *zbuf, *pixbuf;
	unsigned int xsize, ysize, xsamples, ysamples;
	float xsam2div, ysam2div;
	SampleCoords* sample_coords;
	float jitter, shmin, shmax;
	bool empty_pixbuf, no_shading, has_z, only_z;

	unsigned int gridsize;

	// buckets
	typedef std::vector<Bucket>::iterator BucketIterator;
	BucketIterator cur_bucket;
	unsigned int bucketsize, bucket_sizeX, bucket_sizeY;
	std::vector<Bucket> buckets;
	std::vector<MicroPolygon*> transp_mps;

	// mp rasterization
	void draw_forwarded(bool opaque_only);
	void drawMP(const MicroPolygon* mp);
	void drawMP_transp(const MicroPolygon* mp);
	// for points
	void drawMP_point(const MicroPolygon* mp);
	void drawMP_transp_point(const MicroPolygon* mp);

	bool boundOccluded(const Bound2D &b, float zval);

	float dof_sp;
	bool sample_dof, sample_motion;
	float CoC_radius(float depth) const;
	
	void calcSamples();
};

__END_QDRENDER

#endif //_ZBUFFERHIDER_H

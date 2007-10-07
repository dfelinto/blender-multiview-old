#ifndef _RAYTRACEHIDER_H
#define _RAYTRACEHIDER_H

#include "Hider.h"
#include "RenderBucket.h"
#include "ri.h"
#include "Camera.h"

#include "QDRender.h"
__BEGIN_QDRENDER

class MicroPolygon;
class Attributes;
class Options;
class FrameBuffer;

class RayTraceHider : public Hider
{
public:
	RayTraceHider(const Attributes& attr, const Options& opt);
	virtual ~RayTraceHider();
	virtual void worldBegin();
	virtual void worldEnd();
	virtual void insert(Primitive*);
	virtual void remove(const Primitive*);
	virtual bool bucketBegin();
	virtual Primitive *firstPrim();
	virtual void hide(MicroPolygonGrid &g);
	virtual Color trace(const Point3 &p, const Vector &r);
	virtual void bucketEnd();

	//virtual int inFrustum(Primitive* p);

	virtual unsigned int gridSize();
	virtual float rasterEstimate(const Bound &b);
	virtual float rasterEstimate(const Point3 &p0, const Point3 &p1);

	// for Z files, no shading needed, only need to know depth data
	virtual bool noShading() const { return no_shading; }

protected:
	// local camera because of sampling at higher resolution
	Camera lcam;

	RtFilterFunc pixelfilter;
	float xwidth, ywidth;
	int X_ftmax, X_xt_smp, Y_ftmax, Y_xt_smp;

	float *pixbuf;
	int xsize, ysize, xsamples, ysamples;
	int bucketsize;
	float jitter, shmin, shmax;
	bool empty_pixbuf, no_shading, has_z, only_z;

	// buckets
	typedef std::vector<Bucket>::iterator BucketIterator;
	BucketIterator cur_bucket;
	unsigned int bucket_sizeX, bucket_sizeY;
	std::vector<Bucket> buckets;

	float dof_sp;
	bool sample_dof, sample_motion;
};

__END_QDRENDER

#endif //_RAYTRACEHIDER_H

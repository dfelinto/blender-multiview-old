//---------------------------------------------------------------------------
// Doesn't do anything yet, currently mostly a copy of code in ZbufferHider
//---------------------------------------------------------------------------

#include "RayTraceHider.h"
#include "Attributes.h"
#include "Options.h"
#include "MicroPolygonGrid.h"
#include "Mathutil.h"
#include "State.h"
#include "FrameBuffer.h"
#include "Primitive.h"

#include <iostream>

__BEGIN_QDRENDER

using namespace std;

RayTraceHider::RayTraceHider(const Attributes& attr, const Options& opt)
{
	jitter = opt.jitter;
	bucketsize = opt.bucketsize;

	pixelfilter = opt.pixelFilter;
	// filterwidth not less than a pixel
	xwidth = MAX2(1.f, opt.xWidth);
	ywidth = MAX2(1.f, opt.yWidth);
	
	xsamples = MAX2(1, (int)opt.xSamples);
	ysamples = MAX2(1, (int)opt.ySamples);
	pixbuf = NULL;

	// set no_shading flag when only doing depth and/or alpha render
	no_shading = ((opt.displayMode == Options::DM_Z) || (opt.displayMode == Options::DM_AZ));
	// if z channel in display channels, can't do empty_pixbuf shortcut
	has_z = (no_shading || (opt.displayMode == Options::DM_RGBAZ));
	// if only z channel set, then filtering can be skipped
	only_z = (opt.displayMode == Options::DM_Z);

	// the amount of extra samples needed for filtering
	// one pixel covers one full set of samples in each direction,
	// filterwidth is in pixels, so the number of samples covered by the filter is width*samples,
	// sampling is done with respect to the pixel center, which then is samples/2,
	// so the extra samples needed is (width-1)*samples/2 (width cannot be < 1, is clamped above)
	X_xt_smp = CEILI(xsamples*(xwidth-1)*0.5f);
	Y_xt_smp = CEILI(ysamples*(ywidth-1)*0.5f);

	// xsize & ysize are the width & height of the complete unfiltered image at the 'virtual' render resolution
	// used for the bucket coordinate calculations as well as primitive culling
	xsize = opt.xRes * xsamples;
	ysize = opt.yRes * ysamples;

	// if shutter close > shutter open times, motion blur enabled, have to sample moving mp's
	sample_motion = (opt.closeShutter > opt.openShutter);
	if (sample_motion)
		shmin = opt.openShutter, shmax = opt.closeShutter;
	else
		shmin = shmax = 0.f;

	// local camera used for projection calculations, because of sampling at higher resolution
	Options tmpOpts(opt);
	tmpOpts.xRes *= xsamples;
	tmpOpts.yRes *= ysamples;
	tmpOpts.fStop /= (float)MAX2(xsamples, ysamples);
	lcam.init(tmpOpts);
	// add projection camera to State, temporary solution (or not, bust() needs it too) for Points primitive
	State::Instance()->projcam = lcam;
	const float aspect = (opt.xRes > opt.yRes) ? (2.f*opt.yRes/(float)opt.xRes) : (2.f*opt.xRes/(float)opt.yRes);
	dof_sp = sqrtf(float(opt.xRes*opt.xRes + opt.yRes*opt.yRes)) * aspect / tanf(State::Instance()->cam.getFOV()*(float)M_PI/180.f);

	sample_dof = (lcam.getAperture() > 0.f);

	// bucket size, enlarged for the framebuffer number of samples
	bucket_sizeX = bucketsize * xsamples;
	bucket_sizeY = bucketsize * ysamples;

	// init the bucket traversal order
	const FrameBuffer* fb = State::Instance()->framebuffer;
	RenderBucket rbk(fb->Width(), fb->Height(), xsize, ysize, bucket_sizeX, bucket_sizeY, X_xt_smp, Y_xt_smp);
	rbk.SweepOrder(buckets);

	// current bucket iterator
	cur_bucket = buckets.begin();

	// now that the bucket parameters have been initialized
	// add the extra bucket samples to get the real bucket_size
	bucket_sizeX += 2*X_xt_smp;
	bucket_sizeY += 2*Y_xt_smp;

	// pixelbuffer, rgba
	unsigned int array_size = bucket_sizeX * bucket_sizeY;
	pixbuf = new float[array_size << 2]; //rgba

	X_ftmax = int(xwidth*xsamples + 0.5f);
	Y_ftmax = int(ywidth*ysamples + 0.5f);
}

RayTraceHider::~RayTraceHider()
{
	if (pixbuf) { delete[] pixbuf;  pixbuf = NULL; }
}

void RayTraceHider::worldBegin()
{
}

void RayTraceHider::worldEnd()
{
	cout << "WorldEnd(): handling framebuffer finals...\n";
	// signal to state that rendering is done, so time can be displayed
	State::Instance()->renderEnd();
}

void RayTraceHider::insert(Primitive* p)
{
	buckets[0].prims->push_back(p);
}

void RayTraceHider::remove(const Primitive* p)
{
}

bool RayTraceHider::bucketBegin()
{
	if (cur_bucket == buckets.end()) {
		// last bucket processed, done
		// newline because of '\r' below
		cout << "\n";
		return false;
	}

	cout << "\rbucket: " << (cur_bucket - buckets.begin() + 1) << " / " << buckets.size() << "       ";

	// flag to optimize drawing of empty buckets, filtering can be skipped
	empty_pixbuf = true;

	// clear pixbuf & re-init zbuf to max depth
	const int array_size = bucket_sizeX * bucket_sizeY;
	memset(pixbuf, 0, array_size*4*sizeof(float));

	return true;
}

Primitive* RayTraceHider::firstPrim()
{
	return NULL;
}

void RayTraceHider::hide(MicroPolygonGrid &g)
{
}

Color RayTraceHider::trace(const Point3 &p, const Vector &r)
{
	return Color(0);
}

void RayTraceHider::bucketEnd()
{
	const int xofs = (cur_bucket->xmin + X_xt_smp)/xsamples,
						yofs = (cur_bucket->ymin + Y_xt_smp)/ysamples;

	if (!cur_bucket->prims->empty()) {
		deque<Primitive*>::iterator pi;
		for (pi=cur_bucket->prims->begin(); pi!=cur_bucket->prims->end(); ++pi)
			delete *pi;
		cout << cur_bucket->prims->size() << " undeleted prims in bucketlist !\n";
	}
	delete cur_bucket->mps;
	delete cur_bucket->Tmps;
	delete cur_bucket->prims;

	FrameBuffer* const fb = State::Instance()->framebuffer;
	fb->toImage(xofs, yofs, bucketsize, bucketsize, true);

	// next bucket
	cur_bucket++;
}

unsigned int RayTraceHider::gridSize()
{
	return 1024;
}

float RayTraceHider::rasterEstimate(const Bound &b)
{
	return 1;
}

float RayTraceHider::rasterEstimate(const Point3 &p0, const Point3 &p1)
{
	return 1;
}

__END_QDRENDER

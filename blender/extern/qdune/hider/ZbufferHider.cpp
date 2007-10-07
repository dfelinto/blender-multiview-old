///---------------------------------------------------------------------------------
/// ZbufferHider
///---------------------------------------------------------------------------------
///
/// Simple hider based on zbuffer, transparent mp's are dealt with separately,
/// they are collected until bucketEnd(), sorted back to front and then composited.
/// This will produce artifacts in some cases, since micropolygons might overlap,
/// so really would need a per pixel list-based representation in that case.
/// The same is true for opaque mp's and midpoint shadowmaps (not yet implemented),
/// though possibly a secondary zbuffer might be an option too.
///
/// Occlusion testing is done here by simply brute force testing all zbuffer
/// pixels one by one, see comments in HierarchicalZbuffer.cpp
///
/// Grid occlusion code was removed, since it only seemed to slow things down.
/// And didn't quite work properly anyway.
///

#include "ZbufferHider.h"
#include "Color.h"
#include "FrameBuffer.h"
#include "MicroPolygonGrid.h"
#include "MicroPolygon.h"
#include "Mathutil.h"
#include "Parametric.h"
#include "mcqmc.h"
#include "State.h"
// for samplecoords
#include "Noise.h"

#include <map>
#include <iostream>
#include <algorithm>

#include "QDRender.h"
__BEGIN_QDRENDER

using namespace std;

// temporary for testing
//#define USE_HZBUF

// cached 2D bounds, static here because insertion comparator also needs access to it
static map<const Primitive*, CachedBound2D> boundcache;

// some vars used for some statistics,
// not particularly nice, but was just temporary, should be implemented better
static unsigned int prims_inserted = 0;
static unsigned int total_grids = 0;
static unsigned int total_mps = 0;
static unsigned int mp_drawn = 0;
static unsigned int mp_del = 0;
static unsigned int max_bk_mp = 0;
static unsigned int mp_culled = 0;
static unsigned int prim_culled = 0;
static unsigned int grid_culled = 0;
static unsigned int curbucket_mp = 0;
static unsigned int mp_trans = 0;
static unsigned int curbucket_mp_cull = 0;

//---------------------------------------------------------------------------
// ctor
ZbufferHider::ZbufferHider(const Attributes& attr, const Options& opt)
{
	jitter = opt.jitter;
	bucketsize = opt.bucketsize;
	gridsize = opt.gridsize;

	//pixelfilter = opt.pixelFilter;
#ifndef USE_HZBUF
	// use separable filters instead
	if (opt.pixelFilter == RiBoxFilter)
		pixelfilter = RiSepBoxFilter;
	else if (opt.pixelFilter == RiTriangleFilter)
		pixelfilter = RiSepTriangleFilter;
	else if (opt.pixelFilter == RiCatmullRomFilter)
		pixelfilter = RiSepCatmullRomFilter;
	else if (opt.pixelFilter == RiSincFilter)
		pixelfilter = RiSepSincFilter;
	else
		pixelfilter = RiSepGaussianFilter;

	// filterwidth not less than a pixel
	xwidth = MAX2(1.f, opt.xWidth);
	ywidth = MAX2(1.f, opt.yWidth);
#else
	// temporary for hzbuf testing, always box 1 1
	pixelfilter = RiSepBoxFilter;
	xwidth = ywidth = 1;
#endif
	
	xsamples = MAX2(1, (int)opt.xSamples);
	ysamples = MAX2(1, (int)opt.ySamples);
	zbuf = NULL;
	hzbuf = NULL;
	pixbuf = NULL;
	filtX = filtY = NULL;
	sample_coords = NULL;
	// used for rasterEstimate()
	xsam2div = 1.f/(xsamples*xsamples);
	ysam2div = 1.f/(ysamples*ysamples);
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

	// xsize & ysize are the width & height of the complete unfiltered image at the higher sampling resolution
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

	// initialize bucket traversal pattern
	const FrameBuffer* fb = State::Instance()->framebuffer;
	RenderBucket rbk(fb->Width(), fb->Height(), xsize, ysize, bucket_sizeX, bucket_sizeY, X_xt_smp, Y_xt_smp);
	rbk.SweepOrder(buckets);

	// current bucket iterator
	cur_bucket = buckets.begin();

	// now that the bucket parameters have been initialized
	// add the extra bucket samples to get the real bucket_size
	bucket_sizeX += 2*X_xt_smp;
	bucket_sizeY += 2*Y_xt_smp;

	// alloc sample- and z-buffer for a single bucket
	// pixelbuffer, rgba
	unsigned int array_size = bucket_sizeX * bucket_sizeY;
	pixbuf = new float[array_size << 2]; //rgba
	// z-buffer
#ifndef USE_HZBUF
	zbuf = new float[array_size];
#else
	hzbuf = new HierarchicalZbuffer(ilog2(MAX2(bucket_sizeX, bucket_sizeY)));
#endif

	// In this case, the filter weights are always the same for every bucket.
	// Since in this hider mp's are rasterized before filtering,
	// in essence it has become an image resampling problem, so filter weights are independent of
	// sample coordinates, and can be precalculated here. This is a *lot* faster too.
	X_ftmax = int(xwidth*xsamples + 0.5f);
	Y_ftmax = int(ywidth*ysamples + 0.5f);
	const float X_ft_ct = 0.5f*X_ftmax, Y_ft_ct = 0.5f*Y_ftmax;
	filtX = new float[X_ftmax]; // X filter weigths
	filtY = new float[Y_ftmax]; // Y filter weights
	filtbuf = new float[bucket_sizeY << 2]; // rgba buffer for filter, one column, size of source
	const float xdiv = 1.f/xsamples, ydiv = 1.f/ysamples;
	// X wts
	float wtsum = 0;
	float* fwts = filtX;
	for (unsigned int x=0; x<X_ftmax; ++x) {
		*fwts = pixelfilter((x - X_ft_ct + 0.5f)*xdiv, xwidth);
		wtsum += *fwts++;
	}
	if (wtsum!=0.f) {
		wtsum = 1.f/wtsum;
		fwts = filtX;
		unsigned int cnt = X_ftmax;
		while (cnt--) *fwts++ *= wtsum;
	}
	// Y wts
	wtsum = 0;
	fwts = filtY;
	for (unsigned int y=0; y<Y_ftmax; ++y) {
		*fwts = pixelfilter((y - Y_ft_ct + 0.5f)*ydiv, ywidth);
		wtsum += *fwts++;
	}
	if (wtsum!=0.f) {
		wtsum = 1.f/wtsum;
		fwts = filtY;
		unsigned int cnt = Y_ftmax;
		while (cnt--) *fwts++ *= wtsum;
	}

	// the sample coordinate array
	sample_coords = new SampleCoords[array_size];

}


// dtor
ZbufferHider::~ZbufferHider()
{
	if (sample_coords) delete[] sample_coords;
	if (filtX) delete[] filtX;
	if (filtY) delete[] filtY;
	if (filtbuf) delete[] filtbuf;
	if (zbuf) delete[] zbuf;
	if (hzbuf) delete hzbuf;
	if (pixbuf) delete[] pixbuf;
}


void ZbufferHider::worldBegin()
{
	total_grids = 0;
	total_mps = 0;
	mp_drawn = 0;
	mp_del = 0;
	max_bk_mp = 0;
	mp_culled = 0;
	prim_culled = 0;
	grid_culled = 0;
	curbucket_mp = 0;
	curbucket_mp_cull = 0;
	mp_trans = 0;
}


void ZbufferHider::worldEnd()
{
	// signal to state that rendering is done, so time can be displayed
	// and save the picture or wait for the displaywindow to be closed
	State::Instance()->renderEnd();
	cout << "Done\n";

	cout << "Total prims inserted: " << prims_inserted << "\n";

	cout << "Grids created       : " << total_grids << "\n";
	cout << "Total MicroPolygon's: " << total_mps << "\n";

	cout << "Total mp drawn      : " << mp_drawn << "\n";
	cout << "Total mp deleted    : " << mp_del << "\n";

	cout << "Max mp per bucket   : " << max_bk_mp << "\n";

	cout << "Transparent mp's    : " << mp_trans << "\n";

	cout << "Prims culled        : " << prim_culled << "\n";
	cout << "Grids culled        : " << grid_culled << "\n";
	cout << "mp's culled         : " << mp_culled << endl;
}


//------------------------------------------------------------------------------


// comparator used for front to back depth ordering in insert() below
inline bool compZ(Primitive* p, float zval)
{
	return boundcache[p].minz < zval;
}


//------------------------------------------------------------------------------


// insert a new primitive in the main list,
// list is maintained in front to back depth sorted order
void ZbufferHider::insert(Primitive* p)
{
	prims_inserted++;

	const Bound2D* cb = &boundcache[p].bound;
	const float minz = boundcache[p].minz;

	// for all buckets yet to draw, assign primitive to the FIRST bucket it intersects
	for (BucketIterator bk=cur_bucket; bk!=buckets.end(); ++bk)
	{
		if ((cb->xmax >= bk->xmin) && (bk->xmax >= cb->xmin)
		&& (cb->ymax >= bk->ymin) && (bk->ymax >= cb->ymin))
		{
			// depth ordered insertion
			// (now uses deque instead of list, despite not optimal for random insertion,
			//  it still is a bit faster than lists and uses less memory as well,
			//  difference is not much, but even little bits help...)
			// for whatever reason, this does not compile in debug mode in VC8..., release is somehow no prob.
			bk->prims->insert(lower_bound(bk->prims->begin(), bk->prims->end(), minz, compZ), p);
			//bk->prims->push_back(p);
			return;
		}
	}

	// if we get here, the primitive was not assigned to any bucket, so discard it
	delete p;
}


// remove specified primitive from list
void ZbufferHider::remove(const Primitive* p)
{
	// already removed from current bucket, remove from other buckets as well
	for (BucketIterator bk=cur_bucket+1; bk!=buckets.end(); ++bk) {
		deque<Primitive*>::iterator fp = find(bk->prims->begin(), bk->prims->end(), p);
		if (fp!=bk->prims->end()) bk->prims->erase(fp);
	}
}


void ZbufferHider::calcSamples()
{
	// fill in mp sample coordinates
	// calculated for the full bucket, not just the visible portion (border overlap case)
	const int xs = cur_bucket->xmin, ys = cur_bucket->ymin;
	const unsigned int QS1=Noise1D_int(xs), QS2=Noise1D_int(ys);
	SampleCoords* scp = sample_coords;
	const float smpdiv = 1.f/(xsamples*ysamples);
	for (int y=ys; y<(ys+(int)bucket_sizeY); ++y) {
		for (int x=xs; x<(xs+(int)bucket_sizeX); ++x) {
			// jittered coordinate, re-used for dof
			const unsigned int pt = x + y*(xsize + 2*X_xt_smp);
			// when profiling, if I understand the results correctly, this turns out to be quite expensive,
			// but is definitely best for DoF/Mblur, have to find faster way TODO
			scp->jx = RI_Sb(pt, QS1);
			scp->jy = RI_vdC(pt, QS2);
			// dof, concentric disk
			if (sample_dof)
				shirleyDisc(scp->jx, scp->jy, scp->lu, scp->lv);
			else
				scp->lu = scp->lv = 0.f;
			if (sample_motion)	// might as well re-use jx again here, no artifacts, apart from usual QMC patterns (noisy stratified ok too)
				scp->tm = (jitter*scp->jx + (x % xsamples) + (y % ysamples)*xsamples)*smpdiv*(shmax - shmin);
			else
				scp->tm = 0;
			// jittered sample offsets (interpolated for centering, must be 0.5 when jitter==0)
			scp->jx = 0.5f + jitter * (scp->jx - 0.5f);
			scp->jy = 0.5f + jitter * (scp->jy - 0.5f);
			scp++;
		}
	}
}

// bucket initialization
bool ZbufferHider::bucketBegin()
{
	if (cur_bucket == buckets.end()) { // last bucket processed, done
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

#ifndef USE_HZBUF
	int cnt = array_size;
	while (cnt--) zbuf[cnt] = 1e10f;
#else
	hzbuf->initLayers();
#endif

	calcSamples();

	// reset bucket mp counter
	curbucket_mp = curbucket_mp_cull = 0;

	return true;
}

//------------------------------------------------------------------------------

// Comparator for transparent mp back to front depth sorting
inline bool compZ_mp(MicroPolygon* mp1, MicroPolygon* mp2)
{
	return (mp1->zmin > mp2->zmin);
}

//------------------------------------------------------------------------------

void ZbufferHider::draw_forwarded(bool opaque_only)
{
	vector<MicroPolygon*>* mps = cur_bucket->mps;
	vector<MicroPolygon*>* Tmps = cur_bucket->Tmps;

	const unsigned int ts = (unsigned int)((mps ? mps->size() : 0) + (Tmps ? Tmps->size() : 0) + transp_mps.size());
	if (ts > max_bk_mp) max_bk_mp = ts;

	curbucket_mp += ts;

	vector<MicroPolygon*>::iterator mi;

	if (mps) {
		// opaque mp's
		for (mi=mps->begin(); mi!=mps->end(); ++mi) {
			MicroPolygon* mp = *mi;
			drawMP(mp);
			// delete if no longer shared
			if (mp->decRefCount()==0) {
				delete mp;
				mp_del++;
			}
		}
		delete cur_bucket->mps;
		cur_bucket->mps = NULL;
	}

	if (not opaque_only) {
		// transparent mp's, merge current bucket mp's with forwarded mp's
		if (Tmps) {
			size_t tmc = transp_mps.size();
			transp_mps.resize(tmc + Tmps->size());
			for (mi=Tmps->begin(); mi!=Tmps->end(); ++mi)
				transp_mps[tmc++] = *mi;
		}
		mp_trans += (unsigned int)transp_mps.size();
		if (not transp_mps.empty()) {
			// back to front depth sort first
			sort(transp_mps.begin(), transp_mps.end(), compZ_mp);
			// rasterize all in order
			for (mi=transp_mps.begin(); mi!=transp_mps.end(); ++mi) {
				MicroPolygon* mp = *mi;
				drawMP_transp(mp);
				// delete if no longer shared
				if (mp->decRefCount()==0) {
					delete mp;
					mp_del++;
				}
			}
			transp_mps.clear();
		}
		delete cur_bucket->Tmps;
		cur_bucket->Tmps = NULL;
	}

}


void ZbufferHider::bucketEnd()
{
	if (curbucket_mp > max_bk_mp) max_bk_mp = curbucket_mp;

	// bucket framebuffer offset
	const int xofs = (cur_bucket->xmin + X_xt_smp)/xsamples,
	          yofs = (cur_bucket->ymin + Y_xt_smp)/ysamples;

	// primitive list for current bucket can now be deleted,
	// make sure there are no 'leftovers' which were not deleted,
	// This should *never* happen though...
	if (!cur_bucket->prims->empty()) {
		deque<Primitive*>::iterator pi;
		for (pi=cur_bucket->prims->begin(); pi!=cur_bucket->prims->end(); ++pi)
			delete *pi;
		cout << cur_bucket->prims->size() << " undeleted prims in bucketlist !\n";
	}
	delete cur_bucket->prims;

	FrameBuffer& fb = *State::Instance()->framebuffer;
	// if hide() was not called because no primitives intersected the bucket,
	// any forwarded mp's in the current bucket still need to be drawn here
	draw_forwarded(false);
	// resample bucket and write to framebuffer
	float coldat[5];
	// z is maximum of samples in pixel
	FixedArray<float> zmax(bucket_sizeY);
	if (only_z) {
		// for depth renders, only need to apply z depth filter
		coldat[0] = coldat[1] = coldat[2] = coldat[3] = 0.f;
		for (unsigned int k=0; k<bucketsize; ++k) {
			for (unsigned int i=0; i<bucket_sizeY; ++i) {
				const unsigned int p = k*xsamples + i*bucket_sizeX;
#ifdef USE_HZBUF
				const float* Z = &hzbuf->maxLayer()[p];
#else
				const float* Z = &zbuf[p];
#endif
				coldat[4] = 0.f;
				for (unsigned int sx=0; sx<X_ftmax; ++sx)
					coldat[4] = MAX2(coldat[4], *Z++);
				zmax[i] = coldat[4];
			}
			for (unsigned int j=0; j<bucketsize; ++j) {
				const float* Z = &zmax[j*ysamples];
				coldat[4] = 0.f;
				for (unsigned int sy=0; sy<Y_ftmax; ++sy)
					coldat[4] = MAX2(coldat[4], *Z++);
				//printf("coldat[4] -> %g\n", coldat[4]);
				fb(k+xofs, j+yofs, coldat);
			}
		}
	}
	// else skip if no mp's were drawn (can't skip if z channel also needed)
	else if (!has_z && !empty_pixbuf) {
		for (unsigned int k=0; k<bucketsize; ++k) {
			// H
			for (unsigned int i=0; i<bucket_sizeY; ++i) {
				coldat[0] = coldat[1] = coldat[2] = coldat[3] = 0.f;
				const float* fw = filtX;
				const unsigned int p = k*xsamples + i*bucket_sizeX;
				const float* fbuf = &pixbuf[p << 2];
#ifndef USE_HZBUF
				const float* Z = &zbuf[p];
#else
				const float* Z = &hzbuf->maxLayer()[p];
#endif
				coldat[4] = 0.f;
				for (unsigned int sx=0; sx<X_ftmax; ++sx) {
					coldat[0] += *fbuf++ * *fw;
					coldat[1] += *fbuf++ * *fw;
					coldat[2] += *fbuf++ * *fw;
					coldat[3] += *fbuf++ * *fw++;
					coldat[4] = MAX2(coldat[4], *Z++);
				}
				const unsigned int i4 = i << 2;
				filtbuf[i4] = coldat[0];
				filtbuf[i4+1] = coldat[1];
				filtbuf[i4+2] = coldat[2];
				filtbuf[i4+3] = coldat[3];
				zmax[i] = coldat[4];
			}
			// V
			for (unsigned int j=0;j<bucketsize; ++j) {
				coldat[0] = coldat[1] = coldat[2] = coldat[3] = 0.f;
				const float* fw = filtY;
				const unsigned int p = j*ysamples;
				const float* fbuf = &filtbuf[p << 2];
				const float* Z = &zmax[p];
				coldat[4] = 0.f;
				for (unsigned int sy=0; sy<Y_ftmax; ++sy) {
					coldat[0] += *fbuf++ * *fw;
					coldat[1] += *fbuf++ * *fw;
					coldat[2] += *fbuf++ * *fw;
					coldat[3] += *fbuf++ * *fw++;
					coldat[4] = MAX2(coldat[4], *Z++);
				}
				fb(k+xofs, j+yofs, coldat);
			}
		}

	}

	// optimize display draw, only redraw once every scanline of buckets (only works for 'sweep')
	// (I'm sure it's possible to update the CImg display for the intended block only, but still haven't looked at that yet)
	static int lastxofs=0, lastyofs=0;
	fb.toImage(xofs, yofs, bucketsize, bucketsize, (fb.Width() < fb.Height()) ?  (yofs != lastyofs) : (xofs != lastxofs));
	lastxofs = xofs, lastyofs = yofs;

	// next bucket
	cur_bucket++;
}


int ZbufferHider::inFrustum(Primitive* p)
{
	// Frustum test done here by comparing the prim's projected bound with screen bounds.
	// Done instead of actual frustum test because 2D bounds needed elsewhere too,
	// and can be cached here for later use.
	// Disadvantage is that all this is also done for prim's which will be rejected...
	// but probably is not that a major slowdown at all compared to everything else...

	Bound b = p->bound();

	// compensate for displacement
	const Attributes* attr = p->getAttributeReference();
	const Point3 disp(ABS(attr->displacementbound_sphere));
	b.minmax[0] -= disp, b.minmax[1] += disp;

	// easiest case, test if outside of near/far clipping planes
	const float p_zmin = MIN2(b.minmax[0].z, b.minmax[1].z);
	const float p_zmax = MAX2(b.minmax[0].z, b.minmax[1].z);
	const Options& opts = State::Instance()->topOptions();
	if ((p_zmax < opts.nearClip) || (p_zmin > opts.farClip)) // completely behind nearclip or farclip
	{
		prim_culled++;
		return NOT_VISIBLE;
	}

	// calc. 2D bound
	const Point3 pp[8] = {lcam.project(Point3(b.minmax[0].x, b.minmax[0].y, b.minmax[0].z)),
	                      lcam.project(Point3(b.minmax[0].x, b.minmax[0].y, b.minmax[1].z)),
	                      lcam.project(Point3(b.minmax[0].x, b.minmax[1].y, b.minmax[0].z)),
	                      lcam.project(Point3(b.minmax[0].x, b.minmax[1].y, b.minmax[1].z)),
	                      lcam.project(Point3(b.minmax[1].x, b.minmax[0].y, b.minmax[0].z)),
	                      lcam.project(Point3(b.minmax[1].x, b.minmax[0].y, b.minmax[1].z)),
	                      lcam.project(Point3(b.minmax[1].x, b.minmax[1].y, b.minmax[0].z)),
	                      lcam.project(Point3(b.minmax[1].x, b.minmax[1].y, b.minmax[1].z)) };
	int p_xmin = FLOORI(MIN2(MIN4(pp[0].x, pp[1].x, pp[2].x, pp[3].x),
	                         MIN4(pp[4].x, pp[5].x, pp[6].x, pp[7].x))),
	    p_xmax = CEILI (MAX2(MAX4(pp[0].x, pp[1].x, pp[2].x, pp[3].x),
	                         MAX4(pp[4].x, pp[5].x, pp[6].x, pp[7].x))),
	    p_ymin = FLOORI(MIN2(MIN4(pp[0].y, pp[1].y, pp[2].y, pp[3].y),
	                         MIN4(pp[4].y, pp[5].y, pp[6].y, pp[7].y))),
	    p_ymax = CEILI (MAX2(MAX4(pp[0].y, pp[1].y, pp[2].y, pp[3].y),
	                         MAX4(pp[4].y, pp[5].y, pp[6].y, pp[7].y)));

	// adjust 2D bound for DoF if needed (only need to adjust for DoF, bound already adjusted for movement if needed)
	if (sample_dof) {
		const int maxdof = CEILI(MAX2(CoC_radius(p_zmin), CoC_radius(p_zmax)));
		p_xmin -= maxdof;
		p_ymin -= maxdof;
		p_xmax += maxdof;
		p_ymax += maxdof;
	}

	// 2d frustum test, taking cropwindow and extra filter samples into account
	const FrameBuffer* fb = State::Instance()->framebuffer;
	if ((p_xmax < (int(fb->xStart()*xsamples) - (int)X_xt_smp)) or
	    (int((fb->xStart() + fb->Width())*xsamples + X_xt_smp) < p_xmin) or
			(p_ymax < (int(fb->yStart()*ysamples) - (int)Y_xt_smp)) or
			(int((fb->yStart() + fb->Height())*ysamples + Y_xt_smp) < p_ymin))
	{
		prim_culled++;
		return NOT_VISIBLE;
	}
	else if ((p_zmin < (float)RI_EPSILON) && (p_zmax < opts.farClip))
	{
		// crossing eyeplane, the dreaded 'eyesplit'!
		prim_culled++;
		return EYE_SPLIT;
	}

	// ok, in view and (completely or partially) inside clippingplanes
	boundcache[p] = CachedBound2D(Bound2D(p_xmin, p_ymin, p_xmax, p_ymax), p_zmin);
	return IS_VISIBLE;
}


// brute force per pixel occlusion test of 2D bound
bool ZbufferHider::boundOccluded(const Bound2D &b, float zval)
{
	for (int y=b.ymin; y<=b.ymax; ++y) {
		const float* zb = &zbuf[y*bucket_sizeX + b.xmin];
		for (int x=b.xmin; x<=b.xmax; ++x)
			if (zval < *zb++) return false;
	}
	return true;
}


// removes and returns first Primitive in list
// only removes from list, does not delete, that is handled by framework
Primitive* ZbufferHider::firstPrim()
{
	// traverse the current bucket primitive list
	while (!cur_bucket->prims->empty()) {
		Primitive* p = cur_bucket->prims->front();
		cur_bucket->prims->pop_front();

		// occlusion cull test
		const Bound2D* cb = &boundcache[p].bound;
		const float minz = boundcache[p].minz;

		// bucket clip (local coords)
		const int xmin = MIN2((int)bucket_sizeX-1, MAX2(0, cb->xmin - cur_bucket->xmin)),
		          ymin = MIN2((int)bucket_sizeY-1, MAX2(0, cb->ymin - cur_bucket->ymin)),
		          xmax = MIN2((int)bucket_sizeX-1, MAX2(0, cb->xmax - cur_bucket->xmin)),
		          ymax = MIN2((int)bucket_sizeY-1, MAX2(0, cb->ymax - cur_bucket->ymin));

		// if occluded, forward to FIRST NEXT bucket it overlaps
#ifndef USE_HZBUF
		if (boundOccluded(Bound2D(xmin, ymin, xmax, ymax), minz)) {
#else
		if (!empty_pixbuf && hzbuf->boundOccluded(Bound2D(xmin, ymin, xmax, ymax), minz)) {
#endif
			prim_culled++;
			bool prim_assigned = false;
			for (BucketIterator bk2=cur_bucket+1; bk2!=buckets.end(); ++bk2) {
				if ((cb->xmax >= bk2->xmin) && (bk2->xmax >= cb->xmin) and
				    (cb->ymax >= bk2->ymin) && (bk2->ymax >= cb->ymin))
				{
					// depth ordered insertion
					// for whatever reason, this does not compile in debug mode in VC8..., release is somehow no prob.
					bk2->prims->insert(lower_bound(bk2->prims->begin(), bk2->prims->end(), minz, compZ), p);
					//bk2->prims->push_back(p);
					prim_assigned = true;
					break;
				}
			}
			// if primitive could not be forwarded to any other bucket,
			// it can be deleted, not visible (off screen or total occlusion)
			if (!prim_assigned) delete p;
			continue;
		}

		// primitive is visible
		return p;
	}

	// empty list
	return NULL;
}


// calculates barycentric coords s/t of point P in triangle V0/V1/V2
inline bool get_st(const Point2 &P,
									const Point3 &V0, const Point3 &V1, const Point3 &V2,
									float &s, float &t)
{
	const float a00 = V1.x-V0.x, a01 = V2.x-V0.x,
							a10 = V1.y-V0.y, a11 = V2.y-V0.y;
	float d = a00*a11 - a01*a10;
	if (d==0.f) {
		s = t = 0.3333333f;
		return true;
	}
	else {
		const float b0 = P.x-V0.x, b1 = P.y-V0.y;
		d = 1.f/d;
		s = (a11*b0 - a01*b1) * d;
		t = (a00*b1 - a10*b0) * d;
		return ((s>=0.f) && (t>=0.f) && ((s+t)<=1.f));
	}
}


// calculates interpolation coordinates for quad.
// simply calculates barycentric coords of a subtriangle of quad,
// if not in range then uses bcc of other tri.
// returns true if in range, so can be used as point_in_quad test as well
inline bool quadIntpol(const Point3* pts, const Point2 &P, float *wts)
{
	if (get_st(P, pts[0], pts[1], pts[2], wts[1], wts[2]))
	{
		wts[0] = 1.f - wts[1] - wts[2];
		wts[3] = 0.f;
		return true;
	}
	else if (get_st(P, pts[0], pts[2], pts[3], wts[2], wts[3]))
	{
		wts[0] = 1.f - wts[2] - wts[3];
		wts[1] = 0.f;
		return true;
	}
	return false;
}


// as above, but calculates interpol. coords directly from quad.
// uses 'mean value coordinates', which are similar to barycentric coords for triangles,
// but suitable for any n-sided polygon, even if concave or self-intersecting.
// slower though than the above simpler two-triangle method
// Would also be very useful for dicing polygons TODO
// ----------
// Also can be used to test for point_in_quad test just like tri.barycentric coords
inline bool quadIntpol_mvc(const Point3* pts, const Point2 &P, float *wts)
{
	const float s0x = pts[0].x - P.x, s0y = pts[0].y - P.y;
	const float s1x = pts[1].x - P.x, s1y = pts[1].y - P.y;
	const float s2x = pts[2].x - P.x, s2y = pts[2].y - P.y;
	const float s3x = pts[3].x - P.x, s3y = pts[3].y - P.y;
	const float r0 = sqrt(s0x*s0x + s0y*s0y);
	const float r1 = sqrt(s1x*s1x + s1y*s1y);
	const float r2 = sqrt(s2x*s2x + s2y*s2y);
	const float r3 = sqrt(s3x*s3x + s3y*s3y);
	const float D0 = s0x*s1x + s0y*s1y;
	const float D1 = s1x*s2x + s1y*s2y;
	const float D2 = s2x*s3x + s2y*s3y;
	const float D3 = s3x*s0x + s3y*s0y;
	const float A0 = s0x*s1y - s1x*s0y;
	const float A1 = s1x*s2y - s2x*s1y;
	const float A2 = s2x*s3y - s3x*s2y;
	const float A3 = s3x*s0y - s0x*s3y;
	float t0 = r0*r1 - D0, t1 = r1*r2 - D1,
				t2 = r2*r3 - D2, t3 = r3*r0 - D3;
	if (A0!=0.f) t0 /= A0;
	if (A1!=0.f) t1 /= A1;
	if (A2!=0.f) t2 /= A2;
	if (A3!=0.f) t3 /= A3;
	wts[0] = t3 + t0;
	wts[1] = t0 + t1;
	wts[2] = t1 + t2;
	wts[3] = t2 + t3;
	if (r0!=0.f) wts[0] /= r0;
	if (r1!=0.f) wts[1] /= r1;
	if (r2!=0.f) wts[2] /= r2;
	if (r3!=0.f) wts[3] /= r3;
	float sw = wts[0] + wts[1] + wts[2] + wts[3];
	if (sw!=0.f) {
		sw = 1.f/sw;
		wts[0] *= sw;
		wts[1] *= sw;
		wts[2] *= sw;
		wts[3] *= sw;
	}
	// negative test can't be done earlier, sw can change sign!
	if ((wts[0]<0.f) || (wts[1]<0.f) || (wts[2]<0.f) || (wts[3]<0.f)) return false;
	if ((wts[0]>1.f) || (wts[1]>1.f) || (wts[2]>1.f) || (wts[3]>1.f)) return false;
	return true;
}

// Radius of 'Circle Of Confusion'
float ZbufferHider::CoC_radius(float depth) const
{
	const float iz = (depth==0.f) ? 0.f : (1.f/depth);
	return 0.5f*ABS(lcam.getAperture()*(dof_sp*(lcam.getInvFDist() - iz) - 1.f));
}

// test if point in (micro)quad, point order must be ccw, quad must be convex
// though fast, some quads won't be drawn because of those restrictions,
// so can lead to shading artifacts
// pts array 0-3 are the 4 mp vertices,
//           4-7 are the precomputed differences of points v[i-1]-v[i] for mp verts 0 to 3
/*
inline bool point_In_Quad(const Point3 pts[8], float x, float y)
{
	if ((pts[5].x*(y - pts[0].y) - pts[5].y*(x - pts[0].x)) >= 0.f) return false;
	if ((pts[6].x*(y - pts[1].y) - pts[6].y*(x - pts[1].x)) >= 0.f) return false;
	if ((pts[7].x*(y - pts[2].y) - pts[7].y*(x - pts[2].x)) >= 0.f) return false;
	if ((pts[4].x*(y - pts[3].y) - pts[4].y*(x - pts[3].x)) >= 0.f) return false;
	return true;
}
*/

// as point_In_Quad(), but this can be used for any completely general quad,
// convex/concave/self-intersecting, cw or ccw, all don't matter.
// of course it is therefore also a bit slower, but not really all that much.
// This is useful however for extreme displacement, it's not unusual
// that some (if not most) mp's when projected will be concave or self-intersecting quads.
// point_In_Quad() will then actually cause rendering artifacts because of missed samples.
// Might be possible to optimize further.
// adapted from Eric Haines' point-in-poly strategies test program in GGIV
inline bool point_In_gQuad(const Point3 pts[8], float x, float y)
{
	bool yflag0 = (pts[3].y >= y), yflag1 = (pts[0].y >= y);
	bool inside_flag = (yflag0 != yflag1) ? (((pts[0].y - y)*pts[4].x >= (pts[0].x - x)*pts[4].y) == yflag1) : 0;

	yflag0 = yflag1;  yflag1 = (pts[1].y >= y);
	inside_flag ^= (yflag0 != yflag1) ? (((pts[1].y - y)*pts[5].x >= (pts[1].x - x)*pts[5].y) == yflag1) : 0;

	yflag0 = yflag1;  yflag1 = (pts[2].y >= y);
	inside_flag ^= (yflag0 != yflag1) ? (((pts[2].y - y)*pts[6].x >= (pts[2].x - x)*pts[6].y) == yflag1) : 0;

	yflag0 = yflag1;  yflag1 = (pts[3].y >= y);
	return (inside_flag ^ ((yflag0 != yflag1) ? (((pts[3].y - y)*pts[7].x >= (pts[3].x - x)*pts[7].y) == yflag1) : 0));
}


// rasterizes the micropolygon, lots of room for optimization here, especially both
// point in quad testing as well as for calculation of interpolation weights.
// Currently all mp's are gouraud shaded, but this really isn't required at all
// unless maybe the shadingrate area is larger than a pixel, so could be removed.
void ZbufferHider::drawMP(const MicroPolygon* mp)
{
	if (mp->v1 == NULL) {
		drawMP_point(mp);
		return;
	}

	mp_drawn++;
	empty_pixbuf = false;

	Options& opts = State::Instance()->topOptions();

	// mp coords and precomputed differences for point_in_Quad() test
	Point3 pts[8] = {mp->v0->pp, mp->v1->pp, mp->v2->pp, mp->v3->pp,
	                 Point3(mp->v3->pp - mp->v0->pp), Point3(mp->v0->pp - mp->v1->pp),
	                 Point3(mp->v1->pp - mp->v2->pp), Point3(mp->v2->pp - mp->v3->pp)};

	// clip to current bucket bound
	const int xmin = MAX2(cur_bucket->xmin, mp->xmin),
	          ymin = MAX2(cur_bucket->ymin, mp->ymin),
	          xmax = MIN2(cur_bucket->xmax, mp->xmax),
	          ymax = MIN2(cur_bucket->ymax, mp->ymax);

	// color at quad corners
	Color col0(static_cast<ShadedVertex*>(mp->v0)->Ci);
	Color col1(static_cast<ShadedVertex*>(mp->v1)->Ci);
	Color col2(static_cast<ShadedVertex*>(mp->v2)->Ci);
	Color col3(static_cast<ShadedVertex*>(mp->v3)->Ci);
	// interpolation weights
	float wts[4] = {0, 0, 0, 0};

	for (int by=ymin; by<=ymax; ++by) {
		const int zy = by - cur_bucket->ymin;
		for (int bx=xmin; bx<=xmax; ++bx) {
			const int zx = bx - cur_bucket->xmin;
			// zbuffer index, also use for sample coord array & pixelbuffer
			int zp = zx + zy*bucket_sizeX;
			// current sample coordinate
			const SampleCoords* scp = &sample_coords[zp];
			if (sample_dof) {
				// dof offsets, point 0
				pts[0].x = mp->v0->pp.x + scp->lu*mp->v0->crad;
				pts[0].y = mp->v0->pp.y + scp->lv*mp->v0->crad;
				// point 1
				pts[1].x = mp->v1->pp.x + scp->lu*mp->v1->crad;
				pts[1].y = mp->v1->pp.y + scp->lv*mp->v1->crad;
				// point 2
				pts[2].x = mp->v2->pp.x + scp->lu*mp->v2->crad;
				pts[2].y = mp->v2->pp.y + scp->lv*mp->v2->crad;
				// point 3
				pts[3].x = mp->v3->pp.x + scp->lu*mp->v3->crad;
				pts[3].y = mp->v3->pp.y + scp->lv*mp->v3->crad;
				// could do local bound check here, but determining the bound and
				// then testing sample point only adds rendertime (possibly doubles even)
				// pt diffs
				pts[4].x = pts[3].x - pts[0].x;   pts[4].y = pts[3].y - pts[0].y;
				pts[5].x = pts[0].x - pts[1].x;   pts[5].y = pts[0].y - pts[1].y;
				pts[6].x = pts[1].x - pts[2].x;   pts[6].y = pts[1].y - pts[2].y;
				pts[7].x = pts[2].x - pts[3].x;   pts[7].y = pts[2].y - pts[3].y;
			}
			if (sample_motion) {
				// mblur offsets, point 0
				pts[0].x = mp->v0->pp.x + scp->tm*(static_cast<ShadedVertex*>(mp->v0)->ppc.x - mp->v0->pp.x);
				pts[0].y = mp->v0->pp.y + scp->tm*(static_cast<ShadedVertex*>(mp->v0)->ppc.y - mp->v0->pp.y);
				// point 1
				pts[1].x = mp->v1->pp.x + scp->tm*(static_cast<ShadedVertex*>(mp->v1)->ppc.x - mp->v0->pp.x);
				pts[1].y = mp->v1->pp.y + scp->tm*(static_cast<ShadedVertex*>(mp->v1)->ppc.y - mp->v0->pp.y);
				// point 2
				pts[2].x = mp->v2->pp.x + scp->tm*(static_cast<ShadedVertex*>(mp->v2)->ppc.x - mp->v0->pp.x);
				pts[2].y = mp->v2->pp.y + scp->tm*(static_cast<ShadedVertex*>(mp->v2)->ppc.y - mp->v0->pp.y);
				// point 3
				pts[3].x = mp->v3->pp.x + scp->tm*(static_cast<ShadedVertex*>(mp->v3)->ppc.x - mp->v0->pp.x);
				pts[3].y = mp->v3->pp.y + scp->tm*(static_cast<ShadedVertex*>(mp->v3)->ppc.y - mp->v0->pp.y);
				pts[4].x = pts[3].x - pts[0].x;   pts[4].y = pts[3].y - pts[0].y;
				pts[5].x = pts[0].x - pts[1].x;   pts[5].y = pts[0].y - pts[1].y;
				pts[6].x = pts[1].x - pts[2].x;   pts[6].y = pts[1].y - pts[2].y;
				pts[7].x = pts[2].x - pts[3].x;   pts[7].y = pts[2].y - pts[3].y;
			}
			if (point_In_gQuad(pts, scp->jx+bx, scp->jy+by)) {
				quadIntpol(pts, Point2(scp->jx+bx, scp->jy+by), wts);
				const float zval = wts[0]*pts[0].z + wts[1]*pts[1].z + wts[2]*pts[2].z + wts[3]*pts[3].z;
				if ((zval < opts.nearClip) || (zval > opts.farClip)) continue;
#ifndef USE_HZBUF
				if (zval < zbuf[zp]) {
					zbuf[zp] = zval;
#else
				if (!hzbuf->pointOccluded(zx, zy, zval)) {
				  hzbuf->updatePoint(zx, zy, zval);
#endif
					float* sdat = &pixbuf[zp << 2];
					//sdat[0] = col0.r, sdat[1] = col0.g, sdat[2] = col0.b;
					// color interpolation really only needed when ShadingInterpolation == 'smooth'
					sdat[0] = wts[0]*col0.r + wts[1]*col1.r + wts[2]*col2.r + wts[3]*col3.r;
					sdat[1] = wts[0]*col0.g + wts[1]*col1.g + wts[2]*col2.g + wts[3]*col3.g;
					sdat[2] = wts[0]*col0.b + wts[1]*col1.b + wts[2]*col2.b + wts[3]*col3.b;
					sdat[3] = 1.f; // alpha
				} // zbuf
			} // pointinpatch
		} // bx
	}  // by
}

// as above, for transparent mp's
// zbuffer is only tested here, composites on top of current pixels
void ZbufferHider::drawMP_transp(const MicroPolygon* mp)
{
	if (mp->v1 == NULL) {
		drawMP_transp_point(mp);
		return;
	}

	mp_drawn++;
	empty_pixbuf = false;

	Options& opts = State::Instance()->topOptions();

	// mp coords and precomputed differences for point_in_Quad() test
	Point3 pts[8] = {mp->v0->pp, mp->v1->pp, mp->v2->pp, mp->v3->pp,
	                 Point3(mp->v3->pp - mp->v0->pp), Point3(mp->v0->pp - mp->v1->pp),
	                 Point3(mp->v1->pp - mp->v2->pp), Point3(mp->v2->pp - mp->v3->pp)};

	// clip to current bucket bound
	const int xmin = MAX2(cur_bucket->xmin, mp->xmin),
	          ymin = MAX2(cur_bucket->ymin, mp->ymin),
	          xmax = MIN2(cur_bucket->xmax, mp->xmax),
	          ymax = MIN2(cur_bucket->ymax, mp->ymax);

	// surface color
	Color cl0(static_cast<ShadedVertex*>(mp->v0)->Ci);
	Color cl1(static_cast<ShadedVertex*>(mp->v1)->Ci);
	Color cl2(static_cast<ShadedVertex*>(mp->v2)->Ci);
	Color cl3(static_cast<ShadedVertex*>(mp->v3)->Ci);
	// opacity
	Color op0(static_cast<ShadedVertex*>(mp->v0)->Oi);
	Color op1(static_cast<ShadedVertex*>(mp->v1)->Oi);
	Color op2(static_cast<ShadedVertex*>(mp->v2)->Oi);
	Color op3(static_cast<ShadedVertex*>(mp->v3)->Oi);
	float wts[4] = {0,0,0,0};

	for (int by=ymin; by<=ymax; ++by) {
		const int zy = by - cur_bucket->ymin;
		for (int bx=xmin; bx<=xmax; ++bx) {
			const int zx = bx - cur_bucket->xmin;
			// zbuffer index, also use for sample coord array & pixelbuffer
			const int zp = zx + zy*bucket_sizeX;
			// current sample coordinate
			const SampleCoords* scp = &sample_coords[zp];
			if (sample_dof) {
				// dof offsets, point 0
				pts[0].x = mp->v0->pp.x + scp->lu*mp->v0->crad;
				pts[0].y = mp->v0->pp.y + scp->lv*mp->v0->crad;
				// point 1
				pts[1].x = mp->v1->pp.x + scp->lu*mp->v1->crad;
				pts[1].y = mp->v1->pp.y + scp->lv*mp->v1->crad;
				// point 2
				pts[2].x = mp->v2->pp.x + scp->lu*mp->v2->crad;
				pts[2].y = mp->v2->pp.y + scp->lv*mp->v2->crad;
				// point 3
				pts[3].x = mp->v3->pp.x + scp->lu*mp->v3->crad;
				pts[3].y = mp->v3->pp.y + scp->lv*mp->v3->crad;
				// pt diffs
				pts[4].x = pts[3].x - pts[0].x;   pts[4].y = pts[3].y - pts[0].y;
				pts[5].x = pts[0].x - pts[1].x;   pts[5].y = pts[0].y - pts[1].y;
				pts[6].x = pts[1].x - pts[2].x;   pts[6].y = pts[1].y - pts[2].y;
				pts[7].x = pts[2].x - pts[3].x;   pts[7].y = pts[2].y - pts[3].y;
			}
			if (sample_motion) {
				// mblur offsets, point 0
				pts[0].x = mp->v0->pp.x + scp->tm*(static_cast<ShadedVertex*>(mp->v0)->ppc.x - mp->v0->pp.x);
				pts[0].y = mp->v0->pp.y + scp->tm*(static_cast<ShadedVertex*>(mp->v0)->ppc.y - mp->v0->pp.y);
				// point 1
				pts[1].x = mp->v1->pp.x + scp->tm*(static_cast<ShadedVertex*>(mp->v1)->ppc.x - mp->v0->pp.x);
				pts[1].y = mp->v1->pp.y + scp->tm*(static_cast<ShadedVertex*>(mp->v1)->ppc.y - mp->v0->pp.y);
				// point 2
				pts[2].x = mp->v2->pp.x + scp->tm*(static_cast<ShadedVertex*>(mp->v2)->ppc.x - mp->v0->pp.x);
				pts[2].y = mp->v2->pp.y + scp->tm*(static_cast<ShadedVertex*>(mp->v2)->ppc.y - mp->v0->pp.y);
				// point 3
				pts[3].x = mp->v3->pp.x + scp->tm*(static_cast<ShadedVertex*>(mp->v3)->ppc.x - mp->v0->pp.x);
				pts[3].y = mp->v3->pp.y + scp->tm*(static_cast<ShadedVertex*>(mp->v3)->ppc.y - mp->v0->pp.y);
				pts[4].x = pts[3].x - pts[0].x;   pts[4].y = pts[3].y - pts[0].y;
				pts[5].x = pts[0].x - pts[1].x;   pts[5].y = pts[0].y - pts[1].y;
				pts[6].x = pts[1].x - pts[2].x;   pts[6].y = pts[1].y - pts[2].y;
				pts[7].x = pts[2].x - pts[3].x;   pts[7].y = pts[2].y - pts[3].y;
			}
			// see comments above in drawMP(), changed to _gQuad()...
			if (point_In_gQuad(pts, scp->jx+bx, scp->jy+by)) {
				quadIntpol(pts, Point2(scp->jx+bx, scp->jy+by), wts);
				const float zval = wts[0]*pts[0].z + wts[1]*pts[1].z + wts[2]*pts[2].z + wts[3]*pts[3].z;
				if ((zval < opts.nearClip) || (zval > opts.farClip)) continue;
#ifndef USE_HZBUF
				if (zval < zbuf[zp]) {
#else
				if (!hzbuf->pointOccluded(zx, zy, zval)) {
#endif
					float op[3], *sdat = &pixbuf[zp << 2];
					//sdat[0] = (1.f-op0.r)*sdat[0] + op0.r*cl0.r;
					//sdat[1] = (1.f-op0.g)*sdat[1] + op0.g*cl0.g;
					//sdat[2] = (1.f-op0.b)*sdat[2] + op0.b*cl0.b;
					// color interpolation only needed when ShadingInterpolation == 'smooth'
					// opacity
					op[0] = wts[0]*op0.r + wts[1]*op1.r + wts[2]*op2.r + wts[3]*op3.r;
					op[1] = wts[0]*op0.g + wts[1]*op1.g + wts[2]*op2.g + wts[3]*op3.g;
					op[2] = wts[0]*op0.b + wts[1]*op1.b + wts[2]*op2.b + wts[3]*op3.b;
					// color, taking opacity into account
					// this for premul in shader (Oi=Os, Ci=Os*Cs)
					sdat[0] = (1.f-op[0])*sdat[0] + (wts[0]*cl0.r + wts[1]*cl1.r + wts[2]*cl2.r + wts[3]*cl3.r);
					sdat[1] = (1.f-op[1])*sdat[1] + (wts[0]*cl0.g + wts[1]*cl1.g + wts[2]*cl2.g + wts[3]*cl3.g);
					sdat[2] = (1.f-op[2])*sdat[2] + (wts[0]*cl0.b + wts[1]*cl1.b + wts[2]*cl2.b + wts[3]*cl3.b);
					// alpha, using opacity average, not sure if this is ok
					const float oa = (op[0] + op[1] + op[2])*(1.f/3.f);
					sdat[3] = (1.f-oa)*sdat[3] + oa;
				}
			} // pointinpatch
		} // bx
	}  // by
}

//--------------------------------------------------------------------------------------------------
// Specialized functions to rasterize point (particle) micropolygons for perfect circles

inline bool point_In_Circle(const Point3& ct, float radsq, float u, float v, float asp)
{
	const float dx = ct.x - u, dy = (ct.y - v)*asp;
	return ((dx*dx + dy*dy) <= radsq);
}

void ZbufferHider::drawMP_point(const MicroPolygon* mp)
{
	Options& opts = State::Instance()->topOptions();
	// z is constant here
	const float zval = mp->zmin;
	if ((zval < opts.nearClip) || (zval > opts.farClip)) return;

	mp_drawn++;
	empty_pixbuf = false;

	// clip to current bucket bound
	const int xmin = MAX2(cur_bucket->xmin, mp->xmin),
	          ymin = MAX2(cur_bucket->ymin, mp->ymin),
	          xmax = MIN2(cur_bucket->xmax, mp->xmax),
	          ymax = MIN2(cur_bucket->ymax, mp->ymax);

	// color
	Color col0(static_cast<ShadedVertex*>(mp->v0)->Ci);

	// factor to account for possibly different sampling rates in each direction
	const float asp = xsamples/float(ysamples);

	for (int by=ymin; by<=ymax; ++by) {
		const int zy = by - cur_bucket->ymin;
		for (int bx=xmin; bx<=xmax; ++bx) {
			const int zx = bx - cur_bucket->xmin;
			const int zp = zx + zy*bucket_sizeX;
			const SampleCoords* scp = &sample_coords[zp];
			Point3 ct(mp->v0->pp);
			if (sample_dof) {
				ct.x += scp->lu*mp->v0->crad;
				ct.y += scp->lv*mp->v0->crad;
			}
			if (sample_motion) {
				// mblur offsets, point 0
				ct.x += scp->tm*(static_cast<ShadedVertex*>(mp->v0)->ppc.x - ct.x);
				ct.y += scp->tm*(static_cast<ShadedVertex*>(mp->v0)->ppc.y - ct.y);
			}
			if (point_In_Circle(ct, mp->radsq, scp->jx + bx, scp->jy + by, asp)) {
#ifndef USE_HZBUF
				if (zval < zbuf[zp]) {
					zbuf[zp] = zval;
#else
				if (!hzbuf->pointOccluded(zx, zy, zval)) {
				  hzbuf->updatePoint(zx, zy, zval);
#endif
					float* sdat = &pixbuf[zp << 2];
					sdat[0] = col0.r, sdat[1] = col0.g, sdat[2] = col0.b, sdat[3] = 1.f;
				}
			}
		}
	}
}


void ZbufferHider::drawMP_transp_point(const MicroPolygon* mp)
{
	Options& opts = State::Instance()->topOptions();
	// z is constant here
	const float zval = mp->zmin;
	if ((zval < opts.nearClip) || (zval > opts.farClip)) return;

	mp_drawn++;
	empty_pixbuf = false;

	// clip to current bucket bound
	const int xmin = MAX2(cur_bucket->xmin, mp->xmin),
	          ymin = MAX2(cur_bucket->ymin, mp->ymin),
	          xmax = MIN2(cur_bucket->xmax, mp->xmax),
	          ymax = MIN2(cur_bucket->ymax, mp->ymax);

	// color
	Color col0(static_cast<ShadedVertex*>(mp->v0)->Ci);
	// opacity
	Color op0(static_cast<ShadedVertex*>(mp->v0)->Oi);

	// factor to account for possibly different sampling rates in each direction
	const float asp = xsamples/float(ysamples);

	for (int by=ymin; by<=ymax; ++by) {
		const int zy = by - cur_bucket->ymin;
		for (int bx=xmin; bx<=xmax; ++bx) {
			const int zx = bx - cur_bucket->xmin;
			const int zp = zx + zy*bucket_sizeX;
			const SampleCoords* scp = &sample_coords[zp];
			Point3 ct(mp->v0->pp);
			if (sample_dof) {
				ct.x += scp->lu*mp->v0->crad;
				ct.y += scp->lv*mp->v0->crad;
			}
			if (sample_motion) {
				// mblur offsets, point 0
				ct.x += scp->tm*(static_cast<ShadedVertex*>(mp->v0)->ppc.x - ct.x);
				ct.y += scp->tm*(static_cast<ShadedVertex*>(mp->v0)->ppc.y - ct.y);
			}
			if (point_In_Circle(ct, mp->radsq, scp->jx + bx, scp->jy + by, asp)) {
#ifndef USE_HZBUF
				if (zval < zbuf[zp]) {
#else
				if (!hzbuf->pointOccluded(zx, zy, zval)) {
#endif
					float *sdat = &pixbuf[zp << 2];
					sdat[0] = (1.f - op0.r)*sdat[0] + col0.r;
					sdat[1] = (1.f - op0.g)*sdat[1] + col0.g;
					sdat[2] = (1.f - op0.b)*sdat[2] + col0.b;
					const float oa = (op0.r + op0.g + op0.b)*(1.f/3.f);
					sdat[3] = (1.f - oa)*sdat[3] + oa;
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

void ZbufferHider::hide(MicroPolygonGrid &g)
{
	// first draw any forwarded mp's for current bucket (opaque only here!).
	// done here because it is needed for occlusion cull test in firstPrim()
	draw_forwarded(true);

	// bust the new mpgrid
	vector<MicroPolygon*> mpL;
	// have to handle pointgrid slightly differently
	bool pointgrid = (g.gtype == MicroPolygonGrid::G_POINTS);
	g.bust(mpL, no_shading);
	if (mpL.empty()) return;

	total_grids++;
	total_mps += (unsigned int)mpL.size();

	vector<MicroPolygon*>::iterator mi;

	// current attributes
	const Primitive* par = g.get_parent();
	const Attributes* attr = par->getAttributeReference();

	// assign mp's to the buckets that contain it.
	// for the current bucket, opaque mp's can be drawn immediately,
	// any transparent mp's are collected first.
	for (mi=mpL.begin(); mi!=mpL.end(); ++mi) {

		MicroPolygon* mp = *mi;
		if (sample_dof) {
			// adjust 2D bound for DoF if needed (bound already includes moving points, done in bust())
			const int xt_dof = CEILI(MAX2(CoC_radius(mp->zmin), CoC_radius(mp->zmax)));
			mp->xmin -= xt_dof;
			mp->ymin -= xt_dof;
			mp->xmax += xt_dof;
			mp->ymax += xt_dof;
			// calculate the CoC radius for each quad vertex if not done yet
			if (mp->v0->crad == -1.f) mp->v0->crad = CoC_radius(mp->v0->pp.z);
			if (not pointgrid) {
				if (mp->v1->crad == -1.f) mp->v1->crad = CoC_radius(mp->v1->pp.z);
				if (mp->v2->crad == -1.f) mp->v2->crad = CoC_radius(mp->v2->pp.z);
				if (mp->v3->crad == -1.f) mp->v3->crad = CoC_radius(mp->v3->pp.z);
			}
		}

		bool imm_draw = false;
		for (BucketIterator bk=cur_bucket; bk!=buckets.end(); ++bk) {
			if ((mp->xmax < bk->xmin) || (bk->xmax < mp->xmin) ||
					(mp->ymax < bk->ymin) || (bk->ymax < mp->ymin)) continue;
			// opaque mp's for the current bucket are processed and discarded immediately,
			// unless also visible in other buckets
			const float opsum = pointgrid
			                    ?
			                    (static_cast<ShadedVertex*>(mp->v0)->Oi[0] + static_cast<ShadedVertex*>(mp->v0)->Oi[1] + static_cast<ShadedVertex*>(mp->v0)->Oi[2])
			                    :
			                    0.25f*((static_cast<ShadedVertex*>(mp->v0)->Oi[0] + static_cast<ShadedVertex*>(mp->v1)->Oi[0] +
			                            static_cast<ShadedVertex*>(mp->v2)->Oi[0] + static_cast<ShadedVertex*>(mp->v3)->Oi[0]) +
			                           (static_cast<ShadedVertex*>(mp->v0)->Oi[1] + static_cast<ShadedVertex*>(mp->v1)->Oi[1] +
			                            static_cast<ShadedVertex*>(mp->v2)->Oi[1] + static_cast<ShadedVertex*>(mp->v3)->Oi[1]) +
			                           (static_cast<ShadedVertex*>(mp->v0)->Oi[2] + static_cast<ShadedVertex*>(mp->v1)->Oi[2] +
			                            static_cast<ShadedVertex*>(mp->v2)->Oi[2] + static_cast<ShadedVertex*>(mp->v3)->Oi[2]));
			if (bk==cur_bucket) {
				curbucket_mp++;
				// NOTE depth only renders really still should do shading so that opacity info is available,
				// then the depth value can also properly take full opacity clipped geometry into account TODO
				// for now have to consider depth renders fully opaque
				if (only_z or (opsum >= attr->opacity_threshold)) {
					drawMP(mp); // opaque
					imm_draw = true;
					// refcount is not incremented so that after this loop it will be deleted below
				}
				else {
					mp->incRefCount();
					transp_mps.push_back(mp); // transparent, collect
				}
				continue;
			}
			// for other buckets, incr. refcount & add to either the opaque- or transp-list
			mp->incRefCount();
			if (only_z or (opsum >= attr->opacity_threshold))
				bk->mps->push_back(mp);  // opaque
			else
				bk->Tmps->push_back(mp); // transparent
		}

		// if mp at this point still has a refcount of 0,
		// then it was not assigned to any bucket, so can be deleted.
		// NOTE: this also includes mp's that were actually drawn above if possible, so mp_cull is not increased in that case.
		if (mp->getRefCount()==0) {
			if (!imm_draw) mp_culled++, curbucket_mp_cull++;
			delete mp;
			mp_del++;
		}

	}

}


// smallest useful gridsize possible wrt memory usage, according to old pixar prman manual,
// is bucketsize^2 / shadingrate, less than that in general won't improve memory usage much anymore
// (though in practice minor gains are still possible, but this als depends on dicing criterion I suppose)
// too small grids will also take longer to render.
// as far as rendertime is concerned, optimal seems to be
// around 1/4 or 1/8 * (bucketsize^2 /shadingRate)
unsigned int ZbufferHider::gridSize()
{
	// diceable() tests number of faces,
	// for binary dicing it uses power of 2 sizes, so vertices = (xdim+1)*(ydim+1), -> 32 faces = 33*33 verts
	return gridsize;
}


// return area of projected bound, not used (yet)
float ZbufferHider::rasterEstimate(const Bound &b)
{
	return 1;
}


// return pixel distance between the two projected points
float ZbufferHider::rasterEstimate(const Point3 &p0, const Point3 &p1)
{
	const Point3 pp0 = lcam.project(p0), pp1 = lcam.project(p1);
	const float xd = pp1.x - pp0.x, yd = pp1.y - pp0.y;
	// take higher sampling resolution into account
	return sqrt(xd*xd*xsam2div + yd*yd*ysam2div);
}

__END_QDRENDER

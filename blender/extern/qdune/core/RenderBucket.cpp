#include "RenderBucket.h"

#include "Mathutil.h"

#include <algorithm>

__BEGIN_QDRENDER

// bucket initialization macro
#define BUCKET_INIT {\
	Bucket* bk = &buckets[bkn++];\
	bk->xmin = bk_x*bucket_sizeX - X_xt_smp;\
	bk->ymin = bk_y*bucket_sizeY - Y_xt_smp;\
	if (bk_x==borderX)\
		bk->xmax = bk_x*bucket_sizeX + (xsize - borderX*bucket_sizeX) - 1;\
	else\
		bk->xmax = (bk_x + 1)*bucket_sizeX - 1;\
	if (bk_y==borderY)\
		bk->ymax = bk_y*bucket_sizeY + (ysize - borderY*bucket_sizeY) - 1;\
	else\
		bk->ymax = (bk_y + 1)*bucket_sizeY - 1;\
	bk->xmax += X_xt_smp;\
	bk->ymax += Y_xt_smp;\
	bk->prims = new std::deque<Primitive*>;\
	bk->mps = new std::vector<MicroPolygon*>;\
	bk->Tmps = new std::vector<MicroPolygon*>;\
}

//------------------------------------------------------------------------------
// For Hilbert/Peano Curve buckettraversal, based on code from GGems,
// here stepsize is always 1
#define hp_step(angle) {\
	while (angle > 270) angle -= 360;\
	while (angle <   0) angle += 360;\
	if      (angle == 0)   bk_x++;\
	else if (angle == 90)  bk_y++;\
	else if (angle == 180) bk_x--;\
	else if (angle == 270) bk_y--;\
	if ((bkn < (bk_maxX*bk_maxY)) &&\
			(bk_x >= 0) && (bk_y >= 0) && (bk_x < bk_maxX) && (bk_y < bk_maxY)) BUCKET_INIT\
}
void RenderBucket::hilbert(std::vector<Bucket>& buckets, int orient, int& angle, int level)
{
	if ((bkn >= (bk_maxX*bk_maxY)) || (level-- <= 0)) return;
	angle += orient * 90;
	hilbert(buckets, -orient, angle, level);
	hp_step(angle);
	angle -= orient * 90;
	hilbert(buckets, orient, angle, level);
	hp_step(angle);
	hilbert(buckets, orient, angle, level);
	angle -= orient * 90;
	hp_step(angle);
	hilbert(buckets, -orient, angle, level);
	angle += orient * 90;
}

void RenderBucket::peano(std::vector<Bucket>& buckets, int orient, int& angle, int level)
{
	if ((bkn >= (bk_maxX*bk_maxY)) || (level-- <= 0)) return;
	peano(buckets, orient, angle, level);
	hp_step(angle);
	peano(buckets, -orient, angle, level);
	hp_step(angle);
	peano(buckets, orient, angle, level);
	angle -= orient * 90;
	hp_step(angle);
	angle -= orient * 90;
	peano(buckets, -orient, angle, level);
	hp_step(angle);
	peano(buckets, orient, angle, level);
	hp_step(angle);
	peano(buckets, -orient, angle, level);
	angle += orient * 90;
	hp_step(angle);
	angle += orient * 90;
	peano(buckets, orient, angle, level);
	hp_step(angle);
	peano(buckets, -orient, angle, level);
	hp_step(angle);
	peano(buckets, orient, angle, level);
}
#undef hp_step

//---------------------------------------------------------------------------
void RenderBucket::spiralN(int n, int nx, int ny, int &bkx, int &bky)
{
	const int center = (MIN2(nx, ny) - 1) >> 1;
	while (n < (nx*ny)) { nx--;  ny--; }
	const int nxny = nx*ny;
	const int mnxny = MIN2(nx, ny);
	const int mhm = -(mnxny >> 1);
	if (mnxny & 1) {
		if (n <= (nxny+ny)) {
			bkx = nx + mhm;
			bky = mhm + n - nxny;
		}
		else {
			bkx = nx + mhm - (n - (nxny + ny));
			bky = ny + mhm;
		}
	}
	else {
		if (n <= (nxny+ny)) {
			bkx = mhm;
			bky = ny + mhm - (n - nxny);
		}
		else {
			bkx = mhm + (n - (nxny + ny));
			bky = mhm;
		}
	}
	bkx += center;
	bky += center;
}

//------------------------------------------------------------------------------
// 'sweep' pattern order,
// depending on aspect ratio,
// either top to bottom: left to right for even, right to left for odd rows,
// or left to right: top to bottom for even, bottom to top for odd columns.
// Besides image aspect ratio, the width/height ratio of objects in the image
// also have a significant impact on memory use, therefore definitely should
// make bucket traversal method a user option.
void RenderBucket::SweepOrder(std::vector<Bucket>& buckets)
{
	buckets.resize(bk_maxX * bk_maxY);
	if (width < height) {
		// top to bottom
		for (bk_y=0; bk_y<bk_maxY; ++bk_y)
			if (bk_y & 1)
				for (bk_x=bk_maxX-1; bk_x>=0; --bk_x) BUCKET_INIT
			else
				for (bk_x=0; bk_x<bk_maxX; ++bk_x) BUCKET_INIT
	}
	else {
		// left to right
		for (bk_x=0; bk_x<bk_maxX; ++bk_x)
			if (bk_x & 1)
				for (bk_y=bk_maxY-1; bk_y>=0; --bk_y) BUCKET_INIT
			else
				for (bk_y=0; bk_y<bk_maxY; ++bk_y) BUCKET_INIT
	}
}

//------------------------------------------------------------------------------
// simple standard scanline order, top to bottom, left to right
// slightly worse memory usage than sweep
void RenderBucket::ScanOrder(std::vector<Bucket>& buckets)
{
	buckets.resize(bk_maxX * bk_maxY);
	for (bk_y=0; bk_y<bk_maxY; ++bk_y)
	  for (bk_x=0; bk_x<bk_maxX; ++bk_x) BUCKET_INIT
}

//------------------------------------------------------------------------------
// hilbert/peano curve traversal, uses more memory than simple scanline
// peano traversal seems to be faster though
void RenderBucket::HilbertPeanoOrder(std::vector<Bucket>& buckets, bool _hilbert)
{
	buckets.resize(bk_maxX * bk_maxY);
	int init_angle = 0;
	// init of first bucket, both hilbert & peano skip it
	BUCKET_INIT
	if (_hilbert) {
		const int level = ilog2_roundup(MAX2(bk_maxX, bk_maxY));
		hilbert(buckets, 1, init_angle, level);
	}
	else { // peano
		const int level = int(1 + log(MAX2(bk_maxX, bk_maxY))/log(3));
		peano(buckets, -1, init_angle, level);
	}
}

//------------------------------------------------------------------------------
// spiral order, from pr book, uses more memory than hilbert/peano, also slower
void RenderBucket::SpiralOrder(std::vector<Bucket>& buckets, bool out2in)
{
	buckets.resize(bk_maxX * bk_maxY);
	if (out2in) {	// outward to inward
		for (int tb=(bk_maxX*bk_maxY-1); tb>=0; --tb) {
			spiralN(tb, bk_maxX, bk_maxY, bk_x, bk_y);
			BUCKET_INIT
		}
	}
	else {	// inward to outward
		for (int tb=0; tb<(bk_maxX*bk_maxY); ++tb) {
			spiralN(tb, bk_maxX, bk_maxY, bk_x, bk_y);
			BUCKET_INIT
		}
	}
}

//------------------------------------------------------------------------------
// totally useless, just as test, random bucket traversal,
// *really* bad memory coherence...
void RenderBucket::RandomOrder(std::vector<Bucket>& buckets)
{
	buckets.resize(bk_maxX * bk_maxY);
	std::vector<int> tv(bk_maxX*bk_maxY);
	for (bk_x=0; bk_x<(int)tv.size(); ++bk_x) tv[bk_x]=bk_x;
	std::random_shuffle(tv.begin(), tv.end());
	for (int i=0;i<(int)tv.size();++i) {
		bk_x = tv[i] % bk_maxX, bk_y = tv[i] / bk_maxX;
		BUCKET_INIT
	}
}

#undef BUCKET_INIT

__END_QDRENDER

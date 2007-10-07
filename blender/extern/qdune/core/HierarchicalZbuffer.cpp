//----------------------------------------------------------------------------------------------
// Simple Hierarchical Zbuffer implementation for occlusion culling.
// Always square, side length must be a power of 2. To avoid having to check this,
// the contructor must be called with the log2 of the side length of the lowest main level.
// maximum possible sidelength is 32768 (maxlog2 == 15)
// It is not adapted yet to work with size != pow of 2.
//----------------------------------------------------------------------------------------------
// NOTE:
// Using a hierarchical zbuffer didn't seem to be of any advantage,
// it's slower (not by much though), but also uses more memory...
// So either I'm doing something wrong (not unlikely of course),
// or my implementation is not efficient enough...
// (recursion & all those overlap tests probably don't help)
// In any case, the code is still here if anyone knows how to do it better.
// Of course other occlusion culling methods might be more suitable.
// minor update: now a tiny bit faster than zbuf in some cases for high depth complexity,
// otherwise, for most scenes, still slower...
// (failed to make a non-recursive version sofar... code removed)

#include "HierarchicalZbuffer.h"
#include "Mathutil.h"

#include "QDRender.h"
__BEGIN_QDRENDER

HierarchicalZbuffer::HierarchicalZbuffer(unsigned int maxlog2)
{
	zLayer = 0;

	// maxlog2 must be < 16
	//assert(maxlog2<16)

	sideLen = 1 << maxlog2;
	levels = maxlog2+1;

	// allocate array of layers
	zLayer = new float*[levels];

	// size in bytes of all required levels combined
	totalSize = 0x55555555 & ((2 << (maxlog2 << 1)) - 1);
	// allocate the pyramid
	zLayer[0] = new float[totalSize];

	// from that, assign the next pointers to the individual layers
	unsigned int size = sideLen*sideLen;
	for (unsigned int i=1; i<levels; ++i) {
		zLayer[i] = zLayer[i-1] + size;
		size >>= 2;
	}
	// initialize all to maximum depth
	initLayers();
}

HierarchicalZbuffer::~HierarchicalZbuffer()
{
	if (zLayer) {
		// only need to delete zLayer[0];
		if (zLayer[0]) delete[] zLayer[0];
		delete[] zLayer;
	}
}

// initialize all layers
void HierarchicalZbuffer::initLayers()
{
	for (unsigned int i=0; i<totalSize; ++i)
		zLayer[0][i] = 1e10f;
}

//------------------------------------------------------------------------------
// Point Occlusion testing

// tests the hzbuf. at coord. (x, y), for points, only need to test main layer
bool HierarchicalZbuffer::pointOccluded(int x, int y, float zval) const
{
	if ((x<0) || (y<0) || (x>=sideLen) || (y>=sideLen)) return false;
	// if zval > z value at main level, occlusion is true
	return (zval > zLayer[0][x + (y << (levels-1))]);
}

// updates all layers for coord(x, y) (coord of main level)
void HierarchicalZbuffer::updatePoint(int x, int y, float zval)
{
	if ((x<0) || (y<0) || (x>=sideLen) || (y>=sideLen)) return;

	// update first layer
	int sl = levels-1;  // y mult. factor (as shift)
	zLayer[0][x + (y << sl)] = zval;

	// starting from second layer, test the maximum z value of previous level 2x2 sub square
	// to check if update needed for next level, continue until no more update needed
	for (unsigned int i=1; i<levels; ++i) {
		// x, y coords for next level
		x >>= 1;
		y >>= 1;
		// previous level 2x2 subsquare coords
		const int x1 = x << 1, y1 = y << 1,
							x2 = x1 + 1, y2 = y1 + 1, im = i-1;
		// max z of subsquare
		const float maxz = MAX4(zLayer[im][x1 + (y1 << sl)], zLayer[im][x2 + (y1 << sl)],
														zLayer[im][x2 + (y2 << sl)], zLayer[im][x1 + (y2 << sl)]);
		// if no update needed in next layer (maximum did not change), nothing to do further
		if (sl!=0) sl--;
		if (maxz==zLayer[i][x + (y << sl)]) return;
		// otherwise update, and test next layer
		zLayer[i][x + (y << sl)] = maxz;
	}
}

//------------------------------------------------------------------------------
// (2D) Bound occlusion testing

bool HierarchicalZbuffer::bOcc(const Bound2D& b, float zval, int curlev, int cell) const
{
	if (zval >= zLayer[curlev][cell]) return true;

	// if max level reached, no occlusion
	if (--curlev < 0) return false;

	// check for overlap with child cells, recurse if so
	// 0 | 1
	// -----
	// 2 | 3

	// log2 width in new cell
	const int w = (levels - 1) - curlev;
	// log2 width in old cell, half new cell width
	const int hw = w - 1;
	// new cell start coords x, y = old cell x*2, y*2
	int x = (cell & ((1 << hw)-1)) << 1, y = (cell >> hw) << 1;

	// new cells (x,y), (x,y+1)
	const int c0 = x + (y << w), c2 = c0 + (1 << w);

	// current cell bound max width/height
	const int cbm = (1 << curlev) - 1;

	// cell 0, (x, y)
	int xmin = x << curlev, ymin = y << curlev;
	int xmax = xmin + cbm, ymax = ymin + cbm;
	if ((xmax >= b.xmin) && (b.xmax >= xmin) && (ymax >= b.ymin) && (b.ymax >= ymin))
		if (!bOcc(b, zval, curlev, c0)) return false;

	// cell 1, (x+1, y)
	xmin += cbm + 1;
	xmax = xmin + cbm;
	if ((xmax >= b.xmin) && (b.xmax >= xmin) && (ymax >= b.ymin) && (b.ymax >= ymin))
		if (!bOcc(b, zval, curlev, c0+1)) return false;

	// cell 3 (x+1, y+1)
	ymin += cbm + 1;
	ymax = ymin + cbm;
	if ((xmax >= b.xmin) && (b.xmax >= xmin) && (ymax >= b.ymin) && (b.ymax >= ymin))
		if (!bOcc(b, zval, curlev, c2+1)) return false;

	// cell 2 (x, y+1)
	xmin -= cbm + 1;
	xmax = xmin + cbm;
	if ((xmax >= b.xmin) && (b.xmax >= xmin) && (ymax >= b.ymin) && (b.ymax >= ymin))
		if (!bOcc(b, zval, curlev, c2)) return false;

	return true;
}

bool HierarchicalZbuffer::boundOccluded(const Bound2D &b, float zval) const
{
	return bOcc(b, zval, levels-1, 0);
}

__END_QDRENDER

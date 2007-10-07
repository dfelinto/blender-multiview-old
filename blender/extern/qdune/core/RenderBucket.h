#ifndef _RENDERBUCKET_H
#define _RENDERBUCKET_H

#include <deque>
#include <vector>

#include "QDRender.h"
__BEGIN_QDRENDER

class Primitive;
class MicroPolygon;
struct Bucket
{
	int xmin, ymin, xmax, ymax;        // 2D bucket bound
	std::deque<Primitive*>* prims;     // references to primitives intersecting this bucket
	std::vector<MicroPolygon*>* mps;   // references to micropolygons intersecting this bucket
	std::vector<MicroPolygon*>* Tmps;  // same for transparent mpoly's
};

class RenderBucket
{
public:
	RenderBucket(int _width, int _height, int _xsize, int _ysize,
		int _bucket_sizeX, int _bucket_sizeY, int _X_xt_smp, int _Y_xt_smp)
	: width(_width), height(_height), xsize(_xsize), ysize(_ysize),
		bucket_sizeX(_bucket_sizeX), bucket_sizeY(_bucket_sizeY), X_xt_smp(_X_xt_smp), Y_xt_smp(_Y_xt_smp),
		bk_x(0), bk_y(0),	// bucket coords
		bkn(0),	// bucket counter
		bk_maxX(xsize / bucket_sizeX), bk_maxY(ysize / bucket_sizeY),	// bucket x & y maximum in bucket coordinates
		borderX(-1), borderY(-1)	
	{
		// account for partial bucket overlap at image borders
		if ((xsize % bucket_sizeX) != 0) { borderX=bk_maxX;  bk_maxX++; }
		if ((ysize % bucket_sizeY) != 0) { borderY=bk_maxY;  bk_maxY++; }
	}
	// traversal methods, in order of best to worst memory usage
	void SweepOrder(std::vector<Bucket>& buckets);
	void ScanOrder(std::vector<Bucket>& buckets);
	void HilbertPeanoOrder(std::vector<Bucket>& buckets, bool _hilbert=false);
	void SpiralOrder(std::vector<Bucket>& buckets, bool out2in=false);
	void RandomOrder(std::vector<Bucket>& buckets);
protected:
	void hilbert(std::vector<Bucket>& buckets, int orient, int& angle, int level);
	void peano(std::vector<Bucket>& buckets, int orient, int& angle, int level);
	void spiralN(int n, int nx, int ny, int &bkx, int &bky);

	int width, height, xsize, ysize, bucket_sizeX, bucket_sizeY, X_xt_smp, Y_xt_smp;
	int bk_x, bk_y, bkn, bk_maxX, bk_maxY, borderX, borderY;
};

__END_QDRENDER

#endif	// _RENDERBUCKET_H

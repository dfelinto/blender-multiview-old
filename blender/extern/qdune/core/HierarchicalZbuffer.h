#ifndef _HIERARCHICALZBUFFER_H
#define _HIERARCHICALZBUFFER_H

#include "QDRender.h"
__BEGIN_QDRENDER

struct Bound2D;

class HierarchicalZbuffer
{
public:
	// ctor
	HierarchicalZbuffer(unsigned int maxlog2);
	// dtor
	~HierarchicalZbuffer();

	// mtds
	// initialize all layers
	void initLayers();
	// tests the hzbuf. point (x, y)
	bool pointOccluded(int x, int y, float zval) const;
	// updates all layers for coord(x, y) (coord of main level)
	void updatePoint(int x, int y, float zval);
	// as above, but for 2d bound
	bool boundOccluded(const Bound2D &b, float zval) const;

	// for reading the maximum level, read-only
	const float* maxLayer() const { return zLayer[0]; }

protected:
	float** zLayer; // the main pyramid of zbuffers

	unsigned int totalSize, levels;
	int sideLen;

	// recursive sub section occlusion test
	bool bOcc(const Bound2D &b, float zval, int curlev, int cell) const;
};

__END_QDRENDER

#endif // _HIERARCHICALZBUFFER_H

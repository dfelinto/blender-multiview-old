#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include "Options.h"

#include "QDRender.h"
__BEGIN_QDRENDER

struct Quantizer
{
	int one, maximum, minimum;
	float dither_amplitude;
};

class Transform;
class FrameBuffer
{
public:
	// ctor
	FrameBuffer(const Options& options);
	// dtor
	~FrameBuffer();

	// mtds
	void operator()(int x, int y, float rgbaz[5]);
	void finalize();
	void toImage(int bx, int by, int bw, int bh, bool draw);
	unsigned int Width() const { return width; }
	unsigned int Height() const { return height; }
	unsigned int xStart() const { return xstart; }
	unsigned int yStart() const { return ystart; } 

protected:
	// internal mtds
	void quantizeImage();
	bool saveImage();
	bool saveZ();	// save z as single channel float

	// data
	enum {TP_FILE, TP_ZFILE, TP_FRAMEBUFFER} type;
	enum {MD_A, MD_Z, MD_AZ, MD_RGB, MD_RGBA, MD_RGBZ, MD_RGBAZ} mode;
	unsigned int dataPerPix, width, height, xstart, ystart;
	float* fbdata; // float ddata (includes Z)
	unsigned char* qdata; // quantized 8bit RGBA data
	// RiExposure
	float gain, gamma;
	// quantizers, depthQ actually not used at all, should remove
	Quantizer colQ, depthQ;
	bool ortho;
	// display name
	char dispname[80];
};

__END_QDRENDER

#endif //_FRAMEBUFFER_H

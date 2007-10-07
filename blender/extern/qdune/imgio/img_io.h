#ifndef _IMG_IO_H
#define _IMG_IO_H

#include "Color.h"

#include "QDRender.h"
__BEGIN_QDRENDER

// IMG_HALF only used for writing exr files
enum imgType_t {IMG_BYTE, IMG_FLOAT, IMG_HALF};

// to simplify code elsewhere, not templated in this case, just uses plain c-style void ptr...
class imgbuf_t
{
private:
	imgbuf_t(const imgbuf_t&);
	imgbuf_t& operator=(const imgbuf_t&);
public:
	imgbuf_t(unsigned int w, unsigned int h, unsigned int c, imgType_t t)
		: width(w), height(h), numchan(c), imgtype(t)
	{
		if (t == IMG_BYTE)
			data = new unsigned char [w*h*c];
		else
			data = new float [w*h*c];
	}
	~imgbuf_t() { if (data) delete[] (unsigned long*)data;  data = NULL; }
	unsigned int Width() const { return width; }
	unsigned int Height() const { return height; }
	unsigned int Channels() const { return numchan; }
	imgType_t Type() const { return imgtype; }
	Color getColor(unsigned int x, unsigned int y) const
	{
		const unsigned int p = (x + y*width)*numchan;
		if (imgtype == IMG_FLOAT) {
			float* fdata = reinterpret_cast<float*>(data);
			if (numchan == 1) return Color(fdata[p]);
			return Color(fdata[p], fdata[p+1], fdata[p+2]);
		}
		unsigned char* cdata = reinterpret_cast<unsigned char*>(data);
		if (numchan == 1) return (1.f/255.f)*Color(cdata[p]);
		return (1.f/255.f)*Color(cdata[p], cdata[p+1], cdata[p+2]);
	}
	void putColor(const Color &c, unsigned int x, unsigned int y)
	{
		const unsigned int p = (x + y*width)*numchan;
		if (imgtype == IMG_FLOAT) {
			float* fdata = reinterpret_cast<float*>(data);
			fdata[p] = c.r;
			if (numchan > 1) fdata[p+1] = c.g, fdata[p+2] = c.b;
		}
		else {
			unsigned char* cdata = reinterpret_cast<unsigned char*>(data);
			cdata[p] = (c.r < 0.f) ? 0 : ((c.r > 1.f) ? 255 : (unsigned char)(255.f*c.r));
			if (numchan > 1) {
				cdata[p+1] = (c.g < 0.f) ? 0 : ((c.g > 1.f) ? 255 : (unsigned char)(255.f*c.g));
				cdata[p+2] = (c.b < 0.f) ? 0 : ((c.b > 1.f) ? 255 : (unsigned char)(255.f*c.b));
			}
		}
	}
	void* getData() const { return data; }
protected:
	unsigned int width, height, numchan;
	imgType_t imgtype;
	void* data;
};

__END_QDRENDER

#endif // _IMG_IO_H

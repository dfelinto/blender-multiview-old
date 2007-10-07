///---------------------------------------------------------------------------
/// RiMakeTexture implementation, converts image to multi-resolution texture
///---------------------------------------------------------------------------
///
/// Currently computes all levels in memory before saving the files,
/// obvious first improvement would be to write after calculating every level,
/// but much better still is to only load the scanlines of the input image that
/// are needed to calculate a filtered/resampled scanline of the target image.
/// This would allow minimal memory usage, and would be able to deal with
/// images larger than available memory.
/// Another option would be to load enough scanlines to make a row of tiles.
/// Also, it currently uses the source image to compute all mipmap levels,
/// which is of course quite slow. There is code that does compute from
/// the previous level, but since some filters (notably unwindowed sinc,
/// unless very wide) will make the image progressively worse because of
/// accumulating errors, it is currently disabled.
///

#include "maketexture.h"
#include "Mathutil.h"
#include "jpeg_img.h"
#include "exr_io.h"

#include <iostream>
#include <vector>
#include <cstring>

#include <ImfTiledOutputFile.h>
#include <ImfChannelList.h>
#include <ImfIntAttribute.h>

__BEGIN_QDRENDER

using namespace std;
using namespace Imf;
using namespace Imath;

//---------------------------------------------------------------------------
// resampling filters
// box, quadratic, cubic, catrom, mitchell & sinc filters don't use width arg

inline float box(float x, float w) { return 1.f; }
//---------------------------------------------------------------------------
// 2nd order (linear) b-spline (aka triangle)
inline float linear(float x, float w)
{
	if ((x = ABS(x/w)) < 1.f) return (1.f - x);
	return 0.f;
}
//---------------------------------------------------------------------------
// 3rd order (quadratic) b-spline (aka bell)
inline float quadratic(float x, float w)
{
	if ((x = ABS(x)) < 0.5f) return 0.75f - x*x;
	if (x < 1.5f) { x -= 1.5f;  return 0.5f*x*x; }
	return 0.f;
}
//---------------------------------------------------------------------------
// 4th order (cubic) b-spline
const float OSX = 1.f/6.f;
const float OTH = 1.f/3.f;
inline float cubic(float x, float w)
{
	if ((x = ABS(x)) < 0.f) return 4.f + x*x*(-6.f-3.f*x)*OSX;
	if (x < 1.f) return 4.f + x*x*(x*3.f - 6.f)*OSX;
	if (x < 2.f) { x=2.f-x;  return x*x*x*OSX; }
	return 0.f;
}
//---------------------------------------------------------------------------
// catmull-rom
inline float catrom(float x, float w)
{
	if ((x = ABS(x)) < 1.f) return 2.f + x*x*(x*3.f - 5.f);
	if (x < 2.f) return 4.f + x*(x*(5.f - x) - 8.f);
	return 0.f;
}
//---------------------------------------------------------------------------
// mitchell/netravali
inline float mitchell(float x, float w)
{
	const float x2 = x*x;
	x = ABS(x);
	if (x < 1.f)
		return (((12.f - 9.f*OTH - 6.f*OTH)*x*x2)
	       +  ((-18.f + 12.f*OTH + 6.f*OTH)*x2)
	       +  (6.f - 2.f*OTH))*OSX;
	else if (x < 2.f)
		return (((-OTH - 6.f*OTH)*x*x2)
		      + ((6.f*OTH + 30.f*OTH)*x2)
		      + ((-12.f*OTH - 48.f*OTH)*x)
		      + (8.f*OTH + 24.f*OTH))*OSX;
	return 0.f;
};
//---------------------------------------------------------------------------
// sinc, infinite unwindowed, and lanczos, windowed version
inline float sinc(float x, float w)
{
	if (x == 0.f) return 1.f;
	x *= (float)M_PI;
	return sinf(x) / x;
}

inline float lanczos(float x, float w)
{
	if ((x = ABS(x)) < w) return sinc(x, w)*sinc(x/w, w);
	return 0.f;
}

//---------------------------------------------------------------------------
// gaussian
inline float gaussian(float x, float w)
{
	x /= w;
	return expf(-4.f*x*x);
}

//-----------------------------------------------------------------------------------
// General Image Resampling, initially partially based on "Filtered Image Rescaling"
// by Dale Schumacher from GrapicsGems III as well as modifications to
// include filter normalization and corrections for continous sampling based
// on Paul Heckbert's 'zoom' program.

//------------------------------------------------------------------------------
// image resampler

struct cbtor_t
{
	cbtor_t():pxl(0), wt(0.f) {}
	int pxl;
	float wt;
};

struct resample_info_t {
	float swidth, twidth;
	int mode_s, mode_t;
	float (*filter)(float, float);
};

// macros to handle different wrap modes

// set filter weights outside image to zero
// (pixels are set to wrap around though, but only to make sure to not access out of range values, which could cause crashes)
#define fweights_Black(clist, mx, wd) {\
	float sum = 0.f;\
	const float center = (i + 0.5f) * cscale;\
	const int left = int(center - fwd_scaled + 0.5f);\
	const int right = int(center + fwd_scaled + 0.5f);\
	vector<cbtor_t>& cbl = clist[i];\
	cbl.clear();\
	if ((right-left)<=0) continue;\
	cbl.resize(right-left);\
	for (CL=cbl.begin(), j=left; j<right; ++j, ++CL) {\
		const float weight = rsmi.filter((j + 0.5f - center) * iascale, wd);\
		CL->pxl = (j<0) ? ((j % mx)+mx) : (j % mx);\
		CL->wt = (j < 0) ? 0.f : weight;\
		sum += CL->wt;\
	}\
	if (sum != 0.f) {\
		sum = 1.f/sum;\
		for (CL=cbl.begin(); CL!=cbl.end(); ++CL)\
			CL->wt *= sum;\
	}\
}

// wrap around borders
#define fweights_Wrap(clist, mx, wd) {\
	float sum = 0.f;\
	const float center = (i + 0.5f) * cscale;\
	const int left = int(center - fwd_scaled + 0.5f);\
	const int right = int(center + fwd_scaled + 0.5f);\
	vector<cbtor_t>& cbl = clist[i];\
	cbl.clear();\
	if ((right-left)<=0) continue;\
	cbl.resize(right-left);\
	for (CL=cbl.begin(), j=left; j<right; ++j, ++CL) {\
		const float weight = rsmi.filter((j + 0.5f - center) * iascale, wd);\
		CL->pxl = (j<0) ? ((j % mx)+mx) : (j % mx);\
		CL->wt = weight;\
		sum += weight;\
	}\
	if (sum != 0.f) {\
		sum = 1.f/sum;\
		for (CL=cbl.begin(); CL!=cbl.end(); ++CL)\
			CL->wt *= sum;\
	}\
}

// clamps to borders of image
#define fweights_Clamp(clist, mx, wd) {\
	float sum = 0.f;\
	const float center = (i + 0.5f) * cscale;\
	const int left = int(center - fwd_scaled + 0.5f);\
	const int right = int(center + fwd_scaled + 0.5f);\
	vector<cbtor_t>& cbl = clist[i];\
	cbl.clear();\
	if ((right-left)<=0) continue;\
	cbl.resize(right-left);\
	for (CL=cbl.begin(), j=left; j<right; ++j, ++CL) {\
		const float weight = rsmi.filter((j + 0.5f - center) * iascale, wd);\
		CL->pxl = (j<0) ? 0 : ((j >= mx) ? (mx - 1) : j);\
		CL->wt = weight;\
		sum += weight;\
	}\
	if (sum != 0.f) {\
		sum = 1.f/sum;\
		for (CL=cbl.begin(); CL!=cbl.end(); ++CL)\
			CL->wt *= sum;\
	}\
}

static void resample_image(imgbuf_t* dbuf, imgbuf_t* sbuf, const resample_info_t& rsmi)
{
	const int src_x = sbuf->Width(), src_y = sbuf->Height();
	const int dst_x = dbuf->Width(), dst_y = dbuf->Height();

	if ((src_x <= 0) || (src_y <= 0) || (dst_x <= 0) || (dst_y <= 0)) return;

	// intermediate scanline
	vector<Color> tempbuf(src_y);

	const float xscale = dst_x / float(src_x);
	const float yscale = dst_y / float(src_y);

	vector<vector<cbtor_t> > clistX(dst_x);
	vector<vector<cbtor_t> > clistY(dst_y);
	vector<cbtor_t>::iterator CL;

	// row weights
	float cscale = 1.f/xscale;
	float ascale = MAX2(1.f, cscale);
	float iascale = 1.f/ascale;
	float fwd_scaled = MAX2(0.5f, ascale*rsmi.swidth);
	int i, j, k;
	if (rsmi.mode_s == 0) {
		for (i=0; i<dst_x; ++i)
			fweights_Black(clistX, src_x, rsmi.swidth);
	}
	else if (rsmi.mode_s == 1) {
		for (i=0; i<dst_x; ++i)
			fweights_Wrap(clistX, src_x, rsmi.swidth);
	}
	else {
		for (i=0; i<dst_x; ++i)
			fweights_Clamp(clistX, src_x, rsmi.swidth);
	}

	// column weights
	cscale = 1.f/yscale;
	ascale = MAX2(1.f, cscale);
	iascale = 1.f/ascale;
	fwd_scaled = MAX2(0.5f, ascale*rsmi.twidth);
	if (rsmi.mode_t == 0) {
		for (i=0; i<dst_y; ++i)
			fweights_Black(clistY, src_y, rsmi.twidth);
	}
	else if (rsmi.mode_t == 1) {
		for (i=0; i<dst_y; ++i)
			fweights_Wrap(clistY, src_y, rsmi.twidth);
	}
	else {
		for (i=0; i<dst_y; ++i)
			fweights_Clamp(clistY, src_y, rsmi.twidth);
	}

	// filter
	for (k=0; k<dst_x; ++k) {

		// H
		for (i=0; i<src_y; ++i) {
			Color drgb(0);
			for (CL=clistX[k].begin(); CL!=clistX[k].end(); ++CL)
				drgb += sbuf->getColor(CL->pxl, i) * CL->wt;
			tempbuf[i] = drgb;
		}

		// V
		for (j=0; j<dst_y; ++j) {
			Color drgb(0);
			for (CL=clistY[j].begin(); CL!=clistY[j].end(); ++CL)
				drgb += tempbuf[CL->pxl] * CL->wt;
			dbuf->putColor(drgb, k, j);
		}

	}

}


// default fixed size, maybe make user-option?
// WARNING, duplicate! also defined in exr_io.cpp
#define TILESIZE 64

template<PixelType TILE_EXRTYPE, typename TILE_VARTYPE, typename imgtype>
void writeTiles(TiledOutputFile& out, imgtype* mipdata,
                unsigned int width, unsigned int height, unsigned int numchan, int L)
{
	for (int tileY=0; tileY<out.numYTiles(L); ++tileY)
		for (int tileX=0; tileX<out.numXTiles(L); ++tileX) {
			// intermediate single tile for conversion
			vector<TILE_VARTYPE> tile(TILESIZE*TILESIZE*numchan);
			for (int ty=0; ty<TILESIZE; ++ty) {
				const unsigned int yp = (tileY*TILESIZE + ty)*width;
				for (int tx=0; tx<TILESIZE; ++tx) {
					const unsigned int p = ((tileX*TILESIZE + tx) + yp)*numchan;
					if (p >= (width*height*numchan)) continue; // skip if out of range (borders) !!!
					tile[(tx + TILESIZE*ty)*numchan] = mipdata[p];
					if (numchan > 1) {
						tile[(tx + TILESIZE*ty)*numchan + 1] = mipdata[p + 1];
						tile[(tx + TILESIZE*ty)*numchan + 2] = mipdata[p + 2];
						if (numchan == 4) tile[(tx + TILESIZE*ty)*numchan + 3] = mipdata[p + 3];
					}
				}
			}
			Box2i range = out.dataWindowForTile(tileX, tileY);
			const int ofs = (-range.min.y*TILESIZE - range.min.x)*numchan;
			FrameBuffer frameBuffer;
			if (numchan == 1)
				frameBuffer.insert("Y", Slice(TILE_EXRTYPE, (char*)&tile[ofs], sizeof(TILE_VARTYPE), sizeof(TILE_VARTYPE)*TILESIZE));
			else {
				frameBuffer.insert("R", Slice(TILE_EXRTYPE, (char*)&tile[ofs], sizeof(TILE_VARTYPE)*numchan, sizeof(TILE_VARTYPE)*TILESIZE*numchan));
				frameBuffer.insert("G", Slice(TILE_EXRTYPE, (char*)&tile[ofs+1], sizeof(TILE_VARTYPE)*numchan, sizeof(TILE_VARTYPE)*TILESIZE*numchan));
				frameBuffer.insert("B", Slice(TILE_EXRTYPE, (char*)&tile[ofs+2], sizeof(TILE_VARTYPE)*numchan, sizeof(TILE_VARTYPE)*TILESIZE*numchan));
				if (numchan == 4)
					frameBuffer.insert("A", Slice(TILE_EXRTYPE, (char*)&tile[ofs+3], sizeof(TILE_VARTYPE)*numchan, sizeof(TILE_VARTYPE)*TILESIZE*numchan));
			}
			out.setFrameBuffer(frameBuffer);
			out.writeTile(tileX, tileY, L);
		}
}


// note: filter argument differs from RiMakeTextureV(), here it is the name of the filter, not the Rifilter function itself
// (std RiFilters are in fact not used here at all, see above)
void makeTexture(const char* pic, const char* tex, RtToken swrap, RtToken twrap,
                 RtToken filtername, RtFloat swidth, RtFloat twidth,
                 RtInt n, RtToken tokens[], RtPointer parms[],
                 bool savehalf, bool forcegray)
{
	assert((pic != NULL) && (tex != NULL));	// just in case...

	resample_info_t rsmi;
	rsmi.swidth = swidth;
	rsmi.twidth = twidth;

	// default 'black' st wrap mode
	rsmi.mode_s = rsmi.mode_t = 0;
	if (!strcmp(swrap, "periodic"))
		rsmi.mode_s = 1;
	else if (!strcmp(swrap, "clamp"))
		rsmi.mode_s = 2;
	else if (strcmp(swrap, "black"))
		cout << "unknown swrap mode \"" << swrap << "\"\n";
	if (!strcmp(twrap, "periodic"))
		rsmi.mode_t = 1;
	else if (!strcmp(twrap, "clamp"))
		rsmi.mode_t = 2;
	else if (strcmp(twrap, "black"))
		cout << "unknown twrap mode \"" << twrap << "\"\n";

	if (!strcmp(filtername, "box"))
		rsmi.filter = box;
	else if (!strcmp(filtername, "triangle"))
		rsmi.filter = linear;
	else if (!strcmp(filtername, "catmull-rom"))
		rsmi.filter = catrom;
	else if (!strcmp(filtername, "b-spline"))
		rsmi.filter = cubic;
	else if (!strcmp(filtername, "mitchell"))
		rsmi.filter = mitchell;
	else if (!strcmp(filtername, "sinc"))
		rsmi.filter = sinc;
	else if (!strcmp(filtername, "lanczos"))
		rsmi.filter = lanczos;
	else if (!strcmp(filtername, "gaussian"))
		rsmi.filter = gaussian;
	else {
		cout << "Unknown filter: \"" << filtername << "\"\n";
		// use mitchell default
		rsmi.filter = mitchell;
	}

	imgbuf_t* img = NULL;
	// for now detects image type by extension only...
	const char* ext = strchr(pic, '.');
	if (!strcasecmp(ext, ".jpg"))
		img = loadJPG(pic);
	else if (!strcasecmp(ext, ".exr"))
		img = loadEXR(pic, forcegray);
	/* TODO
	else if (!strncasecmp(ext, ".tif", 4))
		img = loadTIFF(pic);
	else if (!strcasecmp(ext, ".hdr"))
		img = loadHDR(pic);
	else if (!strcasecmp(ext, ".tga"))
		img = loadTGA(pic);
	*/
	else {
		cout << "Unknown image type (has extension: '" << ext << "')\n";
		return;
	}

	if (img == NULL) {
		cout << "Could not load image\n";
		return;
	}

	// for regular mipmaps, width must be equal to height, choose largest,
	// this will cause the first level to be upsampled in one direction if needed, that way no detail is lost
	const unsigned int wd = MAX2(img->Width(), img->Height());
	const unsigned int ht = wd;
	// number of levels until 1 X 1
	const unsigned int max_mipLOD = ilog2(MAX2(wd, ht)) + 1;
	const unsigned int numchan = img->Channels();
	const imgType_t imgtype = img->Type();

	unsigned w2 = wd, h2 = ht;

	// output to tiled exr
	try
	{
		// initial level
		imgbuf_t* level0 = new imgbuf_t(w2, h2, numchan, imgtype);
		resample_image(level0, img, rsmi);
		Header header(level0->Width(), level0->Height());
		header.insert("QD_texturemap", IntAttribute(0));
		PixelType pt = (level0->Type() == IMG_FLOAT) ? FLOAT : ((level0->Type() == IMG_HALF) ? HALF : UINT);
		if ((pt == FLOAT) && savehalf) pt = HALF;
		if ((numchan == 1) || forcegray)
			header.channels().insert("Y", Channel(pt));
		else {
			header.channels().insert("R", Channel(pt));
			header.channels().insert("G", Channel(pt));
			header.channels().insert("B", Channel(pt));
			if (numchan == 4) header.channels().insert("A", Channel(pt));
		}
		header.setTileDescription(TileDescription(TILESIZE, TILESIZE, MIPMAP_LEVELS, ROUND_DOWN));
		TiledOutputFile out(tex, header);
		for (unsigned int L=0; L<max_mipLOD; ++L)
		{
			imgbuf_t* miplevel;
			if (L == 0)
				miplevel = level0;
			else {
				w2 >>= 1, h2 >>= 1;
				miplevel = new imgbuf_t(w2, h2, numchan, imgtype);
				resample_image(miplevel, img, rsmi);
			}
			const unsigned int width = miplevel->Width();
			const unsigned int height = miplevel->Height();
			cout << "Level resolution: " << width << " X " << height << ", ";
			cout << "EXR ROUND_DOWN resolution: " << out.levelWidth(L) << " X " << out.levelHeight(L) << endl;
			if (miplevel->Type() == IMG_FLOAT) {
				if (savehalf)
					writeTiles<HALF, half, float>(out, reinterpret_cast<float*>(miplevel->getData()), width, height, numchan, L);
				else
					writeTiles<FLOAT, float, float>(out, reinterpret_cast<float*>(miplevel->getData()), width, height, numchan, L);
			}
			else if (miplevel->Type() == IMG_HALF)
				writeTiles<HALF, half, half>(out, reinterpret_cast<half*>(miplevel->getData()), width, height, numchan, L);
			else
				writeTiles<UINT, unsigned int, unsigned char>(out, reinterpret_cast<unsigned char*>(miplevel->getData()), width, height, numchan, L);
			// next level
			delete miplevel;
		}
	}
	catch (const std::exception &exc)
	{
		cout << "makeTexture() -> " << exc.what() << endl;
	}
	// the end
	delete img;
	cout << " Done\n";
}
#undef TILE_SIZE

__END_QDRENDER

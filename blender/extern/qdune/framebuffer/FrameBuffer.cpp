//---------------------------------------------------------------------------------
// Simple framebuffer for display, uses CImg library by David Tschumperle
// Currently the whole image exists at once, but it should be modified to
// write just a single bucket, using some appropriate image format like exr.
// Similarly, drawing to the display should be one bucket at a time,
// currently redraws the whole image for every bucket, which is quite 'expensive'.
// But while there are functions to draw a subimage in another image in CImg,
// I just can't seem to find (or figure out) something similar for a subrect
// *display* update. So for now this is taken care of by the hider,
// which passes a flag to only draw every column/row.
//---------------------------------------------------------------------------------

#include "FrameBuffer.h"
#include "Color.h"
#include "Mathutil.h"
#include "Transform.h"
#include "State.h"
#include <iostream>
#include <cstdio>

// VC8 warn flags
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#pragma warning(disable : 4312)
#pragma warning(disable : 4311)
#endif

// bcb flags removed

#ifdef WITH_CIMG
// though here only used for its multiplatform display features,
// the CImg library by David Tschumperle is an extremely nice
// general image processing library, all in one include file
#if defined (__GNUC__) && !defined(__SVR4)
// somewhat faster display update
#define cimg_use_xshm
#endif
#include "CImg.h"
#endif

#include "exr_io.h"

#include "QDRender.h"
__BEGIN_QDRENDER

#ifdef WITH_CIMG
// framebuffer display
static cimg_library::CImgDisplay* fbdisplay = NULL;
static cimg_library::CImg<unsigned char>* fbdisplay_img = NULL;
#endif

// ctor
FrameBuffer::FrameBuffer(const Options& options)
{
	fbdata = NULL;
	ortho = (options.projection == Options::PROJ_ORTHOGRAPHIC);

	// start coords with respect to cropwindow
	xstart = CLAMP(CEILI(options.xRes*options.xMin), 0, options.xRes-1);
	ystart = CLAMP(CEILI(options.yRes*options.yMin), 0, options.yRes-1);
	const int xend = CLAMP(CEILI(options.xRes*options.xMax-1), 0, options.xRes-1);
	const int yend = CLAMP(CEILI(options.yRes*options.yMax-1), 0, options.yRes-1);
	// width & height with respect to cropwindow, one pixel minimum
	width = xend - xstart + 1;
	height = yend - ystart + 1;

	// mode, as above, default is RI_RGB, error check during parse
	mode = MD_RGB;
	dataPerPix = 3;
	if (options.displayMode == Options::DM_Z)
		{ mode=MD_Z;  dataPerPix=1; }
	else if (options.displayMode == Options::DM_A)
		{ mode=MD_A;  dataPerPix=1; }
	else if (options.displayMode == Options::DM_AZ)
		{ mode=MD_AZ;  dataPerPix=2; }
	else if (options.displayMode == Options::DM_RGBA)
		{ mode=MD_RGBA;  dataPerPix=4; }
	else if (options.displayMode == Options::DM_RGBZ)
		{ mode=MD_RGBZ;  dataPerPix=4; }
	else if (options.displayMode ==  Options::DM_RGBAZ)
		{ mode=MD_RGBAZ;  dataPerPix=5; }

	//type
	if (options.displayType == Options::DT_FILE)
		type = TP_FILE;
	else if (options.displayType == Options::DT_ZFILE)
		type = TP_ZFILE;
	else if  (options.displayType == Options::DT_FRAMEBUFFER)
		type = TP_FRAMEBUFFER;

	// display name, 80 max chars
	strncpy(dispname, options.displayName, 80);

	// RiExposure
	gain = options.gain;
	gamma = options.gamma;

	// color quantizer
	colQ.one = options.cqOne;
	colQ.minimum = options.cqMin;
	colQ.maximum = options.cqMax;
	colQ.dither_amplitude = options.cqDitherAmplitude;

	// depth quantizer, actually not used at all, might as well remove...
	depthQ.one = options.zqOne;
	depthQ.minimum = options.zqMin;
	depthQ.maximum = options.zqMax;
	depthQ.dither_amplitude = options.zqDitherAmplitude;

	// buffer init, float per data element
	unsigned int sz =  width * height * dataPerPix;
	fbdata = new float[sz];
	// clear
	memset(fbdata, 0, sz*sizeof(float));

	// the quantized color buffer, only used for file mode
	qdata = NULL;

#ifdef WITH_CIMG
	// init. display if 'framebuffer'
	if (type == TP_FRAMEBUFFER) {
		// always just rgb, conversion is handled in toImage() below
		fbdisplay_img = new cimg_library::CImg<unsigned char>(width, height, 1, 3, 0);
		fbdisplay = new cimg_library::CImgDisplay(*fbdisplay_img, dispname, 0);
		// the ubiquitous checkerboard background pattern, white & grey bucketsize rectangles
		const int bucketsize = options.bucketsize;
		for (unsigned int y=0; y<=(height/bucketsize); ++y) {
			for (unsigned int x=0; x<=(width/bucketsize); ++x) {
				unsigned char col  = (unsigned char)(((x & 1) ^ (y & 1)) ? 128 : 255);
				for (unsigned int sy=y*bucketsize; sy<(y+1)*bucketsize; ++sy) {
					if (sy >= height) continue;
					for (unsigned int sx=x*bucketsize; sx<(x+1)*bucketsize; ++sx) {
						if (sx >= width) continue;
						(*fbdisplay_img)(sx, sy, 0) = (*fbdisplay_img)(sx, sy, 1) = (*fbdisplay_img)(sx, sy, 2) = col;
					}
				}
			}
		}
		fbdisplay->display(*fbdisplay_img);
	}
#else
	if (type == TP_FRAMEBUFFER)
		std::cout << "Cannot display, CImg not enabled, writing to file instead." << std::endl;
#endif

}

// dtor
FrameBuffer::~FrameBuffer()
{
	// free alloc'ed data
#ifdef WITH_CIMG
	if (fbdisplay_img) delete fbdisplay_img;
	if (fbdisplay) {
		fbdisplay->close();
		delete fbdisplay;
	}
#endif
	if (qdata) delete[] qdata;
	if (fbdata) delete[] fbdata;
}

void FrameBuffer::operator()(int x, int y, float rgbaz[5])
{
	x -= xstart, y -= ystart;
	if ((x < 0) or (y < 0) or (x >= (int)width) or (y >= (int)height)) return;
	unsigned int ofs = (x + y*width) * dataPerPix;
	switch (mode) {
		case MD_A:
			fbdata[ofs] = rgbaz[3];
			break;
		case MD_Z:
			fbdata[ofs] = rgbaz[4];
			break;
		case MD_AZ:
			fbdata[ofs++] = rgbaz[3];
			fbdata[ofs] = rgbaz[4];
			break;
		case MD_RGBA:
			fbdata[ofs++] = rgbaz[0];
			fbdata[ofs++] = rgbaz[1];
			fbdata[ofs++] = rgbaz[2];
			fbdata[ofs] = rgbaz[3];
			break;
		case MD_RGBZ:
			fbdata[ofs++] = rgbaz[0];
			fbdata[ofs++] = rgbaz[1];
			fbdata[ofs++] = rgbaz[2];
			fbdata[ofs] = rgbaz[4];
			break;
		case MD_RGBAZ:
			fbdata[ofs++] = rgbaz[0];
			fbdata[ofs++] = rgbaz[1];
			fbdata[ofs++] = rgbaz[2];
			fbdata[ofs++] = rgbaz[3];
			fbdata[ofs] = rgbaz[4];
			break;
		default:
		case MD_RGB:
			fbdata[ofs++] = rgbaz[0];
			fbdata[ofs++] = rgbaz[1];
			fbdata[ofs] = rgbaz[2];
	}
}

// Quantize image or given subrect with start coords (xs,ys) & end coords (xe,ye)
void FrameBuffer::quantizeImage()
{
	if (fbdata==NULL) return;

	int dpx = (dataPerPix <= 3) ? 3 : 4;
	qdata = new unsigned char[width*height*dpx];

	const float igam = (gamma==0.f) ? 1.f : (1.f/ABS(gamma));

	for (unsigned int y=0; y<height; ++y) {
		for (unsigned int x=0; x<width; ++x) {

			float* data = &fbdata[(x + y*width) * dataPerPix];
			float fr=0, fg=0, fb=0, fa=1;
			switch (dataPerPix) {
				// Z case is handled separately
				case 1:
				case 2:
				{
					// MD_A or MD_AZ (Z is not saved in image, handled separately)
					fr = fg = fb = 0, fa = 1.f-data[0];
					break;
				}
				case 3:
				{
					fr = data[0];
					fg = data[1];
					fb = data[2];
					break;
				}
				case 4:
				case 5:
				{
					// rgba, composite on top of background
					fa = 1.f - data[3];
					fr = data[0] + fa*data[0];
					fg = data[1] + fa*data[1];
					fb = data[2] + fa*data[2];
				}
			}

			// 'exposure' & gamma
			if (gain != 1.f) {
				fr *= gain;  fg *= gain;  fb *= gain;
			}
			if (igam != 1.f){
				fr = pow(CLAMP0(fr), igam);
				fg = pow(CLAMP0(fg), igam);
				fb = pow(CLAMP0(fb), igam);
			}

			// quantize
			unsigned char* qd = &qdata[(x + y*width)*dpx];
			qd[0] = (unsigned char)CLAMP((int)(0.5f + colQ.one*fr + colQ.dither_amplitude*frand()), colQ.minimum, colQ.maximum);
			qd[1] = (unsigned char)CLAMP((int)(0.5f + colQ.one*fg + colQ.dither_amplitude*frand()), colQ.minimum, colQ.maximum);
			qd[2] = (unsigned char)CLAMP((int)(0.5f + colQ.one*fb + colQ.dither_amplitude*frand()), colQ.minimum, colQ.maximum);
			if (dpx==4)
				qd[3] = (unsigned char)CLAMP((int)(0.5f + colQ.one*fa + colQ.dither_amplitude*frand()), colQ.minimum, colQ.maximum);

		}
	}
}

// save the image as a raw targa file
bool FrameBuffer::saveImage()
{
	if (qdata==NULL) return false;
	// raw tga output for now
	// name is assigned by default
	std::cout << "Saving Targa file as " << dispname << std::endl;

	const unsigned char TGAHDR[12] = {0, 0, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	FILE* fp;
	unsigned short w, h, x, y;
	unsigned char* yscan;
	unsigned char btsdesc[2];
	unsigned int dto;
	int dpx = (dataPerPix <= 3) ? 3 : 4;
	if (dpx==4) {
		btsdesc[0] = 0x20; // 32 bits
		btsdesc[1] = 0x28; // topleft / 8 bit alpha
	}
	else {
		btsdesc[0] = 0x18; // 24 bits
		btsdesc[1] = 0x20; // topleft / no alpha
	}
	w = (unsigned short)width;
	h = (unsigned short)height;
	fp = fopen(dispname, "wb");
	if (fp == NULL) {
		std::cout << "Could not open file for writing!\n";
		return false;
	}
	fwrite(&TGAHDR, 12, 1, fp);
	fputc(w, fp);
	fputc(w >> 8, fp);
	fputc(h, fp);
	fputc(h >> 8, fp);
	fwrite(&btsdesc, 2, 1, fp);
	for (y=0; y<h; y++) {
		// swap R & B channels
		dto = y*w;
		yscan = &qdata[dto*dpx];
		for (x=0; x<w; x++, yscan+=dpx) {
			fputc(*(yscan+2), fp);
			fputc(*(yscan+1), fp);
			fputc(*yscan,  fp);
			if (dpx==4) fputc(*(yscan+3), fp);
		}
	}
	fclose(fp);
	std::cout << "OK" << std::endl;
	return true;
}


bool FrameBuffer::saveZ()
{
	// handles Z data only, MD_AZ/MD_RGBAZ not yet
	if ((fbdata==NULL) || (mode!=MD_Z)) return false;
	std::cout << "Saving Z file as " << dispname << std::endl;
	Transform w2c = *State::Instance()->getNamedCoordSys("world"), 
	          c2r = State::Instance()->cam.getCam2Ras();
	//char fname[512] = {0};
	//Options options = State::Instance()->topOptions();
	//snprintf(fname, 512, "%s%s", options.basepath, dispname);
	writeTiledZ(dispname, fbdata, width, height, (ortho ? 1 : 0), *w2c.getRtMatrixPtr(), *c2r.getRtMatrixPtr());
	return true;
}


void FrameBuffer::finalize()
{
#ifdef WITH_CIMG
	if (fbdisplay_img) {
		// draw final scanline block too
		fbdisplay->display(*fbdisplay_img);
		// framebuffer, wait until window closed
		std::cout << "Close display window or press ESC key to quit...\n";
		while (!fbdisplay->is_closed && (fbdisplay->key!=cimg_library::cimg::keyESC))
			fbdisplay->wait();
		return;
	}
#endif
	// file, z file is saved as float
	if (type == TP_ZFILE)
		saveZ();
	else	{
		// this still includes the AZ & RGBAZ cases, not sure what to do about that yet,
		// will be very straightforward with exr support though... TODO
		quantizeImage();
		saveImage();
	}
}

// bucket to display, only used in framebuffer mode
void FrameBuffer::toImage(int bx, int by, int bw, int bh, bool draw)
{
#ifdef WITH_CIMG
	if (fbdisplay==NULL) return;

	// just exit if window closed
	if (fbdisplay->is_closed)
		exit(0);

	float fr, fg, fb;
	const float igam = (gamma==0.f) ? 1.f : (1.f/ABS(gamma));

	const unsigned int chanofs = width*height;

	for (int y=by; y<(by+bh); y++) {
		if (y >= (int)height)continue;
		const unsigned int yx = y*width;
		for (int x=bx; x<(bx+bw); x++) {
			if (x >= (int)width) continue;

			const float* data = &fbdata[(x + yx) * dataPerPix];
			fr = fg = fb = 0;
			// display color channel indices
			const unsigned int idxR = x + y*width;
			const unsigned int idxG = idxR + chanofs;
			const unsigned int idxB = idxG + chanofs;
			switch (dataPerPix) {
				// z not handled yet
				case 1:
					fr = fg = fb = data[0]*0.0001f;	// scale to make depth more visible, temporary
					break;
				case 2:
					fr = fg = fb = data[0];
					break;
				case 3:
					fr = data[0];
					fg = data[1];
					fb = data[2];
					break;
				case 4:
				case 5:
					// rgba, composite on top of display background.
					// side effect here is that background will also be affected by gam/exp
					// though this might actually be useful as progress visual feedback.
					// and only display is affected anyway, not actual picture
					const float ia = (1.f - data[3])*(1.f/255.f);
					fr = data[0] + ia*fbdisplay_img->data[idxR];
					fg = data[1] + ia*fbdisplay_img->data[idxG];
					fb = data[2] + ia*fbdisplay_img->data[idxB];
			}

			// 'exposure' & gamma
			if (igam != 1.f) {
				fr = (fr < 0.f) ? 0.f : powf(fr*gain, igam);
				fg = (fg < 0.f) ? 0.f : powf(fg*gain, igam);
				fb = (fb < 0.f) ? 0.f : powf(fb*gain, igam);
			}

			// quantize and set pixel
			// R
			fbdisplay_img->data[idxR] = (unsigned char)CLAMP((int)(0.5f + colQ.one*fr + colQ.dither_amplitude*frand()), colQ.minimum, colQ.maximum);
			// G
			fbdisplay_img->data[idxG] = (unsigned char)CLAMP((int)(0.5f + colQ.one*fg + colQ.dither_amplitude*frand()), colQ.minimum, colQ.maximum);
			// B
			fbdisplay_img->data[idxB] = (unsigned char)CLAMP((int)(0.5f + colQ.one*fb + colQ.dither_amplitude*frand()), colQ.minimum, colQ.maximum);

		}
	}
	// only redraw image if draw == true
	if (draw) fbdisplay->display(*fbdisplay_img);
#endif
}

__END_QDRENDER

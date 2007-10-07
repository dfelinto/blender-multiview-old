///-----------------------------------------
///  OpenEXR file read & write
///-----------------------------------------

#include "exr_io.h"

#include <ImfTiledOutputFile.h>
#include <ImfChannelList.h>
#include <ImfMatrixAttribute.h>
#include <ImfIntAttribute.h>
#include <ImfBoxAttribute.h>
// for rgba image loader
#include <ImfRgbaFile.h>
#include <ImfArray.h>
// same, using general interface
#include <ImfInputFile.h>

#include <iostream>

using namespace std;
using namespace Imf;
using namespace Imath;

__BEGIN_QDRENDER

// fixed tilesize value for now, but probably will leave it like that anyway, seems good average value
// WARNING, duplicate! also defined in maketexture.cpp
#define TILESIZE 64
void writeTiledZ(const char fileName[], float* Zbuf, int width, int height,
                 int ortho, const float w2c[4][4], const float c2r[4][4])
{
	try
	{
		Header header(width, height);
		// default zip is best probably, lossles too, pxr24 is possible (though lossy) alternative, seems to work well for Z
		//header.compression() = PXR24_COMPRESSION;
		//header.compression() = NO_COMPRESSION;
		header.channels().insert("Z", Channel(FLOAT));
		header.setTileDescription(TileDescription(TILESIZE, TILESIZE, ONE_LEVEL));
		// this is used as ID, value not used, text itself is the id
		header.insert("QD_shadowmap", IntAttribute(0));
		header.insert("w2c", M44fAttribute(M44f(w2c)));
		header.insert("c2r", M44fAttribute(M44f(c2r)));
		header.insert("ortho", IntAttribute(ortho));
		TiledOutputFile out(fileName, header);
		FrameBuffer frameBuffer;
		frameBuffer.insert("Z", Slice(FLOAT, (char*)Zbuf, sizeof(float), sizeof(float)*width));
		out.setFrameBuffer(frameBuffer);
		for (int tileY=0; tileY<out.numYTiles(); ++tileY)
			for (int tileX=0; tileX<out.numXTiles(); ++tileX)
				out.writeTile(tileX, tileY);
	}
	catch (const std::exception &exc)
	{
		cout << "writeTiledZ() -> " << exc.what() << endl;
	}
}
#undef TILESIZE

float* readTiledZ(const char fileName[], int& width, int& height,
                  int& ortho, float w2c[4][4], float c2r[4][4])
{
	float* zmap = NULL;
	try
	{
		TiledInputFile in(fileName);
		const Header& h = in.header();
		const M44fAttribute& ma1 = h.typedAttribute<M44fAttribute>("w2c").value();
		const M44fAttribute& ma2 = h.typedAttribute<M44fAttribute>("c2r").value();
		const IntAttribute& ia1 = h.typedAttribute<IntAttribute>("ortho").value();
		memcpy(w2c, &(ma1.value()[0][0]), sizeof(float)*16);
		memcpy(c2r, &(ma2.value()[0][0]), sizeof(float)*16);
		ortho = ia1.value();
		const Box2i dw = h.dataWindow();
		width =  dw.max.x - dw.min.x + 1;
		height = dw.max.y - dw.min.y + 1;
		const int dx = dw.min.x, dy = dw.min.y;
		zmap = new float[width*height];
		FrameBuffer frameBuffer;
		frameBuffer.insert("Z", Slice(FLOAT, (char*)&zmap[-dy*width - dx], sizeof(float), sizeof(float)*width));
		in.setFrameBuffer(frameBuffer);
		for (int tileY=0; tileY<in.numYTiles(); ++tileY)
			for (int tileX=0; tileX<in.numXTiles(); ++tileX)
				in.readTile(tileX, tileY);
		return zmap;
	}
	catch (const std::exception &exc)
	{
		cout << "readTiledZ() -> " << exc.what() << endl;
		if (zmap) { delete[] zmap;  zmap = NULL; }
		width = height = 0;
		return NULL;
	}
}

//----------------------------------------------------------------------------------------------------------------
// EXRbuf_t

// yet another stupid error was here, used TILESIZE in the loading routines,
// but of course the image does not have necessarily that tilesize, must get from file
EXRbuf_t::EXRbuf_t(const char fname[]) : in(NULL), tmp_tile(NULL), tdsize(0), numchan(0), have_alpha(false)
{
	try
	{
		in = new TiledInputFile(fname);
		const Header& h = in->header();
		if (h.findTypedAttribute<IntAttribute>("QD_shadowmap")) {
			// regular shadowmap, Z only
			const M44fAttribute& ma1 = h.typedAttribute<M44fAttribute>("w2c").value();
			memcpy(w2c, &(ma1.value()[0][0]), sizeof(float)*16);
			const M44fAttribute& ma2 = h.typedAttribute<M44fAttribute>("c2r").value();
			memcpy(c2r, &(ma2.value()[0][0]), sizeof(float)*16);
			const IntAttribute& ia1 = h.typedAttribute<IntAttribute>("ortho").value();
			ortho = ia1.value();
			imgtype = IT_SHADOWMAP;
			dttype = IT_FLOAT;
			numchan = 1;
			// no temporary tile needed in this case
		}
		else {
			// must be texmap, value not used, just here to trigger exception if not found
			h.typedAttribute<IntAttribute>("QD_texturemap");
			imgtype = IT_TEXTUREMAP;
			const Channel* Ychan = in->header().channels().findChannel("Y");
			// get image data type
			PixelType tp;
			if (Ychan) {
				tp = Ychan->type;
				numchan = 1;
			}
			else {
				const Channel* rchan = in->header().channels().findChannel("R");
				if ((rchan == NULL) || (in->header().channels().findChannel("G") == NULL) || (in->header().channels().findChannel("B") == NULL))
					throw Iex::InputExc("Image has some or all required RGB channels missing?");
				tp = rchan->type;
				have_alpha = (in->header().channels().findChannel("A") != NULL);
				numchan += have_alpha ? 4 : 3;
			}
			const int tilesize = in->tileXSize() * in->tileYSize() * numchan;
			if (tp == UINT) {
				dttype = IT_UINT;
				tmp_tile = new unsigned int [tilesize];
			}
			else if (tp == HALF) {
				dttype = IT_HALF;
				tmp_tile = new half [tilesize];
			}
			else if (tp == FLOAT) {
				dttype = IT_FLOAT;
				tmp_tile = new float [tilesize];
			}
			else	// unless a new type gets added, this should never happen
				throw Iex::InputExc("EXRbuf_t() -> Unknown image data type?");
		}
	}
	catch (const std::exception &exc)
	{
		cout << "EXRbuf_t() -> " << exc.what() << endl;
		delete in;
		in = NULL;
	}
}

EXRbuf_t::~EXRbuf_t()
{
	if (in) { delete in;  in = NULL; }
	if (tmp_tile) { delete[] (int*)tmp_tile;  tmp_tile = NULL; }	// cast only to make compiler happy (delete void undefined)
}

template<typename cpptype, PixelType exrtype>
bool getTile(TiledInputFile* in, const tileID_t& tileID,
	cpptype* Rtile, cpptype* Gtile, cpptype *Btile, cpptype* Atile,
	cpptype* Ztile = NULL, cpptype* Ytile = NULL)
{
	try
	{
		const Box2i b = in->dataWindowForTile(tileID.tileX, tileID.tileY, tileID.levelX, tileID.levelY);
		FrameBuffer fb;
		const int tx = in->tileXSize(), p = -b.min.y*tx - b.min.x;	// must be int, negative offset possible
		if (Rtile) fb.insert("R", Slice(exrtype, (char*)&Rtile[p], sizeof( cpptype ), sizeof( cpptype )*tx));
		if (Btile) fb.insert("G", Slice(exrtype, (char*)&Gtile[p], sizeof( cpptype ), sizeof( cpptype )*tx));
		if (Gtile) fb.insert("B", Slice(exrtype, (char*)&Btile[p], sizeof( cpptype ), sizeof( cpptype )*tx));
		if (Atile) fb.insert("A", Slice(exrtype, (char*)&Atile[p], sizeof( cpptype ), sizeof( cpptype )*tx));
		if (Ztile) fb.insert("Z", Slice(exrtype, (char*)&Ztile[p], sizeof( cpptype ), sizeof( cpptype )*tx));
		if (Ytile) fb.insert("Y", Slice(exrtype, (char*)&Ytile[p], sizeof( cpptype ), sizeof( cpptype )*tx));
		in->setFrameBuffer(fb);
		in->readTile(tileID.tileX, tileID.tileY, tileID.levelX, tileID.levelY);
	}
	catch (const std::exception &exc)
	{
		cout << "getTile<>() -> " << exc.what() << endl;
		return false;
	}
	return true;
}


// Currently this converts the exr data to interleaved form,
// so it uses an intermediate tile, copying data, which is all completely unnecessary overhead.
// It really should be simplified to either save the tiles in the required interleaved format
// (which as far as I know is possible, but would not be compatible anymore with exrmaketiled)
// or let the texture cache code handle it all when a color is requested, which probably is best TODO
RtColor* EXRbuf_t::getColorTile(const tileID_t& tileID) const
{
	if (in) {
		const int tx = in->tileXSize(), ty = in->tileYSize();
		const unsigned int colofs = tx*ty;
		tdsize = colofs*sizeof(RtColor);	// tile data size in bytes
		if (dttype == IT_UINT) {
			unsigned int* icoltile = reinterpret_cast<unsigned int*>(tmp_tile);
			RtColor* coltile = NULL;
			if (getTile<unsigned int, UINT>(in, tileID, icoltile, icoltile + colofs,
			                                icoltile + colofs*2, (have_alpha ? (icoltile + colofs*3) : NULL)))
			{
				// UINT -> RtColor
				const float c2f = 1.f/255.f;
				coltile = new RtColor[colofs];
				for (unsigned int i=0; i<colofs; ++i)
					coltile[i][0] = c2f*icoltile[i], coltile[i][1] = c2f*icoltile[i + colofs], coltile[i][2] = c2f*icoltile[i + colofs*2];
			}
			return coltile;
		}
		else if (dttype == IT_HALF) {
			half* hcoltile = reinterpret_cast<half*>(tmp_tile);
			RtColor* coltile = NULL;
			if (getTile<half, HALF>(in, tileID, hcoltile, hcoltile + colofs,
			                        hcoltile + colofs*2, (have_alpha ? (hcoltile + colofs*3) : NULL)))
			{
				// half r/g/b/a -> RtColor
				coltile = new RtColor[colofs];
				for (unsigned int i=0; i<colofs; ++i)
					coltile[i][0] = hcoltile[i], coltile[i][1] = hcoltile[i + colofs], coltile[i][2] = hcoltile[i + colofs*2];
			}
			return coltile;
		}
		else {	// float
			float* fcoltile = reinterpret_cast<float*>(tmp_tile);
			RtColor* coltile = NULL;
			if (getTile<float, FLOAT>(in, tileID, fcoltile, fcoltile + colofs,
			                          fcoltile + colofs*2, (have_alpha ? (fcoltile + colofs*3) : NULL)))
			{
				// float r/g/b/a -> RtColor
				coltile = new RtColor[colofs];
				for (unsigned int i=0; i<colofs; ++i)
					coltile[i][0] = fcoltile[i], coltile[i][1] = fcoltile[i + colofs], coltile[i][2] = fcoltile[i + colofs*2];
			}
			return coltile;
		}
	}
	return NULL;
}


// used for Z depth shadowmap or Y grayscale displacementmap
float* EXRbuf_t::getFloatTile(const tileID_t& tileID) const
{
	if (in && ((imgtype == IT_SHADOWMAP) || (numchan == 1))) {
		const unsigned int tilesize = in->tileXSize() * in->tileYSize();
		tdsize = tilesize*sizeof(float);	// tile data size in bytes
		float* Ftile = new float[tilesize];
		if (imgtype == IT_SHADOWMAP)	// Z
			getTile<float, FLOAT>(in, tileID, NULL, NULL, NULL, NULL, Ftile);
		else	// probably single channel luminance (possibly displacementmap)
			getTile<float, FLOAT>(in, tileID, NULL, NULL, NULL, NULL, NULL, Ftile);
		return Ftile;
	}
	return NULL;
}

inline void* makeTmpBuf(PixelType pt, unsigned int sz, unsigned int& tpsize)
{
	switch (pt) {
		case UINT:
			tpsize = sizeof(unsigned int);
			return new unsigned int[sz];
		case HALF:
			tpsize = sizeof(half);
			return new half[sz];
		default:
		case FLOAT:
			tpsize = sizeof(float);
			return new float[sz];
	}
}

imgbuf_t* loadEXR(const char filename[], bool force_grayscale)
{
	try
	{
		InputFile file(filename);
		Box2i dw = file.header().dataWindow();
		const int width = dw.max.x - dw.min.x + 1;
		const int height = dw.max.y - dw.min.y + 1;
		const unsigned int sz = width*height;
		const ChannelList &chans = file.header().channels();
		const Channel* Ychan = chans.findChannel("Y");
		if (Ychan) {
			// assume luminance channel only (strangely, channellist does not seem to have a size() type of function ??)
			unsigned int tpsize;
			void* tmpbuf = makeTmpBuf(Ychan->type, sz, tpsize);
			FrameBuffer fb;
			fb.insert("Y", Slice(Ychan->type, (char*)tmpbuf - (dw.min.x - dw.min.y*width)*tpsize, tpsize, tpsize*width));
			file.setFrameBuffer(fb);
			file.readPixels(dw.min.y, dw.max.y);
			// convert
			imgbuf_t* img = new imgbuf_t(width, height, 1, (Ychan->type == UINT) ? IMG_BYTE : IMG_FLOAT);
			if (Ychan->type == HALF) {
				half* tbufp = reinterpret_cast<half*>(tmpbuf);
				float* fbufp = reinterpret_cast<float*>(img->getData());
				for (unsigned int i=0; i<sz; ++i)
					*fbufp++ = *tbufp++;
			}
			else if (Ychan->type == UINT) {	// assume uint in range of char
				unsigned int* tbufp = reinterpret_cast<unsigned int*>(tmpbuf);
				unsigned char* cbufp = reinterpret_cast<unsigned char*>(img->getData());
				for (unsigned int i=0; i<sz; ++i)
					*cbufp++ = (unsigned char)*tbufp++;
			}
			else	// float
				memcpy(img->getData(), tmpbuf, sizeof(float)*sz);
			delete[] (unsigned long*)tmpbuf;	// cast only needed to make compiler happy
			return img;
		}
		// RGB[A]
		const Channel* Rchan = chans.findChannel("R");
		const Channel* Gchan = chans.findChannel("G");
		const Channel* Bchan = chans.findChannel("B");
		const Channel* Achan = chans.findChannel("A");
		if ((Rchan == NULL) || (Gchan == NULL) || (Bchan == NULL))
			throw Iex::InputExc("Image must have at least all RGB color channels!");
		// all channels must be the same pixeltype
		if ((Rchan->type != Gchan->type) || (Rchan->type != Bchan->type) || (Achan ? (Rchan->type != Achan->type) : false))
			throw Iex::InputExc("All image channels must be of the same type!");
		unsigned int tpsize;
		void *Gtmpbuf = NULL, *Btmpbuf = NULL, *Atmpbuf = NULL;
		void* Rtmpbuf = makeTmpBuf(Rchan->type, sz, tpsize);
		if (!force_grayscale) {
			Gtmpbuf = makeTmpBuf(Gchan->type, sz, tpsize);
			Btmpbuf = makeTmpBuf(Bchan->type, sz, tpsize);
			Atmpbuf = Achan ? makeTmpBuf(Achan->type, sz, tpsize) : NULL;
		}
		FrameBuffer fb;
		fb.insert("R", Slice(Rchan->type, (char*)Rtmpbuf - (dw.min.x - dw.min.y*width)*tpsize, tpsize, tpsize*width));
		if (!force_grayscale) {
			fb.insert("G", Slice(Gchan->type, (char*)Gtmpbuf - (dw.min.x - dw.min.y*width)*tpsize, tpsize, tpsize*width));
			fb.insert("B", Slice(Bchan->type, (char*)Btmpbuf - (dw.min.x - dw.min.y*width)*tpsize, tpsize, tpsize*width));
			if (Achan)
				fb.insert("A", Slice(Achan->type, (char*)Atmpbuf - (dw.min.x - dw.min.y*width)*tpsize, tpsize, tpsize*width));
		}
		file.setFrameBuffer(fb);
		file.readPixels(dw.min.y, dw.max.y);
		// convert
		imgbuf_t* img = new imgbuf_t(width, height, (force_grayscale ? 1 : (Achan ? 4 : 3)), (Rchan->type == UINT) ? IMG_BYTE : IMG_FLOAT);
		if (Rchan->type == HALF) {
			half *Gtbufp = NULL, *Btbufp = NULL, *Atbufp = NULL;
			half* Rtbufp = reinterpret_cast<half*>(Rtmpbuf);
			if (!force_grayscale) {
				Gtbufp = reinterpret_cast<half*>(Gtmpbuf);
				Btbufp = reinterpret_cast<half*>(Btmpbuf);
				Atbufp = Atmpbuf ? NULL : reinterpret_cast<half*>(Atmpbuf);
			}
			float* fbufp = reinterpret_cast<float*>(img->getData());
			for (unsigned int i=0; i<sz; ++i) {
				*fbufp++ = *Rtbufp++;
				if (!force_grayscale) {
					*fbufp++ = *Gtbufp++;
					*fbufp++ = *Btbufp++;
					if (Atbufp) *fbufp++ = *Atbufp++;
				}
			}
		}
		else if (Rchan->type == UINT) {	// assume uint in range of char
			unsigned int *Gtbufp = NULL, *Btbufp = NULL, *Atbufp = NULL;
			unsigned int* Rtbufp = reinterpret_cast<unsigned int*>(Rtmpbuf);
			if (!force_grayscale) {
				Gtbufp = reinterpret_cast<unsigned int*>(Gtmpbuf);
				Btbufp = reinterpret_cast<unsigned int*>(Btmpbuf);
				Atbufp = Atmpbuf ? reinterpret_cast<unsigned int*>(Atmpbuf) : NULL;
			}
			unsigned char* cbufp = reinterpret_cast<unsigned char*>(img->getData());
			for (unsigned int i=0; i<sz; ++i) {
				*cbufp++ = (unsigned char)*Rtbufp++;
				if (!force_grayscale) {
					*cbufp++ = (unsigned char)*Gtbufp++;
					*cbufp++ = (unsigned char)*Btbufp++;
					if (Atbufp) *cbufp++ = (unsigned char)*Atbufp++;
				}
			}
		}
		else {	// float
			float *Gtbufp = NULL, *Btbufp = NULL, *Atbufp = NULL;
			float* Rtbufp = reinterpret_cast<float*>(Rtmpbuf);
			if (!force_grayscale) {
				Gtbufp = reinterpret_cast<float*>(Gtmpbuf);
				Btbufp = reinterpret_cast<float*>(Btmpbuf);
				Atbufp = Atmpbuf ? NULL : reinterpret_cast<float*>(Atmpbuf);
			}
			float* fbufp = reinterpret_cast<float*>(img->getData());
			for (unsigned int i=0; i<sz; ++i) {
				*fbufp++ = *Rtbufp++;
				if (!force_grayscale) {
					*fbufp++ = *Gtbufp++;
					*fbufp++ = *Btbufp++;
					if (Atbufp) *fbufp++ = *Atbufp++;
				}
			}
		}
		delete[] (unsigned long*)Rtmpbuf;
		if (Gtmpbuf) delete[] (unsigned long*)Gtmpbuf;
		if (Btmpbuf) delete[] (unsigned long*)Btmpbuf;
		if (Atmpbuf) delete[] (unsigned long*)Atmpbuf;
		return img;
	}
	catch (const exception &exc)
	{
		cerr << "[ERROR] -> loadEXR(): " << exc.what() << endl;
		return NULL;
	}
}

__END_QDRENDER

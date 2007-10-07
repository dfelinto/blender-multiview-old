#ifndef _EXR_IO_H
#define _EXR_IO_H

#include <ImfTiledInputFile.h>
#include "ri.h"
#include "img_io.h"

#include "QDRender.h"
__BEGIN_QDRENDER

class EXRbuf_t;
struct tileID_t
{
	const EXRbuf_t* tex;
	unsigned short tileX, tileY, levelX, levelY;
	bool operator<(const tileID_t& oi)
	{
		return ((oi.tex < tex) &&
		        (oi.tileX < tileX) && (oi.tileY < tileY) &&
		        (oi.levelX < levelX) && (oi.levelY < levelY));
	}
	bool operator==(const tileID_t& oi)
	{
		return ((oi.tex == tex) &&
		        (oi.tileX == tileX) && (oi.tileY == tileY) &&
		        (oi.levelX == levelX) && (oi.levelY == levelY));
	}
	bool operator!=(const tileID_t& oi)
	{
		return (!((oi.tex == tex) &&
		         (oi.tileX == tileX) && (oi.tileY == tileY) &&
		         (oi.levelX == levelX) && (oi.levelY == levelY)));
	}
	bool operator>(const tileID_t& oi)
	{
		return ((oi.tex > tex) &&
		        (oi.tileX > tileX) && (oi.tileY > tileY) &&
		        (oi.levelX > levelX) && (oi.levelY > levelY));
	}
};

enum ImgType_t {IT_TEXTUREMAP, IT_SHADOWMAP};
enum ImgDataType_t {IT_UINT, IT_HALF, IT_FLOAT};

class EXRbuf_t
{
private:
	EXRbuf_t(const EXRbuf_t&);
	EXRbuf_t& operator=(const EXRbuf_t&);
public:
	// ctor
	EXRbuf_t(const char fname[]);
	// dtor
	~EXRbuf_t();
	// mtds
	RtColor* getColorTile(const tileID_t& tileID) const;
	float* getFloatTile(const tileID_t& tileID) const;
	bool valid() const { return (in != NULL); }
	int getLevels() const
	{
		if (in) return in->numLevels();
		return 0;
	}
	int getWidth(int level=0) const
	{
		if (in) return in->levelWidth(level);
		return 0;
	}
	int getHeight(int level=0) const
	{
		if (in) return in->levelHeight(level);
		return 0;
	}
	int getXTiles(int level=0) const
	{
		if (in) return in->numXTiles(level);
		return 0;
	}
	int getYTiles(int level=0) const
	{
		if (in) return in->numYTiles(level);
		return 0;
	}
	int getTileXSize() const
	{
		if (in) return in->tileXSize();
		return 0;
	}
	int getTileYSize() const
	{
		if (in) return in->tileYSize();
		return 0;
	}
	unsigned int getTileDataSize() const { return tdsize; }
	const RtMatrix& get_w2c() const { return w2c; }
	const RtMatrix& get_c2r() const { return c2r; }
	bool isOrtho() const { return ortho; }
	ImgType_t getType() const { return imgtype; }
	ImgDataType_t getDataType() const { return dttype; }
	int numChannels() const { return numchan; }
protected:
	Imf::TiledInputFile* in;
	void* tmp_tile;
	mutable unsigned int tdsize;
	RtMatrix w2c, c2r;
	int ortho, numchan;
	bool have_alpha;
	ImgType_t imgtype;
	ImgDataType_t dttype;
};


// for single level regular shadowmaps
void writeTiledZ(const char fileName[], float* Zbuf, int width, int height,
                 int ortho, const float w2c[4][4], const float c2r[4][4]);

// for makeTexture(), force_grayscale is set when a color image should be interpreted as grayscale
imgbuf_t* loadEXR(const char filename[], bool force_grayscale = false);

__END_QDRENDER

#endif // _EXR_IO_H

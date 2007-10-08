//---------------------------------------------------------------------------------------
// Simple LRU based texture cache
//---------------------------------------------------------------------------------------

#ifndef _TEXCACHE_H
#define _TEXCACHE_H

#include "lrulist.h"
#include "ri.h"

#include "QDRender.h"
__BEGIN_QDRENDER

//---------------------------------------------------------------------------------------

class EXRbuf_t;
struct tileID_t;

union textile_t
{
	RtFloat* float_data;
	RtColor* color_data;
};

class TextureCache
{
private:
	TextureCache(const TextureCache&);
	TextureCache& operator=(const TextureCache&);
public:
	TextureCache(size_t memory_maximum);
	~TextureCache();
	const EXRbuf_t* getTextureInfo(const char texname[]) const;
	// returns color at integer coords x,y at level L in 'col', handles all necessary tile updates
	void getColor(const EXRbuf_t* texinfo, int x, int y, int L, RtColor col) const;
	// returns float at integer coords x,y at level L, handles all necessary tile updates
	float getFloat(const EXRbuf_t* texinfo, int x, int y, int L) const;
	// for use with shadowmaps, returns depth value at integer coords x,y (always level 0), handles all necessary tile updates
	float getDepth(const EXRbuf_t* texinfo, int x, int y) const;
protected:
	// mtds
	textile_t* getTile(const tileID_t& tileID) const;
	// data
	// texture_db is the database of all texture Files in memory.
	mutable hashmap_t<EXRbuf_t*> texture_db;
	// textile_db is the database of all texture Tiles currently in memory.
	// The amount of memory it occupies (based on size of all tile data)
	// is the deciding factor for throwing out and loading tiles from disk when it fills up (maxmem variable).
	lrulist_t<tileID_t, textile_t*>* textile_db;
	size_t maxmem;
};

__END_QDRENDER

#endif // _TEXCACHE_H

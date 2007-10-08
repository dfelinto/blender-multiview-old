//---------------------------------------------------------------------------------------
// Simple LRU based texture cache
//---------------------------------------------------------------------------------------

// TODO:
// While it works as it should, it is rather simplistic and not particularly efficient.
// Probably a much better approach is to actually allocate a single block of memory,
// directly load tiles into it which can be directly accessed.
// Usage counts then should be maintained so that no data needs to be moved around.
// Problem then is how to organize everything by texture file, tile levels, etc.
// TODO

#include "TexCache.h"
#include "exr_io.h"
#include "Mathutil.h"

#include <iostream>

__BEGIN_QDRENDER

//---------------------------------------------------------------------------------------

using namespace std;

// cache stats
static unsigned int cache_total = 0;
static unsigned int cache_hit = 0;
static unsigned int cache_miss = 0;
static size_t texmem_total = 0;

TextureCache::TextureCache(size_t memory_maximum) : maxmem(memory_maximum)
{
	// set textile_db size to hold as many tiles as the specified memory allows
	// this is currently based on float RGB tiles of 64x64 only = 4*3*64*64 = 49152 bytes,
	// so maxtiles is not necessarily the correct number, it depends on the actual data loaded
	// Should remove the maxitem limit from lrulist and let the code here
	// signal the list when the cache is full based on 'texmem_total' TODO
	const size_t maxtiles = (maxmem < 49152) ? 1 : (maxmem / 49152);	// at least one tile
	//cout << "current memory of " << maxmem << " allows " << maxtiles << " color tiles in memory." << endl;
	textile_db = new lrulist_t<tileID_t, textile_t*>(maxtiles);
	cache_total = cache_hit = cache_miss = 0;
}

TextureCache::~TextureCache()
{
	cout << "cache memory total: " << texmem_total << endl;
	cout << "cache total : " << cache_total << endl;
	const float cdv = cache_total ? 100.f/float(cache_total) : 1.f;
	cout << "cache hit   : " << cache_hit << " ( " << cache_hit*cdv << "\% )\n";
	cout << "cache miss  : " << cache_miss << " ( " << cache_miss*cdv << "\% )\n";

	//cout << "texture_db size = " << texture_db.size() << endl;
	EXRbuf_t** ei = texture_db.first();
	int n = 0;
	while (ei) {
		delete *ei;
		ei = texture_db.next();
		n++;
	}
	//cout << "deleted " << n << " textures\n";

	cout <<  "Total tiles in memory: " << textile_db->size() << endl;
	textile_t* tt = textile_db->first();
	while (tt) {
		delete[] tt->color_data;
		delete tt;
		tt = textile_db->next();
	}
	delete textile_db;
}

// loads new texture if not available yet.
// if that failed or some other error occured, will store NULL ptr to prevent the file from trying to be opened continuously
const EXRbuf_t* TextureCache::getTextureInfo(const char texname[]) const
{
	EXRbuf_t** ei = texture_db.find(texname);
	if (ei) return *ei;
	// not found, open new .tqd file
	//char fullpath[512] = {0};
	//snprintf(fullpath, 512, "%s%s", State::Instance()->topOptions().basepath, texname);
	EXRbuf_t* ebc = new EXRbuf_t(texname); //fullpath);
	if (!ebc->valid()) {
		texture_db.insert(texname, NULL);
		delete ebc;
		return NULL;
	}
	texture_db.insert(texname, ebc);
	return ebc;
}

textile_t* TextureCache::getTile(const tileID_t& tileID) const
{
	cache_total++;
	textile_t** cached_tile = textile_db->find(tileID);
	if (cached_tile) {
		cache_hit++;
		return *cached_tile;
	}
	cache_miss++;

	// not found, load new tile
	// possibly the main bottleneck in the code, some sort of prefetching technique might make a difference... TODO
	textile_t* tt = new textile_t;
	if (tileID.tex->numChannels() == 1)
		tt->float_data = tileID.tex->getFloatTile(tileID);
	else
		tt->color_data = tileID.tex->getColorTile(tileID);
	if (tt->color_data == NULL) {	// is union, so also valid if float_data==NULL
		delete tt;
		return NULL;
	}

	texmem_total += tileID.tex->getTileDataSize();

	// add new tile to texture tile database,
	// also free data of possibly discarded tile if addition of new tile caused an older one to be thrown out
	textile_t* discarded_tile = NULL;
	textile_db->insert(tileID, tt, discarded_tile);
	if (discarded_tile) {
		texmem_total -= tileID.tex->getTileDataSize();
		delete[] discarded_tile->color_data;
		delete discarded_tile;
	}

	return tt;
}


// returns color at integer coords x,y at level L in 'col', handles all necessary tile updates
void TextureCache::getColor(const EXRbuf_t* texinfo, int x, int y, int L, RtColor col) const
{
	if (texinfo == NULL) return;

	const int maxXtile = texinfo->getXTiles(L) - 1, maxYtile = texinfo->getYTiles(L) - 1;
	const int tilesize = texinfo->getTileXSize();	// mipmap, so always square
	// tile numbers
	const int tx = MIN2(int(x / tilesize), maxXtile), ty = MIN2(int(y / tilesize), maxYtile);
	// tile coords, modulomax w/h not quite correct here yet, depends on wrap modes TODO (same for above tile numbers)
	x %= tilesize, y %= tilesize;
	// correction for tiles that overlap the border, only partially valid
	if (tx == maxXtile) x = MIN2(x, (texinfo->getWidth(L) - 1) % tilesize);
	if (ty == maxYtile) y = MIN2(y, (texinfo->getHeight(L) - 1) % tilesize);

	const tileID_t tileID = {texinfo, tx, ty, L, L};	// mipmap for now, so levelX==levelY
	const textile_t* const tt = getTile(tileID);
	assert(tt != NULL);

	const unsigned int p = y*tilesize + x;
	col[0] = tt->color_data[p][0], col[1] = tt->color_data[p][1], col[2] = tt->color_data[p][2];
}


// returns float at integer coords x,y at level L, handles all necessary tile updates
float TextureCache::getFloat(const EXRbuf_t* texinfo, int x, int y, int L) const
{
	if (texinfo == NULL) return 0.f;

	const int maxXtile = texinfo->getXTiles(L) - 1, maxYtile = texinfo->getYTiles(L) - 1;
	const int tilesize = texinfo->getTileXSize();	// mipmap, so always square
	// tile numbers
	const int tx = MIN2(int(x / tilesize), maxXtile), ty = MIN2(int(y / tilesize), maxYtile);
	// tile coords, modulomax w/h not quite correct here yet, depends on wrap modes TODO (same for above tile numbers)
	x %= tilesize, y %= tilesize;
	// correction for tiles that overlap the border, only partially valid
	if (tx == maxXtile) x = MIN2(x, (texinfo->getWidth(L) - 1) % tilesize);
	if (ty == maxYtile) y = MIN2(y, (texinfo->getHeight(L) - 1) % tilesize);

	const tileID_t tileID = {texinfo, tx, ty, L, L};	// mipmap for now, so levelX==levelY
	const textile_t* const tt = getTile(tileID);
	assert(tt != NULL);

	return tt->float_data[y*tilesize + x];
}


// For use with shadowmaps, returns value at integer coords x,y (always level 0), handles all necessary tile updates
// If out of range, returns -1, valid depthvalues are always positive
float TextureCache::getDepth(const EXRbuf_t* texinfo, int x, int y) const
{
	if ((texinfo == NULL) ||
	    (x < 0) || (y < 0) || (x >= texinfo->getWidth()) || (y >= texinfo->getHeight())) return -1.f;	// out of range
	const int tilesize = texinfo->getTileXSize();	// single level mipmap, always square
	const tileID_t tileID = {texinfo, MIN2(int(x / tilesize), texinfo->getXTiles() - 1),
	                         MIN2(int(y / tilesize), texinfo->getYTiles() - 1), 0, 0};
	const textile_t* const tt = getTile(tileID);
	assert(tt != NULL);
	return tt->float_data[(y % tilesize)*tilesize + (x % tilesize)];
}

__END_QDRENDER

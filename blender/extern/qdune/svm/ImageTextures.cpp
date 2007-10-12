//---------------------------------------------------------------------------------------
// Handles image based textures, including environment & shadow maps
//---------------------------------------------------------------------------------------
// (environmentmaps not yet TODO)

#include "ImageTextures.h"
#include "TexCache.h"

#include "exr_io.h"
#include "State.h"
#include "Mathutil.h"
#include "mcqmc.h"

__BEGIN_QDRENDER

using namespace std;

//---------------------------------------------------------------------------------------

void getTextureColor(RtColor col, const char* texname, float st[8], float A)
{
	const TextureCache* const texcache = State::Instance()->getTextureCache();
	const EXRbuf_t* const texinfo = texcache->getTextureInfo(texname);
	if (texinfo == NULL) {
		// texture could not be opened or possibly some other error occured
		col[0] = col[1] = col[2] = 0.f;
		return;
	}
	// convert to color if texture is actually single channel
	if (texinfo->numChannels() == 1) {
		getTextureFloat(col[0], texname, st, A);
		col[1] = col[2] = col[0];
		return;
	}
	
	const float cs = (st[0] + st[2] + st[4] + st[6])*0.25f, ct = (st[1] + st[3] + st[5] + st[7])*0.25f;
	const int wd = texinfo->getWidth(0), ht = texinfo->getHeight(0);

	float levf = MAX2(0.5f*logf(wd*ht*A)*(float)M_LOG2E, 0.f);	// includes sqrt
	const int maxlevel = texinfo->getLevels() - 2;	// -2 to account for trilerp
	int level = MIN2(FLOORI(levf), maxlevel);
	RtColor col1, col2;
	// color at current level
	int wdl = texinfo->getWidth(level), htl = texinfo->getHeight(level);
	float u0 = wdl*(cs - FLOORF(cs))-0.5f, v0 = htl*(ct - FLOORF(ct))-0.5f;
	// x0/y0 coords change here depending on wrap type, currently wrap for now
	int x0 = int(u0) % wdl, y0 = int(v0) % htl;
	if (x0 < 0) x0 += wdl;
	if (y0 < 0) y0 += htl;
	RtColor c00, c10, c01, c11;
	texcache->getColor(texinfo, x0, y0, level, c00);
	texcache->getColor(texinfo, x0+1, y0, level, c10);
	texcache->getColor(texinfo, x0+1, y0+1, level, c11);
	texcache->getColor(texinfo, x0, y0+1, level, c01);
	bilerp(col1, u0-x0, v0-y0, c00, c10, c01, c11);
	// color at next level
	level++;
	wdl = texinfo->getWidth(level), htl = texinfo->getHeight(level);
	u0 = wdl*(cs - FLOORF(cs))-0.5f, v0 = htl*(ct - FLOORF(ct))-0.5f;
	x0 = int(u0) % wdl, y0 = int(v0) % htl;
	if (x0 < 0) x0 += wdl;
	if (y0 < 0) y0 += htl;
	texcache->getColor(texinfo, x0, y0, level, c00);
	texcache->getColor(texinfo, x0+1, y0, level, c10);
	texcache->getColor(texinfo, x0+1, y0+1, level, c11);
	texcache->getColor(texinfo, x0, y0+1, level, c01);
	bilerp(col2, u0-x0, v0-y0, c00, c10, c01, c11);
	// trilerp
	levf -= FLOORF(levf);
	col[0] = col1[0] + levf*(col2[0] - col1[0]);
	col[1] = col1[1] + levf*(col2[1] - col1[1]);
	col[2] = col1[2] + levf*(col2[2] - col1[2]);

	#if 0
	// test, samples actual st area at level 0
	const int wd = texinfo->getWidth(0), ht = texinfo->getHeight(0);
	const float fxmin = wd*MIN4(st[0], st[2], st[4], st[6]);
	const float fymin = ht*MIN4(st[1], st[3], st[5], st[7]);
	const float fxmax = wd*MAX4(st[0], st[2], st[4], st[6]);
	const float fymax = ht*MAX4(st[1], st[3], st[5], st[7]);
	const float fw = (fxmax - fxmin), fh = (fymax - fymin);
	const int xmin = FLOORI(fxmin), ymin = FLOORI(fymin);
	const int xmax = CEILI(fxmax), ymax = CEILI(fymax);
	col[0] = col[1] = col[2] = 0.f;
	float wts = 0.f;
	for (int y=ymin; y<=ymax; ++y)
		for (int x=xmin; x<=xmax; ++x) {
			const float fx = (x - cs*wd), fy = (y - ct*ht);
			const float wt = RiGaussianFilter(fx, fy, fw, fh);
			wts += wt;
			RtColor tc;
			texcache->getColor(texinfo, x, y, 0, tc);
			col[0] += wt*tc[0], col[1] += wt*tc[1], col[2] += wt*tc[2];
		}
	const float sd = (wts==0.f) ? 1.f : (1.f/wts);
	col[0] *= sd, col[1] *= sd, col[2] *= sd;
	#endif
}


void getTextureFloat(RtFloat& fval, const char* texname, float st[8], float A)
{
	const TextureCache* const texcache = State::Instance()->getTextureCache();
	const EXRbuf_t* const texinfo = texcache->getTextureInfo(texname);
	if (texinfo == NULL) {
		// texture could not be opened or possibly some other error occured
		fval = 0.f;
		return;
	}
	if (!texinfo->numChannels() == 1) {
		// actually a color image, return red channel only (could also average or weight channels)
		RtColor col;
		getTextureColor(col, texname, st, A);
		fval = col[0];
		return;
	}
	
	const float cs = (st[0] + st[2] + st[4] + st[6])*0.25f, ct = (st[1] + st[3] + st[5] + st[7])*0.25f;
	const int wd = texinfo->getWidth(0), ht = texinfo->getHeight(0);

	float levf = MAX2(0.5f*logf(wd*ht*A)*(float)M_LOG2E, 0.f);	// includes sqrt
	const int maxlevel = texinfo->getLevels() - 2;
	int level = MIN2(FLOORI(levf), maxlevel);

	RtFloat fval1, fval2;
	// value at current level
	int wdl = texinfo->getWidth(level), htl = texinfo->getHeight(level);
	float u0 = wdl*(cs - FLOORF(cs))-0.5f, v0 = htl*(ct - FLOORF(ct))-0.5f;
	// WRAPMODES!!! TODO
	int x0 = int(u0) % wdl, y0 = int(v0) % htl;
	if (x0 < 0) x0 += wdl;
	if (y0 < 0) y0 += htl;
	float f00 = texcache->getFloat(texinfo, x0, y0, level);
	float f10 = texcache->getFloat(texinfo, x0+1, y0, level);
	float f11 = texcache->getFloat(texinfo, x0+1, y0+1, level);
	float f01 = texcache->getFloat(texinfo, x0, y0+1, level);
	bilerpF(fval1, u0-x0, v0-y0, f00, f10, f01, f11);
	// value at next level
	level++;
	wdl = texinfo->getWidth(level), htl = texinfo->getHeight(level);
	u0 = wdl*(cs - FLOORF(cs))-0.5f, v0 = htl*(ct - FLOORF(ct))-0.5f;
	x0 = int(u0) % wdl, y0 = int(v0) % htl;
	if (x0 < 0) x0 += wdl;
	if (y0 < 0) y0 += htl;
	f00 = texcache->getFloat(texinfo, x0, y0, level);
	f10 = texcache->getFloat(texinfo, x0+1, y0, level);
	f11 = texcache->getFloat(texinfo, x0+1, y0+1, level);
	f01 = texcache->getFloat(texinfo, x0, y0+1, level);
	bilerpF(fval2, u0-x0, v0-y0, f00, f10, f01, f11);
	// trilerp
	levf -= FLOORF(levf);
	fval = fval1 + levf*(fval2 - fval1);
}


float getShadowFactor(const char* texname, float st[8], const RtPoint& wldP, float* samples, float* blur, float* bias)
{
	const TextureCache* const texcache = State::Instance()->getTextureCache();
	const EXRbuf_t* const texinfo = texcache->getTextureInfo(texname);
	if (texinfo == NULL) return 0.f;	// texture could not be opened or possibly some other error occured
	RtPoint ltP;
	mulPMP(ltP, texinfo->get_w2c(), wldP);
	const float wz = bias ? (ltP[2] - *bias) : (ltP[2] - 0.01f);	// 0.01 default bias
	const RtMatrix& c2r = texinfo->get_c2r();
	if (texinfo->isOrtho()) {
		ltP[0] = c2r[0][0]*ltP[0] + c2r[0][1]*ltP[1] + c2r[0][2]*ltP[2] + c2r[0][3];
		ltP[1] = c2r[1][0]*ltP[0] + c2r[1][1]*ltP[1] + c2r[1][2]*ltP[2] + c2r[1][3];
	}
	else {
		ltP[0] = c2r[0][0]*ltP[0] + c2r[0][1]*ltP[1] + c2r[0][2]*ltP[2];
		ltP[1] = c2r[1][0]*ltP[0] + c2r[1][1]*ltP[1] + c2r[1][2]*ltP[2];
		if (ltP[2] != 0.f) { ltP[2] = 1.f/ltP[2];  ltP[0] *= ltP[2];  ltP[1] *= ltP[2]; }
	}
	const float blur_rad = blur ? (*blur)*0.5f : 0;
	const float maxwd = float(texinfo->getWidth() - 1), maxht = float(texinfo->getHeight() - 1);
	const float xmin = MIN2(MAX2(ltP[0] - blur_rad, 0.f), maxwd);
	const float xmax = MIN2(MAX2(ltP[0] + blur_rad, 0.f), maxwd);
	const float ymin = MIN2(MAX2(ltP[1] - blur_rad, 0.f), maxht);
	const float ymax = MIN2(MAX2(ltP[1] + blur_rad, 0.f), maxht);
	const float dx = xmax - xmin, dy = ymax - ymin;
	if ((dx < 0.f) || (dy < 0.f)) return 0.f;
	const int numsam = samples ? int(*samples) : 1;	// single sample default
	const float sdiv = 1.f/(float)numsam;
	int ts = 0;
	//static unsigned int VDCC = 0, SBC = 0;
	static Halton H2(2), H3(3);
	for (int sm=0; sm<numsam; ++sm) {
		/*if (sm == 16) {
			if (ts == 16) return 0.f;
			if (ts == 0) return 1.f;
		}*/
		const float zval = texcache->getDepth(texinfo, int(xmin + H2.getNext()*dx), int(ymin + H3.getNext()*dy));
		//const float zval = texcache->getDepth(texinfo, int(xmin + RI_vdC(++VDCC)*dx), int(ymin + RI_Sb(++SBC)*dy));
		//const float zval = texcache->getDepth(texinfo, int(xmin + RI_vdC(++VDCC)*dx), int(ymin + (sm + 0.5f)*sdiv*dy));
		//const float zval = texcache->getDepth(texinfo, int(xmin + frand()*dx), int(ymin + frand()*dy));
		// zval <= 0.f is invalid access return value of getFloat()
		ts += (zval > 0.f) ? ((zval < wz) ? 0 : 1) : 0;
	}
	return 1.f - ts*sdiv;
}

__END_QDRENDER

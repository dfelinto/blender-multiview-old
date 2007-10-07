
#ifdef __BCPLUSPLUS__
#pragma hdrstop
#include <condefs.h>
#endif

#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma warning(disable : 4996)
#endif

#include <iostream>

#include "ri.h"
#include "RIB_parser.h"

// for points/curves test
#include "qdVector.h"
#include "Noise.h"

#include <float.h>
#include <malloc.h>

//---------------------------------------------------------------------------
// global operators new & delete, to keep track of total memory allocated

// this code snippet from heaplayers library, not sure if flags are really all necessary (or correct..)
#if defined (_MSC_VER)
inline size_t getSize(void* ptr) { return _msize(ptr); }
#elif defined (__GNUC__) && !defined(__SVR4)
inline size_t getSize(void* ptr) { return malloc_usable_size(ptr); }
#elif defined (__APPLE__)
inline size_t getSize(void* ptr) { return malloc_size(ptr); }
#endif

static size_t curmem = 0; // current amount of memory allocated
static size_t maxmem = 0; // maximum amount of memory allocated

void* operator new(std::size_t sz) throw (std::bad_alloc)
{
#if defined (__GNUC__) && !defined(__SVR4)
	void* ptr = memalign(8, sz);
#else
	void* ptr = malloc((sz & 7) ? ((sz & 0xfffffffffffffff8) + 8) : (sz & 0xfffffffffffffff8));
#endif
	if (ptr == NULL) throw std::bad_alloc();
	if ((curmem += getSize(ptr)) > maxmem) maxmem = curmem;
	return ptr;
}

void* operator new[](std::size_t sz) throw (std::bad_alloc)
{
#if defined (__GNUC__) && !defined(__SVR4)
	void* ptr = memalign(8, sz);
#else
	void* ptr = malloc((sz & 7) ? ((sz & 0xfffffffffffffff8) + 8) : (sz & 0xfffffffffffffff8));
#endif
	if (ptr == NULL) throw std::bad_alloc();
	if ((curmem += getSize(ptr)) > maxmem) maxmem = curmem;
	return ptr;
}

void operator delete(void* ptr) throw()
{
	if (ptr) {
		curmem -= getSize(ptr);
		free(ptr);
	}
}

void operator delete[](void* ptr) throw()
{
	if (ptr) {
		curmem -= getSize(ptr);
		free(ptr);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------
// basic commandline render, no option overrides yet

#if 1
void dbout(const std::string &msg)
{
	std::cout << msg << std::endl;
}

int main(int argc, char* argv[])
{
	if (argc == 1) {
		std::cout << "Usage: " << argv[0] << " ribfile" << std::endl;
		return 0;
	}

	#ifdef WIN32
	// disable float exceptions
	_control87(MCW_EM, MCW_EM);
	#endif

	QDRender::RIBparse_t rp; //(dbout);
	rp.parse(argv[1]);

	std::cout << "Total memory allocated: " << maxmem <<  "( " <<  (maxmem/(float)(1 << 20)) << " Mb )\n" << std::endl;
}
#endif

//-------------------------------------------------------------------------------------------------------------------------------------

#if 0
// curves test
int main()
{
	#ifdef WIN32
	// disable float exceptions
	_control87(MCW_EM, MCW_EM);
	#endif

	RiBegin(NULL);

		//RiDisplay("curvehair.tga", RI_FILE, RI_RGB, RI_NULL);
		RiDisplay("curvehair", RI_FRAMEBUFFER, RI_RGB, RI_NULL);
		RtFloat fov = 55;
		RiProjection("perspective", RI_FOV, &fov, RI_NULL);

		//RiExposure(1.f, 0.666666f);
		RiFormat(800, 800, 1);
		RiPixelSamples(4, 4);
		// cropwindow is useful to only render parts of image when rendering huge number of hairs.
		//RiCropWindow(0, 0.5, 0, 0.5);

		RiTranslate(0, 0.1, 5);

		RiWorldBegin();

			//RtFloat int1 = 0;
			//RiLightSource("ambientlight", "intensity", &int1, RI_NULL);

			RtPoint from = {6, 6, 6};
			RtPoint to = {0, 0, 0};
			RtFloat int2 = 1.5f;
			RiLightSource("distantlight", "intensity", &int2, "from", &from, "to", &to, RI_NULL);

			const float srad = 1.25f;
			//RiAttributeBegin();
			//RiSurface("matte", RI_NULL);
			//RtColor sc = {0.25, 0.025, 0.0025};
			//RiColor(sc);
			//RiSphere(srad, -srad, srad, 360, RI_NULL);
			//RiAttributeEnd();

			//RtFloat rg = 0.15f, ks = 0.2, kd = 0.8;
			RiSurface("hair", RI_NULL); //"roughness", &rg, "Ks", &ks, "Kd", &kd, RI_NULL);
			//RtFloat sp1 = 0.1, sp2 = 0.03;
			//RiSurface("SIG2k_srf_fur", "color rootcolor", &sc, "float SPEC1", &sp1, "float SPEC2", &sp2, RI_NULL);

			// 1500000 hairs at 1000x1000 res with 8x8 samples and 4x4 sinc filter, 16 max gridsize:
			// 10080772 prims, 5173370 grids, 39143712 mp's, +23285sec, 319.9Mb... horrible...
			// (Cause of the bad rendertime is only having 512Mb, so it was basicaly all HD swapping,
			//  taskmanager indicated a mem usage of about 770Mb, cpu was barely active at all...)

			// +14.6sec 47.6Mb
			// -16.4sec 28.6Mb with sqrt(maxgridsize) splitting
			// +15.1sec 28.4Mb
			// +15.5sec 26.7Mb, immediate split into individual segments (with forced no split 14.2s 47Mb)
			// +15.3sec 24.4Mb separate saving of P, excluded from primvar

			//RiBasis(RiBezierBasis, RI_BEZIERSTEP, RiBSplineBasis, RI_BSPLINESTEP);
			// make a sphere with hair
			const int numhairs = 100000;
			const int numcp = 4; // number of control points, 4 minimum
			RtPoint* pts = new RtPoint[numhairs*numcp];
			RtInt* verts = new RtInt[numhairs];
			RtFloat* wd = new RtFloat[numhairs*2]; // 4 cp, 2 varyings
			const QDRender::stdPerlin_t ngen;
			for (int i=0; i<numhairs; ++i) {
				QDRender::Point3 sp = srad*QDRender::RandomVectorSphere();
				if (sp.z > 0.f) sp *= -1.f; // all hairs on the font of the sphere
				QDRender::Vector op = toVector(sp).normalize();
				for (int j=0; j<numcp; ++j) {
					int icp = i*numcp + j;
					pts[icp][0] = sp.x, pts[icp][1] = sp.y, pts[icp][2] = sp.z;
					QDRender::Point3 tp = sp*4.38462f;
					QDRender::Point3 ofs(QDRender::getSignedNoise(&ngen, tp),
					                     QDRender::getSignedNoise(&ngen, tp + QDRender::Point3(10.1)),
					                     QDRender::getSignedNoise(&ngen, tp - QDRender::Point3(10.1)));
					ofs = 0.5f*(op + ofs);
					sp += (0.3f*QDRender::frand() + 0.2f)*ofs;
				}
				// add some random 'gravity' to the hair endpoint to make it slope downwards
				pts[i*numcp + numcp - 1][1] -= 0.25f*QDRender::frand();
				verts[i] = numcp;
				wd[i*2] = 0.01f;
				wd[i*2+1] = 0.001f;
			}
			RiCurves("cubic", numhairs, verts, "nonperiodic", "P", pts, "width", wd, RI_NULL);
			//RtFloat cwd = 0.001f;
			//RiCurves("cubic", numhairs, verts, "nonperiodic", "P", pts, "constantwidth", &cwd, RI_NULL);
			// arrays can now be deleted here
			//delete[] wd;
			delete[] verts;
			delete[] pts;

		RiWorldEnd();

	RiEnd();

	std::cout << "Total memory allocated: " << maxmem <<  "( " <<  (maxmem/(float)(1 << 20)) << " Mb )\n" << std::endl;
	return 0;
}
#endif

#if 0
// Points test
// while it works, memory usage is rather inefficient, just one million particles already use up 150-200Mb
// It is possible to rasterize directly though, no micropolys needed TODO
int main()
{
	#ifdef WIN32
	// disable float exceptions
	_control87(MCW_EM, MCW_EM);
	#endif

	RiBegin(NULL);

		RiDisplay("points test", RI_FRAMEBUFFER, RI_RGB, RI_NULL);
		RtFloat fov = 55;
		RiProjection("perspective", RI_FOV, &fov, RI_NULL);

		RiFormat(512, 512, 1);
		RiPixelSamples(4, 4);
		RiExposure(1.5, 0.75);

		RiTranslate(0, 0, 5);

		RiWorldBegin();

			// constant color with additive blending
			RiSurface("constadd", RI_NULL);

			const float rad = 3;
			const int nh3 = 50;
			const int nump = (int)pow(nh3*2, 3);
			RtPoint* pts = new RtPoint[nump];
			RtColor* cols = new RtColor[nump];
			const QDRender::blenderNoise_t ngen;
			const float sc = 0.025;
			const float csc = 0.0025;
			int i = 0;
			const float nh3d = 1.f/(2*nh3);
			for (int z=-nh3; z<nh3; ++z) {
				for (int y=-nh3; y<nh3; ++y) {
					for (int x=-nh3; x<nh3; ++x) {
						const QDRender::Point3 tp(x*sc, y*sc, z*sc);
						cols[i][0] = ngen(tp);
						cols[i][1] = ngen(tp + QDRender::Point3(1.984851f));
						cols[i][2] = ngen(tp - QDRender::Point3(5.8456f));
						//cols[i][0] = 0.5f + x*nh3d, cols[i][1] = 0.5f + y*nh3d, cols[i][2] = 0.5f + z*nh3d;
						pts[i][0] = rad * (2.f*cols[i][0] - 1.f);
						pts[i][1] = rad * (2.f*cols[i][1] - 1.f);
						pts[i][2] = rad * (2.f*cols[i][2] - 1.f);
						cols[i][0] *= csc, cols[i][1] *= csc, cols[i][2] *= csc;
						i++;
					}
				}
			}
			std::cout << "TOTAL POINTS: " << nump << std::endl;
			const float cw = 0.075;
			RiTransformBegin();
				RiRotate(45, 1, 0, 0);
				RiPoints(nump, "P", pts, "constantwidth", &cw, "Cs", cols, RI_NULL);
			RiTransformEnd();
			delete[] cols;
			delete[] pts;

		RiWorldEnd();

	RiEnd();

	std::cout << "Total memory allocated: " << maxmem <<  "( " <<  (maxmem/(float)(1 << 20)) << " Mb )\n" << std::endl;
	return 0;
}
#endif

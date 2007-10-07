//------------------------------------------------------------------------------
// RI API
//------------------------------------------------------------------------------

#include "ri.h"
#include <cmath>
#include <cstdlib>

#include "State.h"
#include "Quadrics.h"
#include "Patches.h"
#include "CCSubdivision.h"
#include "Polygons.h"
#include "Curves.h"
#include "Nurbs.h"
#include "Points.h"
#include "ribInterface.h"
// for ReadArchive handling...
#include "RIB_parser.h"

#include "slshader.h"
#include "Mathutil.h"
#include <cstdarg>
#include <cassert>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

// for error/warning debug output, just use C funcs
#include <cstdio>
#define ERROUT stderr

using namespace QDRender;

// Token definitions
RtToken RI_FRAMEBUFFER          =       "framebuffer";
RtToken RI_FILE                 =       "file";
RtToken RI_RGB                  =       "rgb";
RtToken RI_RGBA                 =       "rgba";
RtToken RI_RGBZ                 =       "rgbz";
RtToken RI_RGBAZ                =       "rgbaz";
RtToken RI_A                    =       "a";
RtToken RI_Z                    =       "z";
RtToken RI_AZ                   =       "az";
RtToken RI_PERSPECTIVE          =       "perspective";
RtToken RI_ORTHOGRAPHIC         =       "orthographic";
RtToken RI_HIDDEN               =       "hidden";
RtToken RI_PAINT                =       "paint";
RtToken RI_CONSTANT             =       "constant";
RtToken RI_SMOOTH               =       "smooth";
RtToken RI_FLATNESS             =       "flatness";
RtToken RI_FOV                  =       "fov";
RtToken RI_AMBIENTLIGHT         =       "ambientlight";
RtToken RI_POINTLIGHT           =       "pointlight";
RtToken RI_DISTANTLIGHT         =       "distantlight";
RtToken RI_SPOTLIGHT            =       "spotlight";
RtToken RI_INTENSITY            =       "intensity";
RtToken RI_LIGHTCOLOR           =       "lightcolor";
RtToken RI_FROM                 =       "from";
RtToken RI_TO                   =       "to";
RtToken RI_CONEANGLE            =       "coneangle";
RtToken RI_CONEDELTAANGLE       =       "conedeltaangle";
RtToken RI_BEAMDISTRIBUTION     =       "beamdistribution";
RtToken RI_MATTE                =       "matte";
RtToken RI_METAL                =       "metal";
RtToken RI_SHINYMETAL           =       "shinymetal";
RtToken RI_PLASTIC              =       "plastic";
RtToken RI_PAINTEDPLASTIC       =       "paintedplastic";
RtToken RI_KA                   =       "Ka";
RtToken RI_KD                   =       "Kd";
RtToken RI_KS                   =       "Ks";
RtToken RI_ROUGHNESS            =       "roughness";
RtToken RI_KR                   =       "Kr";
RtToken RI_TEXTURENAME          =       "texturename";
RtToken RI_SPECULARCOLOR        =       "specularcolor";
RtToken RI_DEPTHCUE             =       "depthcue";
RtToken RI_FOG                  =       "fog";
RtToken RI_BUMPY                =       "bumpy";
RtToken RI_MINDISTANCE          =       "mindistance";
RtToken RI_MAXDISTANCE          =       "maxdistance";
RtToken RI_BACKGROUND           =       "background";
RtToken RI_DISTANCE             =       "distance";
RtToken RI_AMPLITUDE            =       "amplitude";
RtToken RI_RASTER               =       "raster";
RtToken RI_SCREEN               =       "screen";
RtToken RI_CAMERA               =       "camera";
RtToken RI_WORLD                =       "world";
RtToken RI_OBJECT               =       "object";
RtToken RI_INSIDE               =       "inside";
RtToken RI_OUTSIDE              =       "outside";
RtToken RI_LH                   =       "lh";
RtToken RI_RH                   =       "rh";
RtToken RI_P                    =       "P";
RtToken RI_PZ                   =       "Pz";
RtToken RI_PW                   =       "Pw";
RtToken RI_N                    =       "N";
RtToken RI_NP                   =       "Np";
RtToken RI_CS                   =       "Cs";
RtToken RI_OS                   =       "Os";
RtToken RI_S                    =       "s";
RtToken RI_T                    =       "t";
RtToken RI_ST                   =       "st";
RtToken RI_BILINEAR             =       "bilinear";
RtToken RI_BICUBIC              =       "bicubic";
RtToken RI_LINEAR               =       "linear";
RtToken RI_CUBIC                =       "cubic";
RtToken RI_PRIMITIVE            =       "primitive";
RtToken RI_INTERSECTION         =       "intersection";
RtToken RI_UNION                =       "union";
RtToken RI_DIFFERENCE           =       "difference";
RtToken RI_PERIODIC             =       "periodic";
RtToken RI_NONPERIODIC          =       "nonperiodic";
RtToken RI_CLAMP                =       "clamp";
RtToken RI_BLACK                =       "black";
RtToken RI_IGNORE               =       "ignore";
RtToken RI_PRINT                =       "print";
RtToken RI_ABORT                =       "abort";
RtToken RI_HANDLER              =       "handler";
RtToken RI_COMMENT              =       "comment";
RtToken RI_STRUCTURE            =       "structure";
RtToken RI_VERBATIM             =       "verbatim";
RtToken RI_IDENTIFIER           =       "identifier";
RtToken RI_NAME                 =       "name";
RtToken RI_SHADINGGROUP         =       "shadinggroup";
RtToken RI_WIDTH                =       "width";
RtToken RI_CONSTANTWIDTH        =       "constantwidth";
// extra's
RtToken RI_JITTER               =       "jitter";
RtToken RI_ZFILE                =       "zfile";
RtToken RI_SPHERE               =       "sphere";
RtToken RI_RAYTRACE             =       "raytrace";
RtToken RI_HYBRID               =       "hybrid";
RtToken RI_BUCKETSIZE           =       "bucketsize";
RtToken RI_TEXTUREMEMORY        =       "texturememory";
RtToken RI_GRIDSIZE             =       "gridsize";
RtToken RI_BINARY               =       "binary";

//------------------------------------------------------------------------------
// Basis Matrices

RtBasis RiBezierBasis   =       {{-1,  3, -3, 1},
                                 { 3, -6,  3, 0},
                                 {-3,  3,  0, 0},
                                 { 1,  0,  0, 0}};

RtBasis RiBSplineBasis    = {{(RtFloat)-1/6, 0.5, -0.5, (RtFloat)1/6},
                             {0.5, -1, 0.5, 0},
                             {-0.5, 0, 0.5, 0},
                             {(RtFloat)1/6, (RtFloat)4/6, (RtFloat)1/6, 0}};

RtBasis RiCatmullRomBasis = {{-0.5,  1.5, -1.5,  0.5},
                             {   1, -2.5,    2, -0.5},
                             {-0.5,    0,  0.5,    0},
                             {   0,    1,    0,    0}};

RtBasis RiHermiteBasis    = {{ 2,  1, -2,  1},
                             {-3, -2,  3, -1},
                             { 0,  1,  0,  0},
                             { 1,  0,  0,  0}};

RtBasis RiPowerBasis      = {{1, 0, 0, 0},
                             {0, 1, 0, 0},
                             {0, 0, 1, 0},
                             {0, 0, 0, 1}};

//------------------------------------------------------------------------------
// helper functions for building parameter lists from var args

static RtToken* _tokens = NULL;
static RtPointer* _parms = NULL;

static void getArgs(va_list args, unsigned int n)
{
	for (unsigned int i=0; i<n; i++) {
		_tokens[i] = va_arg(args, const char*);
		_parms[i] = va_arg(args, void*);
	}
}

static int cntArgs(va_list args)
{
	const char* t;
	void* p;
	int n = 0;
	t = va_arg(args, const char*);
	while (t!=NULL) {
		n++;
		p = va_arg(args, void*);
		// p should never be NULL, this would indicate an error in the parameter list
		assert(p != NULL);
		t = va_arg(args, const char*);
	}
	return n;
}

// note: on linux got crashes here,
// obviously have to restart the list after first pass in CountArgs()
// why it doesn't happen on win32 I do not know...
#define BUILD_TOKENPARAM_LIST(start_arg)\
	va_list args;\
	va_start(args, start_arg);\
	unsigned int n = cntArgs(args);\
	va_end(args);\
	if (n!=0) {\
		delete[] _tokens;\
		delete[] _parms;\
		_tokens = new RtToken[n];\
		_parms = new RtPointer[n];\
		va_start(args, start_arg);\
		getArgs(args, n);\
		va_end(args);\
	}\

//------------------------------------------------------------------------------
// helper functions to test for presence of 'P' array in parameter list (or Pz/Pw)
// P only
bool haveP(const char* funcname, RtInt n, RtToken tokens[], RtPointer parms[])
{
	for (int i=0; i<n; ++i)
		if (!strcmp(tokens[i], RI_P)) return true;
	fprintf(ERROUT, "[ERROR]: %s() -> required 'P' array is missing\n", funcname);
	return false;
}

// P or Pz
bool haveP_Pz(const char* funcname, RtInt n, RtToken tokens[], RtPointer parms[])
{
	for (int i=0; i<n; ++i)
		if ((!strcmp(tokens[i], RI_P)) || (!strcmp(tokens[i], RI_PZ))) return true;
	fprintf(ERROUT, "[ERROR]: %s() -> required 'P' or 'Pz' array is missing\n", funcname);
	return false;
}

// P or Pw
bool haveP_Pw(const char* funcname, RtInt n, RtToken tokens[], RtPointer parms[])
{
	for (int i=0; i<n; ++i)
		if ((!strcmp(tokens[i], RI_P)) || (!strcmp(tokens[i], RI_PW))) return true;
	fprintf(ERROUT, "[ERROR]: %s() -> required 'P' or 'Pw' array is missing\n", funcname);
	return false;
}

// any of P/Pz/Pw
bool haveP_any(const char* funcname, RtInt n, RtToken tokens[], RtPointer parms[])
{
	for (int i=0; i<n; ++i)
		if ((!strcmp(tokens[i], RI_P)) || (!strcmp(tokens[i], RI_PZ)) || (!strcmp(tokens[i], RI_PW))) return true;
	fprintf(ERROUT, "[ERROR]: %s() -> required 'P', 'Pz' or 'Pw' array is missing\n", funcname);
	return false;
}

//------------------------------------------------------------------------------
// motion block error checking

static int motion_pos = -1;
enum mbCmdType {MB_NONE,
// transforms
MB_TRANSFORM, MB_CONCATTRANSFORM, MB_PERSPECTIVE, MB_TRANSLATE, MB_ROTATE, MB_SCALE, MB_SKEW,
MB_PROJECTION, MB_DISPLACEMENT, MB_DEFORMATION,
// geometry
MB_BOUND, MB_DETAIL, MB_POLYGON, MB_GENERALPOLYGON, MB_POINTSPOLYGONS, MB_POINTSGENERALPOLYGONS, MB_PATCH,
MB_PATCHMESH, MB_NUPATCH, MB_SPHERE, MB_CONE, MB_CYLINDER, MB_HYPERBOLOID, MB_PARABOLOID, MB_DISK, MB_TORUS, MB_POINTS, MB_CURVES,
MB_SUBDIVISIONMESH, MB_BLOBBY,
// not in list of valid commands, but this must be valid too
MB_READARCHIVE,
// shading
MB_COLOR, MB_OPACITY, MB_LIGHTSOURCE, MB_AREALIGHTSOURCE, MB_SURFACE, MB_INTERIOR, MB_EXTERIOR, MB_ATMOSPHERE
};
static mbCmdType prev_motion_cmd = MB_NONE;
// commands in readarchive include files are not tested yet.
// for now temporary solution is to only check commands in main file
static bool in_readarchive = false;

static bool mbCheck(bool valid=false, mbCmdType cur_cmd=MB_NONE)
{
	if (cur_cmd == MB_READARCHIVE)
		in_readarchive = true;
	else if (in_readarchive)	// skip if reading archive files, see comment above
		return true;
	if (!valid) {
		if (motion_pos >= 0) {
			fprintf(ERROUT, "[ERROR]: illegal command inside motion block\n");
			State::Instance()->cancelMotion();
			return false;
		}
		return true;
	}
	if ((motion_pos >= 1) && (prev_motion_cmd != cur_cmd)) {
		fprintf(ERROUT, "[ERROR]: command inside current motion block is different from previous\n");
		State::Instance()->cancelMotion();
		return false;
	}
	if (motion_pos >= 0) {
		motion_pos++;
		prev_motion_cmd = cur_cmd;
	}
	return true;
}

//------------------------------------------------------------------------------
// Pixel Filters

RtFloat RiGaussianFilter(RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth)
{
	x *= (RtFloat)2 / xwidth;
	y *= (RtFloat)2 / ywidth;
	return expf((RtFloat)-2 * (x*x + y*y));
}

RtFloat RiBoxFilter(RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth)
{
	return (RtFloat)1;
}

RtFloat RiTriangleFilter(RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth)
{
	RtFloat hfx = xwidth * (RtFloat)0.5;
	RtFloat hfy = ywidth * (RtFloat)0.5;
	RtFloat ax = fabs(x), ay = fabs(y);
	return MIN2((hfx-ax)/hfx, (hfy-ay)/hfy);
}

RtFloat RiCatmullRomFilter(RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth)
{
	RtFloat r2 = (x*x + y*y);
	RtFloat r = sqrt(r2);
	return (r>=(RtFloat)2) ? (RtFloat)0 :
				(r<(RtFloat)1) ? ((RtFloat)3*r*r2 - (RtFloat)5*r2 + (RtFloat)2) :
				(-r*r2 + (RtFloat)5*r2 - (RtFloat)8*r + (RtFloat)4);
}

RtFloat RiSincFilter(RtFloat x, RtFloat y,
										RtFloat xwidth, RtFloat ywidth)
{
	RtFloat s,t;
	x *= (RtFloat)M_PI;
	y *= (RtFloat)M_PI;
	if ((x>-RI_EPSILON) && (x<RI_EPSILON))
		s = (RtFloat)1;
	else
		s = sinf(x)/x;
	if ((y>-RI_EPSILON) && (y<RI_EPSILON))
		t = (RtFloat)1;
	else
		t = sinf(y)/y;
	return s*t;
}

//------------------------------------------------------------------------------
// separable versions of all of the above (though not necessarly exactly the same...)
RtFloat RiSepGaussianFilter(RtFloat v, RtFloat width)
{
	v *= (RtFloat)2 / width;
	return (RtFloat)expf((RtFloat)-2*v*v);
}

RtFloat RiSepBoxFilter(RtFloat v, RtFloat width)
{
	return (RtFloat)1;
}

RtFloat RiSepTriangleFilter(RtFloat v, RtFloat width)
{
	RtFloat hf = width * (RtFloat)0.5;
	return (hf - ABS(v)) / hf;
}

RtFloat RiSepCatmullRomFilter(RtFloat v, RtFloat width)
{
	if (v < (RtFloat)-2) return (RtFloat)0;
	if (v < (RtFloat)-1) return (RtFloat)0.5*((RtFloat)4 +   v*((RtFloat) 8 + v*((RtFloat) 5 + v)));
	if (v < (RtFloat) 0) return (RtFloat)0.5*((RtFloat)2 + v*v*((RtFloat)-5 + v* (RtFloat)-3));
	if (v < (RtFloat) 1) return (RtFloat)0.5*((RtFloat)2 + v*v*((RtFloat)-5 + v* (RtFloat) 3));
	if (v < (RtFloat) 2) return (RtFloat)0.5*((RtFloat)4 +   v*((RtFloat)-8 + v*((RtFloat) 5 - v)));
	return (RtFloat)0;
}

RtFloat RiSepSincFilter(RtFloat v, RtFloat width)
{
	v *= (RtFloat)M_PI;
	if ((v > -RI_EPSILON) && (v < RI_EPSILON)) return (RtFloat)1;
	return (sinf(v) / v);
}

//------------------------------------------------------------------------------

RtToken RiDeclare(const char* name, const char* declaration)
{
	if (!mbCheck()) return NULL;
	decParam_t dp = {0, 0, 0};
	if (!parseDeclaration(declaration, &dp, NULL)) {
		fprintf(ERROUT, "[ERROR]: RiDeclare() -> parameter declaration error\n");
		return NULL;
	}
	State::Instance()->addDeclared(name, dp);
	return name;
}

// Graphics State
RtVoid RiBegin(RtToken name)
{
	//if (name) newfile?;
	State::Instance()->begin(name);
}

RtVoid RiEnd()
{
	// free last token/parm array allocs
	if (_tokens) delete[] _tokens;
	if (_parms) delete[] _parms;
	// state end
	State::Instance()->end();
}

RtContextHandle RiGetContext()
{
	return NULL;
}

RtVoid RiContext(RtContextHandle)
{
}

RtVoid RiFrameBegin(RtInt frame)
{
	if (!mbCheck()) return;
	State::Instance()->frameBegin(frame);
}

RtVoid RiFrameEnd()
{
	if (!mbCheck()) return;
	State::Instance()->frameEnd();
}

RtVoid RiWorldBegin()
{
	if (!mbCheck()) return;
	State::Instance()->worldBegin();
}

RtVoid RiWorldEnd()
{
	if (!mbCheck()) return;
	State::Instance()->worldEnd();
}

// Camera Options
RtVoid RiFormat(RtInt xres, RtInt yres, RtFloat aspect)
{
	if (!mbCheck()) return;
	Options& options = State::Instance()->topOptions();
	// if negative: use defaults
	if (xres>=0) options.xRes = xres;
	if (yres>=0) options.yRes = yres;
	if (aspect>=0) options.pixelAspectRatio = aspect;
	// also affects other related options
	options.frameAspectRatio = options.pixelAspectRatio * (xres / (float)yres);
	if (xres >= yres) {
		options.left = -options.frameAspectRatio;
		options.right = options.frameAspectRatio;
		options.bottom = -1.f;
		options.top = 1.f;
	}
	else {
		options.left = -1.f;
		options.right = 1.f;
		options.bottom = -1.f/options.frameAspectRatio;
		options.top = 1.f/options.frameAspectRatio;
	}
}

RtVoid RiFrameAspectRatio(RtFloat aspect)
{
	if (!mbCheck()) return;
	Options& options = State::Instance()->topOptions();
	options.frameAspectRatio = aspect;
	State::Instance()->resetFrameAspect = true;
}

RtVoid RiScreenWindow(RtFloat left, RtFloat right, RtFloat bot, RtFloat top)
{
	if (!mbCheck()) return;
	Options& options = State::Instance()->topOptions();
	options.left = left;
	options.right = right;
	options.bottom = bot;
	options.top = top;
	State::Instance()->resetScrWin = true;
}

RtVoid RiCropWindow(RtFloat xmin, RtFloat xmax, RtFloat ymin, RtFloat ymax)
{
	if (!mbCheck()) return;
	Options& options = State::Instance()->topOptions();
	options.xMin = xmin;
	options.xMax = xmax;
	options.yMin = ymin;
	options.yMax = ymax;
}

RtVoid RiProjection(RtToken name, ...)
{
	BUILD_TOKENPARAM_LIST(name)
	RiProjectionV(name, n, _tokens, _parms);
}

RtVoid RiProjectionV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_PROJECTION)) return;
	Options& options = State::Instance()->topOptions();
	if (!strcmp(name, RI_PERSPECTIVE)) {
		options.projection = Options::PROJ_PERSPECTIVE;
		// optional param 'fov'
		for (int i=0; i<n; i++) {
			if (!strcmp(tokens[i], RI_FOV)) {
				options.fov = *((RtFloat*)parms[i]);
				// nothing else yet
				break;
			}
		}
	}
	else if (!strcmp(name, RI_ORTHOGRAPHIC))
		options.projection = Options::PROJ_ORTHOGRAPHIC;
	else
		fprintf(ERROUT, "[WARNING]: RiProjection() -> unknown projection type '%s'\n", name);
}

RtVoid RiClipping(RtFloat hither, RtFloat yon)
{
	if (!mbCheck()) return;
	Options& options = State::Instance()->topOptions();
	options.nearClip = hither;
	options.farClip = yon;
}

RtVoid RiClippingPlane(RtFloat x, RtFloat y, RtFloat z, RtFloat nx, RtFloat ny, RtFloat nz)
{
	if (!mbCheck()) return;
	// not yet...
	fprintf(ERROUT, "'ClippingPlane' not implemented (yet)\n");
}

RtVoid RiDepthOfField(RtFloat fstop, RtFloat focallength, RtFloat focaldistance)
{
	if (!mbCheck()) return;
	Options& options = State::Instance()->topOptions();
	options.fStop = fstop;
	options.focalLength = focallength;
	options.focalDistance = focaldistance;
}

RtVoid RiShutter(RtFloat min, RtFloat max)
{
	if (!mbCheck()) return;
	Options& options = State::Instance()->topOptions();
	options.closeShutter = min;
	options.closeShutter = max;
}

// Display Options
RtVoid RiPixelVariance(RtFloat variation)
{
	if (!mbCheck()) return;
	Options& options = State::Instance()->topOptions();
	options.pixelVariance = variation;
}

RtVoid RiPixelSamples(RtFloat xsamples, RtFloat ysamples)
{
	if (!mbCheck()) return;
	Options& options = State::Instance()->topOptions();
	options.xSamples = xsamples;
	options.ySamples = ysamples;
}

RtVoid RiPixelFilter(RtFilterFunc filterfunc, RtFloat xwidth, RtFloat ywidth)
{
	if (!mbCheck()) return;
	Options& options = State::Instance()->topOptions();
	options.pixelFilter = filterfunc;
	options.xWidth = xwidth;
	options.yWidth = ywidth;
}

RtVoid RiExposure(RtFloat gain, RtFloat gamma)
{
	if (!mbCheck()) return;
	Options& options = State::Instance()->topOptions();
	options.gain = gain;
	options.gamma = gamma;
}

RtVoid RiImager(RtToken name, ...)
{
	BUILD_TOKENPARAM_LIST(name)
	RiImagerV(name, n, _tokens, _parms);
}

RtVoid RiImagerV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck()) return;
	// TODO
	fprintf(ERROUT, "'Imager' not implemented (yet)\n");
}

RtVoid RiQuantize(RtToken type, RtInt one, RtInt min, RtInt max, RtFloat ampl)
{
	if (!mbCheck()) return;
	Options& options = State::Instance()->topOptions();
	if (!strcmp(type, RI_RGBA)) {
		options.cqOne = one;
		options.cqMin = min;
		options.cqMax = max;
		options.cqDitherAmplitude = ampl;
	}
	else if (!strcmp(type, RI_Z)) {
		options.zqOne = one;
		options.zqMin = min;
		options.zqMax = max;
		options.zqDitherAmplitude = ampl;
	}
	else
	fprintf(ERROUT, "Unknown quantizer: '%s'\n", type);
}

RtVoid RiDisplay(const char* name, RtToken type, RtToken mode, ...)
{
	BUILD_TOKENPARAM_LIST(mode)
	RiDisplayV(name, type, mode, n, _tokens, _parms);
}

RtVoid RiDisplayV(const char* name, RtToken type, RtToken mode,
									RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck()) return;
	Options& options = State::Instance()->topOptions();
	// name
	strncpy(options.displayName, name, 80);
	// type
	if (!strcmp(type, RI_FRAMEBUFFER))
		options.displayType = Options::DT_FRAMEBUFFER;
	else if (!strcmp(type, RI_FILE))
		options.displayType = Options::DT_FILE;
	else if (!strcmp(type, RI_ZFILE))
		options.displayType = Options::DT_ZFILE;
	else
		fprintf(ERROUT, "Display type unknown: '%s'\n", type);
	// mode
	if (!strcmp(mode, RI_RGB))
		options.displayMode = Options::DM_RGB;
	else if (!strcmp(mode, RI_RGBA))
		options.displayMode = Options::DM_RGBA;
	else if (!strcmp(mode, RI_RGBZ))
		options.displayMode = Options::DM_RGBZ;
	else if (!strcmp(mode, RI_RGBAZ))
		options.displayMode = Options::DM_RGBAZ;
	else if (!strcmp(mode, RI_A))
		options.displayMode = Options::DM_A;
	else if (!strcmp(mode, RI_Z))
		options.displayMode = Options::DM_Z;
	else if (!strcmp(mode, RI_AZ))
		options.displayMode = Options::DM_AZ;
	else
		fprintf(ERROUT, "[WARNING]: Display mode unknown: '%s'\n", mode);
	// no other parameters used yet
}

// Additional Options
RtVoid RiHider(RtToken type, ...)
{
	BUILD_TOKENPARAM_LIST(type)
	RiHiderV(type, n, _tokens, _parms);
}

RtVoid RiHiderV(RtToken type, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck()) return;
	State* state = State::Instance();
	state->verifyMode(MODE_BASE, RIE_NOTSTARTED, "RiHiderV()");
	Options& options = state->topOptions();
	if (!strcmp(type, RI_HIDDEN))
		options.hider = Options::HD_HIDDEN;
	else if (!strcmp(type, RI_RAYTRACE))
		options.hider = Options::HD_RAYTRACE;
	else if (!strcmp(type, RI_HYBRID))
		options.hider = Options::HD_HYBRID;
	else {
		fprintf(ERROUT, "[WARNING] -> State::worldBegin(): Unknown hider '%s', will use 'hidden' instead\n", type);
		options.hider = Options::HD_HIDDEN;
	}
	// optional jitter param
	for (int i=0; i<n; i++) {
		if (!strcmp(tokens[i], RI_JITTER)) {
			options.jitter = MAX2(0.f, MIN2(1.f, *((RtFloat*)parms[i])));
			break;
		}
	}
}

RtVoid RiColorSamples(RtInt n, RtFloat nRGB[], RtFloat RGBn[])
{
	if (!mbCheck()) return;
	// TODO (possibly), for now fixed at 3 (RGB)
	fprintf(ERROUT, "'ColorSamples' not implemented (yet)\n");
}

RtVoid RiRelativeDetail(RtFloat relativedetail)
{
	if (!mbCheck()) return;
	Options& options = State::Instance()->topOptions();
	options.relativeDetail = relativedetail;
}

// Implementation-specific Options
RtVoid RiOption(RtToken name, ...)
{
	if (!mbCheck()) return;
	BUILD_TOKENPARAM_LIST(name)
	RiOptionV(name, n, _tokens, _parms);
}

RtVoid RiOptionV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck()) return;
	Options& options = State::Instance()->topOptions();
	if (!strcmp(name, "hider")) {
		for (int i=0; i<n; ++i) {
			if (!strcmp(tokens[i], RI_JITTER))
				options.jitter = MAX2(0.f, MIN2(1.f, *((RtFloat*)parms[i])));
			else
				fprintf(ERROUT, "Unknown hider option '%s'\n", tokens[i]);
		}
	}
	else if (!strcmp(name, "limits")) {
		for (int i=0; i<n; ++i) {
			if (!strcmp(tokens[i], RI_BUCKETSIZE))
				options.bucketsize = ABS(*((RtInt*)parms[i]));
			else if (!strcmp(tokens[i], RI_TEXTUREMEMORY))
				options.texturememory = ABS(*((RtInt*)parms[i]))*1024;
			else if (!strcmp(tokens[i], RI_GRIDSIZE))
				options.gridsize = ABS(*((RtInt*)parms[i]));
			else
				fprintf(ERROUT, "Unknown limits option '%s'\n", tokens[i]);
		}
	}
	else
		fprintf(ERROUT, "Unknown option '%s'\n", name);
}

//------------------------------------------------------------------------------
// helper function to get declared parameter values for shaders
bool getShaderParams(SlShaderInstance* SI, RtInt n, RtToken tokens[], RtPointer parms[])
{
	// get declared params
	for (int i=0; i<n; i++) {
		// skip special case 'code' if found
		if (!strcmp(tokens[i], "code")) continue;
		// get parameter
		decParam_t dp = {0, 0, 0};
		char inline_name[256] = {0};
		const char* varname = tokens[i];
		if (!State::Instance()->parameterFromName(varname, dp, inline_name)) continue;
		if (inline_name[0] != 0) varname = inline_name; // was inline declared
		// get value
		if (dp.ct_flags & DT_FLOAT)
			SI->setParameter(varname, SL_FLOAT, ((dp.ct_flags & SC_VARYING)!=0), dp.arlen, parms[i]);
		else if (dp.ct_flags & DT_STRING)
			SI->setParameter(varname, SL_STRING, ((dp.ct_flags & SC_VARYING)!=0), dp.arlen, parms[i]);
		else if (dp.ct_flags & DT_FLOAT3MASK) {
			// to simplify, point/vector/normal/color all accessed as vector,
			// all are float[3] typedefs anyway
			RtVector *parm = (RtVector*)parms[i];
			RtVector* tv = new RtVector[dp.arlen];
			// current space transform
			const Transform* xf = SI->xform;
			if (dp.ct_flags & DT_POINT) {
				for (int j=0; j<dp.arlen; j++) {
					Point3 t = *xf * Point3(parm[j][0], parm[j][1], parm[j][2]);
					tv[j][0] = t.x, tv[j][1] = t.y, tv[j][2] = t.z;
				}
				SI->setParameter(varname, SL_POINT, ((dp.ct_flags & SC_VARYING)!=0), dp.arlen, tv);
			}
			else if (dp.ct_flags & DT_VECTOR) {
				for (int j=0; j<dp.arlen; j++) {
					Vector t = *xf * Vector(parm[j][0], parm[j][1], parm[j][2]);
					tv[j][0] = t.x, tv[j][1] = t.y, tv[j][2] = t.z;
				}
				SI->setParameter(varname, SL_VECTOR, ((dp.ct_flags & SC_VARYING)!=0), dp.arlen, tv);
			}
			else if (dp.ct_flags & DT_NORMAL) {
				Transform nxf = *xf;
				nxf.transpose();
				nxf.invert();
				for (int j=0; j<dp.arlen; j++) {
					Vector t = nxf * Vector(parm[j][0], parm[j][1], parm[j][2]);
					tv[j][0] = t.x, tv[j][1] = t.y, tv[j][2] = t.z;
				}
				SI->setParameter(varname, SL_NORMAL, ((dp.ct_flags & SC_VARYING)!=0), dp.arlen, tv);
			}
			else if (dp.ct_flags & DT_COLOR) // no xform
				SI->setParameter(varname, SL_COLOR, ((dp.ct_flags & SC_VARYING)!=0), dp.arlen, parms[i]);
			delete[] tv;
		}
		else if (dp.ct_flags & DT_MATRIX)
			SI->setParameter(varname, SL_MATRIX, ((dp.ct_flags & SC_VARYING)!=0), dp.arlen, parms[i]);
		/*
		// RSL does not have hpoint type, needs conversion to point. TODO
		else if (ct_flags & DT_HPOINT)
			SI->setParameter(varname, SL_HPOINT, ((dp.ct_flags & SC_VARYING)!=0), dp.arlen, parms[i]);
		// integer type also not supported, should generate error?
		else if (ct_flags & DT_INT)
			SI->setParameter(varname, SL_INTEGER, ((dp.ct_flags & SC_VARYING)!=0), dp.arlen, parms[i]);
		*/
		else {
			fprintf(ERROUT, "[ERROR]: getShaderParams() -> unexpected unknown type '%s'\n", tokens[i]);
			return false;
		}
	}
	return true;
}
//------------------------------------------------------------------------------
// Attributes
RtVoid RiAttributeBegin()
{
	State::Instance()->attributeBegin();
}

RtVoid RiAttributeEnd()
{
	State::Instance()->attributeEnd();
}

// Color & Opacity
RtVoid RiColor(RtColor color)
{
	if (!mbCheck(true, MB_COLOR)) return;
	Attributes& attr = State::Instance()->topAttributes();
	attr.color[0] = color[0];
	attr.color[1] = color[1];
	attr.color[2] = color[2];
}

RtVoid RiOpacity(RtColor color)
{
	if (!mbCheck(true, MB_OPACITY)) return;
	Attributes& attr = State::Instance()->topAttributes();
	attr.opacity[0] = color[0];
	attr.opacity[1] = color[1];
	attr.opacity[2] = color[2];
}

// Texture Coordinates
RtVoid RiTextureCoordinates(RtFloat s1, RtFloat t1, RtFloat s2, RtFloat t2,
														RtFloat s3, RtFloat t3, RtFloat s4, RtFloat t4)
{
	if (!mbCheck()) return;
	Attributes& attr = State::Instance()->topAttributes();
	attr.texCoords[0] = s1;
	attr.texCoords[1] = t1;
	attr.texCoords[2] = s2;
	attr.texCoords[3] = t2;
	attr.texCoords[4] = s3;
	attr.texCoords[5] = t3;
	attr.texCoords[6] = s4;
	attr.texCoords[7] = t4;
}

// Light Sources
RtLightHandle RiLightSource(RtToken name, ...)
{
	BUILD_TOKENPARAM_LIST(name)
	return RiLightSourceV(name, n, _tokens, _parms);
}

RtLightHandle RiLightSourceV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_LIGHTSOURCE)) return NULL;

	State* state = State::Instance();

	SlShaderInstance* SI;
	SI = state->loadShader(name, SL_LIGHT);	// on success will also add lightshader to current lightsource list
	if (SI == NULL) return NULL; // load failed

	// run default initialization code
	SI->run_initcode();
	// then set userdefined params, possibly overriding defaults
	getShaderParams(SI, n, tokens, parms);

	return SI;
}

RtLightHandle RiAreaLightSource(RtToken name, ...)
{
	BUILD_TOKENPARAM_LIST(name)
	RtLightHandle LH = RiAreaLightSourceV(name, n, _tokens, _parms);
	return LH;
}

RtLightHandle RiAreaLightSourceV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_AREALIGHTSOURCE)) return NULL;
	// TODO
	fprintf(ERROUT, "'AreaLightSource' not implemented (yet)\n");
	return NULL;
}

RtVoid RiIlluminate(RtLightHandle light, RtBoolean onoff)
{
	if (!mbCheck()) return;
	State::Instance()->setLight(light, onoff);
}

// Surface Shading
RtVoid RiSurface(RtToken name, ...)
{
	BUILD_TOKENPARAM_LIST(name)
	RiSurfaceV(name, n, _tokens, _parms);
}

RtVoid RiSurfaceV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_SURFACE)) return;

	State* state = State::Instance();

	SlShaderInstance* SI;
	SI = state->loadShader(name, SL_SURFACE);
	if (SI == NULL) return; // load failed

	// run default initialization code
	SI->run_initcode();
	// then set userdefined params, possibly overriding defaults
	getShaderParams(SI, n, tokens, parms);
}

// Displacement Shading
RtVoid RiDisplacement(RtToken name, ...)
{
	BUILD_TOKENPARAM_LIST(name)
	RiDisplacementV(name, n, _tokens, _parms);
}

RtVoid RiDisplacementV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_DISPLACEMENT)) return;

	State* state = State::Instance();

	SlShaderInstance* SI;
	SI = state->loadShader(name, SL_DISPLACEMENT);
	if (SI == NULL) return; // load failed

	// run default initialization code
	SI->run_initcode();
	// then set userdefined params, possibly overriding defaults
	getShaderParams(SI, n, tokens, parms);
}

// Volume Shading
RtVoid RiAtmosphere(RtToken name, ...)
{
	BUILD_TOKENPARAM_LIST(name)
	RiAtmosphereV(name, n, _tokens, _parms);
}

RtVoid RiAtmosphereV(RtToken name,RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_ATMOSPHERE)) return;

	State* state = State::Instance();

	SlShaderInstance* SI;
	SI = state->loadShader(name, SL_VOLUME);
	if (SI == NULL) return; // load failed

	// run default initialization code
	SI->run_initcode();
	// then set userdefined params, possibly overriding defaults
	getShaderParams(SI, n, tokens, parms);
}

RtVoid RiInterior(RtToken name, ...)
{
	BUILD_TOKENPARAM_LIST(name)
	RiInteriorV(name, n, _tokens, _parms);
}

RtVoid RiInteriorV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_INTERIOR)) return;
	// TODO
	fprintf(ERROUT, "'Interior' not implemented (yet)\n");
}

RtVoid RiExterior(RtToken name, ...)
{
	BUILD_TOKENPARAM_LIST(name)
	RiExteriorV(name, n, _tokens, _parms);
}

RtVoid RiExteriorV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_EXTERIOR)) return;
	// TODO
	fprintf(ERROUT, "'Exterior' not implemented (yet)\n");
}

// ShadingRate
RtVoid RiShadingRate(RtFloat size)
{
	if (!mbCheck()) return;
	Attributes& attr = State::Instance()->topAttributes();
	// since the diceable test works with edgelengths and not area's,
	// set effectiveShadingRate to some approximate edgelength assuming a square area
	attr.effectiveShadingRate = sqrtf(size);
}

// ShadingInterpolation
RtVoid RiShadingInterpolation(RtToken type)
{
	if (!mbCheck()) return;
	Attributes& attr = State::Instance()->topAttributes();
	if (!strcmp(type, RI_CONSTANT))
		attr.flags &= ~AF_SMOOTH;
	else if (!strcmp(type, RI_SMOOTH))
		attr.flags |= AF_SMOOTH;
	else
		fprintf(ERROUT, "[WARNING]: ShadingInterpolation type unknown: '%s'\n", type);
}

// Matte Objects
RtVoid RiMatte(RtBoolean onoff)
{
	if (!mbCheck()) return;
	Attributes& attr = State::Instance()->topAttributes();
	if (onoff == (RtBoolean)RI_TRUE)
		attr.flags |= AF_MATTE;
	else
		attr.flags &= ~AF_MATTE;
}

// Bound
RtVoid RiBound(RtBound bound)
{
	if (!mbCheck(true, MB_BOUND)) return;
	// currently not used
	/*
	Attributes& attr = State::Instance()->topAttributes();
	attr.bound[0] = bound[0];
	attr.bound[1] = bound[1];
	attr.bound[2] = bound[2];
	attr.bound[3] = bound[3];
	attr.bound[4] = bound[4];
	attr.bound[5] = bound[5];
	*/
}

// Detail
RtVoid RiDetail(RtBound bound)
{
	if (!mbCheck(true, MB_DETAIL)) return;
	// currently not used
	/*
	Attributes& attr = State::Instance()->topAttributes();
	attr.detail[0] = bound[0];
	attr.detail[1] = bound[1];
	attr.detail[2] = bound[2];
	attr.detail[3] = bound[3];
	attr.detail[4] = bound[4];
	attr.detail[5] = bound[5];
	*/
}

RtVoid RiDetailRange(RtFloat minvis, RtFloat lowtran, RtFloat uptran, RtFloat maxvis)
{
	// currently not used
	/*
	Attributes& attr = State::Instance()->topAttributes();
	attr.minVisible = minvis;
	attr.lowerTransition = lowtran;
	attr.upperTransition = uptran;
	attr.maxVisible = maxvis;
	*/
}

// Geometric Approximation
RtVoid RiGeometricApproximation(RtToken type, RtFloat value)
{
	if (!mbCheck()) return;
	// currently not used
	/*
	Attributes& attr = State::Instance()->topAttributes();
	if (!strcmp(type, RI_FLATNESS))
		attr.valueApproximation = value;
	else
		fprintf(ERROUT, "[WARNING]: GeometricApproximation type unknown: '%s'\n", type);
	*/
}

// Orientation and Sides
RtVoid RiOrientation(RtToken orientation)
{
	if (!mbCheck()) return;
	Attributes& attr = State::Instance()->topAttributes();
	bool isrh = State::Instance()->currentTransform().handedness();
	if (!strcmp(orientation, RI_OUTSIDE))
		attr.flags &= ~AF_ORIENTATION;
	else if (!strcmp(orientation, RI_INSIDE))
		attr.flags |= AF_ORIENTATION;
	else if (!strcmp(orientation, RI_LH)) {
		if (isrh) attr.flags |= AF_ORIENTATION; else attr.flags &= ~AF_ORIENTATION;
	}
	else if (!strcmp(orientation, RI_RH)) {
		if (isrh) attr.flags &= ~AF_ORIENTATION; else attr.flags |= AF_ORIENTATION;
	}
	else
		fprintf(ERROUT, "[WARNING]: Orientation unknown: '%s'\n", orientation);
}

RtVoid RiReverseOrientation()
{
	if (!mbCheck()) return;
	Attributes& attr = State::Instance()->topAttributes();
	attr.flags ^= AF_ORIENTATION;
}

RtVoid RiSides(RtInt sides)
{
	if (!mbCheck()) return;
	Attributes& attr = State::Instance()->topAttributes();
	if (sides == 1)
		attr.flags |= AF_SIDES1;
	else
		attr.flags &= ~AF_SIDES1;
}

// Transformations
RtVoid RiIdentity()
{
	if (!mbCheck()) return;
	State* state = State::Instance();
	const Transform idt = state->transformIdentity();
	Transform& curT = state->currentTransform();
	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			curT[i][j] = idt[i][j];
}

RtVoid RiTransform(RtMatrix transform)
{
	if (!mbCheck(true, MB_TRANSFORM)) return;
	State* state = State::Instance();
	const Transform idt = state->transformIdentity();
	RtMatrix nm;
	mulMMM(nm, *idt.getRtMatrixPtr(), transform);
	// NOTE motion_pos is already incremented at this point, so have to subtract 1,
	// is ok, since currentTransform only checks for negative val. anyway
	Transform& curT = state->currentTransform(motion_pos - 1);
	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			curT[i][j] = nm[i][j];
}

RtVoid RiConcatTransform(RtMatrix transform)
{
	if (!mbCheck(true, MB_CONCATTRANSFORM)) return;
	Transform& curT = State::Instance()->currentTransform(motion_pos - 1);
	curT *= Transform(transform);
}

RtVoid RiPerspective(RtFloat fov)
{
	if (!mbCheck(true, MB_PERSPECTIVE)) return;
	State* state = State::Instance();
	Transform& curT = state->currentTransform(motion_pos - 1);
	Options &options = state->topOptions();
	curT.perspective(fov, options.nearClip, options.farClip);
}

RtVoid RiTranslate(RtFloat dx, RtFloat dy, RtFloat dz)
{
	if (!mbCheck(true, MB_TRANSLATE)) return;
	Transform& curT = State::Instance()->currentTransform(motion_pos - 1);
	curT.translate(dx, dy, dz);
}

RtVoid RiRotate(RtFloat angle, RtFloat dx, RtFloat dy, RtFloat dz)
{
	if (!mbCheck(true, MB_ROTATE)) return;
	Transform& curT = State::Instance()->currentTransform(motion_pos - 1);
	curT.rotate(angle, dx, dy, dz);
}

RtVoid RiScale(RtFloat sx, RtFloat sy, RtFloat sz)
{
	if (!mbCheck(true, MB_SCALE)) return;
	Transform& curT = State::Instance()->currentTransform(motion_pos - 1);
	curT.scale(sx, sy, sz);
}

RtVoid RiSkew(RtFloat angle, RtFloat dx1, RtFloat dy1, RtFloat dz1,
							RtFloat dx2, RtFloat dy2, RtFloat dz2)
{
	if (!mbCheck(true, MB_SKEW)) return;
	Transform& curT = State::Instance()->currentTransform(motion_pos - 1);
	curT.skew(angle, dx1, dy1, dz1, dx2, dy2, dz2);
}

RtVoid RiDeformation(RtToken name, ...)
{
	BUILD_TOKENPARAM_LIST(name)
	RiDeformationV(name, n, _tokens, _parms);
}

RtVoid RiDeformationV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_DEFORMATION)) return;
	// not implemented
	fprintf(ERROUT, "'Deformation' not implemented (yet)\n");
}

// Named Coordinate Systems
RtVoid RiCoordinateSystem(RtToken space)
{
	if (!mbCheck()) return;
	State::Instance()->addNamedCoordSys(space);
}

RtVoid RiCoordSysTransform(RtToken space)
{
	if (!mbCheck()) return;
	State::Instance()->activateNamedCoordSys(space);
}

RtPoint* RiTransformPoints(RtToken fromspace, RtToken tospace, RtInt n, RtPoint points[])
{
	if (!mbCheck()) return NULL;
	// TODO (?)
	fprintf(ERROUT, "RiTransformPoints() not implemented (yet)\n");
	return NULL;
}

// Transformation Stack
RtVoid RiTransformBegin()
{
	State::Instance()->transformBegin();
}

RtVoid RiTransformEnd()
{
	State::Instance()->transformEnd();
}

// Implementation-specific Attributes
RtVoid RiAttribute(RtToken name, ...)
{
	BUILD_TOKENPARAM_LIST(name)
	RiAttributeV(name, n, _tokens, _parms);
}

RtVoid RiAttributeV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck()) return;
	Attributes& attr = State::Instance()->topAttributes();
	if (!strcmp(name, "displacementbound")) {
		for (int i=0; i<n; ++i) {
			if (!strcmp(tokens[i], RI_SPHERE))
				attr.displacementbound_sphere = *((RtFloat*)parms[i]);
			// TODO coordinatesystem
			else
				fprintf(ERROUT, "Unknown displacementbound attribute: '%s'\n", tokens[i]);
		}
	}
	else if (!strcmp(name, "dice")) {
		for (int i=0; i<n; ++i) {
			if (!strcmp(tokens[i], RI_BINARY))
				attr.dice_binary = ABS(*((RtBoolean*)parms[i]));
			else
				fprintf(ERROUT, "Unknown dice attribute: '%s'\n", tokens[i]);
		}
	}
	else fprintf(ERROUT, "Unknown attribute: '%s'\n", name);
}

//------------------------------------------------------------------------------
// Primitives

// Polygons
RtVoid RiPolygon(RtInt nverts, ...)
{
	BUILD_TOKENPARAM_LIST(nverts)
	RiPolygonV(nverts, n, _tokens, _parms);
}

RtVoid RiPolygonV(RtInt nverts, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_POLYGON)) return;
	// three verts at least
	if (nverts < 3) {
		fprintf(ERROUT, "[ERROR]: RiPolygonV() -> must have at least three vertices\n");
		return;
	}
	if (!haveP("RiPolygonV", n, tokens, parms)) return;
	State::Instance()->insert(new Polygon(nverts, n, tokens, parms), motion_pos);
}

RtVoid RiGeneralPolygon(RtInt nloops, RtInt nverts[], ...)
{
	BUILD_TOKENPARAM_LIST(nverts)
	RiGeneralPolygonV(nloops, nverts, n, _tokens, _parms);
}

RtVoid RiGeneralPolygonV(RtInt nloops, RtInt nverts[],
                         RtInt n, RtToken tokens[], RtPointer parms[])
{
	// TODO
	fprintf(ERROUT, "'GeneralPolygon' not implemented (yet)\n");
	// not implemented, so no motion block update
}

RtVoid RiPointsPolygons(RtInt npolys, RtInt nverts[], RtInt verts[], ...)
{
	BUILD_TOKENPARAM_LIST(verts)
	RiPointsPolygonsV(npolys, nverts, verts, n, _tokens, _parms);
}

RtVoid RiPointsPolygonsV(RtInt npolys, RtInt nverts[], RtInt verts[],
                         RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_POINTSPOLYGONS)) return;
	if (!haveP("RiPointsPolygons", n, tokens, parms)) return;
	State::Instance()->insert(new PointsPolygons(npolys, nverts, verts, n, tokens, parms), motion_pos);
}

RtVoid RiPointsGeneralPolygons(RtInt npolys, RtInt nloops[], RtInt nverts[], RtInt verts[], ...)
{
	BUILD_TOKENPARAM_LIST(verts)
	RiPointsGeneralPolygonsV(npolys, nloops, nverts, verts, n, _tokens, _parms);
}

RtVoid RiPointsGeneralPolygonsV(RtInt npolys, RtInt nloops[], RtInt nverts[],
                                RtInt verts[], RtInt n, RtToken tokens[], RtPointer parms[])
{
	// TODO
	fprintf(ERROUT, "'PointsGeneralPolygons' not implemented (yet)\n");
	// not implemented, so no motion block update
}

// Patches
RtVoid RiBasis(RtBasis ubasis, RtInt ustep, RtBasis vbasis, RtInt vstep)
{
	if (!mbCheck()) return;
	State* state = State::Instance();
	// basis allocation is handled by state
	state->newCubicBasis(ubasis, vbasis);
	Attributes& attr = state->topAttributes();
	attr.cubicBasisStep[0] = ustep;
	attr.cubicBasisStep[1] = vstep;
}

RtVoid RiPatch(RtToken type, ...)
{
	BUILD_TOKENPARAM_LIST(type)
	RiPatchV(type, n, _tokens, _parms);
}

RtVoid RiPatchV(RtToken type, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_PATCH)) return;
	if (!haveP_any("RiPatchV", n, tokens, parms)) return;
	State* state = State::Instance();
	if (!strcmp(type, RI_BILINEAR))
		state->insert(new BilinearPatch(n, tokens, parms), motion_pos);
	else if (!strcmp(type, RI_BICUBIC))
		state->insert(new BicubicPatch(n, tokens, parms), motion_pos);
	else
		fprintf(ERROUT, "RiPatchV(): Unknown type '%s'\n", type);
}

RtVoid RiPatchMesh(RtToken type, RtInt nu, RtToken uwrap,
                   RtInt nv, RtToken vwrap, ...)
{
	BUILD_TOKENPARAM_LIST(vwrap)
	RiPatchMeshV(type, nu, uwrap, nv, vwrap, n, _tokens, _parms);
}

RtVoid RiPatchMeshV(RtToken type, RtInt nu, RtToken uwrap,
                    RtInt nv, RtToken vwrap,
                    RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_PATCHMESH)) return;
	if (!haveP_any("RiPatchMeshV", n, tokens, parms)) return;
	bool bilinear = true;  // BILINEAR default
	if (!strcmp(type, RI_BICUBIC))
		bilinear = false;
	else if (strcmp(type, RI_BILINEAR)) {
		// unknown type
		fprintf(ERROUT, "RiPatchV(): Unknown type '%s'", type);
		return;
	}
	// wrap type
	bool uperiodic = false, vperiodic = false;
	if (!strcmp(uwrap, RI_PERIODIC))
		uperiodic = true;
	else if (strcmp(uwrap, RI_NONPERIODIC)) // unknown, assume uperiodic == false
		fprintf(ERROUT, "RiPatchMeshV(): unknown uwrap mode '%s'\n", uwrap);
	if (!strcmp(vwrap, RI_PERIODIC))
		vperiodic = true;
	else if (strcmp(vwrap, RI_NONPERIODIC)) // unknown, assume vperiodic == false
		fprintf(ERROUT, "RiPatchMeshV(): unknown vwrap mode '%s'\n", vwrap);
	if (bilinear)
		State::Instance()->insert(new BilinearPatchMesh(nu, uperiodic, nv, vperiodic, n, tokens, parms), motion_pos);
	else
		State::Instance()->insert(new BicubicPatchMesh(nu, uperiodic, nv, vperiodic, n, tokens, parms), motion_pos);
}

RtVoid RiNuPatch(RtInt nu, RtInt uorder, RtFloat uknot[], RtFloat umin,
                 RtFloat umax, RtInt nv, RtInt vorder, RtFloat vknot[],
                 RtFloat vmin, RtFloat vmax, ...)
{
	BUILD_TOKENPARAM_LIST(vmax)
	RiNuPatchV(nu, uorder, uknot, umin, umax, nv, vorder, vknot, vmin, vmax, n, _tokens, _parms);
}

RtVoid RiNuPatchV(RtInt nu, RtInt uorder, RtFloat uknot[], RtFloat umin,
                  RtFloat umax, RtInt nv, RtInt vorder, RtFloat vknot[],
                  RtFloat vmin, RtFloat vmax,
                  RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_NUPATCH)) return;
	if (!haveP_Pw("RiNuPatchV", n, tokens, parms)) return;
	// check that knots arrays are in increasing order
	for (int i=0; i<(nu + uorder - 1); ++i)
		if (uknot[i] > uknot[i+1]) {
			fprintf(ERROUT, "RiNuPatchV(): invalid decreasing knot value in uknot array\n");
			return;
		}
	for (int i=0; i<(nv + vorder - 1); ++i)
		if (vknot[i] > vknot[i+1]) {
			fprintf(ERROUT, "RiNuPatchV(): invalid decreasing knot value in vknot array\n");
			return;
		}
	// check that u/v min/max range is in valid range from the given knot arrays
	if (umin < uknot[uorder-1]) {
		fprintf(ERROUT, "RiNuPatchV(): umin value out of valid range [%f %f]\n", uknot[uorder-1], uknot[nu]);
		return;
	}
	if (umax > uknot[nu]) {
		fprintf(ERROUT, "RiNuPatchV(): umax value out of valid range [%f %f]\n", uknot[uorder-1], uknot[nu]);
		return;
	}
	if (vmin < vknot[vorder-1]) {
		fprintf(ERROUT, "RiNuPatchV(): vmin value out of valid range [%f %f]\n", vknot[vorder-1], vknot[nv]);
		return;
	}
	if (vmax > vknot[nv]) {
		fprintf(ERROUT, "RiNuPatchV(): vmax value out of valid range [%f %f]\n", vknot[vorder-1], vknot[nv]);
		return;
	}
	State::Instance()->insert(new NuPatch(nu, uorder, uknot, umin, umax,
	                                                          nv, vorder, vknot, vmin, vmax,
	                                                          n, tokens, parms), motion_pos);
}

RtVoid RiTrimCurve(RtInt nloops, RtInt ncurves[], RtInt order[],
                   RtFloat knot[], RtFloat min[], RtFloat max[], RtInt n[],
                   RtFloat u[], RtFloat v[], RtFloat w[])
{
	if (!mbCheck()) return;
	// TODO
	fprintf(ERROUT, "'TrimCurve' not implemented (yet)\n");
}

// Subdivision Meshes
RtVoid RiSubdivisionMesh(RtToken mask, RtInt nf, RtInt nverts[],
                         RtInt verts[],
                         RtInt ntags, RtToken tags[], RtInt numargs[],
                         RtInt intargs[], RtFloat floatargs[], ...)
{
	BUILD_TOKENPARAM_LIST(floatargs)
	RiSubdivisionMeshV(mask, nf, nverts, verts, ntags, tags, numargs, intargs, floatargs, n, _tokens, _parms);
}

RtVoid RiSubdivisionMeshV(RtToken mask, RtInt nf, RtInt nverts[],
                          RtInt verts[], RtInt ntags, RtToken tags[],
                          RtInt nargs[], RtInt intargs[],
                          RtFloat floatargs[], RtInt n,
                          RtToken tokens[], RtPointer *parms)
{
	if (!mbCheck(true, MB_SUBDIVISIONMESH)) return;
	if (strcmp(mask, "catmull-clark")) {
		fprintf(ERROUT, "Unknown subdivision scheme '%s'\n", mask);
		return;
	}
	State::Instance()->insert(new CCSubdivisionMesh(nf, nverts, verts, ntags, tags,
	                                                nargs, intargs, floatargs, n, tokens, parms), motion_pos);
}


// Quadrics
RtVoid RiSphere(RtFloat radius, RtFloat zmin, RtFloat zmax, RtFloat tmax, ...)
{
	BUILD_TOKENPARAM_LIST(tmax)
	RiSphereV(radius, zmin, zmax, tmax, n, _tokens, _parms);
}

RtVoid RiSphereV(RtFloat radius, RtFloat zmin, RtFloat zmax, RtFloat tmax,
                 RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_SPHERE)) return;
	State::Instance()->insert(new Sphere(radius, zmin, zmax, tmax, n, tokens, parms), motion_pos);
}

RtVoid RiCone(RtFloat height, RtFloat radius, RtFloat tmax, ...)
{
	BUILD_TOKENPARAM_LIST(tmax)
	RiConeV(height, radius, tmax, n, _tokens, _parms);
}

RtVoid RiConeV(RtFloat height, RtFloat radius, RtFloat tmax,
               RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_CONE)) return;
	State::Instance()->insert(new Cone(height, radius, tmax, n, tokens, parms), motion_pos);
}

RtVoid RiCylinder(RtFloat radius, RtFloat zmin, RtFloat zmax, RtFloat tmax, ...)
{
	BUILD_TOKENPARAM_LIST(tmax)
	RiCylinderV(radius, zmin, zmax, tmax, n, _tokens, _parms);
}

RtVoid RiCylinderV(RtFloat radius, RtFloat zmin, RtFloat zmax, RtFloat tmax,
                   RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_CYLINDER)) return;
	State::Instance()->insert(new Cylinder(radius, zmin, zmax, tmax, n, tokens, parms), motion_pos);
}

RtVoid RiHyperboloid(RtPoint point1, RtPoint point2, RtFloat tmax, ...)
{
	BUILD_TOKENPARAM_LIST(tmax)
	RiHyperboloidV(point1, point2, tmax, n, _tokens, _parms);
}

RtVoid RiHyperboloidV(RtPoint point1, RtPoint point2, RtFloat tmax,
                      RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_HYPERBOLOID)) return;
	Point3 pt1(point1[0], point1[1], point1[2]);
	Point3 pt2(point2[0], point2[1], point2[2]);
	State::Instance()->insert(new Hyperboloid(pt1, pt2, tmax, n, tokens, parms), motion_pos);
}

RtVoid RiParaboloid(RtFloat rmax, RtFloat zmin, RtFloat zmax, RtFloat tmax, ...)
{
	BUILD_TOKENPARAM_LIST(tmax)
	RiParaboloidV(rmax, zmin, zmax, tmax, n, _tokens, _parms);
}

RtVoid RiParaboloidV(RtFloat rmax, RtFloat zmin, RtFloat zmax, RtFloat tmax,
                     RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_PARABOLOID)) return;
	State::Instance()->insert(new Paraboloid(rmax, zmin, zmax, tmax, n, tokens, parms), motion_pos);
}

RtVoid RiDisk(RtFloat height, RtFloat radius, RtFloat tmax, ...)
{
	BUILD_TOKENPARAM_LIST(tmax)
	RiDiskV(height, radius, tmax, n, _tokens, _parms);
}

RtVoid RiDiskV(RtFloat height, RtFloat radius, RtFloat tmax,
               RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_DISK)) return;
	State::Instance()->insert(new Disk(height, radius, tmax, n, tokens, parms), motion_pos);
}

RtVoid RiTorus(RtFloat majrad, RtFloat minrad, RtFloat phimin,
               RtFloat phimax, RtFloat tmax, ...)
{
	BUILD_TOKENPARAM_LIST(tmax)
	RiTorusV(majrad, minrad, phimin, phimax, tmax, n, _tokens, _parms);
}

RtVoid RiTorusV(RtFloat majrad, RtFloat minrad,
                RtFloat phimin, RtFloat phimax, RtFloat tmax,
                RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_TORUS)) return;
	State::Instance()->insert(new Torus(majrad, minrad, phimin, phimax, tmax, n, tokens, parms), motion_pos);
}

// Point & Curve Primitives
RtVoid RiPoints(RtInt nverts,...)
{
	BUILD_TOKENPARAM_LIST(nverts)
	RiPointsV(nverts, n, _tokens, _parms);
}

RtVoid RiPointsV(RtInt nverts, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_POINTS)) return;
	if (!haveP("RiPointsV", n, tokens, parms)) return;
	State::Instance()->insert(new PointSet(nverts, n, tokens, parms), motion_pos);
}

RtVoid RiCurves(RtToken type, RtInt ncurves,
                RtInt nvertices[], RtToken wrap, ...)
{
	BUILD_TOKENPARAM_LIST(wrap)
	RiCurvesV(type, ncurves, nvertices, wrap, n, _tokens, _parms);
}

RtVoid RiCurvesV(RtToken type, RtInt ncurves, RtInt nvertices[], RtToken wrap,
                 RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_CURVES)) return;
	if (!haveP("RiCurvesV", n, tokens, parms)) return;
	bool islinear;
	if (!strcmp(type, RI_LINEAR))
		islinear = true;
	else if (!strcmp(type, RI_CUBIC))
		islinear = false;
	else {
		fprintf(ERROUT, "RiCurvesV(): unknown type '%s'\n", type);
		return;
	}
	bool _wrap;
	if (!strcmp(wrap, RI_PERIODIC))
		_wrap = true;
	else if (!strcmp(wrap, RI_NONPERIODIC))
		_wrap = false;
	else {
		fprintf(ERROUT, "RiCurvesV():  unknown wrap type '%s'\n", wrap);
		return;
	}
	State::Instance()->insert(new CurveSet(islinear, ncurves, nvertices, _wrap, n, tokens, parms), motion_pos);
}


// 'Blobby' Implicit Surfaces
RtVoid RiBlobby(RtInt nleaf, RtInt ncode, RtInt code[],
                RtInt nflt, RtFloat flt[],
                RtInt nstr, RtToken str[], ...)
{
	BUILD_TOKENPARAM_LIST(str)
	RiBlobbyV(nleaf, ncode, code, nflt, flt, nstr, str, n, _tokens, _parms);
}

RtVoid RiBlobbyV(RtInt nleaf, RtInt ncode, RtInt code[],
                 RtInt nflt, RtFloat flt[],
                 RtInt nstr, RtToken str[],
                 RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_BLOBBY)) return;
	// TODO
	fprintf(ERROUT, "'Blobby' not implemented (yet)\n");
}


// Procedural Primitives
RtVoid RiProcedural(RtPointer data, RtBound bound,
                    RtVoid (*subdivfunc)(RtPointer, RtFloat),
                    RtVoid (*freefunc)(RtPointer))
{
	if (!mbCheck()) return;
	// TODO
	fprintf(ERROUT, "'Procedural' not implemented (yet)\n");
}


RtVoid RiProcDelayedReadArchive(RtPointer data, RtFloat detail)
{
	if (!mbCheck()) return;
	// TODO
	fprintf(ERROUT, "'ProcDelayedReadArchive' not implemented (yet)\n");
}

RtVoid RiProcRunProgram(RtPointer data, RtFloat detail)
{
	if (!mbCheck()) return;
	// TODO
	fprintf(ERROUT, "'ProcRunProgram' not implemented (yet)\n");
}

RtVoid RiProcDynamicLoad(RtPointer data, RtFloat detail)
{
	if (!mbCheck()) return;
	// TODO
	fprintf(ERROUT, "'ProcDynamicLoad' not implemented (yet)\n");
}

// Implementation-specific Geometric Primitives
RtVoid RiGeometry(RtToken type, ...)
{
	BUILD_TOKENPARAM_LIST(type)
	RiSurfaceV(type, n, _tokens, _parms);
}

RtVoid RiGeometryV(RtToken type, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck()) return;
	// TODO
	fprintf(ERROUT, "'Geometry' not implemented (yet)\n");
}

//------------------------------------------------------------------------------
// Solids & Spatial Set operations

RtVoid RiSolidBegin(RtToken operation)
{
	// TODO
	fprintf(ERROUT, "'SolidBegin' not implemented (yet)\n");
}

RtVoid RiSolidEnd()
{
	// TODO
	fprintf(ERROUT, "'SolidEnd' not implemented (yet)\n");
}

// Retained Geometry
RtObjectHandle RiObjectBegin()
{
	// TODO
	fprintf(ERROUT, "'ObjectBegin' not implemented (yet)\n");
	return NULL;
}

RtVoid RiObjectEnd()
{
	// TODO
	fprintf(ERROUT, "'ObjectEnd' not implemented (yet)\n");
}

RtVoid RiObjectInstance(RtObjectHandle handle)
{
	// TODO
	fprintf(ERROUT, "'ObjectInstance' not implemented (yet)\n");
}

//------------------------------------------------------------------------------
// Motion

RtVoid RiMotionBegin(RtInt n2, ...)
{
	va_list args;

	float* times = new float [n2];

	va_start(args, n2);
	for (int i=0; i<n2; ++i)
		times[i] = (float)(va_arg(args, double));
	va_end(args);

	RiMotionBeginV(n2, times);
	delete times;
}

RtVoid RiMotionBeginV(RtInt n, RtFloat times[])
{
	State::Instance()->motionBegin(n, times);
	prev_motion_cmd = MB_NONE;
	motion_pos = 0;
}

RtVoid RiMotionEnd()
{
	State::Instance()->motionEnd();
	prev_motion_cmd = MB_NONE;
	motion_pos = -1;
}

//------------------------------------------------------------------------------
// External Resources

// Texture maps
RtVoid RiMakeTexture(const char* pic, const char* tex, RtToken swrap, RtToken twrap,
                     RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth, ...)
{
	BUILD_TOKENPARAM_LIST(twidth)
	RiMakeTextureV(pic, tex, swrap, twrap, filterfunc, swidth, twidth, n, _tokens, _parms);
}

RtVoid RiMakeTextureV(const char* pic, const char* tex, RtToken swrap, RtToken twrap,
                      RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
                      RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck()) return;
	// TODO
	fprintf(ERROUT, "'MakeTexture' not implemented (yet)\n");
}

// Environment maps
RtVoid RiMakeLatLongEnvironment(const char* pic, const char* tex,
                                RtFilterFunc filterfunc,
                                RtFloat swidth, RtFloat twidth, ...)
{
	BUILD_TOKENPARAM_LIST(twidth)
	RiMakeLatLongEnvironmentV(pic, tex, filterfunc, swidth, twidth, n, _tokens, _parms);
}

RtVoid RiMakeLatLongEnvironmentV(const char* pic, const char* tex,
                                RtFilterFunc filterfunc,
                                RtFloat swidth, RtFloat twidth,
                                RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck()) return;
	// TODO
	fprintf(ERROUT, "'MakeLatLongEnvironment' not implemented (yet)\n");
}

RtVoid RiMakeCubeFaceEnvironment(const char* px, const char* nx, const char* py, const char* ny,
                                 const char* pz, const char* nz, const char* tex, RtFloat fov,
                                 RtFilterFunc filterfunc, RtFloat swidth, RtFloat ywidth, ...)
{
	BUILD_TOKENPARAM_LIST(ywidth)
	RiMakeCubeFaceEnvironmentV(px, nx, py, ny, pz, nz, tex, fov, filterfunc, swidth, ywidth, n, _tokens, _parms);
}

RtVoid RiMakeCubeFaceEnvironmentV(const char* px, const char* nx, const char* py, const char* ny,
                                  const char* pz, const char* nz, const char* tex, RtFloat fov,
                                  RtFilterFunc filterfunc, RtFloat swidth, RtFloat ywidth,
                                  RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck()) return;
	// TODO
	fprintf(ERROUT, "'MakeCubeFaceEnvironment' not implemented (yet)\n");
}

// Shadow maps
RtVoid RiMakeShadow(const char* pic, const char* tex, ...)
{
	BUILD_TOKENPARAM_LIST(tex)
	RiMakeShadowV(pic, tex, n, _tokens, _parms);
}

RtVoid RiMakeShadowV(const char* pic, const char* tex, RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck()) return;
	// TODO
	fprintf(ERROUT, "'MakeShadow' not implemented (yet)\n");
}

// Bump maps (not really supported anymore? 3.2 removed, use displacement instead
RtVoid RiMakeBump(const char* pic, const char* tex, RtToken swrap, RtToken twrap,
                  RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth, ...)
{
	BUILD_TOKENPARAM_LIST(twidth)
	RiMakeBumpV(pic, tex, swrap, twrap, filterfunc, swidth, twidth, n, _tokens, _parms);
}

RtVoid RiMakeBumpV(const char* pic, const char* tex, RtToken swrap, RtToken twrap,
                   RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
                   RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck()) return;
	// TODO
	fprintf(ERROUT, "'MakeBump' not implemented (yet)\n");
}

//------------------------------------------------------------------------------
// all below TODO or ignore...

// Errors
RtVoid RiErrorHandler(RtErrorHandler handler)
{
}

RtVoid RiErrorIgnore(RtInt code, RtInt severity, const char* msg)
{
}

RtVoid RiErrorPrint(RtInt code, RtInt severity, const char* msg)
{
}

RtVoid RiErrorAbort(RtInt code, RtInt severity, const char* msg)
{
}

//------------------------------------------------------------------------------
// Archive Files

RtVoid RiArchiveRecord(RtToken type, const char* format, ...)
{
}

RtVoid RiReadArchive(RtToken name, RtArchiveCallback callback, ...)
{
	BUILD_TOKENPARAM_LIST(callback)
	RiReadArchiveV(name, callback, n, _tokens, _parms);
}

RtVoid RiReadArchiveV(RtToken name, RtArchiveCallback callback,
											RtInt n, RtToken tokens[], RtPointer parms[])
{
	if (!mbCheck(true, MB_READARCHIVE)) return;
	Options& options = State::Instance()->topOptions();
	RIBparse_t rp;
	char fname[512] = {0};
	snprintf(fname, 512, "%s%s", options.basepath, name);
	RtInt err = rp.parse(fname, false);
	if (err!=RIE_NOERROR) {
		switch (err) {
			case RIE_NOFILE:
				fprintf(ERROUT, "No file: '%s'\n", name);
				break;
			case RIE_BADFILE:
				fprintf(ERROUT, "File read error\n");
				break;
			default:
				fprintf(ERROUT, "Unknown error: %d\n", err);
		}
	}
	in_readarchive = false;
}

//------------------------------------------------------------------------------

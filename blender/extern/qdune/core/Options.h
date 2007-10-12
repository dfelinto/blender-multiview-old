#ifndef OPTIONS_H
#define OPTIONS_H

#include "ri.h"

#include "QDRender.h"
__BEGIN_QDRENDER

// unused
/*
struct ClippingPlane
{
	ClippingPlane():x(0), y(0), z(0), nx(0), ny(0), nz(0) {}
	ClippingPlane(float _x, float _y, float _z, float _nx, float _ny, float _nz)
	            : x(_x), y(_y), z(_z), nx(_nx), ny(_ny), nz(_nz) {}
	float x, y, z, nx, ny, nz;
};
*/

class Options
{
public:
	Options();
	Options(const Options &options);
	~Options();
	Options &operator=(const Options &options);

	// Camera
	RtInt xRes, yRes;                     // RiFormat
	RtFloat pixelAspectRatio;
	RtFloat frameAspectRatio;             // RiFrameAspectRatio
	RtFloat left, right, bottom, top;     // RiScreenWindow
	RtFloat xMin, xMax, yMin, yMax;       // RiCropWindow
	enum {PROJ_ORTHOGRAPHIC, PROJ_PERSPECTIVE} projection;  // RiProjection
	RtFloat fov;
	RtFloat nearClip, farClip;            // RiClipping
	// extra clipping planes              // RiClippingPlane
	//std::vector<ClippingPlane> clippingPlanes;
	RtFloat fStop, focalLength,           // RiDepthOfField
					focalDistance;
	RtFloat openShutter, closeShutter;    // RiShutter

	// Display
	RtFloat pixelVariance;                // RiPixelVariance
	RtFloat xSamples, ySamples;           // RiPixelSamples
	RtFilterFunc pixelFilter;             // RiPixelFilter
	RtFloat xWidth, yWidth;
	RtFloat gain, gamma;                  // RiExposure
	RtPointer imager;                     // RiImager (shader)
	RtInt cqOne, cqMin, cqMax;            // RiQuantize (color)
	RtFloat cqDitherAmplitude;
	RtInt zqOne, zqMin, zqMax;            // RiQuantize (depth)
	RtFloat zqDitherAmplitude;
	enum {DT_FRAMEBUFFER, DT_FILE, DT_ZFILE} displayType;   // RiDisplay
	char displayName[80];
	enum {DM_RGB, DM_RGBA, DM_RGBZ, DM_RGBAZ, DM_A, DM_Z, DM_AZ} displayMode;
	// Additional
	enum {HD_HIDDEN, HD_RAYTRACE, HD_HYBRID} hider;         // RiHider
	// Color samples support for RGB only
	// RiColorSamples will return 'unimplemented'
	RtFloat relativeDetail;               // RiRelativeDetail

	// Implementation specifics
	// extra global params, searchpaths etc, later...
	RtFloat jitter;
	RtInt bucketsize;	// size of bucket in pixels
	RtInt texturememory;	// texture cache size in kb
	RtInt gridsize;	// maximum size of grid (num of verts)
	RtInt eyesplits;

	// basepath relative to current file for ReadArchive/etc
	char basepath[256];
};

__END_QDRENDER

#endif // OPTIONS_H

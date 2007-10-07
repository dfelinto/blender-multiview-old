#include <iostream>

#include "Options.h"

__BEGIN_QDRENDER

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

//----------------------------------------------------------
// Options

Options::Options()
{
	// Camera defaults
	// RiFormat
	xRes = 640;
	yRes = 480;
	pixelAspectRatio = 1;
	// RiFrameAspectRatio
	frameAspectRatio = (RtFloat)4/3;
	// RiScreenWindow
	left = -frameAspectRatio;
	right = frameAspectRatio;
	bottom = -1;
	top = 1;
	// RiCropWindow
	xMin = yMin = 0;
	xMax = yMax = 1;
	// RiProjection
	projection = PROJ_ORTHOGRAPHIC;
	fov = 90;
	// RiClipping
	nearClip = RI_EPSILON;
	farClip = RI_INFINITY;
	// RiDepthOfField
	fStop = RI_INFINITY;
	// focal length/distance undefined
	focalLength = 0;
	focalDistance = -1; // set to -1, otherwise camera won't work properly
	// RiShutter
	openShutter = closeShutter = 0;

	// Display defaults
	// RiPixelVariance
	pixelVariance = 0;    // undefined
	// RiPixelSamples
	xSamples = ySamples = 2;
	// RiPixelFilter
	pixelFilter = RiGaussianFilter;
	xWidth = yWidth = 2;
	// RiExposure
	gain = gamma = 1;
	// RiImager, undefined, and for now just an actual pointer, not used
	imager = NULL;
	// RiQuantize (color)
	cqOne = cqMax = 255;
	cqMin = 0;
	cqDitherAmplitude = 0.5;
	// RiQuantize (depth), only 'one' is defined
	zqOne = zqMax = zqMin = 0;
	zqDitherAmplitude = 0.f;
	// RiDisplay, not defined, implementation specific
	// assume framebuffer
	displayType = DT_FRAMEBUFFER;
	strncpy(displayName, "default_rgb_framebuffer", 80);
	displayMode = DM_RGB;
	// Additional
	// RiHider
	hider = HD_HIDDEN;
	// colorsamples fixed at 3 for now
	// RiRelativeDetail
	relativeDetail = 1;

	// optional
	jitter = 1;
	bucketsize = 32;
	texturememory = 10*1024*1024;	// 10Mb default should be enough
	gridsize = 1024;

	// basepath relative to current file for ReadArchive/etc
	basepath[0] = 0;
}

Options::~Options()
{
	//nothing to do
}


// copy constructor
Options::Options(const Options &options)
{
	memcpy(this, &options, sizeof(Options));
}

// assigment operator
Options& Options::operator=(const Options &options)
{
	memcpy(this, &options, sizeof(Options));
	return *this;
}

//------------------------------------------------------------------------------

__END_QDRENDER

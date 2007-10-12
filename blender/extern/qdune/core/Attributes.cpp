#include "Attributes.h"

#include <cstring>

__BEGIN_QDRENDER

//----------------------------------------------------------
// Attributes

// Attributes constructor
Attributes::Attributes()
{
	// default flags:
	// RiOrientation=OUTSIDE, RiSides=2, RiShadingInterpolation=CONSTANT, RiMatte=false, dice_binary=false
	flags = 0;

	// Shading
	// RiColor & RiOpacity
	color[0] = color[1] = color[2] = 1;
	opacity[0] = opacity[1] = opacity[2] = 1;

	// RiTextureCoordinates
	// defaults: [(0, 0), (1, 0), (0, 1), (1, 1)]
	texCoords[0] = texCoords[1] = texCoords[3] = texCoords[4] = 0.f;
	texCoords[2] = texCoords[5] = texCoords[6] = texCoords[7] = 1.f;

	// RiShadingRate
	effectiveShadingRate = 1;

	// array of all current active lightshaders
	lightsources = NULL;
	// shaders
	surface_shader = NULL;
	displacement_shader = NULL;
	atmosphere_shader = NULL;
	interior_shader = NULL;
	exterior_shader = NULL;

	// RiBasis (basis handled in state)
	cubicBasisMatrix[0] = cubicBasisMatrix[1] = NULL;
	cubicBasisStep[0] = cubicBasisStep[1] = RI_BEZIERSTEP;

	// not yet used
	/*
	// RiBound
	bound[0] = bound[2] = bound[4] = -RI_INFINITY;
	bound[1] = bound[3] = bound[5] = RI_INFINITY;
	// RiDetail
	detail[0] = detail[2] = detail[4] = -RI_INFINITY;
	detail[1] = detail[3] = detail[5] = RI_INFINITY;
	// RiDetailRange
	minVisible = lowerTransition = 0;
	upperTransition = maxVisible = RI_INFINITY;
	// RiGeometricApproximation
	valueApproximation = 0; // "flatness" only
	*/

	// optionals
	displacementbound_sphere = 0;
	// opacity threshold is minimum sum of opacity components to consider it opaque
	// (now uses 1 as default. Can be annoying when for instance volume rendering with surface shaders,
	//  since to make that work properly, opacity would have to be set to less than 0.98,
	//  or this value itself must be reset)
	opacity_threshold = 3.f;
	// the number of lights in list
	numlights = 0;
	// NOTE: so, at some point I got an enormous amount of 'uninitialised value' error reports from valgrind, ordering me to 'fix my program!'...
	// seemingly without any apparent reason whatsoever...
	// After spending the whole day grasping at straws, it finally occured to me, padding!!!
	// Valgrind was seeing some padding bytes as an uninitialized value or something. That was not quite the end of it though...
	// In any case, this is why the padding variable itself also needs to be initialized as well... Something to remember...
	pad0 = 0;
}

// copy constructor
Attributes::Attributes(const Attributes &attributes)
{
	// this is ok, all pointers are borrowed references
	memcpy(this, &attributes, sizeof(Attributes));
}

// assignment operator
Attributes& Attributes::operator=(const Attributes &attributes)
{
	// this is ok, all pointers are borrowed references
	memcpy(this, &attributes, sizeof(Attributes));
	return *this;
}

// Attributes destructor
Attributes::~Attributes()
{
	// nothing to delete, all pointers are borrowed references
}

__END_QDRENDER

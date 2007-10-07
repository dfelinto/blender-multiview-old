#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include "ri.h"

#include "QDRender.h"
__BEGIN_QDRENDER

class SlShaderInstance;
class Transform;

// attribute flag bits
enum atFlagBits {AF_SMOOTH=1, AF_MATTE=2, AF_SIDES1=4, AF_ORIENTATION=8};
typedef int atFlags;

class Attributes
{
public:
	Attributes();
	Attributes(const Attributes &attributes);
	~Attributes();
	Attributes &operator=(const Attributes &attributes);

	// attribute flags for RiShadingInterpolation, RiMatte,
	// RiOrientation/RiReverseOrientation and RiSides
	atFlags flags;

	// Shading
	RtColor color, opacity;               // RiColor & RiOpacity
	RtFloat texCoords[8];                 // RiTextureCoordinates

	// shading general
	RtFloat effectiveShadingRate;         // RiShadingRate

	// array of all current active lightsources, borrowed reference, do not delete!
	SlShaderInstance** lightsources;
	unsigned int numlights, pad0;

	// Shaders, borrowed references, do not delete!
	SlShaderInstance* surface_shader;
	SlShaderInstance* displacement_shader;
	SlShaderInstance* atmosphere_shader;
	SlShaderInstance* interior_shader;
	SlShaderInstance* exterior_shader;

	// Geometry
	/*
	RtBound bound;                        // RiBound
	RtBound detail;                       // RiDetail
	RtFloat minVisible, lowerTransition,  // RiDetailRange
					upperTransition, maxVisible;
	RtFloat valueApproximation;           // RiGeometricApproximation (flatnesss)
	*/
	Transform* cubicBasisMatrix[2];       // RiBasis, borrowed references, do not delete!
	RtInt cubicBasisStep[2];
	//trimCurveList; TODO, or not...

	// optional attributes
	RtFloat displacementbound_sphere;
	RtFloat opacity_threshold;
	RtBoolean dice_binary;
};

__END_QDRENDER

#endif // ATTRIBUTES

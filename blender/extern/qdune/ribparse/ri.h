/*
 * RenderMan Interface Standard Include File
 * (for ANSI Standard C)
 * (initially copypasted from RISpec3.2 pdf file, then modified, re-arranged,
 *  added missing defs, extra's, fixed errors and added some as well I'm sure... )
**/

#ifndef RI_H
#define RI_H

#ifdef __cplusplus
extern "C" {
#endif

	/* Definitions of Abstract Types used in RI */
typedef short RtBoolean;
typedef int RtInt;
typedef float RtFloat;

typedef const char* RtToken;

typedef RtFloat RtColor[3];
typedef RtFloat RtPoint[3];
typedef RtFloat RtVector[3];
typedef RtFloat RtNormal[3];
typedef RtFloat RtHpoint[4];
typedef RtFloat RtMatrix[4][4];
typedef RtFloat RtBasis[4][4];
typedef RtFloat RtBound[6];
typedef char* RtString;

typedef void* RtPointer;
#define RtVoid void

/* for separable filters */
typedef RtFloat (*RtSepFilterFunc)(RtFloat, RtFloat);

typedef RtFloat (*RtFilterFunc)(RtFloat, RtFloat, RtFloat, RtFloat);
typedef RtVoid (*RtErrorHandler)(RtInt, RtInt, const char*);

typedef RtVoid (*RtProcSubdivFunc)(RtPointer, RtFloat);
typedef RtVoid (*RtProcFreeFunc)(RtPointer);
typedef RtVoid (*RtArchiveCallback)(RtToken, const char*, ...);

typedef RtPointer RtObjectHandle;
typedef RtPointer RtLightHandle;

typedef RtPointer RtContextHandle;

	/* Extern Declarations for Predefined RI Data Structures */
#define RI_FALSE 0
#define RI_TRUE (! RI_FALSE)
#define RI_INFINITY 1.0e38
#define RI_EPSILON 1.0e-10
#define RI_NULL ((RtToken)0)

extern RtToken RI_FRAMEBUFFER, RI_FILE;
extern RtToken RI_RGB, RI_RGBA, RI_RGBZ, RI_RGBAZ, RI_A, RI_Z, RI_AZ;
extern RtToken RI_PERSPECTIVE, RI_ORTHOGRAPHIC;
extern RtToken RI_HIDDEN, RI_PAINT;
extern RtToken RI_CONSTANT, RI_SMOOTH;
extern RtToken RI_FLATNESS, RI_FOV;
extern RtToken RI_AMBIENTLIGHT, RI_POINTLIGHT, RI_DISTANTLIGHT,
               RI_SPOTLIGHT;
extern RtToken RI_INTENSITY, RI_LIGHTCOLOR, RI_FROM, RI_TO, RI_CONEANGLE,
               RI_CONEDELTAANGLE, RI_BEAMDISTRIBUTION;
extern RtToken RI_MATTE, RI_METAL, RI_SHINYMETAL,
               RI_PLASTIC, RI_PAINTEDPLASTIC;
extern RtToken RI_KA, RI_KD, RI_KS, RI_ROUGHNESS, RI_KR,
               RI_TEXTURENAME, RI_SPECULARCOLOR;
extern RtToken RI_DEPTHCUE, RI_FOG, RI_BUMPY;
extern RtToken RI_MINDISTANCE, RI_MAXDISTANCE, RI_BACKGROUND,
               RI_DISTANCE, RI_AMPLITUDE;
extern RtToken RI_RASTER, RI_SCREEN, RI_CAMERA, RI_WORLD, RI_OBJECT;
extern RtToken RI_INSIDE, RI_OUTSIDE, RI_LH, RI_RH;
extern RtToken RI_P, RI_PZ, RI_PW, RI_N, RI_NP,
               RI_CS, RI_OS, RI_S, RI_T, RI_ST;
extern RtToken RI_BILINEAR, RI_BICUBIC;
extern RtToken RI_LINEAR, RI_CUBIC;
extern RtToken RI_PRIMITIVE, RI_INTERSECTION, RI_UNION, RI_DIFFERENCE;
extern RtToken RI_PERIODIC, RI_NONPERIODIC, RI_CLAMP, RI_BLACK;
extern RtToken RI_IGNORE, RI_PRINT, RI_ABORT, RI_HANDLER;
extern RtToken RI_COMMENT, RI_STRUCTURE, RI_VERBATIM;
extern RtToken RI_IDENTIFIER, RI_NAME, RI_SHADINGGROUP;
extern RtToken RI_WIDTH, RI_CONSTANTWIDTH;
// extra's
extern RtToken RI_JITTER, RI_ZFILE, RI_SPHERE, RI_RAYTRACE, RI_HYBRID,
               RI_BUCKETSIZE, RI_TEXTUREMEMORY, RI_GRIDSIZE, RI_BINARY, RI_EYESPLITS;

extern RtBasis RiBezierBasis, RiBSplineBasis, RiCatmullRomBasis,
               RiHermiteBasis, RiPowerBasis;

#define RI_BEZIERSTEP ((RtInt)3)
#define RI_BSPLINESTEP ((RtInt)1)
#define RI_CATMULLROMSTEP ((RtInt)1)
#define RI_HERMITESTEP ((RtInt)2)
#define RI_POWERSTEP ((RtInt)4)

/*----------------------------------------------------------------------------*/
	/* Declarations of All the RenderMan Interface Subroutines */
/*----------------------------------------------------------------------------*/

extern RtToken RiDeclare(const char* name, const char* declaration);

/* Pixel Filters */
extern RtFloat RiGaussianFilter(RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth);
extern RtFloat RiBoxFilter(RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth);
extern RtFloat RiTriangleFilter(RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth);
extern RtFloat RiCatmullRomFilter(RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth);
extern RtFloat RiSincFilter(RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth);
/* separable versions of all of the above */
extern RtFloat RiSepGaussianFilter(RtFloat v, RtFloat width);
extern RtFloat RiSepBoxFilter(RtFloat v, RtFloat width);
extern RtFloat RiSepTriangleFilter(RtFloat v, RtFloat width);
extern RtFloat RiSepCatmullRomFilter(RtFloat v, RtFloat width);
extern RtFloat RiSepSincFilter(RtFloat v, RtFloat width);

/* Graphics State */
extern RtVoid RiBegin(RtToken name);
extern RtVoid RiEnd(void);
extern RtContextHandle RiGetContext(void);
extern RtVoid RiContext(RtContextHandle);
extern RtVoid RiFrameBegin(RtInt frame);
extern RtVoid RiFrameEnd(void);
extern RtVoid RiWorldBegin(void);
extern RtVoid RiWorldEnd(void);

/* Camera Options */
extern RtVoid RiFormat(RtInt xres, RtInt yres, RtFloat aspect);
extern RtVoid RiFrameAspectRatio(RtFloat aspect);
extern RtVoid RiScreenWindow(RtFloat left, RtFloat right, RtFloat bot, RtFloat top);
extern RtVoid RiCropWindow(RtFloat xmin, RtFloat xmax, RtFloat ymin, RtFloat ymax);
extern RtVoid RiProjection(RtToken name, ...);
extern RtVoid RiProjectionV(RtToken name, RtInt n,RtToken tokens[],RtPointer parms[]);
extern RtVoid RiClipping(RtFloat hither, RtFloat yon);
extern RtVoid RiClippingPlane(RtFloat x, RtFloat y, RtFloat z, RtFloat nx, RtFloat ny, RtFloat nz);
extern RtVoid RiDepthOfField(RtFloat fstop, RtFloat focallength, RtFloat focaldistance);
extern RtVoid RiShutter(RtFloat min, RtFloat max);

/* Display Options */
extern RtVoid RiPixelVariance(RtFloat variation);
extern RtVoid RiPixelSamples(RtFloat xsamples, RtFloat ysamples);
extern RtVoid RiPixelFilter(RtFilterFunc filterfunc, RtFloat xwidth, RtFloat ywidth);
extern RtVoid RiExposure(RtFloat gain, RtFloat gamma);
extern RtVoid RiImager(RtToken name, ...);
extern RtVoid RiImagerV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiQuantize(RtToken type, RtInt one, RtInt min, RtInt max, RtFloat ampl);
extern RtVoid RiDisplay(const char* name, RtToken type, RtToken mode, ...);
extern RtVoid RiDisplayV(const char* name, RtToken type, RtToken mode,
                         RtInt n, RtToken tokens[], RtPointer parms[]);

/* Additional Options */
extern RtVoid RiHider(RtToken type, ...);
extern RtVoid RiHiderV(RtToken type, RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiColorSamples(RtInt n, RtFloat nRGB[], RtFloat RGBn[]);
extern RtVoid RiRelativeDetail(RtFloat relativedetail);
/* Implementation-specific Options */
extern RtVoid RiOption(RtToken name, ...);
extern RtVoid RiOptionV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);

/*----------------------------------------------------------------------------*/
/* Attributes */
extern RtVoid RiAttributeBegin(void);
extern RtVoid RiAttributeEnd(void);
/* Color & Opacity */
extern RtVoid RiColor(RtColor color);
extern RtVoid RiOpacity(RtColor color);
/* Texture Coordinates */
extern RtVoid RiTextureCoordinates(RtFloat s1, RtFloat t1, RtFloat s2, RtFloat t2,
		RtFloat s3, RtFloat t3, RtFloat s4, RtFloat t4);

/* Light Sources */
extern RtLightHandle RiLightSource(RtToken name, ...);
extern RtLightHandle RiLightSourceV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtLightHandle RiAreaLightSource(RtToken name, ...);
extern RtLightHandle RiAreaLightSourceV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);

extern RtVoid RiIlluminate(RtLightHandle light, RtBoolean onoff);

/* Surface Shading */
extern RtVoid RiSurface(RtToken name, ...);
extern RtVoid RiSurfaceV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);
/* Displacement Shading */
extern RtVoid RiDisplacement(RtToken name, ...);
extern RtVoid RiDisplacementV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);
/* Volume Shading */
extern RtVoid RiAtmosphere(RtToken name, ...);
extern RtVoid RiAtmosphereV(RtToken name,RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiInterior(RtToken name, ...);
extern RtVoid RiInteriorV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiExterior(RtToken name, ...);
extern RtVoid RiExteriorV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);
/* Shading Rate */
extern RtVoid RiShadingRate(RtFloat size);
/* Shading Interpolation */
extern RtVoid RiShadingInterpolation(RtToken type);
/* Matte Objects */
extern RtVoid RiMatte(RtBoolean onoff);

/* Bound */
extern RtVoid RiBound(RtBound bound);
/* Detail */
extern RtVoid RiDetail(RtBound bound);
extern RtVoid RiDetailRange(RtFloat minvis, RtFloat lowtran, RtFloat uptran, RtFloat maxvis);
/* Geometric Approximation */
extern RtVoid RiGeometricApproximation(RtToken type, RtFloat value);
/* Orientation and Sides */
extern RtVoid RiOrientation(RtToken orientation);
extern RtVoid RiReverseOrientation(void);
extern RtVoid RiSides(RtInt sides);

/* Transformations */
extern RtVoid RiIdentity(void);
extern RtVoid RiTransform(RtMatrix transform);
extern RtVoid RiConcatTransform(RtMatrix transform);
extern RtVoid RiPerspective(RtFloat fov);
extern RtVoid RiTranslate(RtFloat dx, RtFloat dy, RtFloat dz);
extern RtVoid RiRotate(RtFloat angle, RtFloat dx, RtFloat dy, RtFloat dz);
extern RtVoid RiScale(RtFloat sx, RtFloat sy, RtFloat sz);
extern RtVoid RiSkew(RtFloat angle, RtFloat dx1, RtFloat dy1, RtFloat dz1,
                     RtFloat dx2, RtFloat dy2, RtFloat dz2);
extern RtVoid RiDeformation(RtToken name, ...);
extern RtVoid RiDeformationV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);
/* Named Coordinate Systems */
extern RtVoid RiCoordinateSystem(RtToken space);
extern RtVoid RiCoordSysTransform(RtToken space);

extern RtPoint* RiTransformPoints(RtToken fromspace, RtToken tospace, RtInt n, RtPoint points[]);

/* Transformation Stack */
extern RtVoid RiTransformBegin(void);
extern RtVoid RiTransformEnd(void);

/* Implementation-specific Attributes */
extern RtVoid RiAttribute(RtToken name, ...);
extern RtVoid RiAttributeV(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);

/*----------------------------------------------------------------------------*/
/* Geometric Primitives */

/* Polygons */
extern RtVoid RiPolygon(RtInt nverts, ...);
extern RtVoid RiPolygonV(RtInt nverts, RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiGeneralPolygon(RtInt nloops, RtInt nverts[], ...);
extern RtVoid RiGeneralPolygonV(RtInt nloops, RtInt nverts[],
		RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiPointsPolygons(RtInt npolys, RtInt nverts[], RtInt verts[], ...);
extern RtVoid RiPointsPolygonsV(RtInt npolys, RtInt nverts[], RtInt verts[],
		RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiPointsGeneralPolygons(RtInt npolys, RtInt nloops[], RtInt nverts[],
		RtInt verts[], ...);
extern RtVoid RiPointsGeneralPolygonsV(RtInt npolys, RtInt nloops[], RtInt nverts[],
		RtInt verts[], RtInt n, RtToken tokens[], RtPointer parms[]);
/* Patches */
extern RtVoid RiBasis(RtBasis ubasis, RtInt ustep, RtBasis vbasis, RtInt vstep);
extern RtVoid RiPatch(RtToken type, ...);
extern RtVoid RiPatchV(RtToken type, RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiPatchMesh(RtToken type, RtInt nu, RtToken uwrap,
                          RtInt nv, RtToken vwrap, ...);
extern RtVoid RiPatchMeshV(RtToken type, RtInt nu, RtToken uwrap,
                           RtInt nv, RtToken vwrap,
                           RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiNuPatch(RtInt nu, RtInt uorder, RtFloat uknot[], RtFloat umin,
                        RtFloat umax, RtInt nv, RtInt vorder, RtFloat vknot[],
                        RtFloat vmin, RtFloat vmax, ...);
extern RtVoid RiNuPatchV(RtInt nu, RtInt uorder, RtFloat uknot[], RtFloat umin,
                         RtFloat umax, RtInt nv, RtInt vorder, RtFloat vknot[],
                         RtFloat vmin, RtFloat vmax,
                         RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiTrimCurve(RtInt nloops, RtInt ncurves[], RtInt order[],
                          RtFloat knot[], RtFloat min[], RtFloat max[], RtInt n[],
                          RtFloat u[], RtFloat v[], RtFloat w[]);

/* Subdivision Meshes */
extern RtVoid RiSubdivisionMesh(RtToken mask, RtInt nf, RtInt nverts[],
                                RtInt verts[],
                                RtInt ntags, RtToken tags[], RtInt numargs[],
                                RtInt intargs[], RtFloat floatargs[], ...);
extern RtVoid RiSubdivisionMeshV(RtToken mask, RtInt nf, RtInt nverts[],
                                 RtInt verts[], RtInt ntags, RtToken tags[],
                                 RtInt nargs[], RtInt intargs[],
                                 RtFloat floatargs[], RtInt n,
                                 RtToken tokens[], RtPointer *parms);

/* Quadrics */
extern RtVoid RiSphere(RtFloat radius, RtFloat zmin, RtFloat zmax, RtFloat tmax, ...);
extern RtVoid RiSphereV(RtFloat radius, RtFloat zmin, RtFloat zmax, RtFloat tmax,
                        RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiCone(RtFloat height, RtFloat radius, RtFloat tmax, ...);
extern RtVoid RiConeV(RtFloat height, RtFloat radius, RtFloat tmax,
                      RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiCylinder(RtFloat radius,RtFloat zmin,RtFloat zmax,RtFloat tmax, ...);
extern RtVoid RiCylinderV(RtFloat radius, RtFloat zmin, RtFloat zmax, RtFloat tmax,
                          RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiHyperboloid(RtPoint point1, RtPoint point2, RtFloat tmax, ...);
extern RtVoid RiHyperboloidV(RtPoint point1, RtPoint point2, RtFloat tmax,
                             RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiParaboloid(RtFloat rmax,RtFloat zmin,RtFloat zmax,RtFloat tmax, ...);
extern RtVoid RiParaboloidV(RtFloat rmax, RtFloat zmin, RtFloat zmax, RtFloat tmax,
                            RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiDisk(RtFloat height, RtFloat radius, RtFloat tmax, ...);
extern RtVoid RiDiskV(RtFloat height, RtFloat radius, RtFloat tmax,
                      RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiTorus(RtFloat majrad, RtFloat minrad, RtFloat phimin,
                      RtFloat phimax, RtFloat tmax, ...);
extern RtVoid RiTorusV(RtFloat majrad,RtFloat minrad,
                       RtFloat phimin, RtFloat phimax, RtFloat tmax,
                       RtInt n, RtToken tokens[], RtPointer parms[]);

/* Point & Curve Primitives */
extern RtVoid RiPoints(RtInt nverts,...);
extern RtVoid RiPointsV(RtInt nverts, RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiCurves(RtToken type, RtInt ncurves,
                       RtInt nvertices[], RtToken wrap, ...);
extern RtVoid RiCurvesV(RtToken type, RtInt ncurves, RtInt nvertices[], RtToken wrap,
                        RtInt n, RtToken tokens[], RtPointer parms[]);

/* 'Blobby' Implicit Surfaces */
extern RtVoid RiBlobby(RtInt nleaf, RtInt ncode, RtInt code[],
                       RtInt nflt, RtFloat flt[],
                       RtInt nstr, RtToken str[], ...);
extern RtVoid RiBlobbyV(RtInt nleaf, RtInt ncode, RtInt code[],
                        RtInt nflt, RtFloat flt[],
                        RtInt nstr, RtToken str[],
                        RtInt n , RtToken tokens[], RtPointer parms[]);

/* Procedural Primitives */
extern RtVoid RiProcedural(RtPointer data, RtBound bound,
                           RtVoid (*subdivfunc)(RtPointer, RtFloat),
                           RtVoid (*freefunc)(RtPointer));

extern RtVoid RiProcDelayedReadArchive(RtPointer data, RtFloat detail);
extern RtVoid RiProcRunProgram(RtPointer data, RtFloat detail);
extern RtVoid RiProcDynamicLoad(RtPointer data, RtFloat detail);

/* Implementation-specific Geometric Primitives */
extern RtVoid RiGeometry(RtToken type, ...);
extern RtVoid RiGeometryV(RtToken type, RtInt n, RtToken tokens[], RtPointer parms[]);

/*----------------------------------------------------------------------------*/
/* Solids & Spatial Set operations */
extern RtVoid RiSolidBegin(RtToken operation);
extern RtVoid RiSolidEnd(void) ;

/* Retained Geometry */
extern RtObjectHandle RiObjectBegin(void);
extern RtVoid RiObjectEnd(void);
extern RtVoid RiObjectInstance(RtObjectHandle handle);

/*----------------------------------------------------------------------------*/
/* Motion */
extern RtVoid RiMotionBegin(RtInt n, ...);
extern RtVoid RiMotionBeginV(RtInt n, RtFloat times[]);
extern RtVoid RiMotionEnd(void);

/*----------------------------------------------------------------------------*/
/* External Resources */

/* Texture maps */
extern RtVoid RiMakeTexture(const char* pic, const char* tex, RtToken swrap, RtToken twrap,
                            RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth, ...);
extern RtVoid RiMakeTextureV(const char* pic, const char* tex, RtToken swrap, RtToken twrap,
                             RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
                             RtInt n, RtToken tokens[], RtPointer parms[]);
/* Environment maps */
extern RtVoid RiMakeLatLongEnvironment(const char* pic, const char* tex,
                                       RtFilterFunc filterfunc,
                                       RtFloat swidth, RtFloat twidth, ...);
extern RtVoid RiMakeLatLongEnvironmentV(const char* pic, const char* tex,
                                        RtFilterFunc filterfunc,
                                        RtFloat swidth, RtFloat twidth,
                                        RtInt n, RtToken tokens[], RtPointer parms[]);
extern RtVoid RiMakeCubeFaceEnvironment(const char* px, const char* nx, const char* py, const char* ny,
                                        const char* pz, const char* nz, const char* tex, RtFloat fov,
                                        RtFilterFunc filterfunc, RtFloat swidth, RtFloat ywidth, ...);
extern RtVoid RiMakeCubeFaceEnvironmentV(const char* px, const char* nx, const char* py, const char* ny,
                                         const char* pz, const char* nz, const char* tex, RtFloat fov,
                                         RtFilterFunc filterfunc, RtFloat swidth, RtFloat ywidth,
                                         RtInt n, RtToken tokens[], RtPointer parms[]);
/* Shadow maps */
extern RtVoid RiMakeShadow(const char* pic, const char* tex, ...);
extern RtVoid RiMakeShadowV(const char* pic, const char* tex, RtInt n, RtToken tokens[], RtPointer parms[]);
/* Bump maps (not really supported anymore? 3.2 removed, use displacement instead */
extern RtVoid RiMakeBump(const char* pic, const char* tex, RtToken swrap, RtToken twrap,
                         RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth, ...);
extern RtVoid RiMakeBumpV(const char* pic, const char* tex, RtToken swrap, RtToken twrap,
                          RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth,
                          RtInt n, RtToken tokens[], RtPointer parms[]);

/*----------------------------------------------------------------------------*/
/* Errors */
extern RtVoid RiErrorHandler(RtErrorHandler handler);

extern RtVoid RiErrorIgnore(RtInt code, RtInt severity, const char* msg);
extern RtVoid RiErrorPrint(RtInt code, RtInt severity, const char* msg);
extern RtVoid RiErrorAbort(RtInt code, RtInt severity, const char* msg);
extern RtInt RiLastError;

/*----------------------------------------------------------------------------*/
/* Archive Files */
extern RtVoid RiArchiveRecord(RtToken type, const char* format, ...);
extern RtVoid RiReadArchive(RtToken name, RtArchiveCallback callback, ...);
extern RtVoid RiReadArchiveV(RtToken name, RtArchiveCallback callback,
                             RtInt n, RtToken tokens[], RtPointer parms[]);

/*
	Error Codes
	 1 - 10       System and File Errors
	11 - 20       Program Limitations
	21 - 40       State Errors
	41 - 60       Parameter and Protocol Errors
	61 - 80       Execution Errors
*/
#define RIE_NOERROR	((RtInt)0)
#define RIE_NOMEM	((RtInt)1)     /* Out of memory */
#define RIE_SYSTEM	((RtInt)2)     /* Miscellaneous system error */
#define RIE_NOFILE	((RtInt)3)     /* File nonexistent */
#define RIE_BADFILE	((RtInt)4)     /* Bad file format */
#define RIE_VERSION	((RtInt)5)     /* File version mismatch */
#define RIE_DISKFULL	((RtInt)6)     /* Target disk is full */
#define RIE_INCAPABLE	((RtInt)11)    /* Optional RI feature */
#define RIE_UNIMPLEMENT	((RtInt)12)    /* Unimplemented feature */
#define RIE_LIMIT	((RtInt)13)    /* Arbitrary program limit */
#define RIE_BUG		((RtInt)14)    /* Probably a bug in renderer */
#define RIE_NOTSTARTED	((RtInt)23)    /* RiBegin not called */
#define RIE_NESTING	((RtInt)24)    /* Bad begin-end nesting */
#define RIE_NOTOPTIONS	((RtInt)25)    /* Invalid state for options */
#define RIE_NOTATTRIBS	((RtInt)26)    /* Invalid state for attribs */
#define RIE_NOTPRIMS	((RtInt)27)    /* Invalid state for primitives */
#define RIE_ILLSTATE	((RtInt)28)    /* Other invalid state */
#define RIE_BADMOTION	((RtInt)29)    /* Badly formed motion block */
#define RIE_BADSOLID	((RtInt)30)    /* Badly formed solid block */
#define RIE_BADTOKEN	((RtInt)41)    /* Invalid token for request */
#define RIE_RANGE	((RtInt)42)    /* Parameter out of range */
#define RIE_CONSISTENCY	((RtInt)43)    /* Parameters inconsistent */
#define RIE_BADHANDLE	((RtInt)44)    /* Bad object/light handle */
#define RIE_NOSHADER	((RtInt)45)    /* Cant load requested shader */
#define RIE_MISSINGDATA	((RtInt)46)    /* Required parameters not provided */
#define RIE_SYNTAX	((RtInt)47)    /* Declare type syntax error */
#define RIE_MATH	((RtInt)61)    /* Zerodivide, noninvert matrix, etc. */

	/* Error severity levels */
#define RIE_INFO	((RtInt)0)     /* Rendering stats and other info */
#define RIE_WARNING	((RtInt)1)     /* Something seems wrong, maybe okay */
#define RIE_ERROR	((RtInt)2)     /* Problem. Results may be wrong */
#define RIE_SEVERE	((RtInt)3)     /* So bad you should probably abort */

#ifdef __cplusplus
}
#endif

#endif /* RI_H */


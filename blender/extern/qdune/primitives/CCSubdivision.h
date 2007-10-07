//--------------------------------------------------------------------------------------------------
// Catmull-Clark Subdivision Meshes
//--------------------------------------------------------------------------------------------------

#ifndef CCSUBDIVISION_H
#define CCSUBDIVISION_H

#include "qdVector.h"
#include "Bound.h"
#include "ri.h"
#include "qdtl.h"
#include "Primitive.h"

#include "QDRender.h"
__BEGIN_QDRENDER

// SD_CREASE_EQ_BOUND: special flag indicating all creases are also boundaries
// SD_NOTSET: only used in patch data construction
enum sdFlagBits {SD_UNDEFINED = 0, SD_BOUNDARY = 1, SD_MAINQUAD = 2, SD_NONMANIFOLD = 4, SD_CREASE = 8,
                 SD_CREASE_EQ_BOUND = 16, SD_SUPPORTVERTEX = 32,
                 SD_ALLQUAD = 64, SD_ONE_EOV = 128,	// these only used for the skip-global-subdivision alternate method
                 SD_NOTSET = 256};
typedef int sdFlags;

struct ccEdge;
struct ccFace;
struct ccVert
{
	ccVert() : flags(SD_UNDEFINED) {}
	ccVert(const ccVert& cv) : co(cv.co), edges(cv.edges), faces(cv.faces), flags(cv.flags) {}
	ccVert& operator=(const ccVert& cv)
	{
		co = cv.co;
		edges = cv.edges;
		faces = cv.faces;
		flags = cv.flags;
		return *this;
	}
	Point3 co;
	Array<ccEdge*> edges; // edge references
	// face references
	// (not strictly necessary, can get from edges, uses slightly less memory too, but makes things simpler)
	Array<ccFace*> faces;
	sdFlags flags;
};

struct ccEdge
{
	ccEdge() : v1(NULL), v2(NULL), new_vert(NULL), flags(SD_UNDEFINED) {}
	ccEdge(const ccEdge& ce) : v1(ce.v1), v2(ce.v2), new_vert(ce.new_vert), faces(ce.faces), flags(ce.flags) {}
	ccEdge& operator=(const ccEdge& ce)
	{
		v1 = ce.v1;
		v2 = ce.v2;
		new_vert = ce.new_vert;
		faces = ce.faces;
		flags = ce.flags;
		return *this;
	}
	ccVert *v1, *v2, *new_vert; // edge vertex references
	Array<ccFace*> faces;
	sdFlags flags;
};

struct ccFace
{
	ccFace() : center(NULL), flags(SD_UNDEFINED) {}
	ccFace(const ccFace& cf) : verts(cf.verts), edges(cf.edges), stco(cf.stco), center(cf.center), flags(cf.flags) {}
	ccFace& operator=(const ccFace& cf)
	{
		verts = cf.verts;
		edges = cf.edges;
		stco = cf.stco;
		center = cf.center;
		flags = cf.flags;
		return *this;
	}
	Array<int> verts, edges;	// here these are index arrays
	Array<Point2> stco;	// s/t texcoords
	ccVert* center;	// new center vertex
	sdFlags flags;
};

//------------------------------------------------------------------------------
// explicit subdivision patch

class JS_SDPatch;
class BicubicPatch;
class SDPatch : public Primitive
{
	friend void makePatches(const FixedArray<ccVert>&,
	                        const FixedArray<ccEdge>&, const FixedArray<ccFace>&,
	                        const Primitive*, const Framework*, splitbprims_t*, bool);
private:
	SDPatch(const SDPatch &s);
	SDPatch& operator=(const SDPatch& s);
public:
	SDPatch();
	virtual ~SDPatch();
	// mtds
	virtual void post_init() {}
	virtual bool boundable() { return true; }
	virtual Bound bound() { return bnd; }
	virtual bool splitable() { return true; }
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL);
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit);
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false);
	virtual bool in_camspace() const { return true; }
protected:
	// mtds
	JS_SDPatch* makeJSPatch(const FixedArray<ccVert>& vert_list,
	                        const FixedArray<ccEdge>& edge_list, const FixedArray<ccFace>& face_list,
	                        const Primitive* parentprim);
	BicubicPatch* makeBSplinePatch(const FixedArray<ccVert>& vert_list,
	                               const FixedArray<ccEdge>& edge_list, const FixedArray<ccFace>& face_list,
	                               int border[2], const Primitive* parentprim);
	void makeExplicitSDPatch(const FixedArray<ccVert>& vert_list,
	                         const FixedArray<ccEdge>& edge_list, const FixedArray<ccFace>& face_list,
	                         const Primitive* parentprim);
	// data
	sdFlags flags;
	int mainface, eovert;
	Bound bnd;
	Array<ccFace*> ring;
	FixedArray<ccVert> patch_verts;
	FixedArray<ccEdge> patch_edges;
	FixedArray<ccFace> patch_faces;

	// gridindex cache, see split()
	static tLinkedList_t<int, int*> *gridx_cache;
	static int gridx_refc;
};

//----------------------------------------------------------------------------------------------
// Subdivision Patch which can be evaluated directly using the method from the paper
// "Exact Evaluation Of Catmull-Clark Subdivision Surfaces At Arbitrary Parameter Values"
// by Jos Stam

class JS_SDPatch : public Primitive
{
	friend class SDPatch;
	friend class CCSubdivisionMesh;	// only for prerender flag access
private:
	JS_SDPatch& operator=(const JS_SDPatch&);
public:
	JS_SDPatch(int N);
	JS_SDPatch(const JS_SDPatch& jsp);
	virtual ~JS_SDPatch();
	// mtds
	virtual void post_init();
	virtual bool boundable() { return true; }
	virtual Bound bound() { return bd; }
	virtual bool splitable() { return true; }
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL);
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit);
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false);
	virtual bool in_camspace() const { return true; }

	virtual void get_uvlim(float uvl[4]) const { uvl[0] = umin, uvl[1] = umax, uvl[2] = vmin, uvl[3] = vmax; }

protected:
	// mtds
	void eval(float uf, float vf, Point3& P, Vector* dPdu=NULL, Vector* dPdv=NULL) const;
	void recalc_bound();
	// This only called once, loads data from file
	void getSubdivData();
	// returns copy of self, taking care of ref.count
	JS_SDPatch* duplicateSelf();

	// base patch shared data
	struct jsdata_t
	{
		Point3 *ecp;	// ecp -> control point hull in eigenspace
		int N, refc;	// N -> valence
	};
	jsdata_t* jsdata;
	Bound bd;
	float umin, umax, vmin, vmax;

	// static data shared by all JS_SDPatches
	struct EigenStruct { const double *L, *iV, *x[3]; };
	// flag to make sure when patches are culled at prerender stage that data is not yet deleted until last patch processed
	static bool prerender;
	static double* ccdata;
	static EigenStruct* eigen;
	static unsigned int data_refc;
};

//-------------------------------------------------------------------------------------------------------
// CCSubdivisionMesh, prepares mesh and splits into individual subdivision patches

class CCSubdivisionMesh : public Primitive
{
private:
	CCSubdivisionMesh(const CCSubdivisionMesh&);
	CCSubdivisionMesh& operator=(const CCSubdivisionMesh&);
public:
	CCSubdivisionMesh(RtInt nf, RtInt nverts[], RtInt verts[], RtInt ntags, RtToken tags[],
	                  RtInt nargs[], RtInt intargs[], RtFloat floatargs[], RtInt n, RtToken tokens[], RtPointer *parms);
	virtual ~CCSubdivisionMesh();
	virtual void post_init();
	virtual bool boundable() { return true; }
	virtual Bound bound();
	virtual bool splitable() { return true; }
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL);
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit) { return false; }
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false);
	virtual bool in_camspace() const { return true; }
protected:
	FixedArray<ccVert> vert_list;
	FixedArray<ccEdge> edge_list;
	FixedArray<ccFace> face_list;
	bool presubd;
};

void Subdivide(FixedArray<ccVert> &vert_list,
               FixedArray<ccEdge> &edge_list, FixedArray<ccFace> &face_list,
               bool fromPatch = false, bool limitProjection = false);
void rebuildLists(FixedArray<ccVert> &vert_list,
                  FixedArray<ccEdge> &edge_list, FixedArray<ccFace> &face_list,
                  bool fromPatch = false, bool st_rot = false);
void makePatches(const FixedArray<ccVert> &vert_list,
                 const FixedArray<ccEdge> &edge_list, const FixedArray<ccFace> &face_list,
                 const Primitive* parentprim, const Framework* FW = NULL, splitbprims_t* spb=NULL, bool fromPatch = false);

__END_QDRENDER

#endif // CCSUBDIVISION_H

#ifndef _PRIMITIVE_H
#define _PRIMITIVE_H

#include "ri.h"
#include "qdtl.h"
#include "decparam.h"
#include "Transform.h"
#include <vector>

#include "QDRender.h"
__BEGIN_QDRENDER

// primitive variables
// since string primvars are not supported (yet?), can use float arrays for everything
class vardata_t
{
private:
	vardata_t(const vardata_t&);
	vardata_t& operator=(const vardata_t&);
public:
	vardata_t(): data(NULL) {} // param is only relevant when data available, so no init needed
	vardata_t(const decParam_t& dp, float* dt=NULL):param(dp), data(dt) {}
	~vardata_t() { if (data) delete[] data;  data = NULL; }
	decParam_t param;
	float* data;
};

class PrimVars
{
private:
	PrimVars(const PrimVars&);
	PrimVars& operator=(const PrimVars&);
public:
	PrimVars();
	~PrimVars();
	void add(char* name, vardata_t* data);
	inline void incRefCount() { refc++; }
	inline int decRefCount() { return --refc; }
	// public data, accessed very frequently (split()/dice())
	sklist_t<vardata_t*> pvars;
	// once again a simple list is faster than a hashmap,
	// but besides that, there still seems to be a problem with it somewhere as well,
	// in CCSubdivision.cpp, when texcoords are init. from s/t,
	// memory usage suddenly explodes for some reason, no clue why yet...
	//hashmap_t<vardata_t*> pvars;
protected:
	int refc; // reference count
};


class Attributes;
class Bound;
class Framework;
class Hider;
class MicroPolygonGrid;
class BlurredPrimitive;

// used for splitting motionblurred prims, defined below
struct splitbprims_t
{
	BlurredPrimitive* parent;
	array_t<BlurredPrimitive*> bprims;
};

// base primitive class
class Primitive
{
private:
	Primitive(const Primitive&);
public:
	Primitive() : std_pvar(0), std_dice(0), attr(NULL), xform(NULL), eye_splits(0), primvars(NULL) {}
	virtual ~Primitive(); // !!! do not delete attr/xform !!!
	// use with derived classes that need to copy base primitive data
	Primitive& operator=(const Primitive& p);
	// special function for any post initialization that might be needed (pre transform, etc.)
	virtual void post_init()=0;
	// if pre transform to camspace in post_init() was done, then this must return true
	virtual bool in_camspace() const { return false; }
	virtual bool boundable()=0;
	virtual Bound bound()=0;
	virtual bool splitable()=0;
	// if splitbprims not NULL, it creates and/or appends to new BlurredPrimitive(s) (defined below) instead
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL)=0;
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit)=0;
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false)=0;

	void linear_dice(MicroPolygonGrid &g);

	unsigned int pvar_flags() const { return std_pvar; }
	unsigned int& dice_flags() { return std_dice; }
	int getEyeSplits() const { return eye_splits; }
	void incEyeSplits() { eye_splits++; }

	// !!! references, do not delete! bookkeeping done in State object !!!
	const Attributes* getAttributeReference() const  { return attr; }
	void setAttributeReference(const Attributes* a)  { attr = a; }
	const Transform* getTransformReference() const  { return xform; }
	void setTransformReference(const Transform* xf)  { xform = xf; }

	const PrimVars* getPrimVars() const { return primvars; }
	PrimVars* newPrimVars();
	void removePrimVar(const char* name);

	virtual void get_uvlim(float uvl[4]) const { uvl[0] = uvl[2] = 0.f, uvl[1] = uvl[3] = 1.f; }

protected:
	// initialize primitive variables from Ri parameter list
	// update: added facevarying, guesswork...
	void initPrimVars(RtInt n, RtToken tokens[], RtPointer parms[],
	                  int uniformMax, int varyingMax, int vertexMax, int faceVaryingMax);

	// data
	unsigned int std_pvar;	// as std_dice below, but indicates global primitive vars that require dicing
	unsigned int std_dice;	// copy of 'globals_used' in SlShader, indicates global shader variables that need dicing
	// !!! these are borrowed references, do not delete !!!!
	const Attributes* attr;
	// NOTE the reason the transform is separate from attributes here, is that a special attribute bookkeeping scheme
	// is used that scans attributes for possible re-use, and keeping transform separate from that makes sure that
	// this has a higher chance of being succesful, which can make quite a difference in memory usage.
	// It is also used for the transform itself as well, see State.cpp, cloneAttributes() & cloneTransform()
	const Transform* xform;
	// max eye splits for this prim
	int eye_splits;
	PrimVars* primvars;
};

// motion blurred primitives
class BlurredPrimitive : public Primitive
{
private:
	BlurredPrimitive& operator=(const BlurredPrimitive&);
public:
	BlurredPrimitive();
	BlurredPrimitive(const BlurredPrimitive& bp);
	virtual ~BlurredPrimitive();
	virtual void post_init();
	virtual bool in_camspace() const;
	virtual bool boundable();
	virtual Bound bound();
	virtual bool splitable();
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL);
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit);
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false);
	void append(Primitive* p);
	bool empty() const { return poses.empty(); }
	void addXform(const Transform& mt) { motion_xform.push_back(mt); }
protected:
	// contains multiple instances of the same primitive
	std::vector<Primitive*> poses;
	// only for transformational motion blur, the set of transforms at each shutter time
	std::vector<Transform> motion_xform;
	float shmin, shmax;	// shutter min/max, Options copy
};

__END_QDRENDER

#endif // _PRIMITIVE_H

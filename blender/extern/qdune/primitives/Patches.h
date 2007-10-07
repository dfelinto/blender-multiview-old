//----------------------------------------------
// Bilinear/bicubic patches & patch meshes
//----------------------------------------------

#ifndef _PATCHES_H
#define _PATCHES_H

#include "Parametric.h"
#include "Polygons.h"
#include "qdVector.h"

#include "QDRender.h"
__BEGIN_QDRENDER

//------------------------------------------------------------------------------
// BilinearPatch

class BilinearPatch : public Parametric
{
public:
	BilinearPatch();
	BilinearPatch(RtInt n, RtToken tokens[], RtPointer parms[]);
	// only used for polygon & patchmesh splitting
	BilinearPatch(const Primitive& p, const RtPoint p0, const RtPoint p1,
																		const RtPoint p2, const RtPoint p3);
	virtual ~BilinearPatch();

	virtual void post_init();

	virtual Bound bound();
	virtual Parametric* duplicateSelf() { return new BilinearPatch(*this); }

	virtual bool in_camspace() const { return true; }

protected:
	virtual void eval(float u, float v, RtPoint P, RtVector dPdu=NULL, RtVector dPdv=NULL) const;
	// control points
	RtPoint hull[4];
};

//------------------------------------------------------------------------------
// BilinearPatchMesh
// only splits the patchmesh into its individual patches, dice() never called

class BilinearPatchMesh : public Primitive
{
public:
	BilinearPatchMesh(RtInt nu, bool uperiodic, RtInt nv, bool vperiodic,
										RtInt n, RtToken tokens[], RtPointer parms[]);
	virtual ~BilinearPatchMesh();
	virtual void post_init() {}
	virtual bool boundable() { return true; }
	virtual Bound bound();
	virtual bool splitable() { return true; }
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL);
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit) { return false; }
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false);
protected:
	// data
	RtFloat* pts; // RtFloat here, since besides 'P', data could also be 'Pz' or 'Pw'
	bool uperiodic, vperiodic;
	float du, dv;
	int nu, nv, nupatches, nvpatches, nuvarying, nvvarying;
	int mtype, num_verts; // mtype will be one of (0, 1, 2) for P/Pz/Pw respectivly
};

// helper function, split data for new patch, also called by Polygons.cpp
void splitPrimVars(const Primitive* parent, Primitive* newp,
                   int uni_idx, int vary_idx[4], int* vert_idx, bool cubic=false);

//------------------------------------------------------------------------------
// BicubicPatch

class BicubicPatch : public Primitive
{
	friend class SDPatch;
	friend class JS_SDPatch;
public:
	BicubicPatch();
	// only used for splitting patchmesh
	BicubicPatch(const Primitive& p, const Point3 _hull[16]);
	BicubicPatch(RtInt n, RtToken tokens[], RtPointer parms[]);
	virtual ~BicubicPatch();
	virtual void post_init();

	virtual bool boundable() { return true; }
	virtual Bound bound();

	virtual bool splitable() { return true; }
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL);
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit);
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false);

	Primitive* duplicateSelf();

	virtual bool in_camspace() const { return true; }

	virtual void get_uvlim(float uvl[4]) const { uvl[0] = umin, uvl[1] = umax, uvl[2] = vmin, uvl[3] = vmax; }

protected:
	RtPoint hull[16];
	float umin, umax, vmin, vmax;
	bool subd_bspline;
};

//------------------------------------------------------------------------------
// BicubicPatchMesh
// only splits the patchmesh into its individual patches, dice() is never called

class BicubicPatchMesh : public Primitive
{
public:
	BicubicPatchMesh(RtInt nu, bool uperiodic, RtInt nv, bool vperiodic,
										RtInt n, RtToken tokens[], RtPointer parms[]);
	virtual ~BicubicPatchMesh();
	virtual void post_init() {}
	virtual bool boundable() { return true; }
	virtual Bound bound();
	virtual bool splitable() { return true; }
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL);
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit) { return false; }
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false);
protected:
	RtFloat* pts; // RtFloat here, since besides 'P', data could also be 'Pz' or 'Pw'
	bool uperiodic, vperiodic;
	float du, dv;
	int nu, nv, ustep, vstep, nupatches, nvpatches, nuvarying, nvvarying;
	int mtype, num_verts; // mtype will be one of (0, 1, 2) for P/Pz/Pw respectivly
};

__END_QDRENDER

#endif // _PATCHES_H

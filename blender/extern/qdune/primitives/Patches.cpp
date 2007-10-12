///-----------------------------------------------------
/// Bilinear/bicubic patches & patch meshes
///-----------------------------------------------------
/// Bilinear patches also function as a stand-in
/// for polygons, see Polgons.cpp
/// And bicubic patches too are useful elsewhere,
/// if it behaves well, most of a catmull-clark
/// subdivision mesh should consist of B-spline patches.
/// See CCSubdivision.cpp
///
/// Patchmeshes are not handled optimally, currently
/// once a patchmesh needs to be split, it is split
/// all at once into its individual component patches.
/// Must be optimized to do an actual mesh split TODO
///

#include "Patches.h"
#include "Bound.h"
#include "Mathutil.h"
#include "Framework.h"
#include "MicroPolygonGrid.h"
#include "Hider.h"

#include "State.h"

__BEGIN_QDRENDER

//------------------------------------------------------------------------------
// Bilinear Patches

// empty ctor
BilinearPatch::BilinearPatch()
{
}

BilinearPatch::BilinearPatch(RtInt n, RtToken tokens[], RtPointer parms[])
{
	// control points
	// hull order p00, p01, p10, p11
	for (int i=0; i<n; i++) {
		if (!strcmp(tokens[i], RI_P)) {
			memcpy(hull, *parms, sizeof(RtPoint)*4);
			break;
		}
	}
	// other variables
	Primitive::initPrimVars(n, tokens, parms, 1, 4, 4, 4);
}

// only used for polygon splitting
BilinearPatch::BilinearPatch(const Primitive& p, const RtPoint p0, const RtPoint p1,
                             const RtPoint p2, const RtPoint p3)
{
	static_cast<Primitive&>(*this) = p; // copy primitive data (base class)
	hull[0][0] = p0[0], hull[0][1] = p0[1], hull[0][2] = p0[2];
	hull[1][0] = p1[0], hull[1][1] = p1[1], hull[1][2] = p1[2];
	hull[2][0] = p2[0], hull[2][1] = p2[1], hull[2][2] = p2[2];
	hull[3][0] = p3[0], hull[3][1] = p3[1], hull[3][2] = p3[2];
}

BilinearPatch::~BilinearPatch()
{
}

void BilinearPatch::post_init()
{
	// transform hull to cam space
	const RtMatrix* ri_mtx = xform->getRtMatrixPtr();
	mulPMP(hull[0], *ri_mtx, hull[0]);
	mulPMP(hull[1], *ri_mtx, hull[1]);
	mulPMP(hull[2], *ri_mtx, hull[2]);
	mulPMP(hull[3], *ri_mtx, hull[3]);
}

Bound BilinearPatch::bound()
{
	Bound b;
	const RtVector v1 = {hull[2][0] - hull[0][0], hull[2][1] - hull[0][1], hull[2][2] - hull[0][2]},
	               v2 = {hull[3][0] - hull[1][0], hull[3][1] - hull[1][1], hull[3][2] - hull[1][2]};
	RtPoint L = {hull[0][0] + vmin*v1[0], hull[0][1] + vmin*v1[1], hull[0][2] + vmin*v1[2]};
	RtVector dRL = {(hull[1][0] + vmin*v2[0]) - L[0], (hull[1][1] + vmin*v2[1]) - L[1], (hull[1][2] + vmin*v2[2]) - L[2]};
	b.include(Point3(L[0] + umin*dRL[0], L[1] + umin*dRL[1], L[2] + umin*dRL[2]));
	b.include(Point3(L[0] + umax*dRL[0], L[1] + umax*dRL[1], L[2] + umax*dRL[2]));
	L[0] = hull[0][0] + vmax*v1[0], L[1] = hull[0][1] + vmax*v1[1], L[2] = hull[0][2] + vmax*v1[2];
	dRL[0] = (hull[1][0] + vmax*v2[0]) - L[0], dRL[1] = (hull[1][1] + vmax*v2[1]) - L[1], dRL[2] = (hull[1][2] + vmax*v2[2]) - L[2];
	b.include(Point3(L[0] + umin*dRL[0], L[1] + umin*dRL[1], L[2] + umin*dRL[2]));
	b.include(Point3(L[0] + umax*dRL[0], L[1] + umax*dRL[1], L[2] + umax*dRL[2]));
	b.addEpsilon();
	// hull already in cameraspace, so no xform needed
	return b;
}

void BilinearPatch::eval(float u, float v, RtPoint P, RtVector dPdu, RtVector dPdv) const
{
	const RtPoint lv = {hull[2][0] - hull[0][0], hull[2][1] - hull[0][1], hull[2][2] - hull[0][2]};
	const RtPoint rv = {hull[3][0] - hull[1][0], hull[3][1] - hull[1][1], hull[3][2] - hull[1][2]};
	P[0] = hull[0][0] + v*lv[0];
	P[1] = hull[0][1] + v*lv[1];
	P[2] = hull[0][2] + v*lv[2];
	P[0] += u*((hull[1][0] + v*rv[0]) - P[0]);
	P[1] += u*((hull[1][1] + v*rv[1]) - P[1]);
	P[2] += u*((hull[1][2] + v*rv[2]) - P[2]);
	if (dPdu && dPdv) {
		const RtVector bv = {hull[1][0] - hull[0][0], hull[1][1] - hull[0][1], hull[1][2] - hull[0][2]};
		dPdu[0] = bv[0] + v*((hull[3][0] - hull[2][0]) - bv[0]);
		dPdu[1] = bv[1] + v*((hull[3][1] - hull[2][1]) - bv[1]);
		dPdu[2] = bv[2] + v*((hull[3][2] - hull[2][2]) - bv[2]);
		dPdv[0] = lv[0] + u*(rv[0] - lv[0]);
		dPdv[1] = lv[1] + u*(rv[1] - lv[1]);
		dPdv[2] = lv[2] + u*(rv[2] - lv[2]);
	}
}

//------------------------------------------------------------------------------
// BilinearPatchMesh, never dices, only splits

BilinearPatchMesh::BilinearPatchMesh(RtInt nu, bool uperiodic, RtInt nv, bool vperiodic,
                                     RtInt n, RtToken tokens[], RtPointer parms[])
{
	const int numpts = nu*nv;
	pts = NULL;
	for (int i=0; i<n; ++i) {
		if (!strcmp(tokens[i], RI_P)) {
			pts = new RtFloat[numpts*3];
			memcpy(pts, parms[i], sizeof(RtFloat)*numpts*3);
			mtype = 0;
		}
		else if (!strcmp(tokens[i], RI_PZ)) {
			pts = new RtFloat[numpts];
			memcpy(pts, parms[i], sizeof(RtFloat)*numpts);
			mtype = 1;
		}
		else if (!strcmp(tokens[i], RI_PW)) {
			pts = new RtFloat[numpts*4];
			memcpy(pts, parms[i], sizeof(RtFloat)*numpts*4);
			mtype = 2;
		}
	}
	this->nu = nu;
	this->nv = nv;
	this->uperiodic = uperiodic;
	this->vperiodic = vperiodic;
	nupatches = uperiodic ? nu : (nu - 1);
	nvpatches = vperiodic ? nv : (nv - 1);
	nuvarying = uperiodic ? nupatches : (nupatches + 1);
	nvvarying = vperiodic ? nvpatches : (nvpatches + 1);
	// du/dv are used to create the parametric u,v coords for the 'Pz' case
	du = 1.f/nupatches;
	dv = 1.f/nvpatches;
	Primitive::initPrimVars(n, tokens, parms, nupatches*nvpatches, nuvarying*nvvarying, nu*nv, nupatches*nvpatches*4);
}

BilinearPatchMesh::~BilinearPatchMesh()
{
	if (pts) { delete[] pts;  pts = NULL; }
}

Bound BilinearPatchMesh::bound()
{
	Bound b;
	int idx = 0;
	for (int vi=0; vi<nv; ++vi) {
		const float v = vi / float(nv - 1);
		for (int ui=0; ui<nu; ++ui, ++idx) {
			const float u = ui / float(nu - 1);
			if (mtype == 0) {
				const RtPoint* meshpts = reinterpret_cast<RtPoint*>(pts);
				b.include(meshpts[idx]);
			}
			else if (mtype == 1)
				b.include(Point3(u, v, pts[idx]));
			// hpoint TODO
		}
	}
	b.addEpsilon();
	b.transform(xform);
	return b;
}

// helper function, split data for new patch, also called by Polygons.cpp
void splitPrimVars(const Primitive* parent, Primitive* newp,
                   int uni_idx, int vary_idx[4], int* vert_idx, bool cubic)
{
	const PrimVars* pv = parent->getPrimVars();
	if (pv==NULL) return;
	PrimVars* npv = NULL;
	sklist_t<vardata_t*>& varlist = const_cast<PrimVars*>(pv)->pvars;
	vardata_t** vdt = varlist.first();
	while (vdt) {
		if (vdt) {
			if (npv == NULL) npv = newp->newPrimVars();
			decParam_t& dp = (*vdt)->param;
			vardata_t* nvdt = new vardata_t(dp); // param copy, always the same
			if (dp.ct_flags & SC_CONSTANT) {
				nvdt->data = new float[dp.numfloats];
				memcpy(nvdt->data, (*vdt)->data, sizeof(float)*dp.numfloats);
			}
			else if (dp.ct_flags & SC_UNIFORM) {
				if (dp.ct_flags & DT_FLOAT) {
					float *nfa = new float[1], *ofa = (*vdt)->data;
					nfa[0] = ofa[uni_idx];
					nvdt->data = nfa;
				}
				else if (dp.ct_flags & DT_FLOAT3MASK) {
					// pretend vector, doesn't matter, all are 3-float arrays
					RtVector *nva = new RtVector[1], *pva = reinterpret_cast<RtVector*>((*vdt)->data);
					nva[0][0] = pva[uni_idx][0], nva[0][1] = pva[uni_idx][1], nva[0][2] = pva[uni_idx][2];
					nvdt->data = reinterpret_cast<float*>(nva);
				}
				// mtx TODO
			}
			else if (dp.ct_flags & (SC_VARYING | SC_FACEVARYING | SC_VERTEX)) {
				if (dp.ct_flags & DT_FLOAT) {
					float *nfa = new float[4], *ofa = (*vdt)->data;
					nfa[0] = ofa[vary_idx[0]];  nfa[1] = ofa[vary_idx[1]];
					nfa[2] = ofa[vary_idx[2]];  nfa[3] = ofa[vary_idx[3]];
					nvdt->data = nfa;
				}
				else if (dp.ct_flags & DT_FLOAT3MASK) {
					// pretend vector, doesn't matter, all are 3-float arrays
					RtVector *nva = new RtVector[4], *pva = reinterpret_cast<RtVector*>((*vdt)->data);
					nva[0][0] = pva[vary_idx[0]][0], nva[0][1] = pva[vary_idx[0]][1], nva[0][2] = pva[vary_idx[0]][2];
					nva[1][0] = pva[vary_idx[1]][0], nva[1][1] = pva[vary_idx[1]][1], nva[1][2] = pva[vary_idx[1]][2];
					nva[2][0] = pva[vary_idx[2]][0], nva[2][1] = pva[vary_idx[2]][1], nva[2][2] = pva[vary_idx[2]][2];
					nva[3][0] = pva[vary_idx[3]][0], nva[3][1] = pva[vary_idx[3]][1], nva[3][2] = pva[vary_idx[3]][2];
					nvdt->data = reinterpret_cast<float*>(nva);
				}
				// don't know what to do with matrices yet...
			}
			npv->pvars.insert(varlist.getName(), nvdt);
			vdt = varlist.next();
		}
	}
}

// extract individual patch components from the mesh
void BilinearPatchMesh::split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb)
{
	int uni_idx = 0, vary_idx[4]; // data indices, varying == vertex for bilinear patches
	float uv[8]; // u/v for Pz case
	for (int vp=0; vp<nvpatches; ++vp) {
		for (int up=0; up<nupatches; ++up, ++uni_idx) {
			int idx = 0;
			for (int vi=0; vi<2; ++vi) {
				const float v = (vi + vp)*dv;
				const int vidx = (vp + vi) % nv;
				for (int ui=0; ui<2; ++ui, ++idx) {
					uv[idx*2] = (ui + up)*du, uv[idx*2+1] = v;
					vary_idx[idx] = ((up + ui) % nu) + vidx*nu;
				}
			}
			BilinearPatch* bp = NULL;
			if (mtype == 0) {
				const RtPoint* meshpts = reinterpret_cast<RtPoint*>(pts);
				const RtPoint p1 = {meshpts[vary_idx[0]][0], meshpts[vary_idx[0]][1], meshpts[vary_idx[0]][2]};
				const RtPoint p2 = {meshpts[vary_idx[1]][0], meshpts[vary_idx[1]][1], meshpts[vary_idx[1]][2]};
				const RtPoint p3 = {meshpts[vary_idx[2]][0], meshpts[vary_idx[2]][1], meshpts[vary_idx[2]][2]};
				const RtPoint p4 = {meshpts[vary_idx[3]][0], meshpts[vary_idx[3]][1], meshpts[vary_idx[3]][2]};
				bp = new BilinearPatch(*this, p1, p2, p3, p4);
			}
			else if (mtype == 1) {
				const RtPoint p1 = {uv[0], uv[1], pts[vary_idx[0]]};
				const RtPoint p2 = {uv[2], uv[3], pts[vary_idx[1]]};
				const RtPoint p3 = {uv[4], uv[5], pts[vary_idx[2]]};
				const RtPoint p4 = {uv[6], uv[7], pts[vary_idx[3]]};
				bp = new BilinearPatch(*this, p1, p2, p3, p4);
			}

			// split primitive variables, if any
			splitPrimVars(this, bp, uni_idx, vary_idx, vary_idx);

			bp->post_init();
			f.insert(bp);
		}
	}
}


// does nothing, print msg in case it is called, should never happen though...
void BilinearPatchMesh::dice(MicroPolygonGrid &g, bool Pclose)
{
	printf("[ERROR]: BilinearPatchMesh()->dice() called?\n");
}


//------------------------------------------------------------------------------
// Bezier evaluation funcs for bicubic patches

// not used anymore
/*
// initializes the Bezier forward differences for given hull, returned in point array fd
void calcFD(const Point3 hull[16], float nu, float nv, Point3 fd[16])
{
	const float du = 1.f/(float)(nu-1), dv = 1.f/(float)(nv-1);
	const float du2 = du*du, dv2 = dv*dv;
	const float du3 = du2*du, dv3 = dv2*dv;
	const float ds[16] = { 1.f, 0.f, 0.f, 0.f,
				-du3 + 3.f*(du2 - du), 3.f*(du3 - 2.f*du2 + du), 3.f*(du2 - du3), du3,
				6.f*(du2 - du3), 12.f*(1.5f*du3 - du2), 6.f*(du2 - 3.f*du3), 6.f*du3,
				-6.f*du3, 18.f*du3, -18.f*du3, 6.f*du3};
	// dt is transpose of ds with factor dv
	const float dt[16] = {1.f, -dv3 + 3.f*(dv2 - dv), 6.f*(dv2 - dv3), -6.f*dv3,
				0.f, 3.f*(dv3 - 2.f*dv2 + dv), 12.f*(1.5f*dv3 - dv2), 18.f*dv3,
				0.f, 3.f*(dv2 - dv3), 6.f*(dv2 - 3.f*dv3), -18.f*dv3,
				0.f, dv3, 6.f*dv3, 6.f*dv3};
	Point3 tfd[16];
	for (int y=0, y4=0; y<4; ++y, y4+=4) {
		tfd[    y4] = hull[0]*dt[y] + hull[4]*dt[4+y] + hull[ 8]*dt[8+y] + hull[12]*dt[12+y];
		tfd[1 + y4] = hull[1]*dt[y] + hull[5]*dt[4+y] + hull[ 9]*dt[8+y] + hull[13]*dt[12+y];
		tfd[2 + y4] = hull[2]*dt[y] + hull[6]*dt[4+y] + hull[10]*dt[8+y] + hull[14]*dt[12+y];
		tfd[3 + y4] = hull[3]*dt[y] + hull[7]*dt[4+y] + hull[11]*dt[8+y] + hull[15]*dt[12+y];
	}
	for (int x4=0; x4<16; x4+=4) {
		fd[    x4] = tfd[ 0]*ds[x4] + tfd[ 1]*ds[x4+1] + tfd[ 2]*ds[x4+2] + tfd[ 3]*ds[x4+3];
		fd[1 + x4] = tfd[ 4]*ds[x4] + tfd[ 5]*ds[x4+1] + tfd[ 6]*ds[x4+2] + tfd[ 7]*ds[x4+3];
		fd[2 + x4] = tfd[ 8]*ds[x4] + tfd[ 9]*ds[x4+1] + tfd[10]*ds[x4+2] + tfd[11]*ds[x4+3];
		fd[3 + x4] = tfd[12]*ds[x4] + tfd[13]*ds[x4+1] + tfd[14]*ds[x4+2] + tfd[15]*ds[x4+3];
	}
}
*/

// transform control mesh to equivalent bezier control mesh
static void convert2Bezier(RtPoint hull[16], const RtMatrix ubasis, const RtMatrix vbasis)
{
	const RtMatrix invBezier = {{0.f,     0.f,     0.f, 1.f},
	                            {0.f,     0.f, 1.f/3.f, 1.f},
	                            {0.f, 1.f/3.f, 2.f/3.f, 1.f},
	                            {1.f,     1.f,     1.f, 1.f}};
	RtMatrix pre, post;
	mulMMM(pre, invBezier, vbasis);
	transposeM(pre);
	mulMMM(post, invBezier, ubasis);
	// dimension here is num of point elements,
	// can be hpoint too so then would be 4 (xyzw)
	// TODO
	for (int dim=0; dim<3; ++dim) {
		RtMatrix P;
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				P[i][j] = hull[i*4+j][dim];
		mulMMM(P, P, pre);
		mulMMM(P, post, P);
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				hull[i*4+j][dim] = P[i][j];
	}
}

// as above, but for use with BSpline patches (used in subdiv)
static void bSpline2Bezier(RtPoint hull[16])
{
	const RtMatrix pre = {{1.f/6.f,     0.f,     0.f,     0.f},
	                      {2.f/3.f, 2.f/3.f, 1.f/3.f, 1.f/6.f},
	                      {1.f/6.f, 1.f/3.f, 2.f/3.f, 2.f/3.f},
	                      {    0.f,     0.f,     0.f, 1.f/6.f}};
	const RtMatrix post = {{1.f/6.f, 2.f/3.f, 1.f/6.f,     0.f},
	                      {     0.f, 2.f/3.f, 1.f/3.f,     0.f},
	                      {     0.f, 1.f/3.f, 2.f/3.f,     0.f},
	                      {     0.f, 1.f/6.f, 2.f/3.f, 1.f/6.f}};
	// dimension here is num of point elements,
	// can be hpoint too so then would be 4 (xyzw)
	// TODO (make hpoint by default and use w=1)
	for (int dim=0; dim<3; ++dim) {
		RtMatrix P;
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				P[i][j] = hull[i*4+j][dim];
		mulMMM(P, P, pre);
		mulMMM(P, post, P);
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				hull[i*4+j][dim] = P[i][j];
	}
}

//------------------------------------------------------------------------------
// Bicubic Patches

BicubicPatch::BicubicPatch():subd_bspline(false)
{
	umin = vmin = 0;
	umax = vmax = 1;
}

// only used for splitting patchmesh
BicubicPatch::BicubicPatch(const Primitive& p, const Point3 _hull[16]):subd_bspline(false)
{
	static_cast<Primitive&>(*this) = p; // copy primitive data (base class)
	memcpy(hull, _hull, sizeof(Point3)*16);
	umin = vmin = 0;
	umax = vmax = 1;
}


BicubicPatch::BicubicPatch(RtInt n, RtToken tokens[], RtPointer parms[]):subd_bspline(false)
{
	for (int i=0; i<n; i++) {
		if (!strcmp(tokens[i], RI_P)) {
			memcpy(hull, *parms, sizeof(RtPoint)*16);
			break;
		}
	}
	// dont' forget reference count in duplicateSelf (must do this differently similar to Parametric)
	Primitive::initPrimVars(n, tokens, parms, 1, 4, 16, 4);
	umin = vmin = 0;
	umax = vmax = 1;
}

BicubicPatch::~BicubicPatch()
{
}

void BicubicPatch::post_init()
{
	if (subd_bspline)  { // only convert bspline basis to bezier, no transform needed, already in camspace
		bSpline2Bezier(hull);
		return;
	}
	// convert specified basis to bezier
	convert2Bezier(hull, *attr->cubicBasisMatrix[0]->getRtMatrixPtr(), *attr->cubicBasisMatrix[1]->getRtMatrixPtr());
	// transform hull to camera space
	const RtMatrix* ri_xf = xform->getRtMatrixPtr();
	for (int j=0; j<16; ++j)
		mulPMP(hull[j], *ri_xf, hull[j]);
}

Bound BicubicPatch::bound()
{
	Bound b;
	for (int i=0; i<16; ++i) b.include(hull[i]);
	b.addEpsilon();
	// no transform needed, hull already in camera space
	return b;
}

Primitive* BicubicPatch::duplicateSelf()
{
	BicubicPatch* bcp = new BicubicPatch();
	// attributes is reference, so copy ok
	memcpy(bcp, this, sizeof(BicubicPatch));
	if (primvars) primvars->incRefCount();
	return bcp;
}

inline void bezCurveSplit(const RtPoint P[4], RtPoint Q[4], RtPoint R[4])
{
	Q[0][0] = P[0][0], Q[0][1] = P[0][1], Q[0][2] = P[0][2];
	addVVV(Q[1], P[0], P[1]);
	mulVVF(Q[1], Q[1], 0.5f);
	addVVV(Q[2], P[1], P[2]);
	mulVVF(Q[2], Q[2], 0.25f);
	maddVVF(Q[2], Q[1], 0.5f);
	R[3][0] = P[3][0], R[3][1] = P[3][1], R[3][2] = P[3][2];
	addVVV(R[2], P[2], P[3]);
	mulVVF(R[2], R[2], 0.5f);
	addVVV(R[1], P[1], P[2]);
	mulVVF(R[1], R[1], 0.25f);
	maddVVF(R[1], R[2], 0.5f);
	addVVV(Q[3], Q[2], R[1]);
	mulVVF(Q[3], Q[3], 0.5f);
	R[0][0] = Q[3][0], R[0][1] = Q[3][1], R[0][2] = Q[3][2];
}

inline void bezCurveSplit_VfromSurface(const RtPoint P[16], RtPoint Q[16], RtPoint R[16], int ofs)
{
	const int o4 = ofs + 4, o8 = ofs + 8, o12 = ofs + 12;
	Q[ofs][0] = P[ofs][0], Q[ofs][1] = P[ofs][1], Q[ofs][2] = P[ofs][2];
	addVVV(Q[o4], P[ofs], P[o4]);
	mulVVF(Q[o4], Q[o4], 0.5f);
	addVVV(Q[o8], P[o4], P[o8]);
	mulVVF(Q[o8], Q[o8], 0.25f);
	maddVVF(Q[o8], Q[o4], 0.5f);
	R[o12][0] = P[o12][0], R[o12][1] = P[o12][1], R[o12][2] = P[o12][2];
	addVVV(R[o8], P[o8], P[o12]);
	mulVVF(R[o8], R[o8], 0.5f);
	addVVV(R[o4], P[o4], P[o8]);
	mulVVF(R[o4], R[o4], 0.25f);
	maddVVF(R[o4], R[o8], 0.5f);
	addVVV(Q[o12], Q[o8], R[o4]);
	mulVVF(Q[o12], Q[o12], 0.5f);
	R[ofs][0] = Q[o12][0], R[ofs][1] = Q[o12][1], R[ofs][2] = Q[o12][2];
}

inline void bezSurface_Usplit(const RtPoint hull[16], RtPoint hull_1[16], RtPoint hull_2[16])
{
	bezCurveSplit(&hull[0], &hull_1[0], &hull_2[0]);
	bezCurveSplit(&hull[4], &hull_1[4], &hull_2[4]);
	bezCurveSplit(&hull[8], &hull_1[8], &hull_2[8]);
	bezCurveSplit(&hull[12], &hull_1[12], &hull_2[12]);
}

inline void bezSurface_Vsplit(const RtPoint hull[16], RtPoint hull_1[16], RtPoint hull_2[16])
{
	bezCurveSplit_VfromSurface(hull, hull_1, hull_2, 0);
	bezCurveSplit_VfromSurface(hull, hull_1, hull_2, 1);
	bezCurveSplit_VfromSurface(hull, hull_1, hull_2, 2);
	bezCurveSplit_VfromSurface(hull, hull_1, hull_2, 3);
}

void BicubicPatch::split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb)
{
	if (usplit && vsplit) {
		// left & right
		RtPoint hull_L[16], hull_R[16];
		bezSurface_Usplit(hull, hull_L, hull_R);
		// Left to bottom & top 1
		BicubicPatch *LT = static_cast<BicubicPatch*>(duplicateSelf());
		BicubicPatch *LB = static_cast<BicubicPatch*>(duplicateSelf());
		bezSurface_Vsplit(hull_L, LT->hull, LB->hull);
		const float umid = (umin + umax)*0.5f, vmid = (vmin + vmax)*0.5f;
		LB->umax = LT->umax = umid, LB->vmax = LT->vmin = vmid;
		if (spb) {
			if (spb->bprims.empty()) {
				spb->bprims.resize(4);
				spb->bprims[0] = new BlurredPrimitive(*spb->parent);
				spb->bprims[1] = new BlurredPrimitive(*spb->parent);
				spb->bprims[2] = new BlurredPrimitive(*spb->parent);
				spb->bprims[3] = new BlurredPrimitive(*spb->parent);
			}
			spb->bprims[0]->append(LT), spb->bprims[1]->append(LB);
		}
		else
			f.insert(LT), f.insert(LB);
		// Right to bottom & top 2
		BicubicPatch *RT = static_cast<BicubicPatch*>(duplicateSelf());
		BicubicPatch *RB = static_cast<BicubicPatch*>(duplicateSelf());
		bezSurface_Vsplit(hull_R, RT->hull, RB->hull);
		RB->umin = RT->umin = umid;
		RB->vmax = RT->vmin = vmid;
		if (spb)
			spb->bprims[2]->append(RT), spb->bprims[3]->append(RB);
		else
			f.insert(RT), f.insert(RB);
	}
	else {
		if (usplit) {
			BicubicPatch *L = static_cast<BicubicPatch*>(duplicateSelf());
			BicubicPatch *R = static_cast<BicubicPatch*>(duplicateSelf());
			bezSurface_Usplit(hull, L->hull, R->hull);
			const float umid = (umin + umax)*0.5f;
			L->umax = umid, R->umin = umid;
			if (spb) {
				if (spb->bprims.empty()) {
					spb->bprims.resize(2);
					spb->bprims[0] = new BlurredPrimitive(*spb->parent);
					spb->bprims[1] = new BlurredPrimitive(*spb->parent);
				}
				spb->bprims[0]->append(L), spb->bprims[1]->append(R);
			}
			else
				f.insert(L), f.insert(R);
		}
		else {
			BicubicPatch *T = static_cast<BicubicPatch*>(duplicateSelf());
			BicubicPatch *B = static_cast<BicubicPatch*>(duplicateSelf());
			bezSurface_Vsplit(hull, T->hull, B->hull);
			const float vmid = (vmin + vmax)*0.5f;
			T->vmax = vmid, B->vmin = vmid;
			if (spb) {
				if (spb->bprims.empty()) {
					spb->bprims.resize(2);
					spb->bprims[0] = new BlurredPrimitive(*spb->parent);
					spb->bprims[1] = new BlurredPrimitive(*spb->parent);
				}
				spb->bprims[0]->append(T), spb->bprims[1]->append(B);
			}
			else
				f.insert(T), f.insert(B);
		}
	}
}

bool BicubicPatch::diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit)
{
	// direct dice of P only
	#define TESTSIZE 8
	static Point3 ym[TESTSIZE+1];
	float du = 1.f/(float)TESTSIZE, dv = 1.f/(float)TESTSIZE;
	float u, v;
	int ug, vg;
	float maxUDist=-1e10f, maxVDist=-1e10f;
	for (vg=0, v=0.f; vg<=TESTSIZE; ++vg, v+=dv) {
		Point3 ptp;
		for (ug=0, u=0.f; ug<=TESTSIZE; ++ug, u+=du) {
			RtPoint tC[4], rP;
			deCasteljau_P(u, hull, tC[0]);
			deCasteljau_P(u, hull + 4, tC[1]);
			deCasteljau_P(u, hull + 8, tC[2]);
			deCasteljau_P(u, hull + 12, tC[3]);
			deCasteljau_P(v, tC, rP);
			Point3 tp(rP[0], rP[1], rP[2]);
			if (ug > 0) maxUDist = MAX2(h.rasterEstimate(tp, ptp), maxUDist);
			if (vg > 0) maxVDist = MAX2(h.rasterEstimate(tp, ym[ug]), maxVDist);
			ym[ug] = ptp = tp;
		}
	}

	const Attributes* attr = getAttributeReference();
	// 2x2 mp minimum
	unsigned int xdim = (attr->flags & AF_DICE_BINARY) ? (1 << MAX2(1, int(0.5f + (float)log(MAX2(1e-7f, TESTSIZE*maxUDist) / attr->effectiveShadingRate)*(float)M_LOG2E)))
	                                                   : MAX2(2, int(0.5f + (TESTSIZE*maxUDist) / attr->effectiveShadingRate));
	unsigned int ydim = (attr->flags & AF_DICE_BINARY) ? (1 << MAX2(1, int(0.5f + (float)log(MAX2(1e-7f, TESTSIZE*maxVDist) / attr->effectiveShadingRate)*(float)M_LOG2E)))
	                                                   : MAX2(2, int(0.5f + (TESTSIZE*maxVDist) / attr->effectiveShadingRate));

	g.setDim(xdim, ydim, this);
	xdim++, ydim++;
	if ((xdim<h.gridSize()) && (ydim<h.gridSize()) && ((xdim*ydim)<h.gridSize())) return true;
	usplit = (xdim > ydim);
	vsplit = !usplit;
	return false;
	#undef TESTSIZE
}

void BicubicPatch::dice(MicroPolygonGrid &g, bool Pclose)
{
	int ug, ug4, vg, idx = 0;
	float u, v;
	RtPoint* P_grid = (RtPoint*)(Pclose ? g.addVariable("=Pclose") : g.addVariable("P"));
	RtVector *dPdu_grid = (RtVector*)g.addVariable("dPdu"), *dPdv_grid = (RtVector*)g.addVariable("dPdv");
	const int xdim=g.get_xdim(), ydim=g.get_ydim();
	float du=1.f/(float)xdim, dv=1.f/(float)ydim;

	// u tangent precalc.
	RtPoint* utan = new RtPoint[(xdim+1) << 2];
	const int xdim4 = xdim << 2;
	for (ug4=0, u=0.f; ug4<=xdim4; ug4+=4, u+=du) {
		deCasteljau_P(u, hull, utan[ug4]);
		deCasteljau_P(u, hull + 4, utan[ug4+1]);
		deCasteljau_P(u, hull + 8, utan[ug4+2]);
		deCasteljau_P(u, hull + 12, utan[ug4+3]);
	}

	// Dice P, dPdu & dPdv
	RtPoint tC[4];
	for (vg=0, v=0.f; vg<=ydim; ++vg, v+=dv) {
		deCasteljau_P(v, hull[0], hull[4], hull[ 8], hull[12], tC[0]);
		deCasteljau_P(v, hull[1], hull[5], hull[ 9], hull[13], tC[1]);
		deCasteljau_P(v, hull[2], hull[6], hull[10], hull[14], tC[2]);
		deCasteljau_P(v, hull[3], hull[7], hull[11], hull[15], tC[3]);
		for (ug=0, ug4=0, u=0.f; ug<=xdim; ++ug, ug4+=4, u+=du, ++idx) {
			deCasteljau_P_dPdv(v, utan + ug4, P_grid[idx], dPdv_grid[idx]);
			deCasteljau_dPdu(u, tC, dPdu_grid[idx]);
		}
	}
	delete[] utan;

	// dice other primvars, if any
	g.initVars(std_dice);
	Primitive::linear_dice(g);

	/*
	// old dice code, P only, using forward differences
	const int xdim = g.get_xdim(), ydim = g.get_ydim();
	FloatGrid* P_grid = g.addVariable("P");
	Point3 fd[16];
	calcFD(hull, xdim+1, ydim+1, fd);
	Point3 P = fd[0], d1 = fd[4], d2 = fd[8], d3 = fd[12];
	int idx = 0;
	for (int vg=0; vg<=ydim; ++vg) {
		for (int ug=0; ug<=xdim; ++ug) {
			P_grid->setPoint3(P, idx++);
			P += d1, d1 += d2, d2 += d3;
		}
		P = fd[0]   += fd[1],   fd[1]  += fd[2],   fd[2]  += fd[3];
		d1 = fd[4]  += fd[5],   fd[5]  += fd[6],   fd[6]  += fd[7];
		d2 = fd[8]  += fd[9],   fd[9]  += fd[10],  fd[10] += fd[11];
		d3 = fd[12] += fd[13],  fd[13] += fd[14],  fd[14] += fd[15];
	}
	*/
}

//------------------------------------------------------------------------------
// BicubicPatchMesh
// only splits the patchmesh into its individual patches, dice() is never called

BicubicPatchMesh::BicubicPatchMesh(RtInt nu, bool uperiodic, RtInt nv, bool vperiodic,
                                   RtInt n, RtToken tokens[], RtPointer parms[])
{
	const int numpts = nu*nv;
	pts = NULL;
	for (int i=0; i<n; ++i) {
		if (!strcmp(tokens[i], RI_P)) {
			pts = new RtFloat[numpts*3];
			memcpy(pts, parms[i], sizeof(RtFloat)*numpts*3);
			mtype = 0;
		}
		else if (!strcmp(tokens[i], RI_PZ)) {
			pts = new RtFloat[numpts];
			memcpy(pts, parms[i], sizeof(RtFloat)*numpts);
			mtype = 1;
		}
		else if (!strcmp(tokens[i], RI_PW)) {
			pts = new RtFloat[numpts*4];
			memcpy(pts, parms[i], sizeof(RtFloat)*numpts*4);
			mtype = 2;
		}
	}
	this->nu = nu;
	this->nv = nv;
	this->uperiodic = uperiodic;
	this->vperiodic = vperiodic;
	// have to get u/vstep from state, not known yet
	const Attributes&  attr = State::Instance()->topAttributes();
	ustep = attr.cubicBasisStep[0], vstep = attr.cubicBasisStep[1];
	nupatches = uperiodic ? (nu / ustep) : ((nu - 4) / ustep + 1);
	nvpatches = vperiodic ? (nv / vstep) : ((nv - 4) / vstep + 1);
	nuvarying = uperiodic ? nupatches : (nupatches + 1);
	nvvarying = vperiodic ? nvpatches : (nvpatches + 1);
	// du/dv are used to create the parametric u,v coords for the 'Pz' case
	du = 1.f/(nupatches * 3);
	dv = 1.f/(nvpatches * 3);
	Primitive::initPrimVars(n, tokens, parms, nupatches*nvpatches, nuvarying*nvvarying, nu*nv, nupatches*nvpatches*4);
}


BicubicPatchMesh::~BicubicPatchMesh()
{
	if (pts) { delete[] pts;  pts = NULL; }
}


Bound BicubicPatchMesh::bound()
{
	Bound b;
	int idx = 0;
	for (int vi=0; vi<nv; ++vi) {
		const float v = vi / float(nv - 1);
		for (int ui=0; ui<nu; ++ui, ++idx) {
			const float u = ui / float(nu - 1);
			if (mtype == 0) {
				const RtPoint* meshpts = reinterpret_cast<RtPoint*>(pts);
				b.include(meshpts[idx]);
			}
			else if (mtype == 1)
				b.include(Point3(u, v, pts[idx]));
			// hpoint TODO
		}
	}
	b.addEpsilon();
	b.transform(xform);
	return b;
}

// split a bicubic patchmesh into its individual bicubic patches
void BicubicPatchMesh::split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb)
{
	Point3 hull[16];
	int uni_idx = 0, vary_idx[4], vtx_idx[16];
	for (int vp=0; vp<nvpatches; ++vp) {
		const int vstart = vp*vstep;
		for (int up=0; up<nupatches; ++up, ++uni_idx) {
			const int ustart = up*ustep;
			// extract patch
			Point3* curp = hull;
			for (int vi=0; vi<4; ++vi) {
				const int vidx = (vstart + vi) % nv;
				for (int ui=0; ui<4; ++ui, ++curp) {
					const int idx = ((ustart + ui) % nu) + vidx*nu;
					vtx_idx[ui + vi*4] = idx;
					if (mtype == 0) {
						const RtPoint* meshpts = reinterpret_cast<RtPoint*>(pts);
						curp->set(meshpts[idx][0], meshpts[idx][1], meshpts[idx][2]);
					}
					else if (mtype == 1)
						curp->set((ui + up*3)*du, (vi + vp*3)*dv, pts[idx]);
					// hpoint TODO
				}
			}

			// new varying indices
			for (int vi=0; vi<2; ++vi) {
				const int pvidx = (vp + vi) % nvvarying;
				for (int ui=0; ui<2; ++ui)
					vary_idx[ui + vi*2] = ((up + ui) % nuvarying) + pvidx*nuvarying;
			}

			BicubicPatch* bp = new BicubicPatch(*this, hull);
			splitPrimVars(this, bp, uni_idx, vary_idx, vtx_idx);
			bp->post_init();
			f.insert(bp);
		}
	}
}

// does nothing, print msg in case it is called, should never happen though...
void BicubicPatchMesh::dice(MicroPolygonGrid &g, bool Pclose)
{
	printf("[ERROR]: BicubicPatchMesh()->dice() called?\n");
}

__END_QDRENDER

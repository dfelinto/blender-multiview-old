///----------------------------------------------------------------
/// Points (particles)
///----------------------------------------------------------------
/// Points now are a single mp vertex, see MicroPolygon.h
/// Can render +/-4 times more particles than old code.
/// Point grids are split by projecting points to raster,
/// determining a split axis from bound.
/// Old code used an intermediate buffer to do the projection
/// only once. This is now done by doing projection where needed.
/// Doesn't improve anything though, neither memory usage or speed.
/// (At least it doesn't seem to make things worse)
/// So it might be more convenient to change things back to do the
/// projection only once.

#include "Points.h"
#include "Mathutil.h"
#include "Transform.h"
#include "qdVector.h"
#include "Bound.h"
#include "Camera.h"
#include "Framework.h"
#include "MicroPolygonGrid.h"
#include "Hider.h"
#include "State.h"

__BEGIN_QDRENDER

//PointSet::PointSet(RtInt nverts, RtInt n, RtToken tokens[], RtPointer parms[])
//	: P(NULL), pp(NULL), vary_width(NULL), totverts(nverts), const_width(0), split_axis(0)
PointSet::PointSet(RtInt nverts, RtInt n, RtToken tokens[], RtPointer parms[])
	: P(NULL), vary_width(NULL), totverts(nverts), const_width(0), split_axis(0)
{
	// get P
	for (int i=0; i<n; ++i) {
		if (!strcmp(tokens[i], RI_P)) {
			P = new RtPoint[totverts];
			memcpy(P, parms[i], sizeof(RtPoint)*totverts);
			break;
		}
	}
	bool has_width = false;
	// others
	Primitive::initPrimVars(n, tokens, parms, 1, totverts, totverts, totverts);
	if (primvars) {
		vardata_t** vdt = primvars->pvars.first();
		while (vdt)  {
			const char* name = primvars->pvars.getName();
			if (!strcmp(name, RI_WIDTH)) {
				vary_width = (*vdt)->data;
				has_width = true;
			}
			else if (!strcmp(name, RI_CONSTANTWIDTH)) {
				const_width = (*vdt)->data[0];
				has_width = true;
			}
			vdt = primvars->pvars.next();
		}
	}
	if (!has_width) const_width = 1.f;

	// NOTE: 'width' var can not be removed in this case! data directly referenced!
	// only 'constantwidth' is safe to remove
	Primitive::removePrimVar("constantwidth");
}

PointSet::~PointSet()
{
	//if (pp) { delete[] pp; pp = NULL; }
	if (P) { delete[] P;  P = NULL; }
}

void PointSet::post_init()
{
	// transform to camspace
	const RtMatrix* ri_mtx = xform->getRtMatrixPtr();
	for (unsigned int i=0; i<totverts; ++i)
		mulPMP(P[i], *ri_mtx, P[i]);
}

Bound PointSet::bound()
{
	Bound b;
	if (vary_width) {
		for (unsigned int i=0; i<totverts; ++i) {
			const float hw = 0.5f*vary_width[i];
			b.include(Point3(P[i][0]-hw, P[i][1]-hw, P[i][2]-hw));
			b.include(Point3(P[i][0]+hw, P[i][1]+hw, P[i][2]+hw));
		}
	}
	else {
		for (unsigned int i=0; i<totverts; ++i)
			b.include(Point3(P[i][0], P[i][1], P[i][2]));
		const float hw = 0.5f*const_width;
		b.minmax[0] -= Point3(hw);
		b.minmax[1] += Point3(hw);
	}
	b.addEpsilon();
	// no xform, already in camspace
	return b;
}

// see split() for explanation of 'halfnum'
void PointSet::splitPrimVars(Primitive* prim1, Primitive* prim2, int tv1, int tv2, int axis_idx, unsigned int halfnum)
{
	if (primvars == NULL) return;
	Camera& cam = State::Instance()->projcam;
	PrimVars *npv1 = NULL, *npv2 = NULL;
	sklist_t<vardata_t*>& varlist = primvars->pvars;
	vardata_t** vdt = varlist.first();
	while (vdt) {
		const char* name = varlist.getName();
		if ((!strcmp(name, RI_WIDTH)) || (!strcmp(name, RI_CONSTANTWIDTH))) {
			// width vars already handled in split()
			vdt = varlist.next();
			continue;
		}
		if (vdt) {
			if (npv1 == NULL) npv1 = prim1->newPrimVars();
			if (npv2 == NULL) npv2 = prim2->newPrimVars();
			decParam_t& dp = (*vdt)->param;
			vardata_t *nvdt1 = new vardata_t(dp), *nvdt2 = new vardata_t(dp);
			if (dp.ct_flags & (SC_CONSTANT | SC_UNIFORM)) {
				nvdt1->data = new float[dp.numfloats];
				memcpy(nvdt1->data, (*vdt)->data, sizeof(float)*dp.numfloats);
				nvdt2->data = new float[dp.numfloats];
				memcpy(nvdt2->data, (*vdt)->data, sizeof(float)*dp.numfloats);
			}
			else if (dp.ct_flags & (SC_VARYING | SC_VERTEX)) {
				if (dp.ct_flags & DT_FLOAT) {
					float *nfa1 = new float[tv1], *nfa2 = new float[tv2], *ofa = (*vdt)->data;
					int c1 = 0, c2 = 0;
					for (unsigned int i=0; i<totverts; ++i) {
						const Point3 pp = cam.project(Point3(P[i][0], P[i][1], P[i][2]));
						if (halfnum ? (i < halfnum) : (pp[axis_idx] < split_axis))
							nfa1[c1++] = ofa[i];
						else
							nfa2[c2++] = ofa[i];
					}
					nvdt1->data = nfa1;
					nvdt2->data = nfa2;
				}
				else if (dp.ct_flags & DT_FLOAT3MASK) {
					// pretend vector, doesn't matter, all are 3-float arrays
					RtVector *nva1 = new RtVector[tv1], *nva2 = new RtVector[tv2], *pva = reinterpret_cast<RtVector*>((*vdt)->data);
					int c1 = 0, c2 = 0;
					for (unsigned int i=0; i<totverts; ++i) {
						const Point3 pp = cam.project(Point3(P[i][0], P[i][1], P[i][2]));
						if (halfnum ? (i < halfnum) : (pp[axis_idx] < split_axis)) {
							nva1[c1][0] = pva[i][0], nva1[c1][1] = pva[i][1], nva1[c1][2] = pva[i][2];
							c1++;
						}
						else {
							nva2[c2][0] = pva[i][0], nva2[c2][1] = pva[i][1], nva2[c2][2] = pva[i][2];
							c2++;
						}
					}
					nvdt1->data = reinterpret_cast<float*>(nva1);
					nvdt2->data = reinterpret_cast<float*>(nva2);
				}
				else if (dp.ct_flags & DT_MATRIX) {
					RtMatrix *nma1 = new RtMatrix[tv1], *nma2 = new RtMatrix[tv2], *oma = reinterpret_cast<RtMatrix*>((*vdt)->data);
					int c1 = 0, c2 = 0;
					for (unsigned int i=0; i<totverts; ++i) {
						const Point3 pp = cam.project(Point3(P[i][0], P[i][1], P[i][2]));
						if (halfnum ? (i < halfnum) : (pp[axis_idx] < split_axis))
							memcpy(nma1[c1++], oma[i], sizeof(RtMatrix));
						else
							memcpy(nma2[c2++], oma[i], sizeof(RtMatrix));
					}
					nvdt1->data = reinterpret_cast<float*>(nma1);
					nvdt2->data = reinterpret_cast<float*>(nma2);
				}
			}
			npv1->pvars.insert(name, nvdt1);
			npv2->pvars.insert(name, nvdt2);
			vdt = varlist.next();
		}
	}
}

void PointSet::calcSplitAxis(bool &usplit, bool &vsplit)
{
	Camera& cam = State::Instance()->projcam;
	//float xmin=1e10f, ymin=1e10f, xmax=-1e10f, ymax=-1e10f;
	float gx = 0.f, gy = 0.f;
	//if (pp) delete[] pp;
	//pp = new Point3[totverts];
	for (unsigned int i=0; i<totverts; ++i) {
		const Point3 pp = cam.project(Point3(P[i][0], P[i][1], P[i][2]));
		gx += pp.x, gy += pp.y;
		//xmin = MIN2(xmin, pp.x), ymin = MIN2(ymin, pp.y);
		//xmax = MAX2(xmax, pp.x), ymax = MAX2(ymax, pp.y);
	}
	gx /= (float)totverts;
	gy /= (float)totverts;
	// could use longest bound axis as split axis,
	// but calculating distribution variance to decide which axis to use is a tiny bit more optimal
	float varx = 0, vary = 0;
	for (unsigned int i=0; i<totverts; ++i) {
		const Point3 pp = cam.project(Point3(P[i][0], P[i][1], P[i][2]));
		float t = pp.x - gx;
		varx += t*t;
		t = pp.y - gy;
		vary += t*t;
	}
	//if ((xmax - xmin) > (ymax - ymin)) {
	if (varx > vary) {
		split_axis = gx;
		usplit = true;
		vsplit = false;
	}
	else {
		split_axis = gy;
		usplit = false;
		vsplit = true;
	}
}

// NOTE: This does not work properly when split() called from framework (both u/v split and has no split_axis set) TODO
void PointSet::split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb)
{
	Camera& cam = State::Instance()->projcam;
	if (usplit and vsplit) {
		// both usplit & vsplit, only happens when called from framework,
		// calculate a new splitaxis here, this will split only along one axis though
		usplit = vsplit = false;
		calcSplitAxis(usplit, vsplit);
	}
	if (usplit) {
		PointSet *psL = new PointSet(), *psR = new PointSet();
		static_cast<Primitive&>(*psL) = static_cast<Primitive&>(*this);	// copy data
		static_cast<Primitive&>(*psR) = static_cast<Primitive&>(*this);	// copy data
		unsigned int tvL = 0, tvR = 0;
		for (unsigned int i=0; i<totverts; ++i){
			const Point3 pp = cam.project(Point3(P[i][0], P[i][1], P[i][2]));
			if (pp.x < split_axis) tvL++; else tvR++;
		}
		// it is possible that a set of points occupies the same position,
		// which means that all points will end up in one box,
		// if that happens, just put half in each box
		unsigned int halfnum = 0;
		if ((tvL == 0) or (tvR == 0)) {
			const unsigned int numpts = tvL + tvR;
			tvL = tvR = numpts >> 1;
			if (numpts & 1) tvL++;
			halfnum = tvL;
		}
		psL->P = new RtPoint[tvL], psR->P = new RtPoint[tvR];
		psL->totverts = tvL, psR->totverts = tvR;
		if (vary_width)
			psL->vary_width = new float[tvL], psR->vary_width = new float[tvR];
		else
			psL->vary_width = psR->vary_width = NULL;
		psL->const_width = psR->const_width = const_width;
		tvL = 0, tvR = 0;
		for (unsigned int i=0; i<totverts; ++i) {
			const Point3 pp = cam.project(Point3(P[i][0], P[i][1], P[i][2]));
			if (halfnum ? (i < halfnum) : (pp.x < split_axis)) {
				psL->P[tvL][0] = P[i][0], psL->P[tvL][1] = P[i][1], psL->P[tvL][2] = P[i][2];
				if (vary_width) psL->vary_width[tvL] = vary_width[i];
				tvL++;
			}
			else {
				psR->P[tvR][0] = P[i][0], psR->P[tvR][1] = P[i][1], psR->P[tvR][2] = P[i][2];
				if (vary_width) psR->vary_width[tvR] = vary_width[i];
				tvR++;
			}
		}
		splitPrimVars(psL, psR, tvL, tvR, 0, halfnum);
		if (spb) {	// motion blurred points
			if (spb->bprims.empty()) {
				spb->bprims.resize(2);
				spb->bprims[0] = new BlurredPrimitive(*spb->parent);
				spb->bprims[1] = new BlurredPrimitive(*spb->parent);
			}
			spb->bprims[0]->append(psL);
			spb->bprims[1]->append(psR);
		}
		else
			f.insert(psL), f.insert(psR);
	}
	else {
		PointSet *psB = new PointSet(), *psT = new PointSet();
		static_cast<Primitive&>(*psB) = static_cast<Primitive&>(*this);	// copy data
		static_cast<Primitive&>(*psT) = static_cast<Primitive&>(*this);	// copy data
		unsigned int tvB = 0, tvT = 0;
		for (unsigned int i=0; i<totverts; ++i) {
			const Point3 pp = cam.project(Point3(P[i][0], P[i][1], P[i][2]));
			if (pp.y < split_axis) tvB++; else tvT++;
		}
		// see comment above
		unsigned int halfnum = 0;
		if ((tvB == 0) or (tvT == 0)) {
			const unsigned int numpts = tvB + tvT;
			tvB = tvT = numpts >> 1;
			if (numpts & 1) tvB++;
			halfnum = tvB;
		}
		psB->P = new RtPoint[tvB], psT->P = new RtPoint[tvT];
		psB->totverts = tvB, psT->totverts = tvT;
		if (vary_width)
			psB->vary_width = new float[tvB], psT->vary_width = new float[tvT];
		else
			psB->vary_width = psT->vary_width = NULL;
		psB->const_width = psT->const_width = const_width;
		tvB = 0, tvT = 0;
		for (unsigned int i=0; i<totverts; ++i) {
			const Point3 pp = cam.project(Point3(P[i][0], P[i][1], P[i][2]));
			if (halfnum ? (i < halfnum) : (pp.y < split_axis)) {
				psB->P[tvB][0] = P[i][0], psB->P[tvB][1] = P[i][1], psB->P[tvB][2] = P[i][2];
				if (vary_width) psB->vary_width[tvB] = vary_width[i];
				tvB++;
			}
			else {
				psT->P[tvT][0] = P[i][0], psT->P[tvT][1] = P[i][1], psT->P[tvT][2] = P[i][2];
				if (vary_width) psT->vary_width[tvT] = vary_width[i];
				tvT++;
			}
		}
		splitPrimVars(psB, psT, tvB, tvT, 1, halfnum);
		if (spb) {	// motion blurred points
			if (spb->bprims.empty()) {
				spb->bprims.resize(2);
				spb->bprims[0] = new BlurredPrimitive(*spb->parent);
				spb->bprims[1] = new BlurredPrimitive(*spb->parent);
			}
			spb->bprims[0]->append(psB);
			spb->bprims[1]->append(psT);
		}
		else
			f.insert(psB), f.insert(psT);
	}
}

#ifdef _MSC_VER 
typedef unsigned __int64 uint64;
#else
typedef unsigned long uint64;
#endif
bool PointSet::diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit)
{
	// dimensions don't matter in this case
	g.setDim(totverts-1, 0, this);
	if (((uint64)totverts*totverts) < (uint64)h.gridSize()) return true;
	//if (totverts < (uint64)h.gridSize()) return true;
	calcSplitAxis(usplit, vsplit);
	return false;
}

void PointSet::dice(MicroPolygonGrid &g, bool Pclose)
{
	// has no tangents, only need to copy points/width to grid in this case
	g.gtype = MicroPolygonGrid::G_POINTS;
	RtPoint* P_grid = (RtPoint*)(Pclose ? g.addVariable("=Pclose") : g.addVariable("P"));
	memcpy(P_grid, P, sizeof(RtPoint)*totverts);
	if (vary_width)
		memcpy(g.addVariable("width"), vary_width, sizeof(float)*totverts);
	else {
		float* width_grid = g.addVariable("width");
		for (unsigned int i=0; i<totverts; ++i)
			width_grid[i] = const_width;
	}
	// other variables
	g.initVars(std_dice);
	// linear_dice() not called here, since no interpolation is necesary
	if (primvars) {
		vardata_t** vdt = primvars->pvars.first();
		if (vdt == NULL) return; // nothing in list
		bool varying = (((*vdt)->param.ct_flags & (SC_VARYING | SC_VERTEX)) != 0);
		while (vdt) {
			char* name = primvars->pvars.getName();
			if ((*vdt)->param.ct_flags & DT_FLOAT) {
				RtFloat* Fgrid = g.findVariable(name);
				if (Fgrid) {
					const RtFloat* da = (RtFloat*)(*vdt)->data;
					if (varying)
						memcpy(Fgrid, da, sizeof(float)*totverts);
					else { // uniform/constant
						for (unsigned int i=0; i<totverts; ++i)
							Fgrid[i] = da[0];
					}
				}
			}
			else if ((*vdt)->param.ct_flags & DT_FLOAT3MASK) {
				// point/vector/normal/color
				RtVector* Vgrid = (RtVector*)g.findVariable(name);
				if (Vgrid) {
					const RtVector* da = (RtVector*)(*vdt)->data;
					if (varying)
						memcpy(Vgrid, da, sizeof(RtVector)*totverts);
					else { // uniform/constant
						for (unsigned int i=0; i<totverts; ++i)
							Vgrid[i][0] = da[0][0], Vgrid[i][1] = da[0][1], Vgrid[i][2] = da[0][2];
					}
				}
			}
			// matrix/hpoint todo
			// next variable
			vdt = primvars->pvars.next();
		}
	}
}

__END_QDRENDER

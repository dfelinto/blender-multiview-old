//------------------------------------------------------------------------------
// Base class derived from Primitive for Parametric objects
//------------------------------------------------------------------------------

#include "Parametric.h"
#include "Hider.h"
#include "Bound.h"
#include "Framework.h"
#include "MicroPolygonGrid.h"
#include "Mathutil.h"
#include "Transform.h"
#include "Attributes.h"

#include "QDRender.h"
__BEGIN_QDRENDER

Parametric::Parametric():umin(0), umax(1), vmin(0), vmax(1)
{
}

Parametric::Parametric(const Parametric& p) : Primitive()
{
	static_cast<Primitive&>(*this).operator=(p);
	umin = p.umin;
	umax = p.umax;
	vmin = p.vmin;
	vmax = p.vmax;
}

Parametric::~Parametric()
{
}

void Parametric::split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb)
{
	if (usplit && vsplit) {
		// if both axes split, result is four new prims
		// (four way split is only done when prim overlaps near clip, see Framework insert())
		Parametric *BL = duplicateSelf(), *BR = duplicateSelf();
		Parametric *TL = duplicateSelf(), *TR = duplicateSelf();
		const float umid = (umin + umax)*0.5f;
		const float vmid = (vmin + vmax)*0.5f;
		BL->umax = BR->umin = TL->umax = TR->umin = umid;
		BL->vmax = BR->vmax = TL->vmin = TR->vmin = vmid;
		if (spb) {
			if (spb->bprims.empty()) {
				spb->bprims.resize(4);
				spb->bprims[0] = new BlurredPrimitive(*spb->parent);
				spb->bprims[1] = new BlurredPrimitive(*spb->parent);
				spb->bprims[2] = new BlurredPrimitive(*spb->parent);
				spb->bprims[3] = new BlurredPrimitive(*spb->parent);
			}
			spb->bprims[0]->append(BL), spb->bprims[1]->append(BR), spb->bprims[2]->append(TL), spb->bprims[3]->append(TR);
		}
		else
			f.insert(BL), f.insert(BR), f.insert(TL), f.insert(TR);
	}
	else {
		if (usplit) {
			Parametric *L = duplicateSelf(), *R = duplicateSelf();
			const float umid = (umin + umax)*0.5f;
			L->umax = umid;
			R->umin = umid;
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
		else if (vsplit) {
			Parametric *T = duplicateSelf(), *B = duplicateSelf();
			const float vmid = (vmin + vmax)*0.5f;
			T->vmax = vmid;
			B->vmin = vmid;
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


bool Parametric::diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit)
{
	// TODO: make TESTSIZE user option
	#define TESTSIZE 8

	static RtPoint P_grid[(TESTSIZE+1)*(TESTSIZE+1)];
	float du = 1.f/(float)TESTSIZE, dv = 1.f/(float)TESTSIZE;
	float u, v;
	const float ud = umax - umin, vd = vmax - vmin;
	int ug, vg, idx = 0;
	for (vg=0, v=0.f; vg<=TESTSIZE; ++vg, v+=dv)
		for (ug=0, u=0.f; ug<=TESTSIZE; ++ug, u+=du, ++idx) {
			eval(umin + u*ud, vmin + v*vd, P_grid[idx]);
			if (!in_camspace()) mulPMP(P_grid[idx], *xform->getRtMatrixPtr(), P_grid[idx]);
		}

	float maxUDist=-1e10f, maxVDist=-1e10f;
	for (int vg=0; vg<TESTSIZE; ++vg) {
		const int Tvg = (TESTSIZE+1)*vg;
		for (int ug=0; ug<TESTSIZE; ++ug) {
			const int ugTvg = ug + Tvg;
			Point3 p1(P_grid[ugTvg][0], P_grid[ugTvg][1], P_grid[ugTvg][2]);
			maxUDist = MAX2(h.rasterEstimate(p1, Point3(P_grid[ugTvg + 1][0], P_grid[ugTvg + 1][1], P_grid[ugTvg + 1][2])), maxUDist);
			maxVDist = MAX2(h.rasterEstimate(p1, Point3(P_grid[ugTvg + (TESTSIZE+1)][0], P_grid[ugTvg + (TESTSIZE+1)][1], P_grid[ugTvg + (TESTSIZE+1)][2])), maxVDist);
		}
	}

	const Attributes* attr = getAttributeReference();
	// 2x2 mp minimum
	unsigned int xdim = (attr->flags & AF_DICE_BINARY) ? (1 << MAX2(1, int(0.5f + (float)log(MAX2(1e-7f, TESTSIZE*maxUDist) / attr->effectiveShadingRate)*(float)M_LOG2E)))
	                                                   : MAX2(2, int(0.5f + (TESTSIZE*maxUDist) / attr->effectiveShadingRate));
	unsigned int ydim = (attr->flags & AF_DICE_BINARY) ? (1 << MAX2(1, int(0.5f + (float)log(MAX2(1e-7f, TESTSIZE*maxVDist) / attr->effectiveShadingRate)*(float)M_LOG2E)))
	                                                   : MAX2(2, int(0.5f + (TESTSIZE*maxVDist) / attr->effectiveShadingRate));

	g.setDim(xdim, ydim, this);
	if ((xdim<h.gridSize()) && (ydim<h.gridSize()) && ((xdim*ydim)<h.gridSize())) return true;
	usplit = (xdim > ydim);
	vsplit = !usplit;
	return false;
	#undef TESTSIZE
}


// changed this and eval() to more low level and more C like code, not really faster though...
void Parametric::dice(MicroPolygonGrid &g, bool Pclose)
{
	const int xdim=g.get_xdim(), ydim=g.get_ydim();
	// NOTE don't mult. du/dv by min/max range, causes precision errors
	const float du = 1.f/(float)xdim, dv = 1.f/(float)ydim;
	int ug, vg, idx = 0;
	float u, v;

	// Dice P, dPdu & dPdv
	// '=Pclose' -> motion blurred grid at shutter close time, '=' symbol not used in shader code, so no overwrites (look, it's moving! ;)
	RtPoint* P_grid = (RtPoint*)(Pclose ? g.addVariable("=Pclose") : g.addVariable("P"));
	// for the moving case, all other variables will be diced multiple times, overwriting the previous... not good... optimize TODO
	// (of course, if 'real' motion blur should be done, then everything should be diced and saved, and that data interpolated too)
	RtVector *dPdu_grid = (RtVector*)g.addVariable("dPdu"), *dPdv_grid = (RtVector*)g.addVariable("dPdv");
	const float ud = umax - umin, vd = vmax - vmin;
	for (vg=0, v=0.f; vg<=ydim; ++vg, v+=dv)
		for (ug=0, u=0.f; ug<=xdim; ++ug, u+=du, ++idx) {
			eval(umin + u*ud, vmin + v*vd, P_grid[idx], dPdu_grid[idx], dPdv_grid[idx]);
			if (!in_camspace()) mulPMP(P_grid[idx], *xform->getRtMatrixPtr(), P_grid[idx]);
		}

	// dice other primvars, if any
	g.initVars(std_dice);
	Primitive::linear_dice(g);
}

__END_QDRENDER


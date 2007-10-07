///--------------------------------------------------------------------------------------
/// Non-Uniform Rational B-Splines, NuPatches
///--------------------------------------------------------------------------------------
///
/// Nurbs evaluation funcs are an adaptation of 'nurb_polyg' code from GraphicsGemsIV.
///
/// Currently avoids actual splitting and just adjusts clipping parameters.
/// Bound calculation is probably also not quite correct, currently done by evaluating
/// the control points of the patch.

#include "Nurbs.h"
#include "Bound.h"
#include "Transform.h"
#include "Mathutil.h"
#include "Framework.h"
#include "Hider.h"
#include "MicroPolygonGrid.h"
#include "Attributes.h"

__BEGIN_QDRENDER

//------------------------------------------------------------------------------

/*
 * Return the current knot the parameter u is less than or equal to.
 * Find this "breakpoint" allows the evaluation routines to concentrate on
 * only those control points actually effecting the curve around u.]
 *
 *	m   is the number of points on the curve (or surface direction)
 *	k   is the order of the curve (or surface direction)
 *	kv  is the knot vector ([0..m+k-1]) to find the break point in.
 */
inline int FindBreakPoint(float u, const FixedArray<float>& kv, int m, int k)
{
	if (u == kv[m + 1]) return m;	// Special case for closed interval
	/*
	int i = m + k;
	while ((u < kv[i]) && (i > 0)) i--;
	return i;
	*/
	// do bin.search instead of above linear, might be faster (for larger arrays at least)
	int lo = 0, hi = m+k;
	while (lo != hi) {
		const int md = (hi + lo) >> 1;
		if (u >= kv[md]) lo = md + 1; else hi = md;
	}
	return ((hi == 0) ? 0 : (hi - 1));
	
}

/*
 * Compute Bi,k(u), for i = 0..k.
 *  u		is the parameter of the spline to find the basis functions for
 *  brkPoint	is the start of the knot interval ("segment")
 *  kv		is the knot vector
 *  k		is the order of the curve
 *  bvals	is the array of returned basis values.
 *
 * (From Bartels, Beatty & Barsky, p.387)
 */
inline void BasisFunctions(float u, int brkPoint, const FixedArray<float>& kv, int k, FixedArray<float>& bvals)
{
	bvals[0] = 1.f;
	for (int r=2; r<=k; ++r) {
		bvals[r - 1] = 0.f;
		for (int s=(r-2), i=(brkPoint-r+2); s>=0; --s, ++i) {
			const float dv = kv[i + r - 1] - kv[i];
			const float omega = (i < 0) ? 0.f : ((u - kv[i]) / ((dv==0.f) ? 1.f : dv));
			bvals[s + 1] += (1 - omega)*bvals[s];
			bvals[s] *= omega;
		}
	}
}

// Compute derivatives of the basis functions Bi,k(u)'
inline void BasisDerivatives(float u, int brkPoint, const FixedArray<float>& kv, int k, FixedArray<float>& dvals)
{
	BasisFunctions(u, brkPoint, kv, k-1, dvals);
	dvals[k-1] = 0.f;	    // BasisFunctions misses this
	const float knotScale = kv[brkPoint + 1] - kv[brkPoint];
	for (int s=(k-2), i=(brkPoint-k+2); s>=0; --s, ++i) {
		const float dv = kv[i + k - 1] - kv[i];
		const float omega = knotScale*(k - 1)/((dv==0.f) ? 1.f : dv);
		dvals[s + 1] += -omega*dvals[s];
		dvals[s] *= omega;
	}
}

/*
 * Calculate a point p on NurbSurface n at a specific u, v using the tensor product.
 * If utan and vtan are not nil, compute the u and v tangents as well.
 *
 * Note the valid parameter range for u and v is
 * (kvU[orderU] <= u < kvU[numU), (kvV[orderV] <= v < kvV[numV])
 */
void NuPatch::eval(float u, float v, RtPoint P, RtVector dPdu, RtVector dPdv)
{
	// clamp u & v to make sure it stays within range (precision errors cause havoc)
	u = CLAMP(u, umin, umax);
	v = CLAMP(v, vmin, vmax);
	// Evaluate non-uniform basis functions (and derivatives)
	const int ubrkPoint = FindBreakPoint(u, nudata->uknot, nudata->nu-1, nudata->uorder);
	const int ufirst = ubrkPoint - nudata->uorder + 1;
	BasisFunctions(u, ubrkPoint, nudata->uknot, nudata->uorder, nudata->bu);
	if (dPdu) BasisDerivatives(u, ubrkPoint, nudata->uknot, nudata->uorder, nudata->buprime);

	const int vbrkPoint = FindBreakPoint(v, nudata->vknot, nudata->nv-1, nudata->vorder);
	const int vfirst = vbrkPoint - nudata->vorder + 1;
	BasisFunctions(v, vbrkPoint, nudata->vknot, nudata->vorder, nudata->bv);
	if (dPdv) BasisDerivatives(v, vbrkPoint, nudata->vknot, nudata->vorder, nudata->bvprime);

	// Weight control points against the basis functions
	RtHpoint r = {0,0,0,0}, rutan = {0,0,0,0}, rvtan = {0,0,0,0};
	for (int i=0; i<nudata->vorder; ++i) {
		RtHpoint* pw = &nudata->Pw[(i+vfirst)*nudata->nu + ufirst];
		for (int j=0; j<nudata->uorder; ++j) {
			const int ri = nudata->vorder - 1 - i, rj = nudata->uorder - 1 - j;
			float tmp = nudata->bu[rj] * nudata->bv[ri];
			r[0] += pw[j][0] * tmp;
			r[1] += pw[j][1] * tmp;
			r[2] += pw[j][2] * tmp;
			r[3] += pw[j][3] * tmp;
			if (dPdu) {
				tmp = nudata->buprime[rj] * nudata->bv[ri];
				rutan[0] += pw[j][0] * tmp;
				rutan[1] += pw[j][1] * tmp;
				rutan[2] += pw[j][2] * tmp;
				rutan[3] += pw[j][3] * tmp;
			}
			if (dPdv) {
				tmp = nudata->bu[rj] * nudata->bvprime[ri];
				rvtan[0] += pw[j][0] * tmp;
				rvtan[1] += pw[j][1] * tmp;
				rvtan[2] += pw[j][2] * tmp;
				rvtan[3] += pw[j][3] * tmp;
			}
		}
	}

	// Project tangents, using the quotient rule for differentiation
	const float dv = (r[3]==0.f) ? 1.f : (1.f/r[3]);
	const float wsqrdiv = dv*dv;
	if (dPdu && dPdv) {
		dPdu[0] = (r[3]*rutan[0] - rutan[3]*r[0])*wsqrdiv;
		dPdu[1] = (r[3]*rutan[1] - rutan[3]*r[1])*wsqrdiv;
		dPdu[2] = (r[3]*rutan[2] - rutan[3]*r[2])*wsqrdiv;
		dPdv[0] = (r[3]*rvtan[0] - rvtan[3]*r[0])*wsqrdiv;
		dPdv[1] = (r[3]*rvtan[1] - rvtan[3]*r[1])*wsqrdiv;
		dPdv[2] = (r[3]*rvtan[2] - rvtan[3]*r[2])*wsqrdiv;
	}
	P[0] = r[0]*dv, P[1] = r[1]*dv, P[2] = r[2]*dv;
}

//--------------------------------------------------------------------------
// NuPatch

NuPatch::NuPatch(RtInt _nu, RtInt _uorder, RtFloat _uknot[], RtFloat _umin, RtFloat _umax,
                 RtInt _nv, RtInt _vorder, RtFloat _vknot[], RtFloat _vmin, RtFloat _vmax,
                 RtInt n, RtToken tokens[], RtPointer parms[])
                 : nudata(NULL), umin(_umin), vmin(_vmin), umax(_umax), vmax(_vmax)
{
	// all checking for correctness done in ri/ribInterface
	nudata = new NuData(_nu, _uorder, _uknot, _nv, _vorder, _vknot);
	const int nump = _nu * _nv;
	RtHpoint* Pw = nudata->Pw = new RtHpoint[nump];
	// get control points, P/Pw, in case of 'P', w = 1
	for (int i=0; i<n; ++i) {
		if (!strcmp(tokens[i], RI_P)) {
			const RtPoint* rP = reinterpret_cast<RtPoint*>(parms[i]);
			for (int j=0; j<nump; ++j)
				Pw[j][0] = rP[j][0], Pw[j][1] = rP[j][1], Pw[j][2] = rP[j][2], Pw[j][3] = 1.f;
			break;
		}
		else if (!strcmp(tokens[i], RI_PW)) {
			memcpy(Pw, parms[i], sizeof(RtHpoint)*nump);
			break;
		}
	}
	// dont' forget reference count in duplicateSelf (must do this differently similar to Parametric)
	const int nuseg = _nu - _uorder + 1, nvseg = _nv - _vorder + 1;
	Primitive::initPrimVars(n, tokens, parms, nuseg*nvseg, (nuseg+1)*(nvseg+1), nump, nuseg*nvseg*4);
}

NuPatch::~NuPatch()
{
	if (nudata && (--nudata->refc == 0)) {
		delete nudata;
		nudata = NULL;
	}
}

void NuPatch::post_init()
{
	// only done after initial prim insert in state
	RtHpoint* Pw = nudata->Pw;
	const RtMatrix* const ri_mtx = xform->getRtMatrixPtr();
	const int nump = nudata->nu * nudata->nv;
	for (int i=0; i<nump; ++i)
		mulPMP4(Pw[i], *ri_mtx, Pw[i]);
}

Bound NuPatch::bound()
{
	Bound b;
	// evaluate at cp.verts for bound.
	// NOTE while it seems to work sofar, it can't be right for all cases,
	// surface might have a 'bump' or 'dent' inbetween control points...
	const float du = (umax - umin)/(float)(nudata->nu - 1), dv = (vmax - vmin)/(float)(nudata->nv - 1);
	int ug, vg;
	float u, v;
	RtPoint P;
	for (vg=0, v=vmin; vg<nudata->nv; ++vg, v+=dv)
		for (ug=0, u=umin; ug<nudata->nu; ++ug, u+=du) {
			eval(u, v, P);
			b.include(Point3(P[0], P[1], P[2]));
		}
	b.addEpsilon();
	return b;
}

// splitting is done here simply by only adjusting the clipping coords...
void NuPatch::split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb)
{
	//printf("SPLITTING : %d %d\n", usplit, vsplit);
	if (usplit && vsplit) {
		// Left to bottom & top 1
		NuPatch* LT = static_cast<NuPatch*>(duplicateSelf());
		NuPatch *LB = static_cast<NuPatch*>(duplicateSelf());
		const float umid = (umin + umax)*0.5f;
		const float vmid = (vmin + vmax)*0.5f;
		LB->umax = LT->umax = umid;
		LB->vmax = LT->vmin = vmid;
		f.insert(LT);
		f.insert(LB);
		// Right to bottom & top 2
		NuPatch *RT = static_cast<NuPatch*>(duplicateSelf());
		NuPatch *RB = static_cast<NuPatch*>(duplicateSelf());
		RB->umin = RT->umin = umid;
		RB->vmax = RT->vmin = vmid;
		f.insert(RT);
		f.insert(RB);
	}
	else {
		if (usplit) {
			NuPatch *L = static_cast<NuPatch*>(duplicateSelf());
			NuPatch *R = static_cast<NuPatch*>(duplicateSelf());
			const float umid = (umin + umax)*0.5f;
			L->umax = umid, R->umin = umid;
			f.insert(L);
			f.insert(R);
		}
		else if (vsplit) {
			NuPatch *T = static_cast<NuPatch*>(duplicateSelf());
			NuPatch *B = static_cast<NuPatch*>(duplicateSelf());
			const float vmid = (vmin + vmax)*0.5f;
			T->vmax = vmid, B->vmin = vmid;
			f.insert(T);
			f.insert(B);
		}
	}
}

bool NuPatch::diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit)
{
	// TODO: make TESTSIZE user option
	#define TESTSIZE 8

	MicroPolygonGrid testGrid;
	testGrid.setDim(TESTSIZE, TESTSIZE, this);
	dice(testGrid);
	RtPoint* P_grid = (RtPoint*)testGrid.findVariable("P");
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
	unsigned int xdim = attr->dice_binary ? (1 << MAX2(1, int(0.5f + (float)log(MAX2(1e-7f, TESTSIZE*maxUDist) / attr->effectiveShadingRate)*(float)M_LOG2E)))
	                                      : MAX2(2, int(0.5f + (TESTSIZE*maxUDist) / attr->effectiveShadingRate));
	unsigned int ydim = attr->dice_binary ? (1 << MAX2(1, int(0.5f + (float)log(MAX2(1e-7f, TESTSIZE*maxVDist) / attr->effectiveShadingRate)*(float)M_LOG2E)))
	                                      : MAX2(2, int(0.5f + (TESTSIZE*maxVDist) / attr->effectiveShadingRate));

	g.setDim(xdim, ydim, this);
	if ((xdim<h.gridSize()) && (ydim<h.gridSize()) && ((xdim*ydim)<h.gridSize())) return true;
	usplit = (xdim > ydim);
	vsplit = !usplit;
	return false;
	#undef TESTSIZE
}

void NuPatch::dice(MicroPolygonGrid &g, bool Pclose)
{
	int ug, vg, idx = 0;
	float u, v;
	RtPoint* P_grid = (RtPoint*)(Pclose ? g.addVariable("=Pclose") : g.addVariable("P"));
	RtVector *dPdu_grid = (RtVector*)g.addVariable("dPdu"), *dPdv_grid = (RtVector*)g.addVariable("dPdv");
	RtFloat *u_grid = g.addVariable("u", 1), *v_grid = g.addVariable("v", 1);
	const int xdim = g.get_xdim(), ydim = g.get_ydim();

	// Dice P, dPdu & dPdv
	// note: du/dv adjusted for clipping,
	// this will result in minute precision errors that make u and/or v
	// less or greater than the valid u/v min/max range causing lots of chaos...
	// To prevent this, even though it happens less (or not at all) with just using 0-1 range, eval() clamps the values
	const float du = (umax - umin)/(float)xdim, dv = (vmax - vmin)/(float)ydim;
	for (vg=0, v=vmin; vg<=ydim; ++vg, v+=dv)
		for (ug=0, u=umin; ug<=xdim; ++ug, u+=du, ++idx) {
			eval(u, v, P_grid[idx], dPdu_grid[idx], dPdv_grid[idx]);
			u_grid[idx] = u, v_grid[idx] = v;
		}

	// does not deal with 'vertex' type yet (here does same as varying), TODO

	// dice other primvars, if any
	g.initVars(std_dice);
	Primitive::linear_dice(g);
}

Primitive* NuPatch::duplicateSelf()
{
	NuPatch* np = new NuPatch();
	memcpy(np, this, sizeof(NuPatch));
	if (nudata) nudata->refc++;	// after copying, not before!!!
	if (primvars) primvars->incRefCount();
	return np;
}

__END_QDRENDER

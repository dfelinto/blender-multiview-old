///------------------------------------------------------------------------------------------------------------
/// Linear & Cubic Curves
///------------------------------------------------------------------------------------------------------------
///
/// Again far from optimal, tried different things, the main problem being splitting.
/// Currently, an initial 'curveset' is split into individual curve segments all at once
/// as soon as splitting is required. Each curve segment is then diced as a partial grid, a strip of vertices.
/// An obvious idea is to try to use a full grid for a group of curves TODO.
///
/// Cubic curves are converted to bezier first.
/// (could omit in Blender, just bezier curves probably sufficient)
/// which are then split into their individual 4-point bezier segments.
/// These segments are handled as primitives themselves, which probably is not optimal.
/// Keeping them grouped as long as possible might be a better idea, or at least keep
/// the whole curve, and let split() deal with the individual segments.
/// I did this originally, but could not find a good way to properly dice them without
/// artifacts in all cases. But in Blender again, just the basic 4 point bezier curve is
/// really all that is required, so in that case everything should be a lot simpler.
///
/// Also nothing to do with Blender but another area for improvement are the way how some initial vars
/// (like the cp.verts) are handled. Currently they are copied from primvars, while it should be
/// modified to directly use them instead. Some earlier versions did this and used less memory,
/// but there were some other problems, can't remember what exactly. Must be made simpler though.
///
///------------------------------------------------------------------------------------------------------------

#include "Curves.h"
#include "Bound.h"
#include "Mathutil.h"
#include "Transform.h"
#include "MicroPolygonGrid.h"
#include "Framework.h"
#include "Hider.h"
#include "State.h"

__BEGIN_QDRENDER

//------------------------------------------------------------------------------
// helper function for cubic curves,
// similar to convert2Bezier() in patches.cpp, adapted for curves
void convert2Bezier_curve(RtPoint bezpts[4], const RtMatrix convmtx)
{
	RtFloat t0 = bezpts[0][0], t1 = bezpts[1][0], t2 = bezpts[2][0], t3 = bezpts[3][0];
	bezpts[0][0] = t0*convmtx[0][0] + t1*convmtx[0][1] + t2*convmtx[0][2] + t3*convmtx[0][3];
	bezpts[1][0] = t0*convmtx[1][0] + t1*convmtx[1][1] + t2*convmtx[1][2] + t3*convmtx[1][3];
	bezpts[2][0] = t0*convmtx[2][0] + t1*convmtx[2][1] + t2*convmtx[2][2] + t3*convmtx[2][3];
	bezpts[3][0] = t0*convmtx[3][0] + t1*convmtx[3][1] + t2*convmtx[3][2] + t3*convmtx[3][3];
	t0 = bezpts[0][1], t1 = bezpts[1][1], t2 = bezpts[2][1], t3 = bezpts[3][1];
	bezpts[0][1] = t0*convmtx[0][0] + t1*convmtx[0][1] + t2*convmtx[0][2] + t3*convmtx[0][3];
	bezpts[1][1] = t0*convmtx[1][0] + t1*convmtx[1][1] + t2*convmtx[1][2] + t3*convmtx[1][3];
	bezpts[2][1] = t0*convmtx[2][0] + t1*convmtx[2][1] + t2*convmtx[2][2] + t3*convmtx[2][3];
	bezpts[3][1] = t0*convmtx[3][0] + t1*convmtx[3][1] + t2*convmtx[3][2] + t3*convmtx[3][3];
	t0 = bezpts[0][2], t1 = bezpts[1][2], t2 = bezpts[2][2], t3 = bezpts[3][2];
	bezpts[0][2] = t0*convmtx[0][0] + t1*convmtx[0][1] + t2*convmtx[0][2] + t3*convmtx[0][3];
	bezpts[1][2] = t0*convmtx[1][0] + t1*convmtx[1][1] + t2*convmtx[1][2] + t3*convmtx[1][3];
	bezpts[2][2] = t0*convmtx[2][0] + t1*convmtx[2][1] + t2*convmtx[2][2] + t3*convmtx[2][3];
	bezpts[3][2] = t0*convmtx[3][0] + t1*convmtx[3][1] + t2*convmtx[3][2] + t3*convmtx[3][3];
}

// linear interpolation
inline void lerpf(RtFloat& r, float t, const RtFloat f0, const RtFloat f1)
{
	r = f0 + t*(f1 - f0);
}

inline void lerpv(RtPoint r, float t, const RtPoint p0, const RtPoint p1)
{
	r[0] = p0[0] + t*(p1[0] - p0[0]);
	r[1] = p0[1] + t*(p1[1] - p0[1]);
	r[2] = p0[2] + t*(p1[2] - p0[2]);
}

//------------------------------------------------------------------------------
// CurveSet

CurveSet::CurveSet(bool linear, int ncurves, int nvertices[], bool periodic,
									RtInt n, RtToken tokens[], RtPointer parms[])
								: islinear(linear), nverts(NULL), P(NULL), vary_width(NULL), vary_N(NULL),
									wrap(periodic), totcurves(ncurves), totverts(0), const_width(0)
{
	vstep = State::Instance()->topAttributes().cubicBasisStep[1];
	// total number of vertices = sum of points per curve in nvertices[]
	nverts = new RtInt[totcurves];
	totvary = 0;
	for (int i=0; i<totcurves; ++i) {
		totverts += nvertices[i];
		nverts[i] = nvertices[i];
		const int nseg = linear ? (periodic ? nverts[i] : (nverts[i] - 1)) :
															(periodic ? (nverts[i]/vstep) : ((nverts[i] - 4)/vstep + 1));
		totvary += (periodic ? nseg : (nseg + 1));
	}

	bool has_width = false;
	max_width = 0; // used for bounding

	// P
	for (int i=0; i<n; ++i) {
		if (!strcmp(tokens[i], RI_P)) {
			P = new RtPoint[totverts];
			memcpy(P, parms[i], sizeof(RtPoint)*totverts);
			break;
		}
	}

	// others
	initPrimVars(n, tokens, parms, totcurves, totvary, totverts, totvary*2);
	if (primvars) {
		vardata_t** vdt = primvars->pvars.firstItem();
		while (vdt)  {
			const char* name = primvars->pvars.getName();
			if (!strcmp(name, RI_WIDTH)) {
				vary_width = (*vdt)->data;
				// calc radius, while this works, it is not optimal,
				// should really do a per vertex radius calculation in bound()
				for (int j=0; j<totvary; ++j)
					max_width = MAX2(max_width, vary_width[j]);
				has_width = true;
			}
			else if (!strcmp(name, RI_CONSTANTWIDTH)) {
				const_width = (*vdt)->data[0];
				max_width = const_width;
				has_width = true;
			}
			else if (!strcmp(name, RI_N))
				vary_N = reinterpret_cast<RtNormal*>((*vdt)->data);
			vdt = primvars->pvars.nextItem();
		}
	}
	if (!has_width) max_width = const_width = 1.f;
	max_width *= 0.5f;

	// NOTE: 'width' var can not be removed in this case! data directly referenced!
	// only 'constantwidth' is safe to remove
	Primitive::removePrimVar("constantwidth");
}

CurveSet::~CurveSet()
{
	if (P) { delete[] P;  P = NULL; }
	if (nverts) { delete[] nverts;  nverts = NULL; }
}

void CurveSet::post_init()
{
	// transform to camspace
	// As for patches, this should really only be done *after* basis conversion,
	// but since this will probaby rarely cause visible errors, just ignore it...
	// (well actually, power and hermite will simply be wrong...
	//  but most other renderers seem to ignore it as well)
	const RtMatrix* ri_mtx = xform->getRtMatrixPtr();
	for (int i=0; i<totverts; ++i)
		mulPMP(P[i], *ri_mtx, P[i]);
}

Bound CurveSet::bound()
{
	// see comments in constructor above!
	Bound b;
	for (int i=0; i<totverts; ++i)
		b.include(Point3(P[i][0], P[i][1], P[i][2]));
	b.minmax[0] -= Point3(max_width);
	b.minmax[1] += Point3(max_width);
	b.addEpsilon();
	// no xform, already in camspace
	return b;
}

void CurveSet::split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb)
{
	RtMatrix convmtx; // bezier basis conversion
	if (!islinear) {
		const RtMatrix invBezier = {{0.f,     0.f,     0.f, 1.f},
		                            {0.f,     0.f, 1.f/3.f, 1.f},
		                            {0.f, 1.f/3.f, 2.f/3.f, 1.f},
		                            {1.f,     1.f,     1.f, 1.f}};
		mulMMM(convmtx, invBezier, *attr->cubicBasisMatrix[1]->getRtMatrixPtr());
	}
	int nvary, lastnv = 0, lastvary = 0, blurseg = 0;
	for (int i=0; i<totcurves; ++i) {
		if (islinear) {
			const int nseg = nverts[i] - (wrap ? 0 : 1);
			nvary = wrap ? nseg : (nseg + 1);
			float vmin = 0;
			for (int n=0; n<nseg; ++n) {
				LinearSegment* lns = new LinearSegment();
				static_cast<Primitive&>(*lns) = static_cast<Primitive&>(*this); // copy data
				for (int j=0; j<2; ++j) {
					const int k = lastnv + ((n + j) % nverts[i]);
					lns->P[j][0] = P[k][0], lns->P[j][1] = P[k][1], lns->P[j][2] = P[k][2];
				}
				if (vary_width) {
					const int vs = lastvary + n;
					lns->width[0] = vary_width[vs], lns->width[1] = vary_width[lastvary + ((n+1) % nvary)];
				}
				else
					lns->width[0] = lns->width[1] = const_width;
				if (vary_N) {
					lns->have_vary_N = true;
					int vs = lastvary + n;
					lns->N[0][0] = vary_N[vs][0], lns->N[0][1] = vary_N[vs][1], lns->N[0][2] = vary_N[vs][2];
					vs = lastvary + ((n + 1) % nvary);
					lns->N[1][0] = vary_N[vs][0], lns->N[1][1] = vary_N[vs][1], lns->N[1][2] = vary_N[vs][2];
				}
				const float vsplit = (n + 1)/float(nseg);
				lns->vmin = vmin;
				lns->vmax = vsplit;
				vmin = vsplit;
				if (spb) {	// mblur prim
					spb->bprims.push_back(new BlurredPrimitive(*spb->parent));
					spb->bprims[blurseg++]->append(lns);
				}
				else
					f.insert(lns);
			}
		}
		else {
			const int nseg = wrap ? (nverts[i] / vstep) : ((nverts[i] - 4) / vstep + 1);
			nvary = wrap ? nseg : (nseg + 1);
			float vmin = 0;
			for (int n=0; n<nseg; ++n) {
				BezierSegment* bzs = new BezierSegment();
				static_cast<Primitive&>(*bzs) = static_cast<Primitive&>(*this); // copy data
				for (int j=0; j<4; ++j) {
					const int k = lastnv + ((n*vstep + j) % nverts[i]);
					bzs->P[j][0] = P[k][0], bzs->P[j][1] = P[k][1], bzs->P[j][2] = P[k][2];
				}
				convert2Bezier_curve(bzs->P, convmtx);
				if (vary_width) {
					const int vs = lastvary + n;
					bzs->width[0] = vary_width[vs], bzs->width[1] = vary_width[lastvary + ((n+1) % nvary)];
				}
				else
					bzs->width[0] = bzs->width[1] = const_width;
				if (vary_N) {
					bzs->have_vary_N = true;
					int vs = lastvary + n;
					bzs->N[0][0] = vary_N[vs][0], bzs->N[0][1] = vary_N[vs][1], bzs->N[0][2] = vary_N[vs][2];
					vs = lastvary + ((n+1) % nvary);
					bzs->N[1][0] = vary_N[vs][0], bzs->N[1][1] = vary_N[vs][1], bzs->N[1][2] = vary_N[vs][2];
				}
				const float vsplit = (n + 1)/float(nseg);
				bzs->vmin = vmin;
				bzs->vmax = vsplit;
				vmin = vsplit;
				if (spb) {	// mblur prim
					spb->bprims.push_back(new BlurredPrimitive(*spb->parent));
					spb->bprims[blurseg++]->append(bzs);
				}
				else
					f.insert(bzs);
			}
		}
		lastnv += nverts[i];
		lastvary += nvary;
	}
}

void CurveSet::dice(MicroPolygonGrid &g, bool Pclose)
{
	printf("[ERROR]: CurveSet()->dice() called?\n");
}

//------------------------------------------------------------------------------
// LinearSegment

LinearSegment::LinearSegment() : have_vary_N(false), vmin(0), vmax(1)
{
}

LinearSegment::~LinearSegment()
{
}

Bound LinearSegment::bound()
{
	Bound b;
	b.include(Point3(P[0][0], P[0][1], P[0][2]));
	b.include(Point3(P[1][0], P[1][1], P[1][2]));
	b.addEpsilon();
	const float mw = MAX2(width[0], width[1]);
	b.minmax[0] -= Point3(mw);
	b.minmax[1] += Point3(mw);
	// no xform, in camspace
	return b;
}

void LinearSegment::split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb)
{
	// for now splitting in u is ommitted, always assuming thin curves.
	// Not difficult to split in u, but typical usage for curves is hair,
	// or curves with a moderate width like grass etc, so should be ok for now
	if (vsplit) {
		const float vmid = 0.5f*(vmin + vmax), wmid = 0.5f*(width[0] + width[1]);
		const RtNormal Nmid = {0.5f*(N[0][0] + N[1][0]), 0.5f*(N[0][1] + N[1][1]), 0.5f*(N[0][2] + N[1][2])};
		// T
		LinearSegment* tp = new LinearSegment();
		tp->P[0][0] = P[0][0], tp->P[0][1] = P[0][1], tp->P[0][2] = P[0][2];
		tp->P[1][0] = 0.5f*(P[0][0] + P[1][0]), tp->P[1][1] = 0.5f*(P[0][1] + P[1][1]), tp->P[1][2] = 0.5f*(P[0][2] + P[1][2]);
		static_cast<Primitive&>(*tp) = *this;
		tp->width[0] = width[0], tp->width[1] = wmid;
		if (have_vary_N) {
			tp->have_vary_N = true;
			tp->N[0][0] = N[0][0], tp->N[0][1] = N[0][1], tp->N[0][2] = N[0][2];
			tp->N[1][0] = Nmid[0], tp->N[1][1] = Nmid[1], tp->N[1][2] = Nmid[2];
		}
		tp->vmin = vmin, tp->vmax = vmid;
		if (spb) {
			if (spb->bprims.empty()) {
				spb->bprims.resize(2);
				spb->bprims[0] = new BlurredPrimitive(*spb->parent);
				spb->bprims[1] = new BlurredPrimitive(*spb->parent);
			}
			spb->bprims[0]->append(tp);
		}
		else
			f.insert(tp);
		// B
		LinearSegment* bt = new LinearSegment();
		bt->P[0][0] = tp->P[1][0], bt->P[0][1] = tp->P[1][1], bt->P[0][2] = tp->P[1][2];
		bt->P[1][0] = P[1][0], bt->P[1][1] = P[1][1], bt->P[1][2] = P[1][2];
		static_cast<Primitive&>(*bt) = *this;
		bt->width[0] = wmid, bt->width[1] = width[1];
		if (have_vary_N) {
			bt->have_vary_N = true;
			bt->N[0][0] = Nmid[0], bt->N[0][1] = Nmid[1], bt->N[0][2] = Nmid[2];
			bt->N[1][0] = N[1][0], bt->N[1][1] = N[1][1], bt->N[1][2] = N[1][2];
		}
		bt->vmin = vmid, bt->vmax = vmax;
		if (spb)	// resize done above
			spb->bprims[1]->append(bt);
		else
			f.insert(bt);
	}
}

bool LinearSegment::diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit)
{
	// assuming thin curves for now, no split in u
	#define TESTSIZE 8
	const float dv = 1.f/float(TESTSIZE);
	float maxVDist = -1e10f;
	RtPoint tP;
	float v = 0.f;
	Point3 lastpt;
	for (unsigned int vg=0; vg<TESTSIZE; ++vg, v+=dv) {
		lerpv(tP, v, P[0], P[1]);
		Point3 pt(tP[0], tP[1], tP[2]);
		if (vg != 0) maxVDist = MAX2(h.rasterEstimate(pt, lastpt), maxVDist);
		lastpt = pt;
	}
	const Attributes* attr = getAttributeReference();
	unsigned int ydim = attr->dice_binary ? (1 << MAX2(0, int(0.5f + (float)log(MAX2(1e-7f, TESTSIZE*maxVDist) / attr->effectiveShadingRate)*(float)M_LOG2E)))
	                                      : MAX2(1, int(0.5f + (TESTSIZE*maxVDist) / attr->effectiveShadingRate));
	g.setDim(0, ydim, this); // xdim always 0, it's a strip of samples
	// since this is only a strip, compare to squareroot of max gridsize
	if ((ydim*ydim) < h.gridSize()) return true;
	vsplit = true;
	return false;
	#undef TESTSIZE
}

void LinearSegment::dice(MicroPolygonGrid &g, bool Pclose)
{
	RtPoint* P_grid = (RtPoint*)(Pclose ? g.addVariable("=Pclose") : g.addVariable("P"));
	RtVector* dPdu_grid = (RtVector*)g.addVariable("dPdu");
	RtVector* dPdv_grid = (RtVector*)g.addVariable("dPdv");
	RtFloat* width_grid = g.addVariable("width", 1);
	g.gtype = MicroPolygonGrid::G_CURVES; // flag as curve grid
	const int ydim = g.get_ydim();
	const float dv = 1.f/(float)ydim;
	int vg;
	float v;
	// dPdv is constant (dPdu too if not varying N, but just in case, calc. from P anyway)
	RtVector dPdv = {P[1][0] - P[0][0], P[1][1] - P[0][1], P[1][2] - P[0][2]};
	for (vg=0, v=0.f; vg<=ydim; ++vg, v+=dv) {
		lerpv(P_grid[vg], v, P[0], P[1]);
		dPdv_grid[vg][0] = dPdv[0], dPdv_grid[vg][1] = dPdv[1], dPdv_grid[vg][2] = dPdv[2];
		lerpf(width_grid[vg], v, width[0], width[1]);
		if (have_vary_N) {
			RtNormal nN;
			lerpv(nN, v, N[0], N[1]);
			vcross(dPdu_grid[vg], nN, dPdv_grid[vg]);
		}
		else	// in this case normals are eqv to -P_grid[vg]
			vcross(dPdu_grid[vg], dPdv_grid[vg], P_grid[vg]);
	}
	// dice other primvars, if any
	g.initVars(std_dice);
	Primitive::linear_dice(g);
}

//------------------------------------------------------------------------------
// BezierSegment

BezierSegment::BezierSegment() : have_vary_N(false), vmin(0), vmax(1)
{
}

BezierSegment::~BezierSegment()
{
}

Bound BezierSegment::bound()
{
	Bound b;
	b.include(Point3(P[0][0], P[0][1], P[0][2]));
	b.include(Point3(P[1][0], P[1][1], P[1][2]));
	b.include(Point3(P[2][0], P[2][1], P[2][2]));
	b.include(Point3(P[3][0], P[3][1], P[3][2]));
	b.addEpsilon();
	const float mw = MAX2(width[0], width[1]);
	b.minmax[0] -= Point3(mw);
	b.minmax[1] += Point3(mw);
	// no xform, in camspace
	return b;
}

inline void bezCurveSplit(const RtPoint P[4], RtPoint Q[4], RtPoint R[4])
{
	Q[0][0] = P[0][0], Q[0][1] = P[0][1], Q[0][2] = P[0][2];
	Q[1][0] = (P[0][0]+ P[1][0])*0.5f, Q[1][1] = (P[0][1]+ P[1][1])*0.5f, Q[1][2] = (P[0][2]+ P[1][2])*0.5f;
	Q[2][0] = Q[1][0]*0.5f + (P[1][0] + P[2][0])*0.25f, Q[2][1] = Q[1][1]*0.5f + (P[1][1] + P[2][1])*0.25f, Q[2][2] = Q[1][2]*0.5f + (P[1][2] + P[2][2])*0.25f;
	R[3][0] = P[3][0], R[3][1] = P[3][1], R[3][2] = P[3][2];
	R[2][0] = (P[2][0] + P[3][0])*0.5f, R[2][1] = (P[2][1] + P[3][1])*0.5f, R[2][2] = (P[2][2] + P[3][2])*0.5f;
	R[1][0] = R[2][0]*0.5f + (P[1][0] + P[2][0])*0.25f, R[1][1] = R[2][1]*0.5f + (P[1][1] + P[2][1])*0.25f, R[1][2] = R[2][2]*0.5f + (P[1][2] + P[2][2])*0.25f;
	Q[3][0] = (Q[2][0] + R[1][0])*0.5f, Q[3][1] = (Q[2][1] + R[1][1])*0.5f, Q[3][2] = (Q[2][2] + R[1][2])*0.5f;
	R[0][0] = Q[3][0], R[0][1] = Q[3][1], R[0][2] = Q[3][2];
	/*
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
	*/
}


void BezierSegment::split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb)
{
	// for now splitting in u is ommitted, always assuming thin curves.
	// Not difficult to split in u, but typical usage for curves is hair,
	// or curves with a moderate width like grass etc, so should be ok for now
	if (vsplit) {
		BezierSegment* tp = new BezierSegment();
		BezierSegment* bt = new BezierSegment();
		// split curves
		bezCurveSplit(P, tp->P, bt->P);
		const float vmid = 0.5f*(vmin + vmax), wmid = 0.5f*(width[0] + width[1]);
		const RtNormal Nmid = {0.5f*(N[0][0] + N[1][0]), 0.5f*(N[0][1] + N[1][1]), 0.5f*(N[0][2] + N[1][2])};
		// T
		static_cast<Primitive&>(*tp) = *this;
		tp->width[0] = width[0], tp->width[1] = wmid;
		if (have_vary_N) {
			tp->have_vary_N = true;
			tp->N[0][0] = N[0][0], tp->N[0][1] = N[0][1], tp->N[0][2] = N[0][2];
			tp->N[1][0] = Nmid[0], tp->N[1][1] = Nmid[1], tp->N[1][2] = Nmid[2];
		}
		tp->vmin = vmin, tp->vmax = vmid;
		if (spb) {
			if (spb->bprims.empty()) {
				spb->bprims.resize(2);
				spb->bprims[0] = new BlurredPrimitive(*spb->parent);
				spb->bprims[1] = new BlurredPrimitive(*spb->parent);
			}
			spb->bprims[0]->append(tp);
		}
		else
			f.insert(tp);
		// B
		static_cast<Primitive&>(*bt) = *this;
		bt->width[0] = wmid, bt->width[1] = width[1];
		if (have_vary_N) {
			bt->have_vary_N = true;
			bt->N[0][0] = Nmid[0], bt->N[0][1] = Nmid[1], bt->N[0][2] = Nmid[2];
			bt->N[1][0] = N[1][0], bt->N[1][1] = N[1][1], bt->N[1][2] = N[1][2];
		}
		bt->vmin = vmid, bt->vmax = vmax;
		if (spb)	// resize done above
			spb->bprims[1]->append(bt);
		else
			f.insert(bt);
	}
}

bool BezierSegment::diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit)
{
	// assuming thin curves for now, no split in u
	#define TESTSIZE 8
	const float dv = 1.f/float(TESTSIZE);
	float maxVDist = -1e10f;
	RtPoint tP;
	float v = 0.f;
	Point3 lastpt;
	for (unsigned int vg=0; vg<TESTSIZE; ++vg, v+=dv) {
		deCasteljau_P(v, P, tP);
		Point3 pt(tP[0], tP[1], tP[2]);
		if (vg != 0) maxVDist = MAX2(h.rasterEstimate(pt, lastpt), maxVDist);
		lastpt = pt;
	}
	const Attributes* attr = getAttributeReference();
	unsigned int ydim = attr->dice_binary ? (1 << MAX2(0, int(0.5f + (float)log(MAX2(1e-7f, TESTSIZE*maxVDist) / attr->effectiveShadingRate)*(float)M_LOG2E)))
	                                      : MAX2(1, int(0.5f + (TESTSIZE*maxVDist) / attr->effectiveShadingRate));
	g.setDim(0, ydim, this); // xdim always 0, it's a strip of samples
	// since this is only a strip, compare to squareroot of max gridsize
	if ((ydim*ydim) < h.gridSize()) return true;
	vsplit = true;
	return false;
	#undef TESTSIZE
}

void BezierSegment::dice(MicroPolygonGrid &g, bool Pclose)
{
	RtPoint* P_grid = (RtPoint*)(Pclose ? g.addVariable("=Pclose") : g.addVariable("P"));
	RtVector* dPdu_grid = (RtVector*)g.addVariable("dPdu");
	RtVector* dPdv_grid = (RtVector*)g.addVariable("dPdv");
	RtFloat* width_grid = g.addVariable("width", 1);
	g.gtype = MicroPolygonGrid::G_CURVES; // flag as curve grid
	RtNormal nN;
	const int ydim = g.get_ydim();
	const float dv = 1.f/(float)ydim;
	int vg;
	float v;
	for (vg=0, v=0.f; vg<=ydim; ++vg, v+=dv) {
		deCasteljau_P_dPdv(v, P, P_grid[vg], dPdv_grid[vg]);
		lerpf(width_grid[vg], v, width[0], width[1]);
		if (have_vary_N) {
			lerpv(nN, v, N[0], N[1]);
			vcross(dPdu_grid[vg], nN, dPdv_grid[vg]);
		}
		else {
			lerpv(nN, v, P[0], P[3]); // campos is always origin, so endpoint normals are simply -P[0] & -P[3]
			vcross(dPdu_grid[vg], dPdv_grid[vg], nN); // -nN !!!
		}
	}
	// dice other primvars, if any
	g.initVars(std_dice);
	Primitive::linear_dice(g);
}

__END_QDRENDER

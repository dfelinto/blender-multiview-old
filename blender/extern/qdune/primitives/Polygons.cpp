//----------------------------------------------------------------
// Polygons
//
// These are dealt with by converting them to bilinear patches.
// Mean value coordinates for interpolation (see ZbufferHider.cpp)
// would seem to be useful here too TODO
//----------------------------------------------------------------

#include "Polygons.h"
#include "Patches.h"
#include "Framework.h"
#include "Mathutil.h"

__BEGIN_QDRENDER

// helper function, splits convex n-gon into a set of bilinear patches
static void splitPolygon(const Primitive& p, const Framework &f, RtPoint* pts,
				int num_verts, int uni_idx, int* idx_ofs = NULL)
{
	const int lastpoint = num_verts-1;
	int sta = 1, end = MIN2(3, lastpoint);
	int vary_idx[4]; // data indices, varying == vertex since bilinear patches used
	// split into set of quads
	while ((end - sta) == 2) {
		if (idx_ofs)
			vary_idx[0] = idx_ofs[0], vary_idx[1] = idx_ofs[sta], vary_idx[2] = idx_ofs[end], vary_idx[3] = idx_ofs[sta+1];
		else
			vary_idx[0] = 0, vary_idx[1] = sta, vary_idx[2] = end, vary_idx[3] = sta+1;
		const RtPoint p1 = {pts[vary_idx[0]][0], pts[vary_idx[0]][1], pts[vary_idx[0]][2]};
		const RtPoint p2 = {pts[vary_idx[1]][0], pts[vary_idx[1]][1], pts[vary_idx[1]][2]};
		const RtPoint p3 = {pts[vary_idx[2]][0], pts[vary_idx[2]][1], pts[vary_idx[2]][2]};
		const RtPoint p4 = {pts[vary_idx[3]][0], pts[vary_idx[3]][1], pts[vary_idx[3]][2]};
		BilinearPatch* bp = new BilinearPatch(p, p1, p2, p3, p4);
		splitPrimVars(&p, bp, uni_idx, vary_idx, vary_idx);
		bp->post_init();
		f.insert(bp);
		sta = end, end = MIN2(sta+2, lastpoint);
	}

	// end triangle when odd number of vertices, prbook seems to have some errors...
	const PrimVars* pv = p.getPrimVars();
	sklist_t<vardata_t*>& varlist = const_cast<PrimVars*>(pv)->pvars;
	if (sta != lastpoint) {
		const int I0 = idx_ofs ? idx_ofs[0] : 0,
		          I1 = idx_ofs ? idx_ofs[sta] : sta,
		          I2 = idx_ofs ? idx_ofs[end] : end;
		const RtPoint ct = {(pts[I0][0] + pts[I1][0] + pts[I2][0])/3.f,
		                    (pts[I0][1] + pts[I1][1] + pts[I2][1])/3.f,
		                    (pts[I0][2] + pts[I1][2] + pts[I2][2])/3.f};
		const RtPoint e1 = {(pts[I0][0] + pts[I1][0])*0.5f,
		                    (pts[I0][1] + pts[I1][1])*0.5f,
		                    (pts[I0][2] + pts[I1][2])*0.5f};
		const RtPoint e2 = {(pts[I1][0] + pts[I2][0])*0.5f,
		                    (pts[I1][1] + pts[I2][1])*0.5f,
		                    (pts[I1][2] + pts[I2][2])*0.5f};
		const RtPoint e3 = {(pts[I2][0] + pts[I0][0])*0.5f,
		                    (pts[I2][1] + pts[I0][1])*0.5f,
		                    (pts[I2][2] + pts[I0][2])*0.5f};
		const RtPoint p1 = {pts[I0][0], pts[I0][1], pts[I0][2]};
		const RtPoint p2 = {pts[I1][0], pts[I1][1], pts[I1][2]};
		const RtPoint p3 = {pts[I2][0], pts[I2][1], pts[I2][2]};
		BilinearPatch* bp1 = new BilinearPatch(p, ct, e3, e1, p1);
		BilinearPatch* bp2 = new BilinearPatch(p, ct, e1, e2, p2);
		BilinearPatch* bp3 = new BilinearPatch(p, ct, e2, e3, p3);

		// primvar split cannot use splitPrimVars() here since new data is interpolated from old
		// (too complex, simplify)
		if (pv && (varlist.size() != 0)) {
			PrimVars *npv1 = NULL, *npv2 = NULL, *npv3 = NULL;

				vardata_t** vdt = varlist.first();
				while (vdt) {
					if (npv1 == NULL) npv1 = bp1->newPrimVars();
					if (npv2 == NULL) npv2 = bp2->newPrimVars();
					if (npv3 == NULL) npv3 = bp3->newPrimVars();
					decParam_t& dp = (*vdt)->param;
					vardata_t* nvdt1 = new vardata_t(dp);
					vardata_t* nvdt2 = new vardata_t(dp);
					vardata_t* nvdt3 = new vardata_t(dp);
					if (dp.ct_flags & SC_CONSTANT) {
						// nothing to do, just copy
						nvdt1->data = new float[dp.numfloats];
						nvdt2->data = new float[dp.numfloats];
						nvdt3->data = new float[dp.numfloats];
						memcpy(nvdt1->data, (*vdt)->data, sizeof(float)*dp.numfloats);
						memcpy(nvdt2->data, (*vdt)->data, sizeof(float)*dp.numfloats);
						memcpy(nvdt3->data, (*vdt)->data, sizeof(float)*dp.numfloats);
					}
					else if (dp.ct_flags & SC_UNIFORM) {
						if (dp.ct_flags & DT_FLOAT) {
							float *nfa1 = new float[1], *nfa2 = new float[1], *nfa3 = new float[1];
							const float *ofa = (*vdt)->data;
							nfa1[0] = nfa2[0] = nfa3[0] = ofa[uni_idx];
							nvdt1->data = nfa1;
							nvdt2->data = nfa2;
							nvdt3->data = nfa3;
						}
						else if ((dp.ct_flags & DT_MATRIX)==0) {
							// color/point/vector/normal, pretend vector, doesn't matter, all are 3-float arrays
							RtVector *nva1 = new RtVector[1], *nva2 = new RtVector[1], *nva3 = new RtVector[1];
							const RtVector *ova = reinterpret_cast<RtVector*>((*vdt)->data);
							nva1[0][0] = nva2[0][0] = nva3[0][0] = ova[uni_idx][0];
							nva1[0][1] = nva2[0][1] = nva3[0][1] = ova[uni_idx][1];
							nva1[0][2] = nva2[0][2] = nva3[0][2] = ova[uni_idx][2];
							nvdt1->data = reinterpret_cast<float*>(nva1);
							nvdt2->data = reinterpret_cast<float*>(nva2);
							nvdt3->data = reinterpret_cast<float*>(nva3);
						}
					}
					else if (dp.ct_flags & (SC_VARYING | SC_VERTEX)) {
						if (dp.ct_flags & DT_FLOAT) {
							float *nfa1 = new float[4], *nfa2 = new float[4],
										*nfa3 = new float[4], *ofa = (*vdt)->data;
							const float ctC = (ofa[I0] + ofa[I1] + ofa[I2])/3.f,
													e1C = (ofa[I0] + ofa[I1])*0.5f,
													e2C = (ofa[I1] + ofa[I2])*0.5f,
													e3C = (ofa[I2] + ofa[I0])*0.5f;
							// T1
							nfa1[0] = ctC, nfa1[1] = e3C, nfa1[2] = e1C, nfa1[3] = ofa[I0];
							// T2
							nfa2[0] = ctC, nfa2[1] = e1C, nfa2[2] = e2C, nfa2[3] = ofa[I1];
							// T3
							nfa3[0] = ctC, nfa3[1] = e2C, nfa3[2] = e3C, nfa3[3] = ofa[I2];
							nvdt1->data = nfa1;
							nvdt2->data = nfa2;
							nvdt3->data = nfa3;
						}
						else if ((dp.ct_flags & DT_MATRIX)==0) {
							// color/point/vector/normal, pretend color, doesn't matter, all are 3-float arrays
							RtColor *nca1 = new RtColor[4], *nca2 = new RtColor[4],
											*nca3 = new RtColor[4], *oca = reinterpret_cast<RtColor*>((*vdt)->data);
							const RtColor ctC = {(oca[I0][0] + oca[I1][0] + oca[I2][0])/3.f,
																	(oca[I0][1] + oca[I1][1] + oca[I2][1])/3.f,
																	(oca[I0][2] + oca[I1][2] + oca[I2][2])/3.f};
							const RtColor e1C = {(oca[I0][0] + oca[I1][0])*0.5f,
																	(oca[I0][1] + oca[I1][1])*0.5f,
																	(oca[I0][2] + oca[I1][2])*0.5f};
							const RtColor e2C = {(oca[I1][0] + oca[I2][0])*0.5f,
																	(oca[I1][1] + oca[I2][1])*0.5f,
																	(oca[I1][2] + oca[I2][2])*0.5f};
							const RtColor e3C = {(oca[I2][0] + oca[I0][0])*0.5f,
																	(oca[I2][1] + oca[I0][1])*0.5f,
																	(oca[I2][2] + oca[I0][2])*0.5f};
							// T1
							nca1[0][0] = ctC[0], nca1[0][1] = ctC[1], nca1[0][2] = ctC[2];
							nca1[1][0] = e3C[0], nca1[1][1] = e3C[1], nca1[1][2] = e3C[2];
							nca1[2][0] = e1C[0], nca1[2][1] = e1C[1], nca1[2][2] = e1C[2];
							nca1[3][0] = oca[I0][0], nca1[3][1] = oca[I0][1], nca1[3][2] = oca[I0][2];
							// T2
							nca2[0][0] = ctC[0], nca2[0][1] = ctC[1], nca2[0][2] = ctC[2];
							nca2[1][0] = e1C[0], nca2[1][1] = e1C[1], nca2[1][2] = e1C[2];
							nca2[2][0] = e2C[0], nca2[2][1] = e2C[1], nca2[2][2] = e2C[2];
							nca2[3][0] = oca[I1][0], nca2[3][1] = oca[I1][1], nca2[3][2] = oca[I1][2];
							// T3
							nca3[0][0] = ctC[0], nca3[0][1] = ctC[1], nca3[0][2] = ctC[2];
							nca3[1][0] = e2C[0], nca3[1][1] = e2C[1], nca3[1][2] = e2C[2];
							nca3[2][0] = e3C[0], nca3[2][1] = e3C[1], nca3[2][2] = e3C[2];
							nca3[3][0] = oca[I2][0], nca3[3][1] = oca[I2][1], nca3[3][2] = oca[I2][2];
							nvdt1->data = reinterpret_cast<float*>(nca1);
							nvdt2->data = reinterpret_cast<float*>(nca2);
							nvdt3->data = reinterpret_cast<float*>(nca3);
						}
						// don't know what to do with matrices yet...
					}
					char* vname = varlist.getName();
					npv1->pvars.insert(vname, nvdt1);
					npv2->pvars.insert(vname, nvdt2);
					npv3->pvars.insert(vname, nvdt3);
					vdt = varlist.next();
				}

		}

		bp1->post_init();
		bp2->post_init();
		bp3->post_init();
		f.insert(bp1);
		f.insert(bp2);
		f.insert(bp3);

	}

}

//------------------------------------------------------------------------------
// Polygon

Polygon::Polygon(RtInt nverts, RtInt n, RtToken tokens[], RtPointer parms[])
				: pts(NULL), num_verts(nverts)
{
	for (int i=0; i<n; ++i) {
		if (!strcmp(tokens[i], RI_P)) {
			pts = new RtPoint[num_verts];
			memcpy(pts, parms[i], sizeof(RtPoint)*num_verts);
			break;
		}
	}
	Primitive::initPrimVars(n, tokens, parms, 1, nverts, nverts, nverts);
}

Polygon::~Polygon()
{
	if (pts) { delete[] pts;  pts = NULL; }
}

Bound Polygon::bound()
{
	Bound b;
	for (int i=0; i<num_verts; ++i)
		b.include(pts[i]);
	b.addEpsilon();
	b.transform(xform);
	return b;
}

void Polygon::split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb)
{
	// usplit/vsplit values don't matter, just splits (never called from diceable())
	splitPolygon(*this, f, pts, num_verts, 0);
}

// does nothing, print msg in case it is called, should never happen though...
void Polygon::dice(MicroPolygonGrid &g, bool Pclose)
{
	printf("[ERROR]: Polygon()->dice() called?\n");
}

//------------------------------------------------------------------------------
// PointsPolygons

PointsPolygons::PointsPolygons(RtInt npolys, RtInt nverts[], RtInt verts[],
															RtInt n, RtToken tokens[], RtPointer parms[])
															: pts(NULL), num_faces(npolys)
{
	// sum of verts ( == max possible vertex index == sizeof verts[] array)
	int sum_faceverts = 0;
	max_fvert = 0; // highest vertex count per face, and also max possible number of facevarying vars
	for (int i=0; i<npolys; ++i) {
		sum_faceverts += nverts[i];
		max_fvert = MAX2(max_fvert, nverts[i]);
	}
	vert_per_face = new RtInt[npolys];
	memcpy(vert_per_face, nverts, sizeof(RtInt)*npolys);

	// determine number of expected "P" vertices from the highest index referenced (+1) in verts[]
	num_verts = 0;
	for (int i=0; i<sum_faceverts; ++i)
		num_verts = MAX2(num_verts, verts[i]);
	num_verts++;
	vert_idx = new RtInt[sum_faceverts];
	memcpy(vert_idx, verts, sizeof(RtInt)*sum_faceverts);

	// get variables
	for (int i=0; i<n; ++i) {
		if (!strcmp(tokens[i], RI_P)) {
			pts = new RtPoint[num_verts];
			memcpy(pts, parms[i], sizeof(RtPoint)*num_verts);
			break;
		}
	}
	Primitive::initPrimVars(n, tokens, parms, npolys, num_verts, num_verts, sum_faceverts);
}

PointsPolygons::~PointsPolygons()
{
	if (vert_per_face) { delete[] vert_per_face;  vert_per_face = NULL; }
	if (vert_idx) { delete[] vert_idx;  vert_idx = NULL; }
	if (pts) { delete[] pts;  pts = NULL; }
}

Bound PointsPolygons::bound()
{
	Bound b;
	for (int i=0; i<num_verts; ++i)
		b.include(pts[i]);
	b.addEpsilon();
	b.transform(xform);
	return b;
}

void PointsPolygons::split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb)
{
	// usplit/vsplit values don't matter, just splits (never called from diceable())
	RtInt* pidx = new RtInt[max_fvert];
	int idx = 0;
	for (int i=0; i<num_faces; ++i) {
		for (int j=0; j<vert_per_face[i]; ++j)
			pidx[j] = vert_idx[j + idx];
		splitPolygon(*this, f, pts, vert_per_face[i], i, pidx);
		idx += vert_per_face[i];
	}
	delete[] pidx;
}

// does nothing, print msg in case it is called, should never happen though...
void PointsPolygons::dice(MicroPolygonGrid &g, bool Pclose)
{
	printf("[ERROR]: PointsPolygons()->dice() called?\n");
}

__END_QDRENDER

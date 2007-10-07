///---------------------------------------------------------------------------------------------------------------------------------------------
/// Catmull-Clark Subdivision Meshes
///---------------------------------------------------------------------------------------------------------------------------------------------
///
/// Unlike most renderman compliant renderers, it also supports non-manifolds, just as Blender does.
/// This does probably make it more complex than needed though (no nice half/winged-edge datastructures).
///
/// The general process is as follows:
/// Initially the mesh will go through possibly two global direct subdivision iterations.
/// (First subdivision is done to make mesh all quad, and the second subdivision
///  is done to make sure that extra-ordinary vertices are isolated)
/// Then, the mesh is converted to a set of patches, one for each face in the mesh.
/// A patch consists of a face in the original mesh with its complete set of neighbour faces.
/// Then, where possible, most (hopefully the majority) will be converted to bspline patches.
/// This might not be possible because the patch has an extra-ordinary vertex.
/// However, if the patch is manifold and its valence is in the range 3 to 50,
/// then it is still possible to evaluate the patch directly using the method devised by Jos Stam.
/// (See "Exact Evaluation Of Catmull-Clark Subdivision Surfaces At Arbitrary Parameter Values")
/// (The 3 to 50 range is not a restriction of the method, but the required lookup table is
///  in that range, and I don't know how to calculate the eigen vectors myself...)
/// For the remaining patches, possibly nonmanifold and/or valence > 50, or has creases,
/// there is no choice left but to dice them by explicit subdivision...
/// hopefully this won't happen too often, since it is very slow...
///
/// Support for creases is rudimentary, only perfectly sharp creases currently.
/// However, while this code is almost more than a year old now, I never got around to even actually testing it...
/// Well, it worked in the python script prototype anyway...
///
/// It is all too much code, clumsy, slow and messy, but it works...
/// A prime candidate for a complete rewrite, that's for sure...
///
/// idea's for optimization:
///--------------------------
/// since explicit subdivision is very slow, and the renderer is intended for Blender anyway, it should be possible to
/// use Blender's subdivision routines for the initial global subdivisions at least, and possibly for the patch stage as well.
/// It might in fact already work similarly even, Blender's code is kind of hard to read, but it seems to work with some sort of grids anyway.
///
/// Currently bspline patches are all created separately, but it must be possible to optimize it by sharing control point info,
/// in fact making a *PATCHMESH* again, should save a bit on memory usage. Before that can be done though, the bicubic patch code
/// must be modified, since that itself splits up a patchmesh into individual patches too.
///
/// The initial required subdivisions are the least efficient and really slow, and can increase memory usage a lot.
/// However, it might be possible to just create patches from the start, end *then* subdivide the *patches* if needed.
/// Not entirely sure yet if that actually is a viable method at all though...
/// UPDATE: This does in fact work. While it does in quite a few cases considerably speed up everything and uses less memory too,
///         (suzgallery.rib almost twice as fast), this is unfortunately not always so, in other cases slightly more memory is used and rendering is a bit slower too.
///         So more work is needed to make this a possible good alternative method. TODO
///         For now the code is disabled, to test this alternate method, uncomment line 84
///
/// If creases exist, then currently there is no other choice yet but to make it an explicit patch.
/// This could however be optimized for the case that the patch is manifold,
/// since then a simpler 2D grid subdivision layout would be possible.
/// (but then again, all that could possibly be done of by blender's subd.code anyway)
///
/// NOTE there is a recent jgt article on evaluating boundary patches with extra-ordinary vertices similar to Jos Stam's method,
///      this would be a very useful addition, since currently they are taken care of by explicit subdivision.
///      (regular (and perfect sharp corner) boundary patches are already taken care of as bspline patches by extrapolating points,
///       but are probably in fact simpler than that, possibly a bilinear patch)
///

#include "CCSubdivision.h"

#include "State.h"
#include "Mathutil.h"
#include "Patches.h"
#include "MicroPolygonGrid.h"
#include "Hider.h"
#include "Framework.h"

__BEGIN_QDRENDER

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#pragma warning(disable : 4996)
#endif

#ifdef _MSC_VER 
typedef unsigned __int64 uint64;
#else
typedef unsigned long uint64;
#endif

// uncomment this line to test the alternate method, see comments above, lines 45-48
//#define SKIP_GLOBAL_SUBDIVISION

static bool intpolBD = false;

typedef FixedArray<ccVert>::iterator VertIter;
typedef FixedArray<ccVert>::const_iterator VertConstIter;
typedef FixedArray<ccFace>::iterator FaceIter;
typedef FixedArray<ccFace>::const_iterator FaceConstIter;
typedef FixedArray<ccEdge>::iterator EdgeIter;
typedef FixedArray<ccEdge>::const_iterator EdgeConstIter;

// ugly macros... needs better implementation TODO

// macro to make new edgelist in rebuildList() and makeExplicitSDPatch()
// Also updates edge ptrs in vert_list, face ptrs in edge_list,
// vice versa, edges in face list as well, but as indices,
// this makes mesh recontruction simpler,
// (in face list, verts & edges are in ccw order)
#define SET_EDGE(_edge_list, _i1, _i2, _vert_list, _face_list, _num_verts, _fc_idx)\
{\
	unsigned int _idx1=_i1, _idx2=_i2;\
	if (_idx1 > _idx2) SWAP(_idx1, _idx2);\
	int e_idx = *edge_id.findItem(_idx1 + (uint64)_num_verts*_idx2);\
	ccFace* f = &_face_list[_fc_idx];\
	f->edges.push_back(e_idx);\
	E = &_edge_list[e_idx];\
	E->v1 = &_vert_list[_i1], E->v2 = &_vert_list[_i2];\
	if (E->v1->edges.size() == 0) {\
		const Array<int>* eiv = *edge_verts.findItem(E->v1);\
		for (Array<int>::const_iterator ei=eiv->begin(); ei!=eiv->end(); ++ei)\
			E->v1->edges.push_back(&_edge_list[*ei]);\
	}\
	if (E->v2->edges.size() == 0) {\
		const Array<int>* eiv = *edge_verts.findItem(E->v2);\
		for (Array<int>::const_iterator ei=eiv->begin(); ei!=eiv->end(); ++ei)\
			E->v2->edges.push_back(&_edge_list[*ei]);\
	}\
	E->faces.push_back(f);\
}

// macro to prepare for edge list initialization
// determines new number of edges & initializes hashtables
#define INIT_EDGES(_vert_list, _face_list, _num_verts, _num_faces, _num_edges)\
	AATree_t<uint64, unsigned int> edge_id;\
	HashTable<const ccVert*, Array<int>* > edge_verts;\
	_num_edges = 0;\
	for (unsigned int fi=0; fi<_num_faces; ++fi) {\
		ccFace* f = &_face_list[fi];\
		const Array<int>& fcverts = f->verts;\
		const unsigned int nv = fcverts.size();\
		for (unsigned int vi=0; vi<nv; ++vi) {\
			unsigned int ri1 = fcverts[vi], ri2 = fcverts[(vi+1) % nv];\
			unsigned int idx1 = ri1, idx2 = ri2;\
			if (idx1 > idx2) SWAP(idx1, idx2);\
			uint64 ID = idx1 + (uint64)_num_verts*idx2;\
			if (edge_id.findItem(ID) == NULL) {\
				edge_id.addItem(ID, _num_edges);\
				const ccVert *v1 = &_vert_list[ri1], *v2 = &_vert_list[ri2];\
				Array<int> **evi1 = edge_verts.findItem(v1), **evi2 = edge_verts.findItem(v2);\
				if (evi1 == NULL) { edge_verts.addItem(v1, new Array<int>);  evi1 = edge_verts.findItem(v1); }\
				if (evi2 == NULL) { edge_verts.addItem(v2, new Array<int>);  evi2 = edge_verts.findItem(v2); }\
				(*evi1)->push_back(_num_edges);\
				(*evi2)->push_back(_num_edges);\
				_num_edges++;\
			}\
		}\
	}


// helper function to set the boundary flag if edge has single face neighbour.
// Also, if edge on boundary then so are its two vertices and adjacent face
inline void setBoundaryFlags(FixedArray<ccEdge>& edge_list)
{
	for (EdgeIter e=edge_list.begin(); e!=edge_list.end(); ++e) {
		if (e->faces.size() == 1) {
			e->flags |= SD_BOUNDARY;
			e->v1->flags |= SD_BOUNDARY;
			e->v2->flags |= SD_BOUNDARY;
			e->faces[0]->flags |= SD_BOUNDARY;
		}
	}
}

//-----------------------------------------------------------------------------------------------------------------
// CCSubdivisionMesh

CCSubdivisionMesh::CCSubdivisionMesh(RtInt nf, RtInt nverts[], RtInt verts[], RtInt ntags, RtToken tags[],
	RtInt nargs[], RtInt intargs[], RtFloat floatargs[], RtInt n, RtToken tokens[], RtPointer *parms)
{
	unsigned int num_faces = nf;

	// test if all quad, if not, one global subdivision iteration is needed
	presubd = false;
	int sum_faceverts = 0; // sum of verts ( == max possible vertex index == sizeof verts[] array)
	for (unsigned int i=0; i<num_faces; i++) {
		if (nverts[i] != 4) presubd = true;
		sum_faceverts += nverts[i];
	}

	// determine number of expected "P" vertices from the highest index referenced (+1) in verts[]
	int num_verts = 0;
	for (int i=0; i<sum_faceverts; i++)
		num_verts = MAX2(num_verts, verts[i]);
	num_verts++;

	vert_list.resize(num_verts);

	// get "P" param (others TODO)
	for (int i=0; i<n; i++) {
		if (!strcmp(tokens[i], RI_P)) {
			RtPoint* P = (RtPoint*)parms[i];
			for (int vi=0; vi<num_verts; vi++)
				vert_list[vi].co.set(P[vi][0], P[vi][1], P[vi][2]);
			// others TODO, so can break once "P" found
			break;
		}
	}

	// check tags, only "interpolateboundary" flag for now
	intpolBD = false;
	if (ntags) {
		if (!strcmp(tags[0], "interpolateboundary"))
			intpolBD = true;
	}

	// init faces, and set face pointers in vert_list
	int lv = 0;
	face_list.resize(num_faces);
	for (unsigned int i=0; i<num_faces; i++) {
		ccFace* f = &face_list[i];
		for (int j=0; j<nverts[i]; j++) {
			f->verts.push_back(verts[lv + j]);
			vert_list[verts[lv+j]].faces.push_back(f);
		}
		lv += nverts[i];
	}

	// Prepare the initial edge list
	unsigned int num_edges = 0;
	INIT_EDGES(vert_list, face_list, vert_list.size(), face_list.size(), num_edges)

	// Finalize initial edge list, and set flags.
	// In rebuildLists() & makeExplicitSDPatch() edgelists are rebuilt using slightly
	// different code to account for proper propagation of flags.
	edge_list.resize(num_edges);
	for (unsigned int fi=0; fi<num_faces; ++fi) {
		const Array<int>& fcverts = face_list[fi].verts;
		const unsigned int nv = fcverts.size();
		ccEdge* E;
		for (unsigned int vi=0; vi<nv; ++vi)
			SET_EDGE(edge_list, fcverts[vi], fcverts[(vi+1) % nv], vert_list, face_list, vert_list.size(), fi);
	}
	// delete array pointers in edge_verts
	edge_verts.clear_delete();
	setBoundaryFlags(edge_list);

	Primitive::initPrimVars(n, tokens, parms, num_faces, num_verts, num_verts, sum_faceverts);

	// if we have s/t coords, set for each face.
	// for now, only facevarying type
	if (primvars) {
		vardata_t **svar = primvars->pvars.findItem("s"), **tvar = primvars->pvars.findItem("t");
		if (svar && tvar) {
			if (((*svar)->param.ct_flags & SC_FACEVARYING) && ((*tvar)->param.ct_flags & SC_FACEVARYING)) {
				const float *sc = (*svar)->data, *tc = (*tvar)->data;
				int idx = 0;
				for (FaceIter fi=face_list.begin(); fi!=face_list.end(); ++fi)
					for (unsigned int i=0; i<fi->verts.size(); ++i, ++idx)
						fi->stco.push_back(Point2(sc[idx], tc[idx]));
			}
		}
	}

}


CCSubdivisionMesh::~CCSubdivisionMesh()
{
	// nothing to do
}


void CCSubdivisionMesh::post_init()
{
	// transform vertices to camera space
	for (VertIter v=vert_list.begin(); v!=vert_list.end(); ++v)
		v->co = *xform * v->co;
}


Bound CCSubdivisionMesh::bound()
{
	Bound b;
	for (VertIter v=vert_list.begin(); v!=vert_list.end(); ++v)
		b.include(v->co);
	b.addEpsilon();
	// already in camspace
	return b;
}


void CCSubdivisionMesh::split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb)
{
#ifndef SKIP_GLOBAL_SUBDIVISION
	// do one iteration of global subdivision if mesh is not totally quadrilateral yet
	if (presubd) {
		//printf("Initial subdivision to make mesh quadrilateral\n");
		Subdivide(vert_list, edge_list, face_list);
		rebuildLists(vert_list, edge_list, face_list);
	}

	// another global subdivision to isolate extra-ordinary vertices,
	// but first test if this is in fact needed at all.
	// (maybe it is best to actually just always do this, since in some cases,
	//  if skipped, the result might be a lot of explicit subdivision patches...)
	int numeo = 0;
	for (FaceConstIter fi=face_list.begin(); fi!=face_list.end(); ++fi) {
		const unsigned int sz = fi->verts.size();
		numeo = 0;
		for (unsigned int fvi=0; fvi<sz; ++fvi) {
			const ccVert& v = vert_list[fi->verts[fvi]];
			const unsigned int val = v.edges.size();
			// for boundary case, 'perfect' valence is 3, otherwise 4
			if (val != 2) {
				if (val != ((v.flags & SD_BOUNDARY) ? 3 : 4)) numeo ++;
			}
		}
		if (numeo > 1) break;
	}
	if (numeo > 1) {
		//printf("Mesh has more than 1 eo.vert per face, global subdivision required\n");
		Subdivide(vert_list, edge_list, face_list);
		rebuildLists(vert_list, edge_list, face_list);
	}
#endif

	// make the subdivision patches. See getSubdivData() for prerender flag purpose
	JS_SDPatch::prerender = State::Instance()->rendering();
	makePatches(vert_list, edge_list, face_list, this, &f, spb);
	JS_SDPatch::prerender = not JS_SDPatch::prerender;
}

void CCSubdivisionMesh::dice(MicroPolygonGrid &g, bool Pclose)
{
	printf("[ERROR]: CCSubdivisionMesh()->dice() called?\n");
}

//-----------------------------------------------------------------------------------------------------------------

// do one iteration of catmull-clark subdivision.
// returns array of repositioned original vertices and new face & edge verts
void Subdivide(FixedArray<ccVert> &vert_list,
               FixedArray<ccEdge> &edge_list, FixedArray<ccFace> &face_list,
               bool fromPatch, bool limitProjection)
{
	const unsigned int num_verts = vert_list.size();
	const unsigned int num_edges = edge_list.size();
	const unsigned int num_faces = face_list.size();
	const unsigned int new_num_verts = num_verts + num_edges + num_faces;

	FixedArray<ccVert> new_verts(new_num_verts);

	// add new face vertices, average of all face verts
	int nvi = num_verts; // start index of new face verts
	for (FaceIter f=face_list.begin(); f!=face_list.end(); ++f) {
		Point3 nv(0);
		for (Array<int>::const_iterator vi=f->verts.begin(); vi!=f->verts.end(); ++vi)
			nv += vert_list[*vi].co;
		new_verts[nvi].co = nv / float(f->verts.size());
		f->center = &new_verts[nvi++];
	}

	// add new edge vertices,
	// average of edge verts and new face verts sharing this edge
	for (EdgeIter e=edge_list.begin(); e!=edge_list.end(); ++e) {
		if (fromPatch) {
			// for patches, outermost ring of vertices when not on border can be ignored,
			// they are only needed to calculate face & edge vertices, but are not needed
			// for the next level, so calculation can be skipped.
			// The complete set is still allocated above though, since the indexing
			// must be consistent for rebuildLists() to work properly.
			if (((e->v1->flags & SD_MAINQUAD)==0) && ((e->v2->flags & SD_MAINQUAD)==0)) {
				e->v1->flags |= SD_SUPPORTVERTEX;
				e->v2->flags |= SD_SUPPORTVERTEX;
				new_verts[nvi++].flags |= SD_SUPPORTVERTEX;
				continue;
			}
		}
		Point3 ne = e->v1->co + e->v2->co;
		// don't include faceverts if edge on boundary or crease
		if (e->flags & (SD_BOUNDARY | SD_CREASE))
			new_verts[nvi].co = ne * 0.5f;
		else {
			for (Array<ccFace*>::const_iterator f=e->faces.begin(); f!=e->faces.end(); ++f)
				ne += (*f)->center->co;
			new_verts[nvi].co = ne / float(e->faces.size() + 2);
		}
		e->new_vert = &new_verts[nvi++];
		if (e->flags & SD_BOUNDARY)
			e->new_vert->flags |= SD_BOUNDARY;
	}

	// reposition original vertices
	nvi = 0;	// start index of repositioned vertices
	for (VertIter v=vert_list.begin(); v!=vert_list.end(); ++v) {
		if (fromPatch) {
			// as above, for patches, ignore 'support' vertices
			if ((v->flags & SD_MAINQUAD)==0) {
				v->flags |= SD_SUPPORTVERTEX;
				new_verts[nvi++].flags = v->flags;
				continue;
			}
		}
		new_verts[nvi].flags = v->flags;

		// determine number of creases and boundaries
		int numcr = 0, numbd = 0;
		for (Array<ccEdge*>::const_iterator e=v->edges.begin(); e!=v->edges.end(); ++e) {
			if ((*e)->flags & SD_CREASE) numcr++;
			if ((*e)->flags & SD_BOUNDARY) numbd++;
		}

		// crease rule first
		if (numcr > 1) {  // if dart, single sharp edge, can be skipped, smooth rule applies
			if (numcr > 2) // corner, if >2 creases, no change in position
				new_verts[nvi++].co = v->co;
			else {
				// a crease vertex, exactly 2 creases, use even boundary rule,
				// *unless* there are only two edges connected to the vertex,
				// and both are also boundary edges, then consider it a corner vertex instead
				if ((v->edges.size() == 2) &&
						(v->edges[0]->flags & SD_BOUNDARY) && (v->edges[1]->flags & SD_BOUNDARY))
					new_verts[nvi++].co = v->co;
				else {
					Point3 R = v->co * 6.f;
					// &(*v) below in the comparison is necessary if Array is implemented using stl,
					// since then it is actually an iterator object.
					// When using the qdtl implementation, which implements 'iterators' directly as pointers, just 'v' would be ok
					// (not true anymore if the block based implementation is used, has 'proper' iterators)
					for (Array<ccEdge*>::const_iterator e=v->edges.begin(); e!=v->edges.end(); ++e)
						if ((*e)->flags & SD_CREASE) R += ((*e)->v1 != &(*v)) ? (*e)->v1->co : (*e)->v2->co;
					new_verts[nvi++].co = R * 0.125f;
				}
			}
			continue;
		}

		// then test if boundary rule needed
		if (numbd) {
			// these rules seem to emulate blender results exactly (even non-manifold),
			// not sure if totally correct though...
			if (numbd & 1) {
				// odd number of sharp edges, avg. of orig.vert and adjacent new edge vertices
				Point3 R = v->co * numbd;
				for (Array<ccEdge*>::const_iterator e=v->edges.begin(); e!=v->edges.end(); ++e)
					if ((*e)->flags & SD_BOUNDARY) R += (*e)->new_vert->co;
				new_verts[nvi++].co = R * (0.5f / (float)numbd);
			}
			else {
				// even, standard vertex rule, use average of pairs
				const float numpairs = float(numbd >> 1);
				Point3 R = v->co * 6.f * numpairs;
				for (Array<ccEdge*>::const_iterator e=v->edges.begin(); e!=v->edges.end(); ++e) {
					if ((*e)->flags & SD_BOUNDARY) {
						if ((*e)->v1 != &(*v)) R += (*e)->v1->co;
						if ((*e)->v2 != &(*v)) R += (*e)->v2->co;
					}
				}
				new_verts[nvi++].co = R * (0.125f / numpairs);
			}
			continue;
		}

		// valence
		const unsigned int n = v->edges.size();

		// Q -> average of all new face verts around this vertex
		Point3 Q(0);
		for (Array<ccFace*>::const_iterator f=v->faces.begin(); f!=v->faces.end(); ++f)
			Q += (*f)->center->co;
		Q /= (float)v->faces.size();

		// R -> average of all edge midpoints on this vertex (from CC paper, prbook info seems incorrect)
		Point3 R(0);
		for (Array<ccEdge*>::const_iterator e=v->edges.begin(); e!=v->edges.end(); ++e)
			R += ((*e)->v1 != &(*v)) ? (*e)->v1->co : (*e)->v2->co;

		const float ni = 1.f / float(n);
		new_verts[nvi++].co = (v->co*(n-2) + R*ni + Q) * ni;
	}

	if (!limitProjection) {
		vert_list = new_verts;
		return;
	}

	// project previous points to limit surface,
	// above subdivision results only needed for this calculation, but not used
	// This code is actually not totally correct, don't know what to do for nonmanifolds,
	// but the most important thing is that the explicit subdivision patches at least match the
	// parametric patches at the borders.
	for (VertIter v=vert_list.begin(), newv=new_verts.begin();
	              v!=vert_list.end(); ++v, ++newv)
	{
		// limit projection is only done for patches, so can skip support vertices
		if (v->flags & SD_SUPPORTVERTEX) continue;
		// Since limit projection is only done for patches,
		// skip any vertex not on main quad
		if ((v->flags & SD_MAINQUAD)==0) continue;
		const unsigned int n = v->edges.size(); // valence
		// first test if vertex has any crease edges
		int numcr = 0;
		for (Array<ccEdge*>::const_iterator e=v->edges.begin(); e!=v->edges.end(); ++e)
			if ((*e)->flags & SD_CREASE) numcr++;
		if (numcr > 1) {  // if dart, single sharp edge, can be skipped, smooth rule applies
			if (numcr > 2) // corner, if >2 creases, no change in position
				v->co = newv->co;
			else {
				// a crease vertex, exactly 2 creases, use even boundary rule,
				// *unless* there are only two edges connected to the vertex,
				// and both are also boundary edges, then consider it a corner vertex instead
				if ((n == 2) && ((v->edges[0]->flags & SD_BOUNDARY) && (v->edges[1]->flags & SD_BOUNDARY)))
					v->co = newv->co;
				else {
					Point3 co = newv->co * 4.f;
					for (Array<ccEdge*>::const_iterator e=v->edges.begin(); e!=v->edges.end(); ++e)
						if ((*e)->flags & SD_CREASE) co += (*e)->new_vert->co;
					v->co = co / 6.f;
				}
			}
			continue;
		}
		if ((numcr == 1) || (v->flags & SD_BOUNDARY)) {
			// boundary or single crease vertex, not sure what to do here,
			// could only find rule for boundary vertex with two edges.
			// At least this seems to work to match up borders of
			// explicit subd.patches with the evaluated patches.
			// What *really* to do for all other cases, I don't know...
			int numbd = 0;
			for (Array<ccEdge*>::const_iterator e=v->edges.begin(); e!=v->edges.end(); ++e)
				if ((*e)->flags & SD_BOUNDARY) numbd++;
			// normaly an integer division by 2, but this makes it at least 'look'
			// as if it works for odd number of boundaries too...
			const float numpairs = numbd*0.5f;
			Point3 co = (newv->co * 4.f * numpairs);
			for (Array<ccEdge*>::const_iterator e=v->edges.begin(); e!=v->edges.end(); ++e)
				if ((*e)->flags & SD_BOUNDARY) co += (*e)->new_vert->co;
			v->co = co / (6.f*numpairs);
		}
		else {
			Point3 co = newv->co * (n*n);
			for (Array<ccEdge*>::const_iterator e=v->edges.begin(); e!=v->edges.end(); ++e)
				co += (4.f * (*e)->new_vert->co);
			for (Array<ccFace*>::const_iterator f=v->faces.begin(); f!=v->faces.end(); ++f)
				co += (*f)->center->co;
			if (v->faces.size() != n)
				// hack, 'almost' works for non-manifold cases...
				v->co = co / float(1 + n*(n + 5));
			else
				v->co = co / float(n*(n + 5));
		}
	}
	// new_verts is now discarded, only used for limit projection

}

// rebuild all lists from *new* vert_list (contains repositioned orig. verts)
// and previous face & edge lists which now contain new face & edge vertices
void rebuildLists(FixedArray<ccVert> &vert_list,
                  FixedArray<ccEdge> &edge_list, FixedArray<ccFace> &face_list,
                  bool fromPatch, bool st_rot)
{
	const unsigned int num_verts = vert_list.size();
	const unsigned int num_edges = edge_list.size();
	const unsigned int num_faces = face_list.size();
	unsigned int orig_numverts = num_verts - num_edges - num_faces;

	// start of new face & edge vertices in vert_list
	unsigned int face_st = orig_numverts;
	unsigned int edge_st = face_st + num_faces;

	// set crease flag of new edge vertices
	// (needed for crease flag propagation in edge_list rebuild below)
	int v_idx = edge_st;
	for (EdgeIter e=edge_list.begin(); e!=edge_list.end(); ++e)
		if (e->flags & SD_CREASE)
			vert_list[v_idx++].flags |= SD_CREASE;

	// vertex index remap, only used if building from patch or 'interpolateboundary' tag not set
	HashTable<int, int> vidx_rm;
	if (fromPatch) {
		// for patches, the outermost ring of vertices can be discarded if not on border.
		int cur_vidx = 0;
		int new_numverts = 0;
		for (VertIter v=vert_list.begin(); v!=vert_list.end(); ++v)
			if ((v->flags & SD_SUPPORTVERTEX)==0) new_numverts++;
		FixedArray<ccVert> nv(new_numverts);
		int nvi = 0, vi = 0;
		for (VertIter v=vert_list.begin(); v!=vert_list.end(); ++v, ++vi) {
			if ((v->flags & SD_SUPPORTVERTEX)==0) {
				nv[nvi++] = *v;
				vidx_rm.addItem(vi, cur_vidx++);
			}
		}
		vert_list = nv;
	}
	else if (!intpolBD) {
		// for global subdivision, remove boundary vertices if 'interpolateboundary' tag was not set
		int cur_vidx = 0;
		int new_numverts = 0;
		for (VertIter v=vert_list.begin(); v!=vert_list.end(); ++v)
			if ((v->flags & SD_BOUNDARY)==0) new_numverts++;
		FixedArray<ccVert> nv(new_numverts);
		int nvi = 0,  vi = 0;
		for (VertIter v=vert_list.begin(); v!=vert_list.end(); ++v, ++vi) {
			if ((v->flags & SD_BOUNDARY)==0) {
				nv[nvi++] = *v;
				vidx_rm.addItem(vi, cur_vidx++);
			}
		}
		vert_list = nv;
	}

	// determine the required new number of faces
	FixedArray<ccFace> old_face_list(face_list);
	int new_numfaces = 0;
	for (unsigned int fi=0; fi<num_faces; ++fi) {
		ccFace* f = &face_list[fi];
		if (vidx_rm.empty())
			new_numfaces += f->edges.size();
		else {
			const int mv = f->verts.size(), num_e = f->edges.size();
			for (int ei=0; ei<num_e; ++ei) {
				if (vidx_rm.findItem(face_st + fi) &&
				    vidx_rm.findItem(edge_st + f->edges[ei]) &&
				    vidx_rm.findItem(f->verts[(ei+1) % mv]) &&
				    vidx_rm.findItem(edge_st + f->edges[(ei+1) % num_e]))
					new_numfaces++;
			}
		}
	}

	// build new faces using edge info
	face_list.resize(new_numfaces);
	int nfi = 0; // new face index
	// edge data for new edge_list
	AATree_t<uint64, int> edge_id;
	HashTable<const ccVert*, Array<int>* > edge_verts;
	unsigned new_num_edges = 0;
	for (unsigned int fi=0; fi<num_faces; ++fi) {
		// orig. face
		const ccFace* OF = &old_face_list[fi];
		// verts and edges of orig. face
		const Array<int>& verts = OF->verts;
		const Array<int>& edges = OF->edges;

		// st test
		const Array<Point2>& stco = OF->stco;
		unsigned int numst = stco.size();
		Point2 st_center(0, 0);
		FixedArray<Point2> st_edges(numst);
		if (numst) {
			// st center & edge midpts
			for (unsigned int sti=0; sti<numst; ++sti) {
				st_edges[sti] = 0.5f*(stco[sti] + stco[(sti + 1) % numst]);
				st_center += stco[sti];
			}
			st_center /= float(numst);
		}

		const unsigned int mv = verts.size();
		// now build all new quads
		const unsigned int fci = face_st + fi;  // new face center vertex index
		const unsigned int num_e = edges.size();
		Array<int> vidx(4);
		for (unsigned int ei=0; ei<num_e; ++ei) {
			if (!vidx_rm.empty()) {
				// for patches, if any vertex no longer in vert_list, skip face
				vidx[0] = fci, vidx[1] = edge_st + edges[ei], vidx[2] = verts[(ei + 1) % mv], vidx[3] = edge_st + edges[(ei + 1) % num_e];
				const int *i1 = vidx_rm.findItem(fci),
				          *i2 = vidx_rm.findItem(edge_st + edges[ei]),
				          *i3 = vidx_rm.findItem(verts[(ei+1) % mv]),
				          *i4 = vidx_rm.findItem(edge_st + edges[(ei + 1) % num_e]);
				if ((i1 == NULL) || (i2 == NULL) || (i3 == NULL) || (i4 == NULL)) continue;
				vidx[0] = *i1;
				vidx[1] = *i2;
				vidx[2] = *i3;
				vidx[3] = *i4;
			}
			else {
				vidx[0] = fci;
				vidx[1] = edge_st + edges[ei];
				vidx[2] = verts[(ei+1) % mv];
				vidx[3] = edge_st + edges[(ei + 1) % num_e];
			}
			ccFace* nf = &face_list[nfi++];
			// copy flags from original face
			nf->flags = OF->flags;
			// quad vertices, edges updated later below
			nf->verts = vidx;

			// st test
			bool do_st = (numst != 0);
			if (fromPatch && (fi == 0) && (numst != 0)) do_st = true;
			if (do_st) {
				nf->stco.resize(4);
				const int sti = st_rot ? ((int)ei - 1) : 0;
				nf->stco[sti & 3] = st_center;
				nf->stco[(sti+1) & 3] = st_edges[ei];
				nf->stco[(sti+2) & 3] = stco[(ei + 1) % mv];
				nf->stco[(sti+3) & 3] = st_edges[(ei + 1) % num_e];
			}

			// set face ptrs in vert_list
			vert_list[vidx[0]].faces.push_back(nf);
			vert_list[vidx[1]].faces.push_back(nf);
			vert_list[vidx[2]].faces.push_back(nf);
			vert_list[vidx[3]].faces.push_back(nf);
			if (fromPatch) {
				// if patch, also set SD_MAINQUAD flag for all vertices on main quad
				if (OF->flags & SD_MAINQUAD) {
					vert_list[vidx[0]].flags |= SD_MAINQUAD;
					vert_list[vidx[1]].flags |= SD_MAINQUAD;
					vert_list[vidx[2]].flags |= SD_MAINQUAD;
					vert_list[vidx[3]].flags |= SD_MAINQUAD;
				}
			}
			// new edge data update
			const Array<int>& nverts = nf->verts;
			unsigned int nmv = nverts.size();
			for (unsigned int vi=0; vi<nmv; ++vi) {
				unsigned int ri1 = nverts[vi], ri2 = nverts[(vi+1) % nmv];
				unsigned int idx1 = ri1, idx2 = ri2;
				if (idx1 > idx2) SWAP(idx1, idx2);
				uint64 ID = idx1 + (uint64)num_verts*idx2;
				if (edge_id.findItem(ID) == NULL) {
					edge_id.addItem(ID, new_num_edges);
					const ccVert *v1 = &vert_list[ri1], *v2 = &vert_list[ri2];
					Array<int> **evi1 = edge_verts.findItem(v1), **evi2 = edge_verts.findItem(v2);
					if (evi1 == NULL) { edge_verts.addItem(v1, new Array<int>);  evi1 = edge_verts.findItem(v1); }
					if (evi2 == NULL) { edge_verts.addItem(v2, new Array<int>);  evi2 = edge_verts.findItem(v2); }
					(*evi1)->push_back(new_num_edges);
					(*evi2)->push_back(new_num_edges);
					new_num_edges++;
				}
			}
		}
	}

	// new edge_list, cannot use makeEdgeList, need old data to pass on crease flags,
	// can't do this in face build above, since it references both old & new face_list
	edge_list.resize(new_num_edges);
	nfi = 0;  // new face index
	for (unsigned int fi=0; fi<num_faces; ++fi) {
		const ccFace* OF = &old_face_list[fi];
		const Array<int>& verts = OF->verts;
		const Array<int>& edges = OF->edges;
		const unsigned int mv = verts.size();
		const unsigned int num_e = edges.size();
		const unsigned int fc = face_st + fi;
		for (unsigned int eidx=0; eidx<num_e; ++eidx) {
			int idx1 = fc, idx2 = edge_st + edges[eidx],
					idx3 = verts[(eidx + 1) % mv], idx4 = edge_st + edges[(eidx + 1) % num_e];
			if (!vidx_rm.empty()) {
				// for patches, if any vertex no longer in vert_list, skip face
				const int *i1 = vidx_rm.findItem(idx1),
				          *i2 = vidx_rm.findItem(idx2),
				          *i3 = vidx_rm.findItem(idx3),
				          *i4 = vidx_rm.findItem(idx4);
				if ((i1 == NULL) || (i2 == NULL) || (i3 == NULL) || (i4 == NULL)) continue;
				idx1 = *i1;
				idx2 = *i2;
				idx3 = *i3;
				idx4 = *i4;
			}
			ccEdge *E, *ed2, *ed3;
			SET_EDGE(edge_list, idx1, idx2, vert_list, face_list, num_verts, nfi);
			SET_EDGE(edge_list, idx2, idx3, vert_list, face_list, num_verts, nfi);  ed2 = E;
			SET_EDGE(edge_list, idx3, idx4, vert_list, face_list, num_verts, nfi);  ed3 = E;
			SET_EDGE(edge_list, idx4, idx1, vert_list, face_list, num_verts, nfi);
			nfi++;
			// pass on the crease flags, new edge verts have the crease flag set temporarily above
			if (vert_list[idx2].flags & SD_CREASE) ed2->flags |= SD_CREASE;
			if (vert_list[idx4].flags & SD_CREASE) ed3->flags |= SD_CREASE;
		}
	}
	setBoundaryFlags(edge_list);

	// delete array pointers in edge_verts
	edge_verts.clear_delete();

	// old face list now discarded
}

//---------------------------------------------------------------------------------------------------------

// For each face, make a subdivision patch.
// The patch consists of the main face, with its one ring neighbourhood of faces.
void makePatches(const FixedArray<ccVert> &vert_list,
                 const FixedArray<ccEdge> &edge_list, const FixedArray<ccFace> &face_list,
                 const Primitive* parentprim, const Framework* FW, splitbprims_t* spb, bool fromPatch)
{
	const unsigned int num_faces = face_list.size();

	int bptot=0, eptot=0, estot=0; // total bspline, extra-ordinary & explicit subdivision patches

	// if splitting a patch, there can be no more than 4 resulting subpatches
	const unsigned int maxf = fromPatch ? 4 : num_faces;
	// for each patch main face
	for (unsigned int fi=0; fi<maxf; ++fi)
	{
		const ccFace* mf = &face_list[fi]; // the main quad

		// new patch, possibly temporary, unless no other choice but explicit subdivision is left
		SDPatch* patch = new SDPatch();

		// set the patch main face index
		patch->mainface = fi;

		// if building from patch, skip faces not on the main quad
		if (fromPatch && ((mf->flags & SD_MAINQUAD)==0)) continue;

		// determine the extra-ordinary vertex
		for (Array<int>::const_iterator vi=mf->verts.begin(); vi!=mf->verts.end(); ++vi) {
			const int val = vert_list[*vi].edges.size();
			if ((val != 2) && (val != 4)) {	// ignore valence 2! see comment right below
				patch->eovert = *vi;
				break;	// only one e.o.vert
			}
		}
		// if no extra-ordinary vertex found, assign any vertex of the main face.
		// NOTE: It must have valence 4! There is a rather odd case where a certain
		// arrangement of quads seems to create a vertex with valence 2...
		// (not a corner vertex with two boundaries, it's an 'interior' vertex)
		// Test case: split a quad along one of its diagonals into two new quads.
		// (remove face only, connect diagonal and split which creates new vert
		// on diagonal edge, then make two new quads on either side of diagonal,
		// so end result will look like original quad is now a quad made out of two triangles)
		// Another example of a valence 2 vertex is on upper part of nose on suzanne mesh, which Pixie will discard as invalid.
		// But by accepting this as a regular vertex, the problem goes away, found no problems sofar anyway...
		if (patch->eovert == -1) {
			for (Array<int>::const_iterator vi=mf->verts.begin(); vi!=mf->verts.end(); ++vi) {
				if (vert_list[*vi].edges.size() == 4) {
					patch->eovert = *vi;
					break;
				}
			}
			// if still not found, possibly single or border face, look for valence 2 on boundary
			if (patch->eovert == -1) {
				for (Array<int>::const_iterator vi=mf->verts.begin(); vi!=mf->verts.end(); ++vi) {
					const ccVert* v = &vert_list[*vi];
					if ((v->edges.size() == 2) && (v->flags & SD_BOUNDARY)) {
						patch->eovert = *vi;
						break;
					}
				}
			}
			// if still not found, then fail... something weird is going on...
			assert(patch->eovert != -1); // really should never happen
		}

		// test for special flag, are all creases also boundaries?
		bool cr_eq_bd = true;
		for (Array<int>::const_iterator vi=mf->verts.begin(); vi!=mf->verts.end(); ++vi) {
			for (unsigned int ei=0; ei<vert_list[*vi].edges.size(); ++ei) {
				const ccEdge* e = &edge_list[ei];
				if (((e->flags & SD_BOUNDARY) && ((e->flags & SD_CREASE)==0)) ||
						((e->flags & SD_CREASE) && ((e->flags & SD_BOUNDARY)==0)))
				{
					cr_eq_bd = false;
					break;
				}
			}
			if (!cr_eq_bd) break;
		}
		if (cr_eq_bd) patch->flags |= SD_CREASE_EQ_BOUND;
		// test if face touches boundary
		// (test verts, not edges, since only 1 edge vert might be on border)
		for (Array<int>::const_iterator vi=mf->verts.begin(); vi!=mf->verts.end(); ++vi) {
			if (vert_list[*vi].flags & SD_BOUNDARY) {
				patch->flags |= SD_BOUNDARY;
				break;
			}
		}
		// test if patch is non-manifold (more than 2 faces per edge)
		// testing only edges of main face is not sufficient, have to
		// test each edge around main quad vertices
		for (Array<int>::const_iterator vi=mf->verts.begin(); vi!=mf->verts.end(); ++vi) {
			const Array<ccEdge*>& edl = vert_list[*vi].edges;
			for (Array<ccEdge*>::const_iterator e=edl.begin(); e!=edl.end(); ++e) {
				if ((*e)->faces.size() > 2) {
					patch->flags |= SD_NONMANIFOLD;
					break;
				}
			}
			if (patch->flags & SD_NONMANIFOLD) break;
		}
		// test if patch contains crease edges
		// (again test edges around verts as for non-manifold case, any vertex
		//  on main quad which is part of 'external' crease edge must also be considered)
		for (Array<int>::const_iterator vi=mf->verts.begin(); vi!=mf->verts.end(); ++vi) {
			const Array<ccEdge*>& edl = vert_list[*vi].edges;
			for (Array<ccEdge*>::const_iterator e=edl.begin(); e!=edl.end(); ++e) {
				if ((*e)->flags & SD_CREASE) {
					patch->flags |= SD_CREASE;
					break;
				}
			}
			if (patch->flags & SD_CREASE) break;
		}

		// Patch ring, all faces connected to the main face.
		// as for the nonmanifold test, testing faces from edges is not sufficient,
		// have to test all faces around each vertex.
		Array<ccFace*>& pr = patch->ring;
		for (Array<int>::const_iterator vi=mf->verts.begin(); vi!=mf->verts.end(); ++vi) {
			const ccVert* v = &vert_list[*vi];
			for (Array<ccFace*>::const_iterator f2=v->faces.begin(); f2!=v->faces.end(); ++f2)
				if ((mf != *f2) && (!pr.contains(*f2))) pr.push_back(*f2);
		}

		//-------------------------------------------------------------------------------------
		// Done, all flags & data are now set.
		// If possible, extract control points for patches that can be evaluated directly,
		// otherwise create data for an explicit subdivision patch.
		//-------------------------------------------------------------------------------------

		// only render boundary patches if interpolateboundary tag given
		if (!intpolBD && (patch->flags & SD_BOUNDARY)) {
			delete patch;
			continue;
		}

#ifdef SKIP_GLOBAL_SUBDIVISION
		// quad test
		bool allquad = (mf->edges.size() == 4);
		for (Array<ccFace*>::const_iterator pfi=pr.begin(); pfi!=pr.end(); ++pfi)
			if ((*pfi)->edges.size() != 4) {
				allquad = false;
				break;
			}

		int numeo = 0;
		for (Array<int>::const_iterator vi=mf->verts.begin(); vi<mf->verts.end(); ++vi) {
			const unsigned int val = vert_list[*vi].edges.size();
			if (val != 2) {
				if (val != ((vert_list[*vi].flags & SD_BOUNDARY) ? 3 : 4)) numeo++;
			}
		}
#endif

		// if patch is non-manifold, has creases, or valence is outside range 3 to 50,
		// then we already know we *must* dice using explicit subdivision.
		const int val = vert_list[patch->eovert].edges.size();
#ifdef SKIP_GLOBAL_SUBDIVISION
		if ((not allquad) or (numeo > 1) or
		    (patch->flags & SD_NONMANIFOLD) or (patch->flags & SD_CREASE) or (val < 3) or (val > 50))
#else
		if ((patch->flags & SD_NONMANIFOLD) or (patch->flags & SD_CREASE) or (val < 3) or (val > 50))
#endif
		{
			patch->makeExplicitSDPatch(vert_list, edge_list, face_list, parentprim);
			// this SDPatch is directly inserted or appended, so not deleted here
			if (spb) {
				if (!fromPatch) // split from initial global subdivision, create new BlurredPrimitive
					spb->bprims.push_back(new BlurredPrimitive(*spb->parent));
				spb->bprims[fi]->append(patch);
			}
			else
				FW->insert(patch);
			estot++;
			continue;
		}

		// Test if this is a regular patch
		bool bspok = true;
		int border[2] = {-1, -1};
		// must have 8 faces in ring
		if (patch->ring.size() != 8) {
			bspok = false;
			// patch may still be valid if on boundary,
			// quad arrangement must be something like:
			//  xxx           xxr
			//  rcr  and not  xcr     (x=empty, r=ring, c=center)
			//  rrr           rrr
			// This means that the number of faces in ring *must* be 5,
			// and main face *must* have a single border.
			// This next arrangement is also possible, 2 borders, 4 known faces,
			//  xxx
			//  xcr
			//  xrr
			// but for the no-crease patch case, I can't figure out the extrapolation of the
			// corner point yet, but judging from experiments trying to find a match by hand,
			// it doesn't seem to be impossible, the corner point tends to be far outside the patch.
			// If both boundaries are perfectly sharp however, then it's simple enough.
			// Other cases for now handled by explicit subdivision
			// (see comments in makeBSplinePatch())
			const unsigned int numrf = patch->ring.size();
			if ((numrf == 5) || (numrf == 3)) {
				int num_bd = 0;
				int bdix[2] = {0, 0};
				const Array<int>& edges = face_list[patch->mainface].edges;
				for (Array<int>::const_iterator ei=edges.begin(); ei!=edges.end(); ++ei) {
					if (edge_list[*ei].flags & SD_BOUNDARY) {
						if (num_bd<2) bdix[num_bd] = *ei;
						num_bd++;
					}
				}
				if (num_bd) {
					if (((numrf == 5) && (num_bd == 1)) ||
						((patch->flags & SD_CREASE_EQ_BOUND) && (numrf == 3) && (num_bd==2)))
					{
						bspok = true;
						border[0] = bdix[0];
						if (num_bd==2) border[1] = bdix[1];
					}
				}
			}
		}
		if (bspok) {
			// valence of all main face verts must either be 3 when on boundary, otherwise 4
			for (unsigned int fvi=0; fvi<mf->verts.size(); ++fvi) {
				const ccVert* v = &vert_list[mf->verts[fvi]];
				if (!(((v->flags & SD_BOUNDARY) && (v->edges.size() == 3)) || (v->edges.size() == 4))) {
					bspok = false;
					break;
				}
			}
		}
		if (bspok) {
			// a valid regular patch
			bptot++;
			BicubicPatch* newbsp = patch->makeBSplinePatch(vert_list, edge_list, face_list, border, parentprim);
			if (spb) {
				if (!fromPatch) // split from initial global subdivision, create new BlurredPrimitive
					spb->bprims.push_back(new BlurredPrimitive(*spb->parent));
				spb->bprims[fi]->append(newbsp);
			}
			else
				FW->insert(newbsp);
			// SDPatch can now be deleted
			delete patch;
			continue;
		}

		// other type boundary patches at this point also have to be diced by explicit subdivision...
		if (patch->flags & SD_BOUNDARY) {
			patch->makeExplicitSDPatch(vert_list, edge_list, face_list, parentprim);
			// this SDPatch is directly inserted into framework, so not deleted here
			if (spb) {
				if (!fromPatch) // split from initial global subdivision, create new BlurredPrimitive
					spb->bprims.push_back(new BlurredPrimitive(*spb->parent));
				spb->bprims[fi]->append(patch);
			}
			else
				FW->insert(patch);
			estot++;
			continue;
		}

		// final possibility, must be a valid extra-ordinary patch which can be evaluated directly
		eptot++;
		JS_SDPatch* newjsp = patch->makeJSPatch(vert_list, edge_list, face_list, parentprim);
		if (spb) {
			if (!fromPatch) // split from initial global subdivision, create new BlurredPrimitive
				spb->bprims.push_back(new BlurredPrimitive(*spb->parent));
			spb->bprims[fi]->append(newjsp);
		}
		else
			FW->insert(newjsp);

		// SDPatch can now be deleted
		delete patch;

	}
	
	//printf("Total bspline patches created: %d\n", bptot);
	//printf("Total extra-ordinary patches created: %d\n", eptot);
	//printf("Total explicit subdivision patches created: %d\n", estot);
	
}

//------------------------------------------------------------------------------
// SDPatch

// init static vars
tLinkedList_t<int, int*>* SDPatch::gridx_cache = NULL;
int SDPatch::gridx_refc = 0;

SDPatch::SDPatch():flags(SD_UNDEFINED), mainface(-1), eovert(-1)
{
	if (gridx_cache == NULL)
		gridx_cache = new tLinkedList_t<int, int*>(); // initial alloc
	gridx_refc++;
}

SDPatch::~SDPatch()
{
	if (--gridx_refc == 0) {
		gridx_cache->clear_delete_array();
		delete gridx_cache;
		gridx_cache = NULL;
	}
}


// helper function for control point extraction
inline const ccFace* neighbourFace(const Array<ccFace*>& faces, const ccFace* from_face)
{
	if (faces[0] != from_face) return faces[0];
	return faces[1];
}

inline void makeRtPoint(RtPoint dst, Point3 src)
{
	dst[0] = src.x, dst[1] = src.y, dst[2] = src.z;
}

// extract the 16 control points for a BSpline patch
BicubicPatch* SDPatch::makeBSplinePatch(const FixedArray<ccVert>& vert_list,
                                        const FixedArray<ccEdge>& edge_list, const FixedArray<ccFace>& face_list,
                                        int border[2], const Primitive* parentprim)
{
	// the center face
	const ccFace* cf = &face_list[mainface];

	// Patch control points order:
	//
	//  12---13---14----15
	//   | TL |    | TR |
	//   8----9---10----11
	//   |    |    |    |
	//   4----5----6----7
	//   | BL |    | BR |
	//   0----1----2----3
	//
	// all below is based on the assumption that verts 10-9-5-6 correspond to
	// the center quad verts 0-1-2-3, and similarly edges 10-9, 9-5, 5-6, 6-10
	// correspond to center quad edges 0-1-2-3.
	// In other words, quad verts 0-1 == quad edge 0, q.verts 1-2 == q.edge 1, etc.
	// Also, all control points are extracted according to the center quad orientation,
	// the orientation of the support quads is irrelevant.
	// Have to do this, since otherwise it could cause incorrect patches.
	// Mesh orientation is not always consistent,
	// possibly because of non-manifolds, or user 'errors'/intention.
	//
	// bspline patches on boundary can be handled similarly,
	// in that case, one row or column of control points is unknown,
	// but can be extrapolated. This is possible too for the corner patch case
	// (two rows unknown), but only if the borders are perfect sharp creases.
	// As of yet I can't figure out how to calculate the corner point of
	// a 'soft' bspline patch... but I can't imagine that would be impossible...
	// Trying to find a match by manually manipulating the corner vertex of a
	// bspline patch, the corner point seems to be lying completely outside it,
	// far beyond the other points, but what the exact relationship is, no clue yet...

	BicubicPatch* bp = new BicubicPatch();

	RtPoint* hull = bp->hull;
	// fill in pts 5/6/9/10 from center face, these can be set directly.
	makeRtPoint(hull[10], vert_list[cf->verts[0]].co);
	makeRtPoint(hull [9], vert_list[cf->verts[1]].co);
	makeRtPoint(hull [5], vert_list[cf->verts[2]].co);
	makeRtPoint(hull [6], vert_list[cf->verts[3]].co);

	// BDF bit 0: boundary -> 12 13 14 15
	//         1:          ->  0  4  8 12
	//         2:          ->  0  1  2  3
	//         3:          ->  3  7 11 15
	int BDF = 0;
	for (int bd=0; bd<2; bd++) {
		if (border[bd]==-1) continue;
		const ccEdge* e = &edge_list[border[bd]];
		if (((e->v1 == &vert_list[cf->verts[0]]) && (e->v2 == &vert_list[cf->verts[1]])) ||
				((e->v1 == &vert_list[cf->verts[1]]) && (e->v2 == &vert_list[cf->verts[0]])))
						BDF |= 1;
		if (((e->v1 == &vert_list[cf->verts[1]]) && (e->v2 == &vert_list[cf->verts[2]])) ||
				((e->v1 == &vert_list[cf->verts[2]]) && (e->v2 == &vert_list[cf->verts[1]])))
						BDF |= 2;
		if (((e->v1 == &vert_list[cf->verts[2]]) && (e->v2 == &vert_list[cf->verts[3]])) ||
				((e->v1 == &vert_list[cf->verts[3]]) && (e->v2 == &vert_list[cf->verts[2]])))
						BDF |= 4;
		if (((e->v1 == &vert_list[cf->verts[3]]) && (e->v2 == &vert_list[cf->verts[0]])) ||
				((e->v1 == &vert_list[cf->verts[0]]) && (e->v2 == &vert_list[cf->verts[3]])))
						BDF |= 8;
	}
	bool CORNER = (BDF && ((BDF & ((BDF << 1) - 1)) != BDF));

	if ((BDF & 1)==0) {
		// UPPER POINTS 13 & 14
		// from center face edge 0 (verts 9-10), get its neighbour face
		const int nb_edge = cf->edges[0];
		const ccFace* nb_face = neighbourFace(edge_list[nb_edge].faces, cf);
		const Array<int>* nfe = &nb_face->edges;
		// index of vert 9 on neighbour face
		int vidx = nb_face->verts.index(cf->verts[1]);
		const bool CCW = (nb_face->verts[(vidx + 1) & 3] == cf->verts[0]);
		if (CCW) {
			// next edge vert from 9 leads to vert 10, assume nb_face is ccw
			makeRtPoint(hull[13], vert_list[nb_face->verts[(vidx - 1) & 3]].co);
			makeRtPoint(hull[14], vert_list[nb_face->verts[(vidx - 2) & 3]].co);
		}
		else {
			// next edge vert from 9 leads to vert 13, assume nb_face is cw
			makeRtPoint(hull[13], vert_list[nb_face->verts[(vidx + 1) & 3]].co);
			makeRtPoint(hull[14], vert_list[nb_face->verts[(vidx + 2) & 3]].co);
		}

		// CORNER POINTS 12 & 15
		// from center face start vertex on neighbour face,
		// its idx+2 always leads to correct corner point, independent of orientation
		if ((BDF & 2)==0) {
			// get corner point 12 from face TL (neighbour of edge 9-13)
			const ccFace* TL;
			if (CCW)
				TL = neighbourFace(edge_list[(*nfe)[(nfe->index(nb_edge) - 1) & 3]].faces, nb_face);
			else
				TL = neighbourFace(edge_list[(*nfe)[(nfe->index(nb_edge) + 1) & 3]].faces, nb_face);
			vidx = TL->verts.index(cf->verts[1]);	// index of vert 9 on face TL
			makeRtPoint(hull[12], vert_list[TL->verts[(vidx + 2) & 3]].co);
		}
		if ((BDF & 8)==0) {
			// get corner point 15 from face TR (neigbour of edge 10-14
			const ccFace* TR;
			if (CCW)
				TR = neighbourFace(edge_list[(*nfe)[(nfe->index(nb_edge) + 1) & 3]].faces, nb_face);
			else
				TR = neighbourFace(edge_list[(*nfe)[(nfe->index(nb_edge) - 1) & 3]].faces, nb_face);
			vidx = TR->verts.index(cf->verts[0]);	// index of vert 10 on face TR
			makeRtPoint(hull[15], vert_list[TR->verts[(vidx + 2) & 3]].co);
		}
	}

	// MID POINTS
	if ((BDF & 2)==0) {
		// 4 & 8
		// from center face edge 1 (verts 9-5), get its neighbour face
		const int nb_edge = cf->edges[1];
		const ccFace* nb_face = neighbourFace(edge_list[nb_edge].faces, cf);
		// index of vert 9 on neighbour face
		const int vidx = nb_face->verts.index(cf->verts[1]);
		if (nb_face->verts[(vidx+1) & 3] == cf->verts[2]) {
			// next edge vert from 9 leads to vert 5
			makeRtPoint(hull[4], vert_list[nb_face->verts[(vidx - 2) & 3]].co);
			makeRtPoint(hull[8], vert_list[nb_face->verts[(vidx - 1) & 3]].co);
		}
		else {
			// next edge vert from 9 leads to vert 8
			makeRtPoint(hull[4], vert_list[nb_face->verts[(vidx + 2) & 3]].co);
			makeRtPoint(hull[8], vert_list[nb_face->verts[(vidx + 1) & 3]].co);
		}
	}
	if ((BDF & 8)==0) {
		// 7 & 11
		// from center face edge 3 (verts 6-10), get its neighbour face
		const int nb_edge = cf->edges[3];
		const ccFace* nb_face = neighbourFace(edge_list[nb_edge].faces, cf);
		// index of vert 6 on neighbour face
		const int vidx = nb_face->verts.index(cf->verts[3]);
		if (nb_face->verts[(vidx+1) & 3] == cf->verts[0]) {
			// next edge vert from 6 leads to vert 10
			makeRtPoint(hull[ 7], vert_list[nb_face->verts[(vidx - 1) & 3]].co);
			makeRtPoint(hull[11], vert_list[nb_face->verts[(vidx - 2) & 3]].co);
		}
		else {
			// next edge vert from 6 leads to vert 7
			makeRtPoint(hull[ 7], vert_list[nb_face->verts[(vidx + 1) & 3]].co);
			makeRtPoint(hull[11], vert_list[nb_face->verts[(vidx + 2) & 3]].co);
		}
	}

	// LOWER POINTS 1 & 2
	if ((BDF & 4)==0) {
		// from center face edge 2 (verts 5-6), get its neighbour face
		const int nb_edge = cf->edges[2];
		const ccFace* nb_face = neighbourFace(edge_list[nb_edge].faces, cf);
		const Array<int>* nfe = &nb_face->edges;
		// index of vert 6 on neighbour face
		int vidx = nb_face->verts.index(cf->verts[3]);
		const bool CCW = (nb_face->verts[(vidx+1) & 3] == cf->verts[2]);
		if (CCW) {
			// next edge vert from 6 leads to vert 5, assume nb_face is ccw
			makeRtPoint(hull[1], vert_list[nb_face->verts[(vidx - 2) & 3]].co);
			makeRtPoint(hull[2], vert_list[nb_face->verts[(vidx - 1) & 3]].co);
		}
		else {
			// next edge vert from 6 leads to vert 2, assume nb_face is cw
			makeRtPoint(hull[1], vert_list[nb_face->verts[(vidx + 2) & 3]].co);
			makeRtPoint(hull[2], vert_list[nb_face->verts[(vidx + 1) & 3]].co);
		}

		// CORNER POINTS 0 & 3
		if ((BDF & 2)==0) {
			// get corner point 0 from face BL (neighbour of edge 5-1)
			const ccFace* BL;
			if (CCW)
				BL = neighbourFace(edge_list[(*nfe)[(nfe->index(nb_edge) + 1) & 3]].faces, nb_face);
			else
				BL = neighbourFace(edge_list[(*nfe)[(nfe->index(nb_edge) - 1) & 3]].faces, nb_face);
			vidx = BL->verts.index(cf->verts[2]); // index of vert 5 on face TL
			makeRtPoint(hull[0], vert_list[BL->verts[(vidx + 2) & 3]].co);
		}
		if ((BDF & 8)==0) {
			// get corner point 3 from face BR (neighbour of edge 6-2)
			const ccFace* BR;
			if (CCW)
				BR = neighbourFace(edge_list[(*nfe)[(nfe->index(nb_edge) - 1) & 3]].faces, nb_face);
			else
				BR = neighbourFace(edge_list[(*nfe)[(nfe->index(nb_edge) + 1) & 3]].faces, nb_face);
			vidx = BR->verts.index(cf->verts[3]); // index of vert 6 on face TR
			makeRtPoint(hull[3], vert_list[BR->verts[(vidx + 2) & 3]].co);
		}
	}

	// fill in any missing border points
	if (BDF) {
		if (BDF & 1) {
			if (!(CORNER && (BDF ==  3))) {
				mulVVF(hull[12], hull[8], 2.f);
				subVVV(hull[12], hull[12], hull[4]);
			}
			mulVVF(hull[13], hull[9], 2.f);
			subVVV(hull[13], hull[13], hull[5]);
			mulVVF(hull[14], hull[10], 2.f);
			subVVV(hull[14], hull[14], hull[6]);
			if (!(CORNER && (BDF ==  9))) {
				mulVVF(hull[15], hull[11], 2.f);
				subVVV(hull[15], hull[15], hull[7]);
			}
		}
		if (BDF & 2) {
			if (!(CORNER && (BDF ==  6))) {
				mulVVF(hull[0], hull[1], 2.f);
				subVVV(hull[0], hull[0], hull[2]);
			}
			mulVVF(hull[4], hull[5], 2.f);
			subVVV(hull[4], hull[4], hull[6]);
			mulVVF(hull[8], hull[9], 2.f);
			subVVV(hull[8], hull[8], hull[10]);
			if (!(CORNER && (BDF ==  3))) {
				mulVVF(hull[12], hull[13], 2.f);
				subVVV(hull[12], hull[12], hull[14]);
			}
		}
		if (BDF & 4) {
			if (!(CORNER && (BDF ==  6))) {
				mulVVF(hull[0], hull[4], 2.f);
				subVVV(hull[0], hull[0], hull[8]);
			}
			mulVVF(hull[1], hull[5], 2.f);
			subVVV(hull[1], hull[1], hull[9]);
			mulVVF(hull[2], hull[6], 2.f);
			subVVV(hull[2], hull[2], hull[10]);
			if (!(CORNER && (BDF == 12))) {
				mulVVF(hull[3], hull[7], 2.f);
				subVVV(hull[3], hull[3], hull[11]);
			}
		}
		if (BDF & 8) {
			if (!(CORNER && (BDF == 12))) {
				mulVVF(hull[3], hull[2], 2.f);
				subVVV(hull[3], hull[3], hull[1]);
			}
			mulVVF(hull[7], hull[6], 2.f);
			subVVV(hull[7], hull[7], hull[5]);
			mulVVF(hull[11], hull[10], 2.f);
			subVVV(hull[11], hull[11], hull[9]);
			if (!(CORNER && (BDF ==  9))) {
				mulVVF(hull[15], hull[14], 2.f);
				subVVV(hull[15], hull[15], hull[13]);
			}
		}
		// corner vertex, if any
		if (CORNER) {
			// cpt = 4*P1 - 2*(P2 + P4) + P3
			// cpt -- x -- x ...
			//   |    |    |   ...
			//   x -- P1 - P4  ...
			//   |    |    |   ...
			//   x -- P2 - P3  ...
			// the points marked 'x' are already calculated above
			if (BDF == 3) {
				addVVV(hull[12], hull[5], hull[10]);
				mulVVF(hull[12], hull[12], -2.f);
				maddVVF(hull[12], hull[9], 4.f);
				addVVV(hull[12], hull[12], hull[6]);
			}
			else if (BDF == 6) {
				addVVV(hull[0], hull[6], hull[9]);
				mulVVF(hull[0], hull[0], -2.f);
				maddVVF(hull[0], hull[5], 4.f);
				addVVV(hull[0], hull[0], hull[10]);
			}
			else if (BDF == 12) {
				addVVV(hull[3], hull[10], hull[5]);
				mulVVF(hull[3], hull[3], -2.f);
				maddVVF(hull[3], hull[6], 4.f);
				addVVV(hull[3], hull[3], hull[9]);
			}
			else { // assume BDF == 9
				addVVV(hull[15], hull[6], hull[9]);
				mulVVF(hull[15], hull[15], -2.f);
				maddVVF(hull[15], hull[10], 4.f);
				addVVV(hull[15], hull[15], hull[5]);
			}
		}
	}

	/*
	// fill in any missing border points
	if (BDF) {
		if (BDF & 1) {
			if (!(CORNER && (BDF ==  3))) hull[12] = 2.f*hull[ 8] - hull[ 4];
			hull[13] = 2.f*hull[ 9] - hull[ 5];
			hull[14] = 2.f*hull[10] - hull[ 6];
			if (!(CORNER && (BDF ==  9))) hull[15] = 2.f*hull[11] - hull[ 7];
		}
		if (BDF & 2) {
			if (!(CORNER && (BDF ==  6))) hull[ 0] = 2.f*hull[ 1] - hull[ 2];
			hull[ 4] = 2.f*hull[ 5] - hull[ 6];
			hull[ 8] = 2.f*hull[ 9] - hull[10];
			if (!(CORNER && (BDF ==  3))) hull[12] = 2.f*hull[13] - hull[14];
		}
		if (BDF & 4) {
			if (!(CORNER && (BDF ==  6))) hull[ 0] = 2.f*hull[ 4] - hull[ 8];
			hull[ 1] = 2.f*hull[ 5] - hull[ 9];
			hull[ 2] = 2.f*hull[ 6] - hull[10];
			if (!(CORNER && (BDF == 12))) hull[ 3] = 2.f*hull[ 7] - hull[11];
		}
		if (BDF & 8) {
			if (!(CORNER && (BDF == 12))) hull[ 3] = 2.f*hull[ 2] - hull[ 1];
			hull[ 7] = 2.f*hull[ 6] - hull[ 5];
			hull[11] = 2.f*hull[10] - hull[ 9];
			if (!(CORNER && (BDF ==  9))) hull[15] = 2.f*hull[14] - hull[13];
		}
		// corner vertex, if any
		if (CORNER) {
			// cpt = 4*P1 - 2*(P2 + P4) + P3
			// cpt -- x -- x ...
			//   |    |    |   ...
			//   x -- P1 - P4  ...
			//   |    |    |   ...
			//   x -- P2 - P3  ...
			// the points marked 'x' are already calculated above
			if (BDF == 3)
				hull[12] = 4.f*hull[ 9] - 2.f*(hull[ 5] + hull[10]) + hull[ 6];
			else if (BDF == 6)
				hull[ 0] = 4.f*hull[ 5] - 2.f*(hull[ 6] + hull[ 9]) + hull[10];
			else if (BDF == 12)
				hull[ 3] = 4.f*hull[ 6] - 2.f*(hull[10] + hull[ 5]) + hull[ 9];
			else // assume BDF == 9
				hull[15] = 4.f*hull[10] - 2.f*(hull[ 6] + hull[ 9]) + hull[ 5];
		}
	}
	*/

	// done, finalize BSpline patch.
	static_cast<Primitive&>(*bp) = *parentprim;	// copy data

	// new st coords
	if (!cf->stco.empty()) {
		PrimVars* npv = bp->newPrimVars();
		decParam_t dp = {SC_FACEVARYING|DT_FLOAT, 1, 4};
		vardata_t* nvdt = new vardata_t(dp);
		nvdt->data = new float[4];
		// not good this, had to go through several permutations to find the right st vertex order,
		// it's a sign of bad coding, having to once again basically guess my way to the solution...
		// 32 23 10 01	x/y mirror
		// 01 10 23 32
		//
		// 32 20 01 13	rotation
		// 01 13 32 20
		nvdt->data[3] = cf->stco[0].x;
		nvdt->data[2] = cf->stco[1].x;
		nvdt->data[0] = cf->stco[2].x;
		nvdt->data[1] = cf->stco[3].x;
		npv->pvars.addItem("s", nvdt);
		nvdt = new vardata_t(dp);
		nvdt->data = new float[4];
		nvdt->data[3] = cf->stco[0].y;
		nvdt->data[2] = cf->stco[1].y;
		nvdt->data[0] = cf->stco[2].y;
		nvdt->data[1] = cf->stco[3].y;
		npv->pvars.addItem("t", nvdt);
	}

	// if this patch is NOT split from explicit subd patch,
	// then post_init() could be skipped, since already converted to bezier and in camspace
	// but converting bezier to bezier is ok, nothing changes. still, needless xtra overhead
	bp->subd_bspline = true;
	bp->post_init();
	return bp;
}


// extract control points for an irregular patch which can be evaluated directly
JS_SDPatch* SDPatch::makeJSPatch(const FixedArray<ccVert>& vert_list,
				const FixedArray<ccEdge>& edge_list, const FixedArray<ccFace>& face_list,
				const Primitive* parentprim)
{
	// the center face
	const ccFace* cf = &face_list[mainface];

	// patch control points order, starting from the extra-ordinary vertex 0:
	// (numbers are -1 with respect to paper here, base 0)
	//  ^  2N
	//  |  :
	//  |  8 ---- 1 ---- 2 --- 2N+7
	//  |  |      |      |      |
	// eov 7 ---- 0 ---- 3 --- 2N+6
	//     |      |      |      |
	//     6 ---- 5 ---- 4 --- 2N+5
	//     |      |      |      |
	//  2N+4 -- 2N+3 -- 2N+2 - 2N+1
	//
	// verts 0 to 6 and 2N+1 to 2N+7 are fixed
	// 'eov' contains the vertices connected to irregular vert 0, verts 7 to 2N.
	// if valence is 3, there is no eov group, and vert 6 connects to 1.
	// As in makeBSplinePatch(), everything is done according to the orientation of
	// the center quad only, support face orientation is irrelevant.
	// Main face orientation is assumed to be ccw, and verts 0/5, 5/4, 4/3, 3/0
	// correspond to edges 0, 1, 2, 3 on main face.
	// Since general shape is symmetric, above still valid if ct.quad is cw instead of ccw

	// 0/3/4/5 can be set from the main face
	const ccVert* eov = &vert_list[eovert]; // the extra-ordinary vertex
	const unsigned int N = eov->edges.size(); // valence
	const unsigned int N2 = N*2;

	JS_SDPatch* jsp = new JS_SDPatch(N);
	Point3* hull = jsp->jsdata->ecp;

	// vertex indices of the main quad, assuming ccw order (actual ori. doesn't matter)
	const int v0_idx = eovert;
	// index of vert 0 on main face, also used to index corresponding edges
	const int fv0_idx = cf->verts.index(v0_idx);
	const int fv1_idx = (fv0_idx + 1) & 3;
	const int fv2_idx = (fv0_idx + 2) & 3;
	const int fv3_idx = (fv0_idx + 3) & 3;
	const int v5_idx = cf->verts[fv1_idx];
	const int v4_idx = cf->verts[fv2_idx];
	const int v3_idx = cf->verts[fv3_idx];

	static_cast<Primitive&>(*jsp) = *parentprim;	// copy data

	// new st coords
	if (!cf->stco.empty()) {
		PrimVars* npv = jsp->newPrimVars();
		decParam_t dp = {SC_FACEVARYING|DT_FLOAT, 1, 4};
		vardata_t* nvdt = new vardata_t(dp);
		nvdt->data = new float[4];
		nvdt->data[0] = cf->stco[fv0_idx].x;
		nvdt->data[1] = cf->stco[fv1_idx].x;
		nvdt->data[3] = cf->stco[fv2_idx].x;
		nvdt->data[2] = cf->stco[fv3_idx].x;
		npv->pvars.addItem("s", nvdt);
		nvdt = new vardata_t(dp);
		nvdt->data = new float[4];
		nvdt->data[0] = cf->stco[fv0_idx].y;
		nvdt->data[1] = cf->stco[fv1_idx].y;
		nvdt->data[3] = cf->stco[fv2_idx].y;
		nvdt->data[2] = cf->stco[fv3_idx].y;
		npv->pvars.addItem("t", nvdt);
	}

	// starting at vert 0 on main face, get points 5/4/3
	hull[0] = eov->co;
	hull[5] = vert_list[v5_idx].co;
	hull[4] = vert_list[v4_idx].co;
	hull[3] = vert_list[v3_idx].co;

	// from edge 0/3, go to neighbour face
	int nb_edge = cf->edges[(fv0_idx - 1) & 3];
	const ccFace* nb_face = neighbourFace(edge_list[nb_edge].faces, cf);
	// neighbour face edges
	const Array<int>* nfe = &nb_face->edges;
	// index of vert 0 on nb_face
	int vidx = nb_face->verts.index(v0_idx);
	// get verts 1 & 2 and next neighbour face across edge 2/3
	if (nb_face->verts[(vidx + 1) & 3] == v3_idx) {
		hull[1] = vert_list[nb_face->verts[(vidx - 1) & 3]].co;
		hull[2] = vert_list[nb_face->verts[(vidx - 2) & 3]].co;
		nb_face = neighbourFace(edge_list[(*nfe)[(nfe->index(nb_edge) + 1) & 3]].faces, nb_face);
	}
	else {
		hull[1] = vert_list[nb_face->verts[(vidx + 1) & 3]].co;
		hull[2] = vert_list[nb_face->verts[(vidx + 2) & 3]].co;
		nb_face = neighbourFace(edge_list[(*nfe)[(nfe->index(nb_edge) - 1) & 3]].faces, nb_face);
	}

	// index of vert 3 on nb_face
	vidx = nb_face->verts.index(v3_idx);
	// get vert 2N+7 (since quad, corner points always index +-2 indep.of ori.)
	hull[N2 + 7] = vert_list[nb_face->verts[(vidx + 2) & 3]].co;

	// from edge 3/4, go to neighbour face
	nb_edge = cf->edges[(fv0_idx - 2) & 3];
	nb_face = neighbourFace(edge_list[nb_edge].faces, cf);
	// neighbour face edges
	nfe = &nb_face->edges;
	// index of vert 3 on nb_face
	vidx = nb_face->verts.index(v3_idx);
	// get verts 2N+6 & 2N+5 and next neighbour face across edge 4/2N+5
	if (nb_face->verts[(vidx + 1) & 3] == v4_idx) {
		hull[N2+5] = vert_list[nb_face->verts[(vidx - 2) & 3]].co;
		hull[N2+6] = vert_list[nb_face->verts[(vidx - 1) & 3]].co;
		nb_face = neighbourFace(edge_list[(*nfe)[(nfe->index(nb_edge) + 1) & 3]].faces, nb_face);
	}
	else {
		hull[N2+5] = vert_list[nb_face->verts[(vidx + 2) & 3]].co;
		hull[N2+6] = vert_list[nb_face->verts[(vidx + 1) & 3]].co;
		nb_face = neighbourFace(edge_list[(*nfe)[(nfe->index(nb_edge) - 1) & 3]].faces, nb_face);
	}

	// index of vert 4 on nb_face
	vidx = nb_face->verts.index(v4_idx);
	// get vert 2N+1 (since quad, corner points always index +-2 indep.of ori.)
	hull[N2 + 1] = vert_list[nb_face->verts[(vidx + 2) & 3]].co;

	// from edge 4/5, go to neighbour face
	nb_edge = cf->edges[(fv0_idx - 3) & 3];
	nb_face = neighbourFace(edge_list[nb_edge].faces, cf);
	// neighbour face edges
	nfe = &nb_face->edges;
	// index of vert 5 on nb_face
	vidx = nb_face->verts.index(v5_idx);
	// get verts 2N+2 & 2N+3 and next neighbour face across edge 5/2N+3
	if (nb_face->verts[(vidx + 1) & 3] == v4_idx) {
		hull[N2+2] = vert_list[nb_face->verts[(vidx - 2) & 3]].co;
		hull[N2+3] = vert_list[nb_face->verts[(vidx - 1) & 3]].co;
		nb_face = neighbourFace(edge_list[(*nfe)[(nfe->index(nb_edge) - 1) & 3]].faces, nb_face);
	}
	else {
		hull[N2+2] = vert_list[nb_face->verts[(vidx + 2) & 3]].co;
		hull[N2+3] = vert_list[nb_face->verts[(vidx + 1) & 3]].co;
		nb_face = neighbourFace(edge_list[(*nfe)[(nfe->index(nb_edge) + 1) & 3]].faces, nb_face);
	}

	// index of vert 5 on nb_face
	vidx = nb_face->verts.index(v5_idx);
	// get vert 2N+4 (since quad, corner points always index +-2 indep.of ori.)
	hull[N2 + 4] = vert_list[nb_face->verts[(vidx + 2) & 3]].co;

	// from edge 5/0, go to neighbour face
	nb_edge = cf->edges[fv0_idx];
	nb_face = neighbourFace(edge_list[nb_edge].faces, cf);
	// index of vert 0 on nb_face
	vidx = nb_face->verts.index(v0_idx);
	// get vert 6
	hull[6] = vert_list[nb_face->verts[(vidx + 2) & 3]].co;

	// if valence is 3, nothing to do further, no eov group
	if (N > 3) {

		// the remaining group of vertices connected to extra-ordinary vertex 0
		// there will be two extra verts per valence up
		// (valence/extra_verts: 3/0, 4/2, 5/4, 6/6, etc)
		// first and last vert are handled separately

		// vert 7
		// neighbour face edges
		int last_vidx, last_eidx;
		if (nb_face->verts[(vidx + 1) & 3] == v5_idx) {
			// needed for orientation test of next face, the index of vert 7 in vert_list
			last_vidx = nb_face->verts[(vidx - 1) & 3];
			last_eidx = nb_face->edges[(vidx - 1) & 3];
		}
		else {
			last_vidx = nb_face->verts[(vidx + 1) & 3];
			last_eidx = nb_face->edges[vidx];
		}
		hull[7] = vert_list[last_vidx].co;

		if (N == 4) {
			// only one vertex left for valence 4 (vert 8)
			nb_face = neighbourFace(edge_list[last_eidx].faces, nb_face);
			// index of vert 0 on nb_face
			vidx = nb_face->verts.index(v0_idx);
			if (nb_face->verts[(vidx+1) & 3] == last_vidx)
				hull[8] = vert_list[nb_face->verts[(vidx + 2) & 3]].co;
			else
				hull[8] = vert_list[nb_face->verts[(vidx - 2) & 3]].co;
		}
		else {
			// N -> 5 - 50
			for (unsigned int i=8; i<N2; i+=2) {
				nb_face = neighbourFace(edge_list[last_eidx].faces, nb_face);
				// index of vert 0 on nb_face
				vidx = nb_face->verts.index(v0_idx);
				if (nb_face->verts[(vidx + 1) & 3] == last_vidx) {
					last_vidx = nb_face->verts[(vidx + 3) & 3];
					last_eidx = nb_face->edges[(vidx - 1) & 3];
					hull[i] = vert_list[nb_face->verts[(vidx + 2) & 3]].co;
					hull[i+1] = vert_list[last_vidx].co;
				}
				else {
					last_vidx = nb_face->verts[(vidx - 3) & 3];
					last_eidx = nb_face->edges[vidx];
					hull[i] = vert_list[nb_face->verts[(vidx - 2) & 3]].co;
					hull[i+1] = vert_list[last_vidx].co;
				}
			}

			// last vertex N2, corner, so no orientation test needed here
			nb_face = neighbourFace(edge_list[last_eidx].faces, nb_face);
			// index of vert 0 on nb_face
			vidx = nb_face->verts.index(v0_idx);
			hull[N2] = vert_list[nb_face->verts[(vidx + 2) & 3]].co;
		}
	}

	// done
	jsp->post_init();
	return jsp;
}


// create new vert, edge and face lists from this patch
void SDPatch::makeExplicitSDPatch(const FixedArray<ccVert>& vert_list,
				const FixedArray<ccEdge>& edge_list, const FixedArray<ccFace>& face_list,
				const Primitive* parentprim)
{
	HashTable<const ccVert*, int> new_idx;  // vert index remap
	int cur_idx = 0;
	// determine new number of verts,
	// number of patch faces is known beforehand,
	// so patch face list can be filled in right away
	patch_faces.resize(ring.size() + 1);
	// mainquad
	int num_patch_verts = 0;
	// main face
	const Array<int>& fv = face_list[mainface].verts;
	for (Array<int>::const_iterator vi=fv.begin(); vi!=fv.end(); ++vi) {
		const ccVert* v = &vert_list[*vi];
		const int* idxp = new_idx.findItem(v);
		int idx;
		if (idxp)
			idx = *idxp;
		else {
			num_patch_verts++;
			idx = cur_idx;
			new_idx.addItem(v, idx);
			cur_idx++;
		}
		patch_faces[0].verts.push_back(idx);
	}
	// copy stco, only needed for main quad
	patch_faces[0].stco = face_list[mainface].stco;
	// set main quad flag
	patch_faces[0].flags |= SD_MAINQUAD;
	// ring vertices
	for (unsigned int fi=0; fi<ring.size(); ++fi) {
		const Array<int>& prv = ring[fi]->verts;
		for (Array<int>::const_iterator vi=prv.begin(); vi!=prv.end(); ++vi) {
			const ccVert* v = &vert_list[*vi];
			const int* idxp = new_idx.findItem(v);
			int idx;
			if (idxp)
				idx = *idxp;
			else {
				num_patch_verts++;
				idx = cur_idx;
				new_idx.addItem(v, idx);
				cur_idx++;
			}
			patch_faces[fi+1].verts.push_back(idx);
		}
	}

	// now fill in the patch vert list
	patch_verts.resize(num_patch_verts);
	// set flag to make sure ring vert coords & flags are only (re)set once
	for (int i=0; i<num_patch_verts; ++i)
		patch_verts[i].flags = SD_NOTSET;
	// main face
	for (Array<int>::const_iterator vi=fv.begin(); vi!=fv.end(); ++vi) {
		const ccVert* v = &vert_list[*vi];
		ccVert* pv = &patch_verts[*new_idx.findItem(v)];
		// copy flags from previous vertex, add SD_MAINQUAD flag
		pv->co = v->co;
		pv->flags = v->flags | SD_MAINQUAD;
	}
	// ring vertices
	for (unsigned int fi=0; fi<ring.size(); ++fi) {
		const Array<int>& prv = ring[fi]->verts;
		for (Array<int>::const_iterator vi=prv.begin(); vi!=prv.end(); ++vi) {
			const ccVert* v = &vert_list[*vi];
			ccVert* pv = &patch_verts[*new_idx.findItem(v)];
			if (pv->flags == SD_NOTSET) { // only set once!
				// new vertex, copy the flags from previous vertex
				pv->co = v->co;
				// since data may require rebuilding when subpatches split,
				// old SD_MAINQUAD flag *must* be cleared
				pv->flags = v->flags & ~SD_MAINQUAD;
			}
		}
	}

	// set face ptrs in patch_verts list
	for (unsigned int fi=0; fi<patch_faces.size(); ++fi) {
		ccFace* f = &patch_faces[fi];
		for (Array<int>::const_iterator vi=f->verts.begin(); vi!=f->verts.end(); ++vi)
			patch_verts[*vi].faces.push_back(f);
	}

	// init edge list
	unsigned int num_patch_edges;
	INIT_EDGES(patch_verts, patch_faces, num_patch_verts, patch_faces.size(), num_patch_edges)

	// finalize edge list
	patch_edges.resize(num_patch_edges);
	// main quad edges
	const Array<int>& fce = face_list[mainface].edges;
	for (Array<int>::const_iterator eidx=fce.begin(); eidx!=fce.end(); ++eidx) {
		const ccEdge* e = &edge_list[*eidx];
		ccEdge* E;
		const unsigned int i1 = *new_idx.findItem(e->v1), i2 = *new_idx.findItem(e->v2);
		SET_EDGE(patch_edges, i1, i2, patch_verts, patch_faces, num_patch_verts, 0);
		E->flags = e->flags;  // copy flags
	}
	// ring edges
	for (unsigned int fi=0; fi<ring.size(); ++fi) {
		const Array<int>& re = ring[fi]->edges;
		for (Array<int>::const_iterator eidx=re.begin(); eidx!=re.end(); ++eidx) {
			const ccEdge* e = &edge_list[*eidx];
			ccEdge* E;
			const unsigned int i1 = *new_idx.findItem(e->v1), i2 = *new_idx.findItem(e->v2), fi1 = fi + 1;
			SET_EDGE(patch_edges, i1, i2, patch_verts, patch_faces, num_patch_verts, fi1);
			E->flags = e->flags;  // copy flags
		}
	}

	// free alloced arrays in edge_verts
	edge_verts.clear_delete();

	// set boundary edge flags
	setBoundaryFlags(patch_edges);

	// for the new patch, mainface is now face number 0
	mainface = 0;
	// calc bound
	for (unsigned int i=0; i<patch_verts.size(); ++i)
		bnd.include(patch_verts[i].co);
	bnd.addEpsilon();
	// done
	static_cast<Primitive&>(*this) = *parentprim;	// copy data
}


void SDPatch::split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb)
{
	// do one subdivision step and make four new subpatches
	// u/v split flags ignored in this case, since this always is a 4-way split
	if (usplit or vsplit) { 
		Subdivide(patch_verts, patch_edges, patch_faces, true);
		rebuildLists(patch_verts, patch_edges, patch_faces, true);
		if (spb) {
			if (spb->bprims.empty()) {
				spb->bprims.resize(4);
				spb->bprims[0] = new BlurredPrimitive(*spb->parent);
				spb->bprims[1] = new BlurredPrimitive(*spb->parent);
				spb->bprims[2] = new BlurredPrimitive(*spb->parent);
				spb->bprims[3] = new BlurredPrimitive(*spb->parent);
			}
		}
		makePatches(patch_verts, patch_edges, patch_faces, this, &f, spb, true);
	}
}

bool SDPatch::diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit)
{
	Array<int>& fv = patch_faces[mainface].verts;
#ifdef SKIP_GLOBAL_SUBDIVISION
		// if patch not all quad, or more than 1 eo.vert on main face, must split first
		if ((flags & SD_ALLQUAD) == 0) {
			if (patch_faces[mainface].edges.size() != 4) {
				usplit = vsplit = true;
				return false;
			}
			for (Array<ccFace*>::const_iterator fi=ring.begin(); fi!=ring.end(); ++fi)
				if ((*fi)->edges.size() != 4) {
					usplit = vsplit = true;
					return false;
				}
			flags |= SD_ALLQUAD;
		}
		if (((flags & SD_NONMANIFOLD) == 0) and ((flags & SD_ONE_EOV) == 0)) {
			int numeo = 0;
			for (Array<int>::const_iterator vi=fv.begin(); vi<fv.end(); ++vi) {
				const unsigned int val = patch_verts[*vi].edges.size();
				if ((val != 2) and (val != ((patch_verts[*vi].flags & SD_BOUNDARY) ? 3 : 4))) {
					if (++numeo > 1) {
						usplit = vsplit = true;
						return false;
					}
				}
			}
			flags |= SD_ONE_EOV;
		}
#endif
	// use max dist. projected corners of main quad to estimate dicing rate and subdiv level
	float maxDist = -1e10f;
	maxDist = MAX2(h.rasterEstimate(patch_verts[fv[0]].co, patch_verts[fv[1]].co), maxDist);
	maxDist = MAX2(h.rasterEstimate(patch_verts[fv[1]].co, patch_verts[fv[2]].co), maxDist);
	maxDist = MAX2(h.rasterEstimate(patch_verts[fv[2]].co, patch_verts[fv[3]].co), maxDist);
	maxDist = MAX2(h.rasterEstimate(patch_verts[fv[3]].co, patch_verts[fv[0]].co), maxDist);
	// the minimum allowed size is 2X2 faces ( == 3X3 verts) (2nd ord. differencing for tangents)
	const float shadingRate = getAttributeReference()->effectiveShadingRate;
	unsigned int dim = 1 << MAX2(1, int(0.5f + logf(MAX2(1e-7f, 0.7071068f*maxDist) / shadingRate)*(float)M_LOG2E));
	g.setDim(dim, dim, this);
	dim++;
	if ((dim < h.gridSize()) && ((dim*dim) < h.gridSize())) return true;
	usplit = vsplit = true;
	return false;
}

//---------------------------------------------------------------------------------------------
// Support code to remap the inner quad vertices (which are all mixed up because of several
// subdivision iterations), to a regular grid.
// This eliminates the need for lookup by edge traversal. Though it is still rather slow...
// updated with similar idea for face st index, needed for st coords only
void makeGridIndex(int* gridx, int numv)
{
	// base quad, with the x/y coords set to the max grid corners
	FixedArray<ccVert> vert_list(4);
	vert_list[3].co.set(0, 0, 0);
	vert_list[2].co.set(0, numv-1, 0);
	vert_list[1].co.set(numv-1, numv-1, 0);
	vert_list[0].co.set(numv-1, 0, 0);
	FixedArray<ccFace> face_list(1);
	face_list[0].verts.resize(4);
	face_list[0].edges.resize(4);
	for (int i=0; i<4; i++) {
		face_list[0].verts[i] = i;
		face_list[0].edges[i] = i;
	}
	const int numf = numv - 1;	// number of faces per side
	face_list[0].stco.resize(4);
	face_list[0].stco[3].set(0.f, 0.f);
	face_list[0].stco[2].set(0.f, (float)numf);
	face_list[0].stco[1].set((float)numf, (float)numf);
	face_list[0].stco[0].set((float)numf, 0.f);
	FixedArray<ccEdge> edge_list(4);
	edge_list[0].v1 = &vert_list[0], edge_list[0].v2 = &vert_list[1];
	edge_list[1].v1 = &vert_list[1], edge_list[1].v2 = &vert_list[2];
	edge_list[2].v1 = &vert_list[2], edge_list[2].v2 = &vert_list[3];
	edge_list[3].v1 = &vert_list[3], edge_list[3].v2 = &vert_list[0];
	int num_verts = 4, num_faces = 1, num_edges = 4;
	// only need to fill in the data needed to rebuild lists, so vert_list faces/edges
	// and edge_list faces do not need to be set here.
	// Subdivide the quad grid (simple subdivision, not cc)
	const int mL = ilog2(numf);
	for (int L=0; L<mL; ++L) {
		const int new_numverts = num_verts + num_faces + num_edges;
		FixedArray<ccVert> new_verts(new_numverts);
		int nvi = num_verts;
		for (int fi=0; fi<num_faces; ++fi) {
			ccFace* f = &face_list[fi];
			new_verts[nvi].co = (vert_list[f->verts[0]].co + vert_list[f->verts[1]].co +
			                     vert_list[f->verts[2]].co + vert_list[f->verts[3]].co) * 0.25f;
			f->center = &new_verts[nvi++];
		}
		for (int ei=0; ei<num_edges; ++ei) {
			ccEdge* e = &edge_list[ei];
			new_verts[nvi].co = (e->v1->co + e->v2->co) * 0.5f;
			e->new_vert = &new_verts[nvi++];
		}
		for (nvi=0; nvi<num_verts; ++nvi)
			new_verts[nvi].co = vert_list[nvi].co;
		vert_list = new_verts;
		num_verts = new_numverts;
		rebuildLists(vert_list, edge_list, face_list, false, true);
		num_faces = face_list.size();
		num_edges = edge_list.size();
	}
	// the resulting x/y coords can now be used to create an index which maps
	// the subdivision result vert indices to a sequentially indexed grid [0,1,2,3,4...]
	for (int vi=0; vi<numv; ++vi) {
		for (int ui=0; ui<numv; ++ui) {
			const int sqidx = ui + vi*numv;
			const ccVert* v = &vert_list[sqidx];
			gridx[(int(v->co.x) + int(v->co.y)*numv)*2] = sqidx;
		}
	}
	// similar for face indices (st coords only)
	for (int vi=0; vi<numf; ++vi) {
		for (int ui=0; ui<numf; ++ui) {
			const int sqidx = ui + vi*numf;
			const ccFace* f = &face_list[sqidx];
			const Point2 co = 0.25f*(f->stco[0] + f->stco[1] + f->stco[2] + f->stco[3]);
			gridx[(int(co.x) + int(co.y)*numf)*2 + 1] = sqidx;
		}
	}
}

void SDPatch::dice(MicroPolygonGrid &g, bool Pclose)
{
	const int xdim = g.get_xdim(), ydim = g.get_ydim();
	// do the required number of subdiv steps
	const unsigned int sdl = ilog2(xdim);
	for (unsigned int L=0; L<sdl; ++L) {
		Subdivide(patch_verts, patch_edges, patch_faces, true);
		rebuildLists(patch_verts, patch_edges, patch_faces, true, true);
	}
	// projection to limit surface
	Subdivide(patch_verts, patch_edges, patch_faces, true, true);

	const unsigned int gsz = xdim + 1;

	// only want mainquad data
	// verts
	Array<ccVert*> nv;
	for (unsigned int i=0; i<patch_verts.size(); ++i)
	for (VertIter vi=patch_verts.begin(); vi!=patch_verts.end(); ++vi)
		if (vi->flags & SD_MAINQUAD) nv.push_back(&(*vi));
	// st coords
	Array<Point2> stco;
	bool have_st = (!patch_faces[0].stco.empty());
	if (have_st) {
		for (FaceConstIter fci=patch_faces.begin(); fci!=patch_faces.end(); ++fci) {
			if (fci->flags & SD_MAINQUAD) {
				for (Array<Point2>::const_iterator sti=fci->stco.begin(); sti!=fci->stco.end(); ++sti)
					stco.push_back(*sti);
			}
		}
	}

	// cache the index grids,
	// this significantly reduces time otherwise spent constantly recalculating it again and again
	// usually, only a small set of resolutions are needed (<=5),
	// but depends on max microgrid size.
	int** gridx = gridx_cache->findItem(gsz);
	if (gridx==NULL) {
		int* new_gridx = new int[gsz*gsz*2];
		memset(new_gridx, 0, sizeof(int)*gsz*gsz*2);
		makeGridIndex(new_gridx, gsz);
		gridx_cache->addItem(gsz, new_gridx);
		gridx = &new_gridx;
	}
	RtPoint* P_grid = (RtPoint*)(Pclose ? g.addVariable("=Pclose") : g.addVariable("P"));
	RtVector* dPdu_grid = (RtVector*)g.addVariable("dPdu");
	RtVector* dPdv_grid = (RtVector*)g.addVariable("dPdv");
	int ug, vg, idx = 0;
	for (vg=0; vg<=ydim; ++vg)
		for (ug=0; ug<=xdim; ++ug, ++idx) {
			const Point3& vco = nv[(*gridx)[idx << 1]]->co;
			P_grid[idx][0] = vco.x, P_grid[idx][1] = vco.y, P_grid[idx][2] = vco.z;
		}

	// temporary, other vars TODO, only st for now
	// 03 21
	// 12 30
	g.initVars(std_dice);
	float *s_grid = NULL, *t_grid = NULL;
	if (have_st) {
		s_grid = g.findVariable("s"), t_grid = g.findVariable("t");
		assert((s_grid!=NULL) && (t_grid!=NULL));
		idx = 0;
		for (vg=0; vg<=ydim; ++vg) {
			for (ug=0; ug<=xdim; ++ug, ++idx) {
				if ((ug == xdim) && (vg == ydim)) {
					const int stidx = (*gridx)[(ug-1 + (vg-1)*xdim)*2 + 1] << 2;
					s_grid[idx] = stco[stidx+1].x;
					t_grid[idx] = stco[stidx+1].y;
				}
				else if (ug == xdim) {
					const int stidx = (*gridx)[(ug-1 + vg*xdim)*2 + 1] << 2;
					s_grid[idx] = stco[stidx].x;
					t_grid[idx] = stco[stidx].y;
				}
				else if (vg == ydim) {
					const int stidx = (*gridx)[(ug + (vg-1)*xdim)*2 + 1] << 2;
					s_grid[idx] = stco[stidx+2].x;
					t_grid[idx] = stco[stidx+2].y;
				}
				else {
					const int stidx = (*gridx)[(ug + vg*xdim)*2 + 1] << 2;
					s_grid[idx] = stco[stidx+3].x;
					t_grid[idx] = stco[stidx+3].y;
				}
			}
		}
	}

	// use second order finite differencing for tangents,
	// probably won't work too well with displacement though... (actually, it turns out it seems to work without (visible) problems)
	idx = 0;
	const int xp1 = xdim+1, xp2 = 2*xp1;
	const float idu2 = xdim*0.5f, idv2 = ydim*0.5f;
	for (vg=0; vg<=ydim; ++vg) {
		for (ug=0; ug<=xdim; ++ug, ++idx) {
			if (ug == 0) {	// forward
				dPdu_grid[idx][0] = (-P_grid[idx + 2][0] + 4.f*P_grid[idx + 1][0] - 3.f*P_grid[idx][0])*idu2;
				dPdu_grid[idx][1] = (-P_grid[idx + 2][1] + 4.f*P_grid[idx + 1][1] - 3.f*P_grid[idx][1])*idu2;
				dPdu_grid[idx][2] = (-P_grid[idx + 2][2] + 4.f*P_grid[idx + 1][2] - 3.f*P_grid[idx][2])*idu2;
			}
			else if (ug == xdim) {	// backward
				dPdu_grid[idx][0] = (3.f*P_grid[idx][0] - 4.f*P_grid[idx - 1][0] + P_grid[idx - 2][0])*idu2;
				dPdu_grid[idx][1] = (3.f*P_grid[idx][1] - 4.f*P_grid[idx - 1][1] + P_grid[idx - 2][1])*idu2;
				dPdu_grid[idx][2] = (3.f*P_grid[idx][2] - 4.f*P_grid[idx - 1][2] + P_grid[idx - 2][2])*idu2;
			}
			else {	// central
				dPdu_grid[idx][0] = (P_grid[idx + 1][0] - P_grid[idx - 1][0])*idu2;
				dPdu_grid[idx][1] = (P_grid[idx + 1][1] - P_grid[idx - 1][1])*idu2;
				dPdu_grid[idx][2] = (P_grid[idx + 1][2] - P_grid[idx - 1][2])*idu2;
			}
			if (vg == 0) {	// forward
				dPdv_grid[idx][0] = (-P_grid[idx + xp2][0] + 4.f*P_grid[idx + xp1][0] - 3.f*P_grid[idx][0])*idv2;
				dPdv_grid[idx][1] = (-P_grid[idx + xp2][1] + 4.f*P_grid[idx + xp1][1] - 3.f*P_grid[idx][1])*idv2;
				dPdv_grid[idx][2] = (-P_grid[idx + xp2][2] + 4.f*P_grid[idx + xp1][2] - 3.f*P_grid[idx][2])*idv2;
			}
			else if (vg == ydim) {	// backward
				dPdv_grid[idx][0] = (3.f*P_grid[idx][0] - 4.f*P_grid[idx - xp1][0] + P_grid[idx - xp2][0])*idv2;
				dPdv_grid[idx][1] = (3.f*P_grid[idx][1] - 4.f*P_grid[idx - xp1][1] + P_grid[idx - xp2][1])*idv2;
				dPdv_grid[idx][2] = (3.f*P_grid[idx][2] - 4.f*P_grid[idx - xp1][2] + P_grid[idx - xp2][2])*idv2;
			}
			else {	// central
				dPdv_grid[idx][0] = (P_grid[idx + xp1][0] - P_grid[idx - xp1][0])*idv2;
				dPdv_grid[idx][1] = (P_grid[idx + xp1][1] - P_grid[idx - xp1][1])*idv2;
				dPdv_grid[idx][2] = (P_grid[idx + xp1][2] - P_grid[idx - xp1][2])*idv2;
			}
		}
	}
}

//------------------------------------------------------------------------------
// JS_SDPatch

// init static vars
double* JS_SDPatch::ccdata = NULL;
JS_SDPatch::EigenStruct* JS_SDPatch::eigen = NULL;
unsigned int JS_SDPatch::data_refc = 0;
bool JS_SDPatch::prerender = true;

// This only called once for subdivision meshes, loads data for direct evaluation method.
// However, since a patch might be inserted in framework and immediately thrown out if it gets culled,
// the data gets deleted immediately as well, and so it keeps loading the datafile again and again.
// This is why the 'prerender' flag is used to make sure this does not happen until
// all patches have been fully processed at the prerender stage.
void JS_SDPatch::getSubdivData()
{
	FILE * f;
	// endianess test
	unsigned int et = 0;
	((char*)&et)[0] = 1;
	if (et == 1)
		f = fopen("ccdata50NT.dat", "rb");
	else
		f = fopen("ccdata50.dat", "rb");
	if (f == NULL) {
		printf("Could not load subdivision data!\n");
		exit(1);
	}

	int Nmax;
	fread (&Nmax, sizeof(int), 1, f);
	// expecting Nmax==50
	if (Nmax != 50) { // should never happen
		printf("[ERROR] -> JS_SDPatch::getCCData(): Unexpected value for Nmax in subdivision data -> %d\n", Nmax);
		exit(1);
	}
	int totdoubles = 0;
	for (int i=0; i<Nmax-2; i++) {
		const int N = i+3;
		const int K = 2*N + 8;
		totdoubles += K + K*K + 3*K*16;
	}
	ccdata = new double[totdoubles];
	fread(ccdata, sizeof(double), totdoubles, f);
	fclose(f);

	// now set the actual EigenStructs as pointers to data in array
	eigen = new EigenStruct[48];
	int ofs1 = 0;
	for (int i=0; i<48; i++) {
		const int K = 2*(i + 3) + 8;
		const int ofs2 = ofs1 + K;
		const int ofs3 = ofs2 + K*K;
		const int ofs4 = ofs3 + K*16;
		const int ofs5 = ofs4 + K*16;
		eigen[i].L = ccdata + ofs1;
		eigen[i].iV = ccdata + ofs2;
		eigen[i].x[0] = ccdata + ofs3;
		eigen[i].x[1] = ccdata + ofs4;
		eigen[i].x[2] = ccdata + ofs5;
		ofs1 = ofs5 + K*16;
	}

	// make bspline evaluation basis
	double buv[16][16];
	memset(buv, 0, sizeof(double)*16*16);
	// bspline basis (could use RiBSplineBasis, but want double prec by default)
	double bsp[4][4] = {{-1.0/6.0,     0.5,    -0.5, 1.0/6.0},
	                    {     0.5,    -1.0,     0.5,     0.0},
	                    {    -0.5,     0.0,     0.5,     0.0},
	                    { 1.0/6.0, 4.0/6.0, 1.0/6.0,     0.0}};
	for (int i=0; i<16; i++) {
		const int d = i >> 2, r = i & 3;
		for (int v=0; v<4; v++)
			for (int u=0; u<4; u++)
				buv[i][v*4 + u] = bsp[u][d]*bsp[v][r];
	}
	double tmp[1728]; // max size needed for N==50
	for (int rn=0; rn<Nmax-2; rn++) {
		const int K = 2*(rn + 3) + 8;
		for (int k=0; k<3; k++) {
			memset(tmp, 0, sizeof(double)*K*16);
			int idx = 0;
			for (int i=0; i<K; i++) {
				for (int j=0; j<16; j++) {
					double sc = eigen[rn].x[k][i + j*K]; // x==Phi here
					for (int y4=0; y4<16; y4+=4)
						for (int x=0; x<4; x++)
							tmp[idx + y4 + x] += sc*buv[j][y4 + x];
				}
				idx += 16;
			}
			// now replace 'Phi' by tmp array
			memcpy(const_cast<double*>(&eigen[rn].x[k][0]), tmp, sizeof(double)*K*16);
		}
	}
}


JS_SDPatch::JS_SDPatch(int val) : umin(0), umax(1), vmin(0), vmax(1)
{
	jsdata = new jsdata_t;
	jsdata->refc = 1;
	jsdata->N = val;
	const int K = 2*val + 8;
	jsdata->ecp = new Point3[K];
	// load subdiv data if not available yet
	if (eigen == NULL) getSubdivData();
	data_refc++; // ref.count subdiv. data to properly delete it when last patch rendered
}


// copy ctor
JS_SDPatch::JS_SDPatch(const JS_SDPatch& jsp) : Primitive()
{
	static_cast<Primitive&>(*this) = static_cast<const Primitive&>(jsp);	// copy common primitive data

	jsdata = jsp.jsdata;
	bd = jsp.bd;
	umin = jsp.umin, umax = jsp.umax;
	vmin = jsp.vmin, vmax = jsp.vmax;

	data_refc++;
}

JS_SDPatch::~JS_SDPatch()
{
	// delete jsdata when no longer referenced
	if (jsdata && (--jsdata->refc == 0)) {
		if (jsdata->ecp) delete[] jsdata->ecp;
		delete jsdata;
		jsdata = NULL;
	}
	// delete subdivision data when last patch processed
	if ((--data_refc == 0) && !prerender) {
		if (ccdata) { delete[] ccdata;  ccdata = NULL; }
		if (eigen) { delete[] eigen;  eigen = NULL; }
	}
}


JS_SDPatch* JS_SDPatch::duplicateSelf()
{
	JS_SDPatch* jsp = new JS_SDPatch(*this);
	if (jsdata) jsdata->refc++;
	return jsp;
}

// *must* be called before primitive insertion (base patch only)
void JS_SDPatch::post_init()
{
	if (jsdata == NULL) return;
	// transform & calculate bound before eigenspace proj.
	const int K = 2*jsdata->N + 8;
	// now do eigenspace projection
	// stupid error was here!
	// used FixedArray for Cp, but blocked array experiments caused days of headaches wondering why nothing seemed to work anymore...
	// of course, memcpy doesn't work in that case...
	Point3* Cp = new Point3[K];
	const double* vecI = eigen[jsdata->N - 3].iV;
	for (int i=0; i<K; ++i) {
		for (int j=0; j<K; ++j)
			Cp[i] += vecI[i + K*j] * jsdata->ecp[j];
	}
	memcpy(jsdata->ecp, Cp, sizeof(Point3)*K);
	delete[] Cp;

	// not sure how to calculate correct bound,
	// for now done by evaluating four corners of main quad,
	// seems to work well sofar, which is somewhat surprising..
	Point3 ep;
	eval(0, 0, ep);
	bd.include(ep);
	eval(1, 0, ep);
	bd.include(ep);
	eval(0, 1, ep);
	bd.include(ep);
	eval(1, 1, ep);
	bd.include(ep);
	bd.addEpsilon();
}

void JS_SDPatch::recalc_bound()
{
	// as above
	bd = Bound();
	Point3 ep;
	eval(umin, vmin, ep);
	bd.include(ep);
	eval(umax, vmin, ep);
	bd.include(ep);
	eval(umin, vmax, ep);
	bd.include(ep);
	eval(umax, vmax, ep);
	bd.include(ep);
	bd.addEpsilon();
}

// returns integer log2(1.0/x)
inline int invilog2_roundup(double x)
{
	if (x==0.0) return 33;
	int xi = (int)(1.0/x);
	int lg2 = 0;
	while (xi > 0) {
		lg2++;
		xi >>= 1;
	}
	return lg2;
}

// Splitting is now done by simply adjusting u/v clipping params, similar to nurbs.
// Not necessarily faster, but definitely far simpler code and uses less memory too (refcounted data).
// Actually, it doesn't really save much memory at all, only when the patch is split a lot of times will it save *some* memory...
// Still, it is definitely simpler though.
// The main problem, as noted in the pr book on doing something similar for bicubic patches, is that it is more difficult to
// compute an exact bound, which I was already not sure of anyway, so still not sure if my approach(eval at main quad points)
// will work for all possible cases...
void JS_SDPatch::split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb)
{
	const float umid = (umin + umax)*0.5f, vmid = (vmin + vmax)*0.5f;

	// bottom left b-spline subpatch
	JS_SDPatch* jsp = duplicateSelf();
	jsp->umin = umid, jsp->vmin = vmin;
	jsp->umax = umax, jsp->vmax = vmid;
	jsp->recalc_bound();
	if (spb) {
		if (spb->bprims.size() < 1) spb->bprims.push_back(new BlurredPrimitive(*spb->parent));
		spb->bprims[0]->append(jsp);
	}
	else
		f.insert(jsp);

	// bottom right b-spline subpatch
	jsp = duplicateSelf();
	jsp->umin = umid, jsp->vmin = vmid;
	jsp->umax = umax, jsp->vmax = vmax;
	jsp->recalc_bound();
	if (spb) {
		if (spb->bprims.size() < 2) spb->bprims.push_back(new BlurredPrimitive(*spb->parent));
		spb->bprims[1]->append(jsp);
	}
	else
		f.insert(jsp);

	// top right b-spline subpatch
	jsp = duplicateSelf();
	jsp->umin = umin, jsp->vmin = vmid;
	jsp->umax = umid, jsp->vmax = vmax;
	jsp->recalc_bound();
	if (spb) {
		if (spb->bprims.size() < 3) spb->bprims.push_back(new BlurredPrimitive(*spb->parent));
		spb->bprims[2]->append(jsp);
	}
	else
		f.insert(jsp);

	// top left extra-ordinary subpatch
	jsp = duplicateSelf();
	jsp->umin = umin, jsp->vmin = vmin;
	jsp->umax = umid, jsp->vmax = vmid;
	jsp->recalc_bound();
	if (spb) {
		if (spb->bprims.size() < 4) spb->bprims.push_back(new BlurredPrimitive(*spb->parent));
		spb->bprims[3]->append(jsp);
	}
	else
		f.insert(jsp);
}


bool JS_SDPatch::diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit)
{
	#define TESTSIZE 8
	// direct dice here, TODO for all other diceable() prim funcs
	static Point3 testGrid[(TESTSIZE+1)*(TESTSIZE+1)];
	const float du=1.f/TESTSIZE, dv=1.f/TESTSIZE;
	int ug, vg, idx = 0;
	float u, v;
	for (vg=0, v=0.f; vg<=TESTSIZE; ++vg, v+=dv)
		for (ug=0, u=0.f; ug<=TESTSIZE; ++ug, u+=du, ++idx)
			eval(umin + u*(umax - umin), vmin + v*(vmax - vmin), testGrid[idx]);

	float maxUDist=-1e10f, maxVDist=-1e10f;
	for (vg=0; vg<TESTSIZE; ++vg) {
		const int Tvg = (TESTSIZE+1)*vg;
		for (ug=0; ug<TESTSIZE; ++ug) {
			Point3& p1 = testGrid[ug + Tvg];
			maxUDist = MAX2(h.rasterEstimate(p1, testGrid[ug + 1 + Tvg]), maxUDist);
			maxVDist = MAX2(h.rasterEstimate(p1, testGrid[ug + Tvg + (TESTSIZE+1)]), maxVDist);
		}
	}

	const Attributes* attr = getAttributeReference();
	// 2x2 mp minimum
	unsigned int xdim = attr->dice_binary ? (1 << MAX2(1, int(0.5f + (float)log(MAX2(1e-7f, TESTSIZE*maxUDist) / attr->effectiveShadingRate)*(float)M_LOG2E)))
	                                      : MAX2(2, int(0.5f + (TESTSIZE*maxUDist) / attr->effectiveShadingRate));
	unsigned int ydim = attr->dice_binary ? (1 << MAX2(1, int(0.5f + (float)log(MAX2(1e-7f, TESTSIZE*maxVDist) / attr->effectiveShadingRate)*(float)M_LOG2E)))
	                                      : MAX2(2, int(0.5f + (TESTSIZE*maxVDist) / attr->effectiveShadingRate));

	g.setDim(xdim, ydim, this);
	xdim++, ydim++;
	if ((xdim < h.gridSize()) && (ydim < h.gridSize()) && ((xdim*ydim) < h.gridSize())) return true;

	usplit = vsplit = true; // can only split in both directions simultaneously
	return false;
	#undef TESTSIZE
}

void JS_SDPatch::dice(MicroPolygonGrid &g, bool Pclose)
{
	// dice the grid
	const int xdim = g.get_xdim(), ydim = g.get_ydim();
	RtPoint* P_grid = (RtPoint*)(Pclose ? g.addVariable("=Pclose") : g.addVariable("P"));
	RtVector* dPdu_grid = (RtVector*)g.addVariable("dPdu");
	RtVector* dPdv_grid = (RtVector*)g.addVariable("dPdv");

	int ug, vg, idx = 0;
	float u, v;
	Point3 P;
	Vector dPdu, dPdv;
	float du = 1.f/xdim, dv = 1.f/ydim;
	for (vg=0, v=0.f; vg<=ydim; ++vg, v+=dv) {
		for (ug=0, u=0.f; ug<=xdim; ++ug, u+=du, ++idx) {
			eval(umin + u*(umax - umin), vmin + v*(vmax - vmin), P, &dPdu, &dPdv);
			P_grid[idx][0] = P.x, P_grid[idx][1] = P.y, P_grid[idx][2] = P.z;
			dPdu_grid[idx][0] = dPdu.x, dPdu_grid[idx][1] = dPdu.y, dPdu_grid[idx][2] = dPdu.z;
			dPdv_grid[idx][0] = dPdv.x, dPdv_grid[idx][1] = dPdv.y, dPdv_grid[idx][2] = dPdv.z;
		}
	}

	// dice other primvars, no vertex yet TODO
	g.initVars(std_dice);
	Primitive::linear_dice(g);
}

// pow(0,0) returns error, don't!
inline double mypow(double x, double y)
{
	if ((x < 0.0) or ((x == 0.0) and (y == 0.0))) return 1.0;
	return pow(x, y);
}


void JS_SDPatch::eval(float uf, float vf, Point3& P, Vector* dPdu, Vector* dPdv) const
{
	// u & v *have* to be clamped to 0-1 range, otherwise will crash (out of range in LUT)
	// update: u & v now can be 0 due to eval. modification, see below
	double u = CLAMP((double)uf, 0.0, 1.0), v = CLAMP((double)vf, 0.0, 1.0);
	bool eop = ((u == 0.0) && (v == 0.0));
	// determine in which domain omega_nk the parameter lies
	//const int n = int(floor(MIN2(-log2(u), -log2(v))) + 1); // can be replaced by integer log2(1/x) (roundup)
	const int n = invilog2_roundup(MAX2(u, v));
	const double pow2 = (double)(1 << (n-1));
	u *= pow2, v *= pow2;
	int k;
	if (v < 0.5)
		k = 0,  u = 2.0*u - 1.0,  v *= 2.0;
	else if (u < 0.5)
		k = 2,  u *= 2.0,  v = 2.0*v - 1.0;
	else
		k = 1,  u = 2.0*u - 1.0,  v = 2.0*v - 1.0;
	// Now evaluate the surface
	const Point3* ecp = jsdata->ecp;
	// Simple modification of the basic evaluation method based on paper by Yasushi Yamaguchi.
	// (See paper, "A basic evaluation method of subdivision surfaces", jgt 2001)
	// Hopefully I did it correctly, but it seems to work.
	P = ecp[0];
	const double *_eb = eigen[jsdata->N - 3].x[k] + 16; // skip first matrix, not used
	const double *eb, *L = eigen[jsdata->N - 3].L;
	int i;
	const int K = 2*jsdata->N + 8;
	if (dPdu && dPdv) {
		// similar to P, initial tangent eval value skipped, always zero
		dPdu->set(0.f);
		dPdv->set(0.f);
		if (eop) {
			// Yamaguchi paper, modification for tangents exactly at extra-ordinary points.
			// Still no idea if I did this correctly, but it yet again seems to work nevertheless...
			// I don't get shading artifacts at those points anymore at least.
			// (although that was really only visible at very low shadingrates anyway)
			for (i=1, eb=_eb; i<K; ++i, eb += 16) {
				const Point3 tcp = ecp[i] * mypow(L[i], n-1);
				const double t1 = ((eb[ 0]*v + eb[ 1])*v + eb[ 2])*v + eb[ 3];
				const double t2 = ((eb[ 4]*v + eb[ 5])*v + eb[ 6])*v + eb[ 7];
				const double t3 = ((eb[ 8]*v + eb[ 9])*v + eb[10])*v + eb[11];
				P += tcp * (((t1*u + t2)*u + t3)*u + (((eb[12]*v + eb[13])*v + eb[14])*v + eb[15]));
			}
			const double* _eb2 = _eb + 16;
			if (mypow(L[1], n-1) * mypow(L[2], n-1) * ((_eb[11] * _eb2[14]) - (_eb2[11] * _eb[14])) < 0.0)
				*dPdu = toVector(ecp[2]), *dPdv = toVector(ecp[1]);
			else
				*dPdu = toVector(ecp[1]), *dPdv = toVector(ecp[2]);
		}
		else {
			for (i=1, eb=_eb; i<K; ++i, eb += 16) {
				const Point3 tcp = ecp[i] * mypow(L[i], n-1);
				const double t1 = ((eb[ 0]*v + eb[ 1])*v + eb[ 2])*v + eb[ 3];
				const double t2 = ((eb[ 4]*v + eb[ 5])*v + eb[ 6])*v + eb[ 7];
				const double t3 = ((eb[ 8]*v + eb[ 9])*v + eb[10])*v + eb[11];
				P += tcp * (((t1*u + t2)*u + t3)*u + (((eb[12]*v + eb[13])*v + eb[14])*v + eb[15]));
				*dPdv += toVector(tcp) *
				  ( ( ( ((eb[ 0]*3.0*v + eb[ 1]*2.0)*v + eb[ 2])  *u +
				        ((eb[ 4]*3.0*v + eb[ 5]*2.0)*v + eb[ 6]) )*u +
				        ((eb[ 8]*3.0*v + eb[ 9]*2.0)*v + eb[10]) )*u +
				        ((eb[12]*3.0*v + eb[13]*2.0)*v + eb[14]) );
				*dPdu += toVector(tcp) * (((t1*3.0*u + t2*2.0)*u + t3));
			}
		}
	}
	else {
		// P only
		for (i=1, eb=_eb; i<K; ++i, eb += 16) {
			const Point3 tcp = ecp[i] * mypow(L[i], n-1);
			P += tcp * ((((((eb[ 0]*v + eb[ 1])*v + eb[ 2])*v + eb[ 3]) *u +
			              (((eb[ 4]*v + eb[ 5])*v + eb[ 6])*v + eb[ 7]))*u +
			              (((eb[ 8]*v + eb[ 9])*v + eb[10])*v + eb[11]))*u +
			              (((eb[12]*v + eb[13])*v + eb[14])*v + eb[15]));
		}
	}
}

__END_QDRENDER

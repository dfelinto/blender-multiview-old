///-------------------------------------------------------------------------
/// The grid of micropolygon variables.
///
/// Originally the actual data was kept in its own class, using the C++
/// datatypes for points/vectors/etc. But since this is intended for
/// Blender, I thought it would be best to keep things low level and
/// reduce everything to its most basic common type, simple float arrays.
/// This means that currently strings (which are valid in the RISpec)
/// are not supported, but I can't imagine that being a problem.
///
/// It is also not complete in other ways, only a few global varying variables
/// are initialized, mostly based on which ones are used by the shaders,
/// and/or variables attached to primitives.
/// Uniform variables are in fact not handled at all here yet.
///

#include "MicroPolygonGrid.h"

#include "MicroPolygon.h"
#include "Primitive.h"
#include "Mathutil.h"
#include "Camera.h"
#include "Noise.h"
#include "slshader.h"
#include "State.h"

#include "QDRender.h"
__BEGIN_QDRENDER

using namespace std;

// xdim & ydim are the number of faces in each direction, so even if 0
// assume there is at least 1 vertex, so nverts is set to 1.
// (vertex is 'virtual' though, since no actual grid exists at this point)
// (reason this needs to be set is for shaders, bind() called from run_initcode() depends on it)
MicroPolygonGrid::MicroPolygonGrid(): gtype(G_NORMAL), Ng_updated(false), xdim(0), ydim(0), nverts(1), parent(NULL)
{
}

MicroPolygonGrid::~MicroPolygonGrid()
{
	float** fg = vertexvars.firstItem();
	while (fg) {
		delete[] *fg;
		fg = vertexvars.nextItem();
	}
	vertexvars.clear();
}

void MicroPolygonGrid::setDim(unsigned int _xdim, unsigned int _ydim, const Primitive* _parent)
{
	xdim = _xdim;
	ydim = _ydim;
	nverts = (xdim+1)*(ydim+1);
	parent = _parent;
}

// copies everything but the grid data.
// only used in shaderVM.cpp to initialize lightshader grids
void MicroPolygonGrid::setFrom(const MicroPolygonGrid& mg)
{
	xdim = mg.xdim;
	ydim = mg.ydim;
	nverts = mg.nverts;
	gtype = mg.gtype;
	parent = mg.parent;
}

float* MicroPolygonGrid::addVariable(RtToken name, int grid_element_size)
{
	float** fg = vertexvars.findItem(name);
	if (fg) return *fg;	// already defined
	// new grid
	const unsigned int numfloats = nverts*grid_element_size;
	float* nfg = new float[numfloats];
	memset(nfg, 0, sizeof(float)*numfloats);
	vertexvars.addItem(name, nfg);
	return nfg;
}

void MicroPolygonGrid::deleteVariable(RtToken name)
{
	float* fg = NULL;
	vertexvars.removeItem(name, fg);
	if (fg) delete[] fg;
}

float* MicroPolygonGrid::findVariable(RtToken name)
{
	float** fg = vertexvars.findItem(name);
	if (fg) return *fg;
	return NULL;
}

// Current place of this function in framework actually doesn't seem to make much sense,
// displacement may make mp's not backfacing
bool MicroPolygonGrid::isBackfacing()
{
	//if ((Options.backfaceCulling==false)  || (parent->Sides==2)) return false;
	// all mps in grid facing away from camera?
	// TODO old code removed (normal calculation)
	return false;
}

// TODO or not
bool MicroPolygonGrid::trim()
{
	//if (parent->attribute->trimcurve clips entire grid) return true;
	return false;
}

void MicroPolygonGrid::displace()
{
	const Attributes* attr = parent->getAttributeReference();
	if (attr->displacement_shader) {

		RtPoint* P_grid = (RtPoint*)findVariable("P");
		assert(P_grid != NULL); // should never happen

		// Ng might not be available yet
		RtNormal* Ng_grid = (RtNormal*)findVariable("Ng");
		if (Ng_grid == NULL) {
			analyticNormals();
			Ng_grid = (RtNormal*)findVariable("Ng");
		}

		// view ray I (== P here since camera is always at origin)
		RtVector* I_grid = (RtVector*)addVariable("I");
		if (State::Instance()->cam.isOrthographic()) {
			for (unsigned int i=0; i<nverts; ++i)
				I_grid[i][0] = I_grid[i][1] = 0, I_grid[i][2] = P_grid[i][2];
		}
		else
			memcpy(I_grid, P_grid, sizeof(RtPoint)*nverts);

		// if N not set by primvars, make a copy of Ng
		RtNormal* N_grid = (RtNormal*)findVariable("N");
		if (N_grid == NULL) {
			N_grid = (RtNormal*)addVariable("N");
			memcpy(N_grid, Ng_grid, sizeof(RtNormal)*nverts);
		}

		Ng_updated = false;
		// execute shader code
		attr->displacement_shader->run(this);
		// if Ng needs to be updated if P was modified,
		// and it was not updated indirectly by shader ('calcnormal N P')
		// then it must be done here
		if (attr->displacement_shader->shader->update_Ng and not Ng_updated)
			calculateNormals(Ng_grid, P_grid);

	}
}


// adds "Ng" grid, calculates normals from dPdu ^ dPdv,
// returns true if all normals backfacing
bool MicroPolygonGrid::analyticNormals()
{
	RtNormal* Ng_grid = (RtNormal*)addVariable("Ng");

	RtPoint* P_grid = (RtPoint*)findVariable("P");
		// if Points grid, only need P and Ng ( == -I == -P )
	if (gtype == G_POINTS) {
		assert(P_grid != NULL);
		for (unsigned int i=0; i<nverts; ++i)
			Ng_grid[i][0] = -P_grid[i][0], Ng_grid[i][1] = -P_grid[i][1], Ng_grid[i][2] = -P_grid[i][2];
		return false;
	}

	RtVector* dPdu_grid = (RtVector*)findVariable("dPdu");
	RtVector* dPdv_grid = (RtVector*)findVariable("dPdv");
	assert((P_grid!=NULL) && (dPdu_grid!=NULL) && (dPdv_grid!=NULL)); // should never happen

	// flip normals if required
	const Transform *xform = parent->getTransformReference();
	if (xform->handedness() != ((parent->getAttributeReference()->flags & AF_ORIENTATION)!=0))
		SWAP(dPdu_grid, dPdv_grid);

	if (parent->in_camspace()) {
		// no transform needed
		for (unsigned int i=0; i<nverts; ++i)
			vcross(Ng_grid[i], dPdu_grid[i], dPdv_grid[i]);
	}
	else {
		// normal transform, transpose of inverse (or inv. of transp., same result, I think...)
		Transform nxform = *xform;
		nxform.invert();
		nxform.transpose();
		const RtMatrix* ri_nxf = nxform.getRtMatrixPtr();
		for (unsigned int i=0; i<nverts; ++i) {
			vcross(Ng_grid[i], dPdu_grid[i], dPdv_grid[i]);
			mulVMV(Ng_grid[i], *ri_nxf, Ng_grid[i]);
		}
	}
	
	return false; // should be backfacing flag, TODO
}


// calculates normals of grid using central differencing on the interior points,
// and either second order back- or forward differencing at the border points.
// Because of this, gridsize *must* be at least 2X2 faces, or 3X3 verts
bool MicroPolygonGrid::calculateNormals(RtNormal* N_grid, const RtPoint* P_grid)
{
	// point or curve grids are dimensionless, have no normal until bust()
	if (gtype != MicroPolygonGrid::G_NORMAL) return false;

	// exit if grid too small, should never happen, somewhere diceable() function probably not correct...
	assert((xdim >= 2) and (ydim >= 2));

	// In this case the normals don't need to be transformed, since P is already in camspace,
	// but still might need to be flipped
	const Transform *xform = parent->getTransformReference();
	bool nflip = (xform->handedness() != ((parent->getAttributeReference()->flags & AF_ORIENTATION)!=0));

	float uvl[4];
	parent->get_uvlim(uvl);
	const float idu2 = (0.5f*(float)xdim)/(uvl[1] - uvl[0]),
	            idv2 = (0.5f*(float)ydim)/(uvl[3] - uvl[2]);

	RtVector dPdu, dPdv;
	unsigned int idx = 0;
	const int xp1 = xdim+1, xp2 = 2*xp1;
	for (unsigned int vg=0; vg<=ydim; ++vg) {
		for (unsigned int ug=0; ug<=xdim; ++ug, ++idx) {
			// testing for processor activity here should really always succeed for all points to get any valid result at all...
			// in other words derivative calc. funcs like calculatenormal/Deriv/Du/Dv/Area should *never* depend on any conditionals.
			//if (context.active(idx)) {
				// dPdu
				if (ug == 0) {	// forward
					const int i1 = idx + 1, i2 = idx + 2;
					dPdu[0] = (-P_grid[i2][0] + 4.f*P_grid[i1][0] - 3.f*P_grid[idx][0])*idu2;
					dPdu[1] = (-P_grid[i2][1] + 4.f*P_grid[i1][1] - 3.f*P_grid[idx][1])*idu2;
					dPdu[2] = (-P_grid[i2][2] + 4.f*P_grid[i1][2] - 3.f*P_grid[idx][2])*idu2;
				}
				else if (ug == xdim) {	// backward
					const int i1 = idx - 1, i2 = idx - 2;
					dPdu[0] = (3.f*P_grid[idx][0] - 4.f*P_grid[i1][0] + P_grid[i2][0])*idu2;
					dPdu[1] = (3.f*P_grid[idx][1] - 4.f*P_grid[i1][1] + P_grid[i2][1])*idu2;
					dPdu[2] = (3.f*P_grid[idx][2] - 4.f*P_grid[i1][2] + P_grid[i2][2])*idu2;
				}
				else {	// central
					const int ip = idx + 1, im = idx - 1;
					dPdu[0] = (P_grid[ip][0] - P_grid[im][0])*idu2;
					dPdu[1] = (P_grid[ip][1] - P_grid[im][1])*idu2;
					dPdu[2] = (P_grid[ip][2] - P_grid[im][2])*idu2;
				}
				// dPdv
				if (vg == 0) {	// forward
					const int i1 = idx + xp1, i2 = idx + xp2;
					dPdv[0] = (-P_grid[i2][0] + 4.f*P_grid[i1][0] - 3.f*P_grid[idx][0])*idv2;
					dPdv[1] = (-P_grid[i2][1] + 4.f*P_grid[i1][1] - 3.f*P_grid[idx][1])*idv2;
					dPdv[2] = (-P_grid[i2][2] + 4.f*P_grid[i1][2] - 3.f*P_grid[idx][2])*idv2;
				}
				else if (vg == ydim) {	// backward
					const int i1 = idx - xp1, i2 = idx - xp2;
					dPdv[0] = (3.f*P_grid[idx][0] - 4.f*P_grid[i1][0] + P_grid[i2][0])*idv2;
					dPdv[1] = (3.f*P_grid[idx][1] - 4.f*P_grid[i1][1] + P_grid[i2][1])*idv2;
					dPdv[2] = (3.f*P_grid[idx][2] - 4.f*P_grid[i1][2] + P_grid[i2][2])*idv2;
				}
				else {	// central
					const int ip = idx+xp1, im = idx-xp1;
					dPdv[0] = (P_grid[ip][0] - P_grid[im][0])*idv2;
					dPdv[1] = (P_grid[ip][1] - P_grid[im][1])*idv2;
					dPdv[2] = (P_grid[ip][2] - P_grid[im][2])*idv2;
				}
				if (nflip)
					vcross(N_grid[idx], dPdv, dPdu);
				else
					vcross(N_grid[idx], dPdu, dPdv);
			//}
		}
	}

	return false;	// should be backfacing flag, TODO
}


void MicroPolygonGrid::DuF(RtFloat* dst_grid, const RtFloat* src_grid)
{
	// point/curve grids are dimensionless
	if (gtype != MicroPolygonGrid::G_NORMAL) return;

	// exit if grid too small, should never happen, somewhere diceable() function probably not correct...
	assert((xdim >= 2) and (ydim >= 2));

	float uvl[4];
	parent->get_uvlim(uvl);
	const float idu2 = (0.5f*(float)xdim)/(uvl[1] - uvl[0]);

	unsigned int idx = 0;
	for (unsigned int vg=0; vg<=ydim; ++vg) {
		for (unsigned int ug=0; ug<=xdim; ++ug, ++idx) {
			if (ug == 0)	// forward
				dst_grid[idx] = (-src_grid[idx + 2] + 4.f*src_grid[idx + 1] - 3.f*src_grid[idx])*idu2;
			else if (ug == xdim)	// backward
				dst_grid[idx] = (3.f*src_grid[idx] - 4.f*src_grid[idx - 1] + src_grid[idx - 2])*idu2;
			else	// central
				dst_grid[idx] = (src_grid[idx + 1] - src_grid[idx - 1])*idu2;
		}
	}

}

void MicroPolygonGrid::DvF(RtFloat* dst_grid, const RtFloat* src_grid)
{
	// point/curve grids are dimensionless
	if (gtype != MicroPolygonGrid::G_NORMAL) return;

	// exit if grid too small, should never happen, somewhere diceable() function probably not correct...
	assert((xdim >= 2) and (ydim >= 2));

	float uvl[4];
	parent->get_uvlim(uvl);
	const float idv2 = (0.5f*(float)ydim)/(uvl[3] - uvl[2]);

	unsigned int idx = 0;
	const unsigned int xp1 = xdim+1, xp2 = 2*xp1;
	for (unsigned int vg=0; vg<=ydim; ++vg) {
		for (unsigned int ug=0; ug<=xdim; ++ug, ++idx) {
			if (vg == 0)	// forward
				dst_grid[idx] = (-src_grid[idx + xp2] + 4.f*src_grid[idx + xp1] - 3.f*src_grid[idx])*idv2;
			else if (vg == ydim)	// backward
				dst_grid[idx] = (3.f*src_grid[idx] - 4.f*src_grid[idx - xp1] + src_grid[idx - xp2])*idv2;
			else	// central
				dst_grid[idx] = (src_grid[idx + xp1] - src_grid[idx - xp1])*idv2;
		}
	}

}

void MicroPolygonGrid::DuV(RtVector* dst_grid, const RtVector* src_grid)
{
	// point/curve grids are dimensionless
	if (gtype != MicroPolygonGrid::G_NORMAL) return;

	// exit if grid too small, should never happen, somewhere diceable() function probably not correct...
	assert((xdim >= 2) and (ydim >= 2));

	float uvl[4];
	parent->get_uvlim(uvl);
	const float idu2 = (0.5f*(float)xdim)/(uvl[1] - uvl[0]);

	unsigned int idx = 0;
	for (unsigned int vg=0; vg<=ydim; ++vg) {
		for (unsigned int ug=0; ug<=xdim; ++ug, ++idx) {
			if (ug == 0) {	// forward
				const unsigned int i1 = idx + 1, i2 = idx + 2;
				dst_grid[idx][0] = (-src_grid[i2][0] + 4.f*src_grid[i1][0] - 3.f*src_grid[idx][0])*idu2;
				dst_grid[idx][1] = (-src_grid[i2][1] + 4.f*src_grid[i1][1] - 3.f*src_grid[idx][1])*idu2;
				dst_grid[idx][2] = (-src_grid[i2][2] + 4.f*src_grid[i1][2] - 3.f*src_grid[idx][2])*idu2;
			}
			else if (ug == xdim) {	// backward
				const int i1 = idx - 1, i2 = idx - 2;
				dst_grid[idx][0] = (3.f*src_grid[idx][0] - 4.f*src_grid[i1][0] + src_grid[i2][0])*idu2;
				dst_grid[idx][1] = (3.f*src_grid[idx][1] - 4.f*src_grid[i1][1] + src_grid[i2][1])*idu2;
				dst_grid[idx][2] = (3.f*src_grid[idx][2] - 4.f*src_grid[i1][2] + src_grid[i2][2])*idu2;
			}
			else {	// central
				const int ip = idx + 1, im = idx - 1;
				dst_grid[idx][0] = (src_grid[ip][0] - src_grid[im][0])*idu2;
				dst_grid[idx][1] = (src_grid[ip][1] - src_grid[im][1])*idu2;
				dst_grid[idx][2] = (src_grid[ip][2] - src_grid[im][2])*idu2;
			}
		}
	}

}

void MicroPolygonGrid::DvV(RtVector* dst_grid, const RtVector* src_grid)
{
	// point or curve grids are dimensionless, have no normal until bust()
	if (gtype != MicroPolygonGrid::G_NORMAL) return;

	// exit if grid too small, should never happen, somewhere diceable() function probably not correct...
	assert((xdim >= 2) and (ydim >= 2));

	float uvl[4];
	parent->get_uvlim(uvl);
	const float idv2 = (0.5f*(float)ydim)/(uvl[3] - uvl[2]);

	unsigned int idx = 0;
	const int xp1 = xdim+1, xp2 = 2*xp1;
	for (unsigned int vg=0; vg<=ydim; ++vg) {
		for (unsigned int ug=0; ug<=xdim; ++ug, ++idx) {
			if (vg == 0) {	// forward
				const int i1 = idx + xp1, i2 = idx + xp2;
				dst_grid[idx][0] = (-src_grid[i2][0] + 4.f*src_grid[i1][0] - 3.f*src_grid[idx][0])*idv2;
				dst_grid[idx][1] = (-src_grid[i2][1] + 4.f*src_grid[i1][1] - 3.f*src_grid[idx][1])*idv2;
				dst_grid[idx][2] = (-src_grid[i2][2] + 4.f*src_grid[i1][2] - 3.f*src_grid[idx][2])*idv2;
			}
			else if (vg == ydim) {	// backward
				const int i1 = idx - xp1, i2 = idx - xp2;
				dst_grid[idx][0] = (3.f*src_grid[idx][0] - 4.f*src_grid[i1][0] + src_grid[i2][0])*idv2;
				dst_grid[idx][1] = (3.f*src_grid[idx][1] - 4.f*src_grid[i1][1] + src_grid[i2][1])*idv2;
				dst_grid[idx][2] = (3.f*src_grid[idx][2] - 4.f*src_grid[i1][2] + src_grid[i2][2])*idv2;
			}
			else {	// central
				const int ip = idx+xp1, im = idx-xp1;
				dst_grid[idx][0] = (src_grid[ip][0] - src_grid[im][0])*idv2;
				dst_grid[idx][1] = (src_grid[ip][1] - src_grid[im][1])*idv2;
				dst_grid[idx][2] = (src_grid[ip][2] - src_grid[im][2])*idv2;
			}
		}
	}

}

// dst = Deriv(num, den) = Du(num)/Du(den) + Dv(num)/Dv(den)
void MicroPolygonGrid::DerivF(RtFloat* dst_grid, const RtFloat* num_grid, const RtFloat* den_grid)
{
	RtFloat *t1 = new RtFloat[nverts], *t2 = new RtFloat[nverts];
	DuF(t1, num_grid);
	DuF(t2, den_grid);
	for (unsigned int i=0; i<nverts; ++i)
		dst_grid[i] = t1[i] / ((t2[i] == 0.f) ? 1.f : t2[i]);
	DvF(t1, num_grid);
	DvF(t2, den_grid);
	for (unsigned int i=0; i<nverts; ++i)
		dst_grid[i] += t1[i] / ((t2[i] == 0.f) ? 1.f : t2[i]);
	delete[] t2;
	delete[] t1;
}

// dst = Deriv(num, den) = Du(num)/Du(den) + Dv(num)/Dv(den)
void MicroPolygonGrid::DerivV(RtVector* dst_grid, const RtVector* num_grid, const RtFloat* den_grid)
{
	RtVector *t1 = new RtVector[nverts];
	RtFloat *t2 = new RtFloat[nverts];
	DuV(t1, num_grid);
	DuF(t2, den_grid);
	for (unsigned int i=0; i<nverts; ++i) {
		const float d = 1.f/t2[i];
		dst_grid[i][0] = t1[i][0]*d, dst_grid[i][1] = t1[i][1]*d, dst_grid[i][2] = t1[i][2]*d;
	}
	DvV(t1, num_grid);
	DvF(t2, den_grid);
	for (unsigned int i=0; i<nverts; ++i) {
		const float d = 1.f/t2[i];
		dst_grid[i][0] += t1[i][0]*d, dst_grid[i][1] += t1[i][1]*d, dst_grid[i][2] += t1[i][2]*d;
	}
	delete[] t2;
	delete[] t1;
}

// RIspec definition 'length( Du(P)*du ˆ Dv(P)*dv )'
void MicroPolygonGrid::Area(RtFloat* dst_grid, const RtPoint* P_grid)
{
	// point or curve grids are dimensionless, have no normal until bust()
	if (gtype != MicroPolygonGrid::G_NORMAL) return;

	// exit if grid too small, should never happen, somewhere diceable() function probably not correct...
	assert((xdim >= 2) and (ydim >= 2));

	unsigned int idx = 0;
	const unsigned int xp1 = xdim+1, xp2 = 2*xp1;
	for (unsigned int vg=0; vg<=ydim; ++vg) {
		for (unsigned int ug=0; ug<=xdim; ++ug, ++idx) {
			RtVector dPdu, dPdv, N;
			// dPdu
			if (ug == 0) { // forward
				const int i1 = idx + 1, i2 = idx + 2;
				dPdu[0] = -0.5f*P_grid[i2][0] + 2.f*P_grid[i1][0] - 1.5f*P_grid[idx][0];
				dPdu[1] = -0.5f*P_grid[i2][1] + 2.f*P_grid[i1][1] - 1.5f*P_grid[idx][1];
				dPdu[2] = -0.5f*P_grid[i2][2] + 2.f*P_grid[i1][2] - 1.5f*P_grid[idx][2];
			}
			else if (ug == xdim) {// backward
				const int i1 = idx - 1, i2 = idx - 2;
				dPdu[0] = 1.5f*P_grid[idx][0] - 2.f*P_grid[i1][0] + 0.5f*P_grid[i2][0];
				dPdu[1] = 1.5f*P_grid[idx][1] - 2.f*P_grid[i1][1] + 0.5f*P_grid[i2][1];
				dPdu[2] = 1.5f*P_grid[idx][2] - 2.f*P_grid[i1][2] + 0.5f*P_grid[i2][2];
			}
			else { // central
				const int ip = idx + 1, im = idx - 1;
				dPdu[0] = (P_grid[ip][0] - P_grid[im][0])*0.5f;
				dPdu[1] = (P_grid[ip][1] - P_grid[im][1])*0.5f;
				dPdu[2] = (P_grid[ip][2] - P_grid[im][2])*0.5f;
			}
			// dPdv
			if (vg == 0) { // forward
				const int i1 = idx + xp1, i2 = idx + xp2;
				dPdv[0] = -0.5f*P_grid[i2][0] + 2.f*P_grid[i1][0] - 1.5f*P_grid[idx][0];
				dPdv[1] = -0.5f*P_grid[i2][1] + 2.f*P_grid[i1][1] - 1.5f*P_grid[idx][1];
				dPdv[2] = -0.5f*P_grid[i2][2] + 2.f*P_grid[i1][2] - 1.5f*P_grid[idx][2];
			}
			else if (vg == ydim) { // backward
				const int i1 = idx - xp1, i2 = idx - xp2;
				dPdv[0] = 1.5f*P_grid[idx][0] - 2.f*P_grid[i1][0] + 0.5f*P_grid[i2][0];
				dPdv[1] = 1.5f*P_grid[idx][1] - 2.f*P_grid[i1][1] + 0.5f*P_grid[i2][1];
				dPdv[2] = 1.5f*P_grid[idx][2] - 2.f*P_grid[i1][2] + 0.5f*P_grid[i2][2];
			}
			else { // central
				const int ip = idx+xp1, im = idx-xp1;
				dPdv[0] = (P_grid[ip][0] - P_grid[im][0])*0.5f;
				dPdv[1] = (P_grid[ip][1] - P_grid[im][1])*0.5f;
				dPdv[2] = (P_grid[ip][2] - P_grid[im][2])*0.5f;
			}
			vcross(N, dPdu, dPdv);
			vlength(dst_grid[idx], N);
		}
	}
}


// initialize the global variables as required by the shader unless declared as primvar
void MicroPolygonGrid::initVars(unsigned int varbits)
{
	const unsigned int primvar_available = parent->pvar_flags();
	unsigned int sa_idx = 0;
	while (_sl_access[sa_idx].name) {
		const slGlobals globit = 1 << sa_idx;
		// all vars always available by default can be skipped
		if (globit & (GB_P | GB_dPdu | GB_dPdv | GB_N | GB_Ng | GB_I)) {
			++sa_idx;
			continue;
		}
		if (varbits & globit) {
			const SlType slt = _sl_access[sa_idx].type;
			if (_sl_access[sa_idx].isvarying) {
				if (slt == SL_FLOAT) {
					float* fg = addVariable(_sl_access[sa_idx].name, 1);
					// initialization can be skipped if available as attached primitive variable, diced later
					if ((primvar_available & globit) == 0) {
						if (globit & (GB_s | GB_t | GB_u | GB_v | GB_du | GB_dv)) {
							float uvl[4];
							parent->get_uvlim(uvl);
							if (globit & (GB_s | GB_t | GB_u | GB_v)) {
								const float du = 1.f/float(xdim), dv = 1.f/float(ydim);
								unsigned int ug, vg, idx = 0;
								if (globit & (GB_s | GB_u)) {
									float u;
									for (vg=0; vg<=ydim; ++vg)
										for (ug=0, u=0.f; ug<=xdim; ++ug, u+=du, ++idx)
											fg[idx] = uvl[0] + u*(uvl[1] - uvl[0]);
								}
								else {	// GB_t | GB_v
									float v;
									for (vg=0, v=0.f; vg<=ydim; ++vg, v+=dv)
										for (ug=0; ug<=xdim; ++ug, ++idx)
											fg[idx] = uvl[2] + v*(uvl[3] - uvl[2]);
								}
							}
							else {	// du | dv (currently are actually uniform per grid, not varying TODO)
								const float du = (uvl[1] - uvl[0])/float(xdim), dv = (uvl[3] - uvl[2])/float(ydim);
								for (unsigned int i=0; i<nverts; ++i)
									fg[i] = (globit & GB_du) ? du : dv;
							}
						}
					}
				}
				else if ((slt == SL_POINT) or (slt == SL_COLOR) or (slt == SL_VECTOR) or (slt == SL_NORMAL)) {
					// initialize default grids
					RtVector* fg = (RtVector*)addVariable(_sl_access[sa_idx].name, 3);
					// initialization can be skipped if available as attached primitive variable, diced later
					if ((primvar_available & globit) == 0) {
						const Attributes* attr = parent->getAttributeReference();
						if (globit & GB_Cs) {
							for (unsigned int i=0; i<nverts; ++i)
								fg[i][0] = attr->color[0], fg[i][1] = attr->color[1], fg[i][2] = attr->color[2];
						}
						else if (globit & GB_Os) {
							for (unsigned int i=0; i<nverts; ++i)
								fg[i][0] = attr->opacity[0], fg[i][1] = attr->opacity[1], fg[i][2] = attr->opacity[2];
						}
					}
				}
				else	// never happens, all standard variables in table are either float or point/color/vector/normal
					assert("Unexpected varying type in _sl_access???" == NULL);
			}
			// uniform variable, TODO
		}
		++sa_idx;
	}
}

void MicroPolygonGrid::shade()
{
	RtPoint* P_grid = (RtPoint*)findVariable("P");
	assert(P_grid!=NULL); // should never happen

	// Ng grid might already be available because of displacement shader,
	// if not, initialize here
	RtNormal* Ng_grid = (RtNormal*)findVariable("Ng");
	if (Ng_grid == NULL) {
		analyticNormals();
		Ng_grid = (RtNormal*)findVariable("Ng");
	}

	// if N not set by primvars, make a copy of Ng
	RtNormal* N_grid = (RtNormal*)findVariable("N");
	if (N_grid == NULL) {
		N_grid = (RtNormal*)addVariable("N");
		memcpy(N_grid, Ng_grid, sizeof(RtVector)*nverts);
	}

	// add the Ci/Oi grids
	RtColor *Ci_grid = (RtColor*)addVariable("Ci"), *Oi_grid = (RtColor*)addVariable("Oi");

	// incident ray I can now be initialized from P (camera always at origin, so I == P)
	// if the 'update_Ng' flag is set in the shader, it means P was modified by displacement,
	// which means I is then also already available, so doesn't have to be re-initialized here
	RtVector* I_grid = (RtVector*)findVariable("I");
	if (I_grid == NULL) I_grid = (RtVector*)addVariable("I");
	if (State::Instance()->cam.isOrthographic()) {
		for (unsigned int i=0; i<nverts; ++i)
			I_grid[i][0] = I_grid[i][1] = 0, I_grid[i][2] = P_grid[i][2];
	}
	else
		memcpy(I_grid, P_grid, sizeof(RtPoint)*nverts);

	const Attributes* attr = parent->getAttributeReference();

	if (attr->surface_shader) {
		// save 'P', since surface shader may modify it, bumpmapping is ok, but displacement should really be done
		// with a displacement shader (could omit this, might be useful to allow displacement in surface shader, not sure yet...)
		// This must be optimized to only do this when the surface shader actually DOES modify P TODO
		RtPoint* tmp_P_grid = new RtPoint[nverts];
		memcpy(tmp_P_grid, P_grid, sizeof(RtPoint)*nverts);
		// execute shader code
		attr->surface_shader->run(this);
		// reset P
		memcpy(P_grid, tmp_P_grid, sizeof(RtPoint)*nverts);
		delete[] tmp_P_grid;
	}
	else {
		// no surface shader assigned, this still would need to take possible light shaders into account,
		// but for now ignored, do a simple fixed light thingy instead,
		// directional, worldspace, camera pos. in z dir, so can use normal.z directly.
		const Color Cs(attr->color[0], attr->color[1], attr->color[2]);
		const Color Os(attr->opacity[0], attr->opacity[2], attr->opacity[2]);
		for (unsigned int idx=0; idx<nverts; ++idx) {
			const Color Ci(Cs * Os * ABS(Vector(N_grid[idx][0], N_grid[idx][1], N_grid[idx][2]).normalize() VDOT
			                             Vector(I_grid[idx][0], I_grid[idx][1], I_grid[idx][2]).normalize()));
			Ci_grid[idx][0] = Ci.r, Ci_grid[idx][1] = Ci.g, Ci_grid[idx][2] = Ci.b;
			Oi_grid[idx][0] = Os.r, Oi_grid[idx][1] = Os.g, Oi_grid[idx][2] = Os.b;
		}
	}

	// volume shaders, if any
	if (attr->atmosphere_shader)
		attr->atmosphere_shader->run(this);

}

// bust microgrid into individual micropolygons
// returns vector of micropolygon pointers
void MicroPolygonGrid::bust(std::vector<MicroPolygon*> &mplist, bool depthonly)
{
	// points & curves have their own bust method
	if (gtype == G_POINTS) {
		bust_points(mplist, depthonly);
		return;
	}
	else if (gtype == G_CURVES) {
		bust_curves(mplist, depthonly);
		return;
	}

	// normal grid
	// points
	RtPoint* P_grid = (RtPoint*)findVariable("P");
	assert(P_grid!=NULL); // should never happen

	// if moving primitive, '=Pclose' is available
	RtPoint* eP_grid = (RtPoint*)findVariable("=Pclose");
	bool moving = (eP_grid != NULL);

	// color data not needed for Z renders
	// NOTE this is not necessarily true, geometry can be 'clipped' by full opacity TODO
	RtColor *Ci_grid = NULL, *Oi_grid = NULL;
	if (!depthonly) {
		// color
		Ci_grid = (RtColor*)findVariable("Ci");
		assert(Ci_grid!=NULL); // should never happen
		// opacity
		Oi_grid = (RtColor*)findVariable("Oi");
		assert(Oi_grid!=NULL); // should never happen
	}

	// mp vertices, reference counted and deleted when possible
	std::vector<ShadedVertex*> mpv(nverts, NULL);

	// get/set the vertex variables
	unsigned int x, y;
	Camera& cam = State::Instance()->projcam;
	for (x=0;x<nverts;++x) {
		mpv[x] = new ShadedVertex();
		const Point3 pt(P_grid[x][0], P_grid[x][1], P_grid[x][2]);
		//mpv[x]->P = pt;
		mpv[x]->pp = cam.project(pt);
		if (moving)
			mpv[x]->ppc = cam.project(Point3(eP_grid[x][0], eP_grid[x][1], eP_grid[x][2]));
		else
			mpv[x]->ppc = mpv[x]->pp;
		if (!depthonly) {
			mpv[x]->Ci.set(Ci_grid[x][0], Ci_grid[x][1], Ci_grid[x][2]);
			mpv[x]->Oi.set(Oi_grid[x][0], Oi_grid[x][1], Oi_grid[x][2]);
		}
	}

	// the new micropolygon list
	mplist.resize(xdim*ydim, NULL);
	const int gx = xdim+1;
	for (y=0; y<ydim; ++y) {
		const int ygx=y*gx, ygx1=(y+1)*gx;
		for (x=0; x<xdim; ++x) {
			ShadedVertex* mpv1 = mpv[ x    + ygx ];
			ShadedVertex* mpv2 = mpv[(x+1) + ygx ];
			ShadedVertex* mpv3 = mpv[(x+1) + ygx1];
			ShadedVertex* mpv4 = mpv[ x    + ygx1];
			MicroPolygon* mp = new MicroPolygon(mpv1, mpv2, mpv3, mpv4);
			// calculate integer 2D bound of projected points
			if (moving) {
				// include points at shutter close time
				mp->xmin = FLOORI(MIN2(MIN4(mpv1->pp.x, mpv2->pp.x, mpv3->pp.x, mpv4->pp.x),
				                       MIN4(mpv1->ppc.x, mpv2->ppc.x, mpv3->ppc.x, mpv4->ppc.x)));
				mp->ymin = FLOORI(MIN2(MIN4(mpv1->pp.y, mpv2->pp.y, mpv3->pp.y, mpv4->pp.y),
				                       MIN4(mpv1->ppc.y, mpv2->ppc.y, mpv3->ppc.y, mpv4->ppc.y)));
				mp->xmax = CEILI(MAX2(MAX4(mpv1->pp.x, mpv2->pp.x, mpv3->pp.x, mpv4->pp.x),
				                      MAX4(mpv1->ppc.x, mpv2->ppc.x, mpv3->ppc.x, mpv4->ppc.x)));
				mp->ymax = CEILI(MAX2(MAX4(mpv1->pp.y, mpv2->pp.y, mpv3->pp.y, mpv4->pp.y),
				                      MAX4(mpv1->ppc.y, mpv2->ppc.y, mpv3->ppc.y, mpv4->ppc.y)));
				mp->zmin = MIN2(MIN4(mpv1->pp.z, mpv2->pp.z, mpv3->pp.z, mpv4->pp.z),
				                MIN4(mpv1->ppc.z, mpv2->ppc.z, mpv3->ppc.z, mpv4->ppc.z));
				mp->zmax = MIN2(MAX4(mpv1->pp.z, mpv2->pp.z, mpv3->pp.z, mpv4->pp.z),
				                MAX4(mpv1->ppc.z, mpv2->ppc.z, mpv3->ppc.z, mpv4->ppc.z));
			}
			else {
				mp->xmin = FLOORI(MIN4(mpv1->pp.x, mpv2->pp.x, mpv3->pp.x, mpv4->pp.x));
				mp->ymin = FLOORI(MIN4(mpv1->pp.y, mpv2->pp.y, mpv3->pp.y, mpv4->pp.y));
				mp->xmax = CEILI(MAX4(mpv1->pp.x, mpv2->pp.x, mpv3->pp.x, mpv4->pp.x));
				mp->ymax = CEILI(MAX4(mpv1->pp.y, mpv2->pp.y, mpv3->pp.y, mpv4->pp.y));
				mp->zmin = MIN4(mpv1->pp.z, mpv2->pp.z, mpv3->pp.z, mpv4->pp.z);
				mp->zmax = MAX4(mpv1->pp.z, mpv2->pp.z, mpv3->pp.z, mpv4->pp.z);
			}
			// add to list
			mplist[x + y*xdim] = mp;
		}
	}

}

// bust a curve grid
// (maybe possible to directly rasterize a curve similar to points?
//  not sure...but then occlusion culling would be more difficult probably...)
void MicroPolygonGrid::bust_curves(std::vector<MicroPolygon*> &mplist, bool depthonly)
{
	// points, colors + curve widths and dPdu & dPdv (for curve orientation)
	RtPoint* P_grid = (RtPoint*)findVariable("P");
	assert(P_grid!=NULL);

	// if moving primitive, '=Pclose' is available
	RtPoint* eP_grid = (RtPoint*)findVariable("=Pclose");
	bool moving = (eP_grid != NULL);

	RtColor *Ci_grid = NULL, *Oi_grid = NULL;
	if (!depthonly) {
		Ci_grid = (RtColor*)findVariable("Ci");
		assert(Ci_grid!=NULL);
		Oi_grid = (RtColor*)findVariable("Oi");
		assert(Oi_grid!=NULL);
	}

	float* width_grid = findVariable("width");
	assert(width_grid!=NULL);
	RtVector* dPdu_grid = (RtVector*)findVariable("dPdu");
	assert(dPdu_grid!=NULL);
	RtVector* dPdv_grid = (RtVector*)findVariable("dPdv");
	assert(dPdv_grid!=NULL);

	// for curves, the number of mp vertices is twice number of curve points
	const unsigned int cverts = nverts << 1;
	std::vector<ShadedVertex*> mpv(cverts, NULL);

	// get/set the vertex variables
	unsigned int x, y;
	// width also affected by transform
	const Vector scalevec = 0.5f * parent->getTransformReference()->getScaleFactors();
	Camera& cam = State::Instance()->projcam;
	for (x=0;x<nverts;++x) {
		unsigned int cidx = x << 1;
		const Vector hwv = width_grid[x] * Vector(dPdu_grid[x][0], dPdu_grid[x][1], dPdu_grid[x][2]).normalize().scale(scalevec);
		// first vertex
		Point3 pt = Point3(P_grid[x][0], P_grid[x][1], P_grid[x][2]) - hwv;
		mpv[cidx] = new ShadedVertex();
		//mpv[cidx]->P = pt;
		mpv[cidx]->pp = cam.project(pt);
		if (moving)
			mpv[cidx]->ppc = cam.project(Point3(eP_grid[x][0], eP_grid[x][1], eP_grid[x][2]) - hwv);
		else
			mpv[cidx]->ppc = mpv[cidx]->pp;
		if (!depthonly) {
			mpv[cidx]->Ci.set(Ci_grid[x][0], Ci_grid[x][1], Ci_grid[x][2]);
			mpv[cidx]->Oi.set(Oi_grid[x][0], Oi_grid[x][1], Oi_grid[x][2]);
		}
		// opposite vertex
		pt = Point3(P_grid[x][0], P_grid[x][1], P_grid[x][2]) + hwv;
		cidx += 1;
		mpv[cidx] = new ShadedVertex();
		//mpv[cidx]->P = pt;
		mpv[cidx]->pp = cam.project(pt);
		if (moving)
			mpv[cidx]->ppc = cam.project(Point3(eP_grid[x][0], eP_grid[x][1], eP_grid[x][2]) + hwv);
		else
			mpv[cidx]->ppc = mpv[cidx]->pp;
		if (!depthonly) {
			mpv[cidx]->Ci.set(Ci_grid[x][0], Ci_grid[x][1], Ci_grid[x][2]);
			mpv[cidx]->Oi.set(Oi_grid[x][0], Oi_grid[x][1], Oi_grid[x][2]);
		}
	}

	// the new micropolygon list
	mplist.resize(ydim, NULL);
	x = 0;
	for (y=0; y<ydim; ++y) {
		ShadedVertex *mpv1 = mpv[x],  *mpv2 = mpv[x+1], *mpv3 = mpv[x+3], *mpv4 = mpv[x+2];
		x += 2;
		MicroPolygon* mp = new MicroPolygon(mpv1, mpv2, mpv3, mpv4);
		// calculate integer 2D bound of projected points
		if (moving) {
			// include points at shutter close time
			mp->xmin = FLOORI(MIN2(MIN4(mpv1->pp.x, mpv2->pp.x, mpv3->pp.x, mpv4->pp.x),
			                       MIN4(mpv1->ppc.x, mpv2->ppc.x, mpv3->ppc.x, mpv4->ppc.x)));
			mp->ymin = FLOORI(MIN2(MIN4(mpv1->pp.y, mpv2->pp.y, mpv3->pp.y, mpv4->pp.y),
			                       MIN4(mpv1->ppc.y, mpv2->ppc.y, mpv3->ppc.y, mpv4->ppc.y)));
			mp->xmax = CEILI(MAX2(MAX4(mpv1->pp.x, mpv2->pp.x, mpv3->pp.x, mpv4->pp.x),
			                      MAX4(mpv1->ppc.x, mpv2->ppc.x, mpv3->ppc.x, mpv4->ppc.x)));
			mp->ymax = CEILI(MAX2(MAX4(mpv1->pp.y, mpv2->pp.y, mpv3->pp.y, mpv4->pp.y),
			                      MAX4(mpv1->ppc.y, mpv2->ppc.y, mpv3->ppc.y, mpv4->ppc.y)));
			mp->zmin = MIN2(MIN4(mpv1->pp.z, mpv2->pp.z, mpv3->pp.z, mpv4->pp.z),
			                MIN4(mpv1->ppc.z, mpv2->ppc.z, mpv3->ppc.z, mpv4->ppc.z));
			mp->zmax = MIN2(MAX4(mpv1->pp.z, mpv2->pp.z, mpv3->pp.z, mpv4->pp.z),
			                MAX4(mpv1->ppc.z, mpv2->ppc.z, mpv3->ppc.z, mpv4->ppc.z));
		}
		else {
			mp->xmin = FLOORI(MIN4(mpv1->pp.x, mpv2->pp.x, mpv3->pp.x, mpv4->pp.x));
			mp->ymin = FLOORI(MIN4(mpv1->pp.y, mpv2->pp.y, mpv3->pp.y, mpv4->pp.y));
			mp->xmax = CEILI(MAX4(mpv1->pp.x, mpv2->pp.x, mpv3->pp.x, mpv4->pp.x));
			mp->ymax = CEILI(MAX4(mpv1->pp.y, mpv2->pp.y, mpv3->pp.y, mpv4->pp.y));
			mp->zmin = MIN4(mpv1->pp.z, mpv2->pp.z, mpv3->pp.z, mpv4->pp.z);
			mp->zmax = MAX4(mpv1->pp.z, mpv2->pp.z, mpv3->pp.z, mpv4->pp.z);
		}
		// add to list
		mplist[y] = mp;
	}

}

// Points are now a single shaded vertex, since shading is constant,
// it is also rasterized differently from regular mp's, so that perfect circles are possible
void MicroPolygonGrid::bust_points(std::vector<MicroPolygon*> &mplist, bool depthonly)
{
	RtPoint* P_grid = (RtPoint*)findVariable("P");
	assert(P_grid != NULL);

	// if moving primitive, '=Pclose' is available
	RtPoint* eP_grid = (RtPoint*)findVariable("=Pclose");
	bool moving = (eP_grid != NULL);

	float* width_grid = findVariable("width");
	assert(width_grid != NULL);

	// color data not needed for depth renders
	RtColor *Ci_grid = NULL, *Oi_grid = NULL;
	if (!depthonly) {
		// color
		Ci_grid = (RtColor*)findVariable("Ci");
		assert(Ci_grid!=NULL); // should never happen
		// opacity
		Oi_grid = (RtColor*)findVariable("Oi");
		assert(Oi_grid!=NULL); // should never happen
	}

	mplist.resize(nverts, NULL);
	Camera& cam = State::Instance()->projcam;
	for (unsigned int x=0; x<nverts; ++x) {

		ShadedVertex* mpv = new ShadedVertex();
		const Point3 ct = Point3(P_grid[x][0], P_grid[x][1], P_grid[x][2]);
		mpv->pp = cam.project(ct);
		if (moving)
			mpv->ppc = cam.project(Point3(eP_grid[x][0], eP_grid[x][1], eP_grid[x][2]));
		else
			mpv->ppc = mpv->pp;
		if (!depthonly) {
			mpv->Ci.set(Ci_grid[x][0], Ci_grid[x][1], Ci_grid[x][2]);
			mpv->Oi.set(Oi_grid[x][0], Oi_grid[x][1], Oi_grid[x][2]);
		}

		// projected radius (both x & y direction because of possibly different sampling rates)
		Point3 prad1 = cam.project(Point3(0.5f*width_grid[x], 0, 0) + ct);
		Point3 prad2 = cam.project(Point3(0, 0.5f*width_grid[x], 0) + ct);
		const float rad1 = (Point3(prad1.x, prad1.y, 0) - Point3(mpv->pp.x, mpv->pp.y, 0)).length();
		const float rad2 = (Point3(prad2.x, prad2.y, 0) - Point3(mpv->pp.x, mpv->pp.y, 0)).length();

		MicroPolygon* mp = new MicroPolygon(mpv, rad1*rad1);
		if (moving) {
			// include points at shutter close time
			mp->xmin = FLOORI(MIN2(mpv->pp.x - rad1, mpv->ppc.x - rad1));
			mp->ymin = FLOORI(MIN2(mpv->pp.y - rad2, mpv->ppc.y - rad2));
			mp->xmax = CEILI(MAX2(mpv->pp.x + rad1, mpv->ppc.x + rad1));
			mp->ymax = CEILI(MAX2(mpv->pp.y + rad2, mpv->ppc.y + rad2));
			mp->zmin = MIN2(mpv->pp.z, mpv->ppc.z);
			mp->zmax = MAX2(mpv->pp.z, mpv->ppc.z);
		}
		else {
			mp->xmin = FLOORI(mpv->pp.x - rad1);
			mp->ymin = FLOORI(mpv->pp.y - rad2);
			mp->xmax = CEILI(mpv->pp.x + rad1);
			mp->ymax = CEILI(mpv->pp.y + rad2);
			mp->zmin = mp->zmax = mpv->pp.z;
		}

		// add to list
		mplist[x] = mp;

	}

}

__END_QDRENDER

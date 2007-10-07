//------------------------------------------------------------------------------
// Base Primitive class
//------------------------------------------------------------------------------

#include "Primitive.h"
#include "State.h"
#include "Mathutil.h"
#include "MicroPolygonGrid.h"
#include "Framework.h"

#include "QDRender.h"
__BEGIN_QDRENDER

//------------------------------------------------------------------------------
// PrimVars

PrimVars::PrimVars() : refc(1)
{
}

PrimVars::~PrimVars()
{
	vardata_t** vdt = pvars.firstItem();
	while (vdt) {
		delete *vdt;
		vdt = pvars.nextItem();
	}
}

//------------------------------------------------------------------------------
// Primitive

Primitive& Primitive::operator=(const Primitive& p)
{
	std_pvar = p.std_pvar;
	std_dice = p.std_dice;
	attr = p.attr;
	xform = p.xform;
	eye_splits = p.eye_splits;
	primvars = p.primvars;
	if (primvars) primvars->incRefCount(); // !!!
	return *this;
}

Primitive::~Primitive()
{
	// !!! do not delete attr/xform !!!
	// delete primvars when no longer referenced
	if (primvars && primvars->decRefCount()==0) {
		delete primvars;
		primvars = NULL;
	}
}

void Primitive::initPrimVars(RtInt n, RtToken tokens[], RtPointer parms[],
				int uniformMax, int varyingMax, int vertexMax, int faceVaryingMax)
{
	if (n == 0) return; // no params
	// current transform, not known yet (this function is only called from geom.ctor's)
	// so have to get from State
	const Transform xf = State::Instance()->currentTransform();
	// normal transform
	Transform nxf = xf;
	nxf.invert().transpose();
	char inline_name[256] = {0};
	for (int i=0; i<n; i++) {
		if ((!strcmp(tokens[i], RI_P)) ||
		    (!strcmp(tokens[i], RI_PZ)) ||
		    (!strcmp(tokens[i], RI_PW)))
			continue; // "P" and variants already handled in geom code
		decParam_t dp = {0, 0, 0};
		if (!State::Instance()->parameterFromName(tokens[i], dp, inline_name, true)) {
			// inline parse error (errmsg already handled in parameterFromName())
			continue;
		}
		// string variables are in fact legal (though always constant),
		// but let's just pretend we don't know that...
		if (dp.ct_flags & DT_STRING) {
			printf("[ERROR]: primitive variables of type 'string' are not supported (yet), skipping...\n");
			continue;
		}
		unsigned int lenmult = 1;
		if (dp.ct_flags & DT_FLOAT3MASK) // point/vector/normal/color
			lenmult = 3;
		else if (dp.ct_flags & DT_MATRIX)
			lenmult = 16;
		else if ((dp.ct_flags & DT_FLOAT)==0) {
			printf("[ERROR]: Unexpected primitive variable data type of '%d' ( '%s' ), skipping...\n", dp.ct_flags, tokens[i]);
			continue;
		}
		// type 'constant' is always 1
		dp.numfloats = dp.arlen * lenmult;
		if (dp.ct_flags & SC_UNIFORM)
			dp.numfloats = dp.arlen * uniformMax * lenmult;
		else if (dp.ct_flags & SC_VARYING)
			dp.numfloats = dp.arlen * varyingMax * lenmult;
		else if (dp.ct_flags & SC_FACEVARYING)
			dp.numfloats = dp.arlen * faceVaryingMax * lenmult;
		else if (dp.ct_flags & SC_VERTEX)
			dp.numfloats = dp.arlen * vertexMax * lenmult;
		// new variable, copy ri array
		vardata_t* vdt = new vardata_t(dp, new float[dp.numfloats]);
		memcpy(vdt->data, parms[i], sizeof(float)*dp.numfloats);
		// transform to current space if necessary
		if (dp.ct_flags & DT_POINT) {
			RtPoint* pa = reinterpret_cast<RtPoint*>(vdt->data);
			for (int j=0; j<(dp.numfloats / 3); ++j)
				mulPMP(pa[j], *xf.getRtMatrixPtr(), pa[j]);
		}
		else if (dp.ct_flags & DT_VECTOR) {
			RtVector* va = reinterpret_cast<RtVector*>(vdt->data);
			for (int j=0; j<(dp.numfloats / 3); ++j)
				mulVMV(va[j], *xf.getRtMatrixPtr(), va[j]);
		}
		else if (dp.ct_flags & DT_NORMAL) {
			RtNormal* na = reinterpret_cast<RtNormal*>(vdt->data);
			for (int j=0; j<(dp.numfloats / 3); ++j)
				mulVMV(na[j], *nxf.getRtMatrixPtr(), na[j]); // can use vector multiply here, mulNMN() is not optimized
		}
		else if (dp.ct_flags & DT_MATRIX) {
			RtMatrix* ma = reinterpret_cast<RtMatrix*>(vdt->data);
			for (int j=0; j<(dp.numfloats / 16); ++j)
				mulMMM(ma[j], *xf.getRtMatrixPtr(), ma[j]);
		}
		const char* varname = (inline_name[0] == 0) ? tokens[i] : inline_name;
		// if global variable, set corresponding bit in std_pvar
		unsigned int sa_idx = 0;
		while (_sl_access[sa_idx].name) {
			if (!strcmp(_sl_access[sa_idx].name, varname)) {
				std_pvar |= (1 << sa_idx);
				break;
			}
			++sa_idx;
		}
		// 'primvars' only created once, duplicates get reference
		if (primvars == NULL) primvars = new PrimVars();
		primvars->pvars.addItem(varname, vdt);
	}
}


PrimVars* Primitive::newPrimVars()
{
	if (primvars) primvars->decRefCount(); // !!!
	primvars = new PrimVars();
	return primvars;
}

void Primitive::removePrimVar(const char* name)
{
	if (primvars) {
		vardata_t* vdt = NULL;
		primvars->pvars.removeItem(name, vdt);
		if (vdt) delete vdt;
	}
}

//------------------------------------------------------------------------------
// Generic dice for linear interpolation of primitive/shader variables

void Primitive::linear_dice(MicroPolygonGrid &g)
{
	if (primvars) {
		vardata_t** vdt = primvars->pvars.firstItem();
		if (vdt == NULL) return; // nothing in list
		float u, v;
		unsigned int ug, vg, idx = 0;
		const unsigned int xdim = g.get_xdim(), ydim = g.get_ydim(), nverts = g.get_nverts();
		float uvl[4];
		get_uvlim(uvl);
		const float ud = uvl[1] - uvl[0], vd = uvl[3] - uvl[2];
		const float du = 1.f/float(xdim), dv = 1.f/float(ydim);
		while (vdt) {
			bool varying = (((*vdt)->param.ct_flags & (SC_VARYING | SC_FACEVARYING | SC_VERTEX)) != 0);
			const char* name = primvars->pvars.getName();
			if ((*vdt)->param.ct_flags & DT_FLOAT) {
				RtFloat* Fgrid = g.findVariable(name);
				if (Fgrid == NULL)	// user defined variable, add a new variable and dice
					Fgrid = g.addVariable(name, 1);
				const RtFloat* da = (RtFloat*)(*vdt)->data;
				if (varying) {
					idx = 0;
					for (vg=0, v=0.f; vg<=ydim; ++vg, v+=dv)
						for (ug=0, u=0.f; ug<=xdim; ++ug, u+=du, ++idx)
							bilerpF(Fgrid[idx], uvl[0] + u*ud, uvl[2] + v*vd, da[0], da[1], da[2], da[3]);
				}
				else { // uniform/constant
					for (idx=0; idx<nverts; ++idx)
						Fgrid[idx] = da[0];
				}
			}
			else if ((*vdt)->param.ct_flags & DT_FLOAT3MASK) {
				// point/vector/normal/color
				RtVector* Vgrid = (RtVector*)g.findVariable(name);
				if (Vgrid == NULL)	// user defined variable, add a new variable and dice
					Vgrid = (RtVector*)g.addVariable(name, 3);
				const RtVector* da = (RtVector*)(*vdt)->data;
				if (varying) {
					idx = 0;
					for (vg=0, v=0.f; vg<=ydim; ++vg, v+=dv)
						for (ug=0, u=0.f; ug<=xdim; ++ug, u+=du, ++idx)
							bilerp(Vgrid[idx], uvl[0] + u*ud, uvl[2] + v*vd, da[0], da[1], da[2], da[3]);
				}
				else { // uniform/constant
					for (idx=0; idx<nverts; ++idx)
						Vgrid[idx][0] = da[0][0], Vgrid[idx][1] = da[0][1], Vgrid[idx][2] = da[0][2];
				}
			}
			// matrix/hpoint TODO
			// next variable
			vdt = primvars->pvars.nextItem();
		}
	}
}

//------------------------------------------------------------------------------
// BlurredPrimitive
// split & dice params of first prim MUST be used for all others!

BlurredPrimitive::BlurredPrimitive()
{
	const Options& opts = State::Instance()->topOptions();
	shmin = opts.openShutter, shmax = opts.closeShutter;
}

BlurredPrimitive::BlurredPrimitive(const BlurredPrimitive& bp) : Primitive()	// init base
{
	// copy base prim data
	static_cast<Primitive&>(*this) = bp;
	// assuming copy ctor is called because of split(), poses array therefore not copied here, gets new ones
	motion_xform = bp.motion_xform;
	shmin = bp.shmin, shmax = bp.shmax;
}

BlurredPrimitive::~BlurredPrimitive()
{
	for (std::vector<Primitive*>::iterator ai=poses.begin(); ai!=poses.end(); ++ai)
		delete *ai;
	poses.clear();
}

void BlurredPrimitive::post_init()
{
	for (std::vector<Primitive*>::iterator ai=poses.begin(); ai!=poses.end(); ++ai)
		(*ai)->post_init();
}

bool BlurredPrimitive::in_camspace() const
{
	if (poses.empty()) return false;
	return poses[0]->in_camspace();
}

bool BlurredPrimitive::boundable()
{
	if (poses.empty()) return false;
	return poses[0]->boundable();
}

Bound BlurredPrimitive::bound()
{
	if (poses.empty()) return Bound();
	// combined bound of all primitives
	Bound b;
	if (!motion_xform.empty()) {
		// include transformational mblur
		Bound tb = poses[0]->bound();
		tb.transform(&motion_xform[0]);
		b.include(tb);
		if (motion_xform.size() == 2) {
			if (poses.size() == 2)
				tb = poses[1]->bound();
			else
				tb = poses[0]->bound();
			tb.transform(&motion_xform[1]);
			b.include(tb);
		}
	}
	else {
		// poses only
		for (std::vector<Primitive*>::iterator ai=poses.begin(); ai!=poses.end(); ++ai)
			b.include((*ai)->bound());
	}
	return b;
}

bool BlurredPrimitive::splitable()
{
	if (poses.empty()) return false;
	return poses[0]->splitable();
}

// split the blurred primitive(s).
// This is done by calling the split() func. of each prim in the poses array,
// using a struct containing the parent BlurredPrimitive and an array, initially empty, which will contain the new split prims.
// The split function of the underlying primitive then is responsible for creating new BlurredPrimitive(s),
// and/or appending new copies of the split base primitive.
// First split() call will create and append, following calls will only append to created bprims in first call.
// The splitbprims arg here in this function has no meaning.
void BlurredPrimitive::split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb)
{
	if (!poses.empty()) {
		// all primitives must be split with the same parameters
		splitbprims_t splitbprims;
		splitbprims.parent = this;
		for (std::vector<Primitive*>::iterator pose=poses.begin(); pose!=poses.end(); ++pose)
			(*pose)->split(f, usplit, vsplit, &splitbprims);
		// if any created, bprim(s) can now be inserted into framework
		for (Array<BlurredPrimitive*>::iterator bi=splitbprims.bprims.begin(); bi!=splitbprims.bprims.end(); ++bi)
			f.insert(*bi);
	}
}

bool BlurredPrimitive::diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit)
{
	if (poses.empty()) return false;
	return poses[0]->diceable(g, h, usplit, vsplit);
}

// Pclose arg has no meaning here
void BlurredPrimitive::dice(MicroPolygonGrid &g, bool Pclose)
{
	if (!poses.empty()) {
		const unsigned int nverts = g.get_nverts();
		// all primitives must be diced exactly the same
		if (poses.size() == 2) {
			// more than one primitive
			poses[0]->dice(g);
			// dice shutter close time, dices to '=Pclose' grid (this is the only dice() call in the program where Pclose arg is used)
			poses[1]->dice(g, true);
			if (motion_xform.size() == 2) {
				// transform needed as well
				// since this is always done for all grids, this can be extremely expensive, especially camera blur...
				// even small shutter times then hardly have any impact on rendertimes.
				// Possible solution would be to concatenate transforms for the base prims, so pre-transform would be possible,
				// but then memory might be a problem, since re-use of transforms would not happen very often anymore probably... TODO
				RtPoint* P_grid = (RtPoint*)g.findVariable("P");
				const RtMatrix* mtx0 = motion_xform[0].getRtMatrixPtr();
				for (unsigned int i=0; i<nverts; ++i)
					mulPMP(P_grid[i], *mtx0, P_grid[i]);
				RtPoint* ePclose_grid = (RtPoint*)g.findVariable("=Pclose");
				const RtMatrix* mtx1 = motion_xform[1].getRtMatrixPtr();
				for (unsigned int i=0; i<nverts; ++i)
					mulPMP(ePclose_grid[i], *mtx1, ePclose_grid[i]);
			}
		}
		else if ((poses.size() == 1) && !motion_xform.empty()) {
			// single blurred primitive, transform only
			poses[0]->dice(g);
			RtPoint* P_grid = (RtPoint*)g.findVariable("P");
			// since only 'P' diced, add '=Pclose' grid here
			RtPoint* ePclose_grid = (RtPoint*)g.addVariable("=Pclose");
			const RtMatrix* mtx1 = motion_xform[1].getRtMatrixPtr();
			for (unsigned int i=0; i<nverts; ++i)
				mulPMP(ePclose_grid[i], *mtx1, P_grid[i]);
			// transform orig. P to first motion xform
			const RtMatrix* mtx0 = motion_xform[0].getRtMatrixPtr();
			for (unsigned int i=0; i<nverts; ++i)
				mulPMP(P_grid[i], *mtx0, P_grid[i]);
		}
	}
}

void BlurredPrimitive::append(Primitive* p)
{
	// for now, not more than two primitives, more shutter key times TODO
	if (poses.size() < 2)
		poses.push_back(p);
	else
		printf("[WARNING]: BlurredPrimitive::append() -> already have two prims\n");
}

__END_QDRENDER

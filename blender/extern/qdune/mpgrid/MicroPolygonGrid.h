#ifndef _MICROPOLYGONGRID_H
#define _MICROPOLYGONGRID_H

#include "qdVector.h"
#include "Color.h"
#include "ri.h"
#include "qdtl.h"
#include <vector>

#include "QDRender.h"
__BEGIN_QDRENDER

class Primitive;
class MicroPolygon;
class Camera;

class SlShader;
class MicroPolygonGrid
{
private:
	MicroPolygonGrid(const MicroPolygonGrid&);
	MicroPolygonGrid& operator=(const MicroPolygonGrid&);
public:
	MicroPolygonGrid();
	~MicroPolygonGrid();

	void setDim(unsigned int _xdim, unsigned int _ydim, const Primitive* _parent);
	void setFrom(const MicroPolygonGrid& mg);
	unsigned int get_xdim() const { return xdim; }
	unsigned int get_ydim() const { return ydim; }
	unsigned int get_nverts() const { return nverts; }
	const Primitive* get_parent() const { return parent; }

	float* addVariable(RtToken name, int grid_element_size=3);
	void deleteVariable(RtToken name);
	float* findVariable(RtToken name);

	void initVars(unsigned int varbits);

	bool isBackfacing();

	bool trim();
	void displace();
	void shade();

	void bust(std::vector<MicroPolygon*> &mplist, bool depthonly=false);

	const Primitive* getParent() const { return parent; }

	// normals/derivatives
	bool calculateNormals(RtNormal* N_grid, const RtPoint* P_grid);
	void DuF(RtFloat* dst_grid, const RtFloat* src_grid);
	void DvF(RtFloat* dst_grid, const RtFloat* src_grid);
	void DuV(RtVector* dst_grid, const RtVector* src_grid);
	void DvV(RtVector* dst_grid, const RtVector* src_grid);
	void DerivF(RtFloat* dst_grid, const RtFloat* num_grid, const RtFloat* den_grid);
	void DerivV(RtVector* dst_grid, const RtVector* num_grid, const RtFloat* den_grid);
	void Area(RtFloat* dst_grid, const RtPoint* P_grid);

	// type of grid, points & curves are special cases, see bust()
	enum {G_NORMAL, G_POINTS, G_CURVES} gtype;

	// this is set to true if Ng was already updated from N in the shader
	// see sl_Calcnormals() in shaderVM.cpp
	bool Ng_updated;

protected:
	// mtds
	void bust_points(std::vector<MicroPolygon*> &mplist, bool depthonly);
	void bust_curves(std::vector<MicroPolygon*> &mplist, bool depthonly);
	bool analyticNormals();	// normals from dPdu ^ dPdv
	// data
	unsigned int xdim, ydim;  // Grid dimensions
	unsigned int nverts;      // Total vertices = (xdim+1) * (ydim+1)
	const Primitive *parent;  // Primitive from which we came
	// a linked list with string keys actually is slightly faster than a hashtable in this case,
	// maybe because the list is usually short, not sure
	NamedLinkedList_t<float*> vertexvars;
};

__END_QDRENDER

#endif //_MICROPOLYGONGRID_H



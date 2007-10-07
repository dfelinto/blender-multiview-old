//----------------------------------------------
// Polygons
//----------------------------------------------

#ifndef _POLYGONS_H
#define _POLYGONS_H

#include "Primitive.h"

#include "QDRender.h"
__BEGIN_QDRENDER

// only used to split polygon into bilinear patches, does not dice
class Polygon : public Primitive
{
public:
	Polygon(RtInt nverts, RtInt n, RtToken tokens[], RtPointer parms[]);
	virtual ~Polygon();
	virtual void post_init() {}
	virtual bool boundable() { return true; }
	virtual Bound bound();
	virtual bool splitable() { return true; }
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL);
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit) { return false; }
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false);
protected:
	RtPoint* pts;
	int num_verts;
};

// as above, does not dice, only splits
class PointsPolygons : public Primitive
{
public:
	PointsPolygons(RtInt npolys, RtInt nverts[], RtInt verts[],
								RtInt n, RtToken tokens[], RtPointer parms[]);
	virtual ~PointsPolygons();
	virtual void post_init() {}
	virtual bool boundable() { return true; }
	virtual Bound bound();
	virtual bool splitable() { return true; }
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL);
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit) { return false; }
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false);
protected:
	RtInt *vert_per_face, *vert_idx;
	RtPoint* pts;
	int num_faces, num_verts, max_fvert;
};

__END_QDRENDER

#endif // _POLYGONS_H


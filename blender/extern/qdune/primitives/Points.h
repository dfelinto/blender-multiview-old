//----------------------------------------------
// Points (particles)
//----------------------------------------------

#ifndef _POINTS_H
#define _POINTS_H

#include "Primitive.h"

#include "QDRender.h"
__BEGIN_QDRENDER

class Point3;
class PointSet : public Primitive
{
	PointSet(const PointSet&);
	PointSet& operator=(const PointSet&);
public:
	//PointSet():P(NULL), pp(NULL), vary_width(NULL), totverts(0), const_width(0), split_axis(0) {}
	PointSet():P(NULL), vary_width(NULL), totverts(0), const_width(0), split_axis(0) {}
	PointSet(RtInt nverts, RtInt n, RtToken tokens[], RtPointer parms[]);
	virtual ~PointSet();
	virtual void post_init();
	virtual bool in_camspace() const { return true; }
	virtual bool boundable() { return true; }
	virtual Bound bound();
	virtual bool splitable() { return true; }
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL);
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit);
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false);
protected:
	void calcSplitAxis(bool &usplit, bool &vsplit);
	void splitPrimVars(Primitive* prim1, Primitive* prim2, int tv1, int tv2, int axis_idx, unsigned int halfnum=0);
	RtPoint* P;
	//Point3* pp;
	RtFloat* vary_width;
	unsigned int totverts;
	float const_width;
	float split_axis;	// only used when splitting
};

__END_QDRENDER

#endif // _POINTS_H

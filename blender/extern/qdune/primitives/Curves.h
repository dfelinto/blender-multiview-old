//----------------------------------------------
// Linear & Cubic Curves
//----------------------------------------------

#ifndef _CURVES_H
#define _CURVES_H

#include "Primitive.h"

#include "QDRender.h"
__BEGIN_QDRENDER

class CurveSet : public Primitive
{
private:
	CurveSet(const CurveSet&);
	CurveSet& operator=(const CurveSet&);
public:
	CurveSet(bool linear, int ncurves, int nvertices[], bool periodic,
							RtInt n, RtToken tokens[], RtPointer parms[]);
	virtual ~CurveSet();
	virtual void post_init();
	virtual bool in_camspace() const { return true; }
	virtual bool boundable() { return true; }
	virtual Bound bound();
	virtual bool splitable() { return true; }
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL);
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit) { return false; }
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false);
protected:
	bool islinear;
	RtInt* nverts;
	RtPoint* P;
	RtFloat* vary_width;
	RtNormal* vary_N;
	bool wrap;
	int totcurves, totverts, totvary, vstep;
	float const_width, max_width;
};


class LinearSegment : public Primitive
{
	friend class CurveSet;
private:
	LinearSegment(const LinearSegment&);
	LinearSegment& operator=(const LinearSegment&);
public:
	LinearSegment(); // no args, all data init done by CurveSet
	virtual ~LinearSegment();
	virtual void post_init() {}
	virtual bool in_camspace() const { return true; }
	virtual bool boundable() { return true; }
	virtual Bound bound();
	virtual bool splitable() { return true; }
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL);
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit);
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false);
	virtual void get_uvlim(float uvl[4]) const { uvl[0] = uvl[1] = 0.f, uvl[2] = vmin, uvl[3] = vmax; }
protected:
	RtPoint P[2];
	RtFloat width[2];
	RtNormal N[2];
	bool have_vary_N;
	float vmin, vmax;
};


class BezierSegment : public Primitive
{
	friend class CurveSet;
private:
	BezierSegment(const BezierSegment&);
	BezierSegment& operator=(const BezierSegment&);
public:
	BezierSegment(); // no args, all data init done by CurveSet
	virtual ~BezierSegment();
	virtual void post_init() {}
	virtual bool in_camspace() const { return true; }
	virtual bool boundable() { return true; }
	virtual Bound bound();
	virtual bool splitable() { return true; }
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL);
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit);
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false);
	virtual void get_uvlim(float uvl[4]) const { uvl[0] = uvl[1] = 0.f, uvl[2] = vmin, uvl[3] = vmax; }
protected:
	RtPoint P[4];
	RtFloat width[2];
	RtNormal N[2];
	bool have_vary_N;
	float vmin, vmax;
};

__END_QDRENDER

#endif // _CURVES_H

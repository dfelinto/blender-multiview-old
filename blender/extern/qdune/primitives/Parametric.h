#ifndef _PARAMETRIC_H
#define _PARAMETRIC_H

#include "Primitive.h"

#include "QDRender.h"
__BEGIN_QDRENDER

// generic parametric primitive
class Parametric : public Primitive
{
private:
	Parametric& operator=(const Parametric&);
public:
	Parametric();
	Parametric(const Parametric& p);
	virtual ~Parametric();
	virtual void post_init() {}
	virtual bool boundable() { return true; }
	virtual Bound bound()=0;
	virtual bool splitable() { return true; }
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL);
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit);
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false);
	virtual Parametric* duplicateSelf()=0;
	virtual void get_uvlim(float uvl[4]) const { uvl[0] = umin, uvl[1] = umax, uvl[2] = vmin, uvl[3] = vmax; }
protected:
	// mtds
	// previous separate evalfuncs replaced with single function for P, dPdu & dPdv
	virtual void eval(float u, float v, RtPoint P, RtVector dPdu=NULL, RtVector dPdv=NULL) const=0;
	// data
	float umin, umax, vmin, vmax;
};

__END_QDRENDER

#endif // _PARAMETRIC_H

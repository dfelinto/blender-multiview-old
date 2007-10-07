//----------------------------------------------
// Non-Uniform Rational B-Splines, NuPatches
//----------------------------------------------

#ifndef _NURBS_H
#define _NURBS_H

#include "Primitive.h"
#include "qdtl.h"

#include "QDRender.h"
__BEGIN_QDRENDER

class NuPatch : public Primitive
{
private:
	NuPatch(const NuPatch&);
	NuPatch& operator=(const NuPatch&);
public:
	NuPatch():nudata(NULL), umin(0), vmin(0), umax(0), vmax(0) {}
	NuPatch(RtInt _nu, RtInt _uorder, RtFloat _uknot[], RtFloat _umin, RtFloat _umax,
	        RtInt _nv, RtInt _vorder, RtFloat _vknot[], RtFloat _vmin, RtFloat _vmax,
	        RtInt n, RtToken tokens[], RtPointer parms[]);
	virtual ~NuPatch();
	virtual void post_init();

	virtual bool boundable() { return true; }
	virtual Bound bound();

	virtual bool splitable() { return true; }
	virtual void split(const Framework &f, bool usplit, bool vsplit, splitbprims_t* spb=NULL);
	virtual bool diceable(MicroPolygonGrid &g, Hider &h, bool &usplit, bool &vsplit);
	virtual void dice(MicroPolygonGrid &g, bool Pclose=false);

	Primitive* duplicateSelf();
	virtual bool in_camspace() const { return true; }

	virtual void get_uvlim(float uvl[4]) const { uvl[0] = umin, uvl[1] = umax, uvl[2] = vmin, uvl[3] = vmax; }

protected:
	// mtds
	void eval(float u, float v, RtPoint P, RtVector dPdu = NULL, RtVector dPdv = NULL);
	// data
	struct NuData {
		//NuData():Pw(NULL), nu(0), nv(0), uorder(0), vorder(0), uknot(NULL), vknot(NULL), refc(1) {}
		NuData(RtInt _nu, RtInt _uorder, RtFloat _uknot[],
	         RtInt _nv, RtInt _vorder, RtFloat _vknot[])
	        : Pw(NULL), nu(_nu), nv(_nv), uorder(_uorder), vorder(_vorder), refc(1)
	  {
			uknot.resize(nu + uorder);
			memcpy(&uknot[0], _uknot, sizeof(float)*(nu + uorder));
			vknot.resize(nv + vorder);
			memcpy(&vknot[0], _vknot, sizeof(float)*(nv + vorder));
			bu.resize(uorder);
			buprime.resize(uorder);
			bv.resize(vorder);
			bvprime.resize(vorder);
		}
		~NuData() { if (Pw) delete[] Pw;  Pw = NULL; }
		FixedArray<float> bu, bv, buprime, bvprime, uknot, vknot;
		RtHpoint* Pw;
		int nu, nv, uorder, vorder;
		int refc;
	};
	NuData* nudata;
	float umin, vmin, umax, vmax;
};

__END_QDRENDER

#endif // _NURBS_H

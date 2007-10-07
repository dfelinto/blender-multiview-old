//-------------------------------------------
// The standard renderman quadric primitives
//-------------------------------------------

#ifndef _QUADRICS_H
#define _QUADRICS_H

#include "Parametric.h"
#include "qdVector.h"

#include "QDRender.h"
__BEGIN_QDRENDER

class Disk : public Parametric
{
public:
	Disk();
	Disk(float height, float radius, float thetamax,
			RtInt n, RtToken tokens[], RtPointer parms[]);
	virtual ~Disk() {}
	virtual Bound bound();
	virtual Parametric* duplicateSelf() { return new Disk(*this); }
protected:
	virtual void eval(float u, float v, RtPoint P, RtVector dPdu=NULL, RtVector dPdv=NULL) const;
	float height, radius, thetamax;
};

class Sphere : public Parametric
{
public:
	Sphere():spdata(NULL) {}
	Sphere(float _radius, float _zmin, float _zmax, float _thetamax,
				RtInt n, RtToken tokens[], RtPointer parms[]);
	virtual ~Sphere();
	virtual Bound bound();
	virtual Parametric* duplicateSelf();
protected:
	virtual void eval(float u, float v, RtPoint P, RtVector dPdu=NULL, RtVector dPdv=NULL) const;
	// reference counted data experiment, not used in other Quadric classes yet
	struct spData
	{
		spData():radius(1), zmin(-1), zmax(1), thetamax(6.283185307f),
		         alphamin(-1.570796327f), alphadelta(3.141592654f), refc(1) {}
		float radius, zmin, zmax, thetamax;
		float alphamin, alphadelta;
		int refc;
	};
	spData* spdata;
};

class Cylinder : public Parametric
{
public:
	Cylinder();
	Cylinder(float _radius, float _zmin, float _zmax, float _thetamax,
					RtInt n, RtToken tokens[], RtPointer parms[]);
	virtual ~Cylinder() {}
	virtual Bound bound();
	virtual Parametric* duplicateSelf() { return new Cylinder(*this); }
protected:
	virtual void eval(float u, float v, RtPoint P, RtVector dPdu=NULL, RtVector dPdv=NULL) const;
	float radius, zmin, zmax, thetamax;
};

class Cone : public Parametric
{
public:
	Cone();
	Cone(float _height, float _radius, float _thetamax,
			RtInt n, RtToken tokens[], RtPointer parms[]);
	virtual ~Cone() {}
	virtual Bound bound();
	virtual Parametric* duplicateSelf() { return new Cone(*this); }
protected:
	virtual void eval(float u, float v, RtPoint P, RtVector dPdu=NULL, RtVector dPdv=NULL) const;
	float height, radius, thetamax;
};

class Paraboloid : public Parametric
{
public:
	Paraboloid();
	Paraboloid(float _rmax, float _zmin, float _zmax, float _thetamax,
						RtInt n, RtToken tokens[], RtPointer parms[]);
	virtual ~Paraboloid() {}
	virtual Bound bound();
	virtual Parametric* duplicateSelf() { return new Paraboloid(*this);}
protected:
	virtual void eval(float u, float v, RtPoint P, RtVector dPdu=NULL, RtVector dPdv=NULL) const;
	float scale, rmax, zmin, zmax, thetamax;
};

class Hyperboloid : public Parametric
{
public:
	Hyperboloid();
	Hyperboloid(const Point3 &_point1, const Point3 &_point2, float _thetamax,
							RtInt n, RtToken tokens[], RtPointer parms[]);
	virtual ~Hyperboloid() {}
	virtual Bound bound();
	virtual Parametric* duplicateSelf() { return new Hyperboloid(*this); }
protected:
	virtual void eval(float u, float v, RtPoint P, RtVector dPdu=NULL, RtVector dPdv=NULL) const;
	Point3 p1, p2;
	float thetamax;
};

class Torus : public Parametric
{
public:
	Torus();
	Torus(float _majrad, float _minrad, float _phimin, float _phimax, float _thetamax,
				RtInt n, RtToken tokens[], RtPointer parms[]);
	virtual ~Torus() {}
	virtual Bound bound();
	virtual Parametric* duplicateSelf() { return new Torus(*this); }
protected:
	virtual void eval(float u, float v, RtPoint P, RtVector dPdu=NULL, RtVector dPdv=NULL) const;
	float majrad, minrad, phimin, phimax, thetamax;
};

__END_QDRENDER

#endif // _QUADRICS_H


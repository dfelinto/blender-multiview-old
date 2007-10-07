//-------------------------------------------
// The standard renderman quadric primitives
//-------------------------------------------

#include "Quadrics.h"
#include "Mathutil.h"
#include "State.h"

__BEGIN_QDRENDER

//------------------------------------------------------------------------------
// functions used for bound calculation

// 2D polar to cartesian
inline Point3 polar2D(float radius, float theta)
{
	return Point3(cosf(theta)*radius, sinf(theta)*radius, 0);
}

// parametric 2D bound calc.
inline Bound bounds2DSweepL(float rmin, float rmax, float tmin, float tmax)
{
	Bound b;
	b.include(polar2D(rmin, tmin));
	b.include(polar2D(rmax, tmin));
	b.include(polar2D(rmin, tmax));
	b.include(polar2D(rmax, tmax));
	if ((tmin < (float)M_PI_2) && (tmax > (float)M_PI_2)) b.include(polar2D(rmax, (float)M_PI_2));
	if ((tmin < (float)M_PI) && (tmax > (float)M_PI)) b.include(polar2D(rmax, (float)M_PI));
	if ((tmin < float(M_PI+M_PI_2)) && (tmax > float(M_PI+M_PI_2))) b.include(polar2D(rmax, float(M_PI+M_PI_2)));
	return b;
}

// only used by torus
inline Bound bounds2DSweepR(float r, float phimin, float phimax)
{
	Bound b;
	b.include(polar2D(r, phimin));
	b.include(polar2D(r, phimax));
	for (int i=-3; i<4; i++) {
		const float ip = i*(float)M_PI_2;
		if ((phimin < ip) && (phimax > ip)) b.include(polar2D(r, ip));
	}
	return b;
}

// only used by hyperboloid
inline Bound bounds2DSweepP(const Point3 &P, float tmin, float tmax)
{
	Bound b;
	const float r = sqrt(P.x*P.x + P.y*P.y);
	const float delta = ((P.x==0.f) && (P.y==0.f)) ? 0.f : atan2f(P.y, P.x);
	tmin += delta;
	tmax += delta;
	b.include(polar2D(r, tmin));
	b.include(polar2D(r, tmax));
	// quadrant points
	for (int i=-1; i<6; i++) {
		const float ip = i*(float)M_PI_2;
		if ((tmin < ip) && (tmax > ip)) b.include(polar2D(r, ip));
	}
	return b;
}

//------------------------------------------------------------------------------
// Quadric primitives (including quartic torus)
// all angle values that specify the extent are clamped to (0, 360) range

//------------------------------------------------------------------------------
// Disk

Disk::Disk():height(1), radius(1), thetamax(2.f*(float)M_PI)
{
}

Disk::Disk(float _height, float _radius, float _thetamax,
					RtInt n, RtToken tokens[], RtPointer parms[])
					: height(_height), radius(_radius)
{
	radians(thetamax, CLAMP(ABS(_thetamax), 0.f, 360.f));
	Primitive::initPrimVars(n, tokens, parms, 1, 4, 4, 4);
}

Bound Disk::bound()
{
	Bound b = bounds2DSweepL(radius*(1.f-vmax), radius*(1.f-vmin), umin*thetamax, umax*thetamax);
	b.minmax[0].z = b.minmax[1].z = height;
	b.addEpsilon();
	b.transform(xform);
	return b;
}

void Disk::eval(float u, float v, RtPoint P, RtVector dPdu, RtVector dPdv) const
{
	const float t = u*thetamax;
	const float sint = sinf(t), cost = cosf(t);
	float r = radius*(1.f - v);
	P[0] = r*cost, P[1] = r*sint, P[2] = height;
	if (dPdu && dPdv) {
		r = radius*thetamax;
		dPdu[0] = -r*sint, dPdu[1] = r*cost, dPdu[2] = 0;
		dPdv[0] = -radius*cost, dPdv[1] = -radius*sint, dPdv[2] = 0;
	}
}

//------------------------------------------------------------------------------
// Sphere, uses reference counted data to test if that would improve
// memory usage, but probably can be removed.
// Probably doesn't help much, unless the sphere is split often.

Sphere::Sphere(float _radius, float _zmin, float _zmax, float _thetamax,
							RtInt n, RtToken tokens[], RtPointer parms[])
{
	spdata = new spData();
	spdata->radius = _radius;
	// keep zmin/zmax within +/- radius range
	spdata->zmin = MAX2(-spdata->radius, _zmin);
	spdata->zmax = MIN2(spdata->radius, _zmax);
	if (spdata->radius<=0.f) spdata->radius = 1e-5f;
	radians(spdata->thetamax, CLAMP(ABS(_thetamax), 0.f, 360.f));
	spdata->alphamin = asinf(CLAMP(spdata->zmin/spdata->radius, -1.f, 1.f));
	spdata->alphadelta = asinf(CLAMP(spdata->zmax/spdata->radius, -1.f, 1.f)) - spdata->alphamin;
	Primitive::initPrimVars(n, tokens, parms, 1, 4, 4, 4);
}

Sphere::~Sphere()
{
	if (spdata && (--spdata->refc == 0)) {
		delete spdata;
		spdata = NULL;
	}
}

Bound Sphere::bound()
{
	const float avmin = spdata->alphamin + vmin*spdata->alphadelta;
	const float avmax = spdata->alphamin + vmax*spdata->alphadelta;
	const float rvmin = cosf(avmin)*spdata->radius;
	const float rvmax = cosf(avmax)*spdata->radius;
	const float rmin = MIN2(rvmin, rvmax);
	const float rmax = ((avmin<0.f) && (avmax>0.f)) ? spdata->radius : (MAX2(rvmin, rvmax));
	Bound b = bounds2DSweepL(rmin, rmax, umin*spdata->thetamax, umax*spdata->thetamax);
	b.minmax[0].z = sinf(avmin)*spdata->radius;
	b.minmax[1].z = sinf(avmax)*spdata->radius;
	b.addEpsilon();
	b.transform(xform);
	return b;
}

void Sphere::eval(float u, float v, RtPoint P, RtVector dPdu, RtVector dPdv) const
{
	// correction for singularity at poles (only needed for correct normals, probably precision issue, get correct results for 0,0 in py)
	u = MAX2(1e-6f, u), v = MAX2(1e-6f, v);
	const float alpha = spdata->alphamin + v*spdata->alphadelta;
	const float car = cosf(alpha)*spdata->radius, t = u*spdata->thetamax;
	const float sint = sinf(t), cost = cosf(t), sina = sinf(alpha);
	P[0] = cost*car, P[1] = sint*car, P[2] = sina*spdata->radius;
	if (dPdu && dPdv) {
		dPdu[0] = -spdata->thetamax*sint*car, dPdu[1] = spdata->thetamax*cost*car, dPdu[2] = 0;
		const float ar = spdata->alphadelta*spdata->radius;
		const float sar = -sina*ar;
		dPdv[0] = cost*sar, dPdv[1] = sint*sar, dPdv[2] = cosf(alpha)*ar;
	}
}

Parametric* Sphere::duplicateSelf()
{
	Sphere* sp = new Sphere(*this);
	if (spdata) spdata->refc++;
	return sp;
}

//------------------------------------------------------------------------------
// Cylinder

Cylinder::Cylinder(): radius(1), zmin(-1), zmax(1), thetamax(2.f*(float)M_PI)
{
}

Cylinder::Cylinder(float _radius, float _zmin, float _zmax, float _thetamax,
									RtInt n, RtToken tokens[], RtPointer parms[])
									: radius(_radius), zmin(_zmin), zmax(_zmax)
{
	radians(thetamax, CLAMP(ABS(_thetamax), 0.f, 360.f));
	Primitive::initPrimVars(n, tokens, parms, 1, 4, 4, 4);
}

Bound Cylinder::bound()
{
	Bound b = bounds2DSweepL(radius, radius, umin*thetamax, umax*thetamax);
	b.minmax[0].z = zmin + vmin*(zmax-zmin);
	b.minmax[1].z = zmin + vmax*(zmax-zmin);
	b.addEpsilon();
	b.transform(xform);
	return b;
}

void Cylinder::eval(float u, float v, RtPoint P, RtVector dPdu, RtVector dPdv) const
{
	const float t = u*thetamax;
	const float sint = sinf(t), cost = cosf(t);
	if (dPdu && dPdv) {
		const float tr = thetamax*radius;
		dPdu[0] = -sint*tr, dPdu[1] = cost*tr;
		dPdu[2] = dPdv[0] = dPdv[1] = 0, dPdv[2] = zmax - zmin;
		P[0] = cost*radius, P[1] = sint*radius, P[2] = zmin + v*dPdv[2];
		return;
	}
	P[0] = cost*radius, P[1] = sint*radius, P[2] = zmin + v*(zmax - zmin);
}

//------------------------------------------------------------------------------
// Cone

Cone::Cone(): height(1), radius(1), thetamax(2.f*(float)M_PI)
{
}

Cone::Cone(float _height, float _radius, float _thetamax,
					RtInt n, RtToken tokens[], RtPointer parms[])
					: height(_height), radius(_radius)
{
	radians(thetamax, CLAMP(ABS(_thetamax), 0.f, 360.f));
	Primitive::initPrimVars(n, tokens, parms, 1, 4, 4, 4);
}

Bound Cone::bound()
{
	Bound b = bounds2DSweepL(radius*(1.f-vmax), radius*(1.f-vmin), umin*thetamax, umax*thetamax);
	b.minmax[0].z = vmin*height;
	b.minmax[1].z = vmax*height;
	b.addEpsilon();
	b.transform(xform);
	return b;
}

void Cone::eval(float u, float v, RtPoint P, RtVector dPdu, RtVector dPdv) const
{
	const float t = u*thetamax, rv = radius*(1.f - v);
	const float sint = sinf(t), cost = cosf(t);
	P[0] = cost*rv, P[1] = sint*rv, P[2] = v*height;
	if (dPdu && dPdv) {
		const float tr = thetamax*radius*(1.f - v);
		dPdu[0] = -sint*tr, dPdu[1] = cost*tr, dPdu[2] = 0;
		dPdv[0] = -cost*radius, dPdv[1] = -sint*radius, dPdv[2] = height;
	}
}

//------------------------------------------------------------------------------
// Paraboloid

Paraboloid::Paraboloid(): scale(1), rmax(1), zmin(0), zmax(1), thetamax(2.f*(float)M_PI)
{
}

Paraboloid::Paraboloid(float _rmax, float _zmin, float _zmax, float _thetamax,
											RtInt n, RtToken tokens[], RtPointer parms[])
{
	rmax = _rmax;
	// zmin cannot be negative in this case
	zmin = (_zmin<0.f) ? 0.f : _zmin;
	zmax = (_zmax<0.f) ? 0.f : _zmax;
	scale = rmax;
	if (zmax!=0.f) scale /= sqrt(zmax);
	radians(thetamax, CLAMP(ABS(_thetamax), 0.f, 360.f));
	Primitive::initPrimVars(n, tokens, parms, 1, 4, 4, 4);
}

Bound Paraboloid::bound()
{
	const float z1 = zmin + vmin*(zmax-zmin);
	const float z2 = zmin + vmax*(zmax-zmin);
	Bound b = bounds2DSweepL(sqrt(z1)*scale, sqrt(z2)*scale, umin*thetamax, umax*thetamax);
	b.minmax[0].z = z1;
	b.minmax[1].z = z2;
	b.addEpsilon();
	b.transform(xform);
	return b;
}

void Paraboloid::eval(float u, float v, RtPoint P, RtVector dPdu, RtVector dPdv) const
{
	const float t = u*thetamax;
	const float sint = sinf(t), cost = cosf(t);
	const float dz = zmax - zmin;
	const float z = zmin + v*dz;
	const float x = sqrt(z)*scale;
	P[0] = x*cost, P[1] = x*sint, P[2] = z;
	if (dPdu && dPdv) {
		float zt = sqrt(z);
		const float xt = zt*scale*thetamax;
		dPdu[0] = -xt*sint, dPdu[1] = xt*cost, dPdu[2] = 0;
		if (zt>0.f) zt = 1.f/zt;
		const float dx = (0.5f*dz*zt)*scale;
		dPdv[0] = dx*cost, dPdv[1] = dx*sint, dPdv[2] = dz;
	}
}

//------------------------------------------------------------------------------
// Hyperboloid

Hyperboloid::Hyperboloid(): p1(-1,1,0), p2(1,-1,0), thetamax(2.f*(float)M_PI)
{
}

Hyperboloid::Hyperboloid(const Point3 &_point1, const Point3 &_point2, float _thetamax,
												RtInt n, RtToken tokens[], RtPointer parms[])
												: p1(_point1), p2(_point2)
{
	radians(thetamax, CLAMP(ABS(_thetamax), 0.f, 360.f));
	Primitive::initPrimVars(n, tokens, parms, 1, 4, 4, 4);
}

Bound Hyperboloid::bound()
{
	Point3 pMin = p1 + (p2 - p1)*vmin, pMax = p1 + (p2 - p1)*vmax;
	const float ut1 = umin*thetamax, ut2 = umax*thetamax;
	Bound b = bounds2DSweepP(pMin, ut1, ut2);
	b.include(bounds2DSweepP(pMax, ut1, ut2));
	b.minmax[0].z = MIN2(pMin.z, pMax.z);
	b.minmax[1].z = MAX2(pMin.z, pMax.z);
	b.addEpsilon();
	b.transform(xform);
	return b;
}

void Hyperboloid::eval(float u, float v, RtPoint P, RtVector dPdu, RtVector dPdv) const
{
	const float x = p1.x + (p2.x - p1.x)*v;
	const float y = p1.y + (p2.y - p1.y)*v;
	const float t = u*thetamax;
	const float sint = sinf(t), cost = cosf(t);
	const float dx = p2.x-p1.x, dy = p2.y-p1.y,  dz = p2.z-p1.z;;
	P[0] = x*cost - y*sint, P[1] = x*sint + y*cost, P[2] = p1.z + dz*v;
	if (dPdu && dPdv) {
		const float xt = thetamax*(p1.x + dx*v), yt = thetamax*(p1.y + dy*v);
		dPdu[0] = -xt*sint - yt*cost, dPdu[1] = xt*cost - yt*sint, dPdu[2] = 0;
		dPdv[0] = dx*cost - dy*sint, dPdv[1] = dx*sint + dy*cost, dPdv[2] = dz;
	}
}

//------------------------------------------------------------------------------
// Torus

Torus::Torus(): majrad(1), minrad(0.25), phimin(0), phimax(2.f*(float)M_PI), thetamax(2.f*(float)M_PI)
{
}

Torus::Torus(float _majrad, float _minrad, float _phimin, float _phimax, float _thetamax,
						RtInt n, RtToken tokens[], RtPointer parms[])
						: majrad(_majrad), minrad(_minrad)
{
	radians(phimin, CLAMP(ABS(_phimin), 0.f, 360.f));
	radians(phimax, CLAMP(ABS(_phimax), 0.f, 360.f));
	radians(thetamax, CLAMP(ABS(_thetamax), 0.f, 360.f));
	Primitive::initPrimVars(n, tokens, parms, 1, 4, 4, 4);
}

Bound Torus::bound()
{
	const float myPhimin = vmin*(phimax - phimin) + phimin;
	const float myPhimax = vmax*(phimax - phimin) + phimin;
	Bound a = bounds2DSweepR(minrad, myPhimin, myPhimax);
	const float rmin = a.minmax[0].x + majrad;
	const float rmax = a.minmax[1].x + majrad;
	Bound b = bounds2DSweepL(rmin, rmax, umin*thetamax, umax*thetamax);
	b.minmax[0].z = a.minmax[0].y;
	b.minmax[1].z = a.minmax[1].y;
	b.addEpsilon();
	b.transform(xform);
	return b;
}

void Torus::eval(float u, float v, RtPoint P, RtVector dPdu, RtVector dPdv) const
{
	const float dphi = phimax - phimin;
	const float phi = v*dphi + phimin;
	const float sinp = sinf(phi), cosp = cosf(phi);
	const float sx = cosp*minrad + majrad;
	const float t = u*thetamax;
	const float sint = sinf(t), cost = cosf(t);
	P[0] = sx*cost, P[1] = sx*sint, P[2] = sinp*minrad;
	if (dPdu && dPdv) {
		const float sxt = sx*thetamax;
		dPdu[0] = -sxt*sint, dPdu[1] = sxt*cost, dPdu[2] = 0;
		const float dsx = -dphi*sinp*minrad;
		dPdv[0] = dsx*cost, dPdv[1] = dsx*sint, dPdv[2] = dphi*cosp*minrad;
	}
}

__END_QDRENDER


#include "qdVector.h"
#include "Mathutil.h"

#include "QDRender.h"
__BEGIN_QDRENDER

unsigned int _randomseed = 314159;

Vector RandomVectorSphere()
{
	float r;
	Vector v(0, 0, frandn());
	if ((r = 1.f - v.z*v.z)>0.f) {
		float a = 6.283185307f * frand();
		r = sqrt(r);
		v.x = r * cos(a);  v.y = r * sin(a);
	}
	else v.z = 1;
	return v;
}

void shirleyDisc(float r1, float r2, float &du, float &dv)
{
	float r, th, u=2.f*r1-1.f, v=2.f*r2-1.f;
	if (u>-v) {
		if (u>v) { //Region 1
			r = u;
			th = v/u;
		}
		else { //Region 2
			r = v;
			th = 2.f - u/v;
		}
	}
	else {
		if (u<v) { //Region 3
			r = -u;
			th = 4.f + v/u;
		}
		else {
			r = -v;
			th = (v==0.f) ? 0.f : (6.f - u/v);
		}
	}
	th *= (float)M_PI_4;
	//du = r*(float)cos(th);
	//dv = r*(float)sin(th);
	// slightly faster:
	du = (float)cos(th);
	dv = (float)sqrt(1.f - du*du) * (((th < 0.f) || (th>(float)M_PI)) ? -r : r);
	du *= r;
}

void createCS(const Vector &N, Vector &u, Vector &v)
{
	if ((N.x==0.f) && (N.z==0.f)) {
		u.set(0, 0, (N.y<0.f) ? -1.f : 1.f);
		v.set(1, 0, 0);
	}
	else {
		const float d = 1.f/sqrt(N.x*N.x + N.z*N.z);
		u.set(N.z*d, 0, -N.x*d);
		v = N VCROSS u;
	}
}

__END_QDRENDER

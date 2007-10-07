#ifndef _NOISE_H
#define _NOISE_H

#include "qdVector.h"
#include "Color.h"

#include "QDRender.h"
__BEGIN_QDRENDER

class noiseGenerator_t
{
public:
	noiseGenerator_t() {}
	virtual ~noiseGenerator_t() {}
	virtual float operator() (const Point3 &pt) const=0;
	// offset only added by blendernoise
	virtual Point3 offset(const Point3 &pt) const { return pt; }
};

//---------------------------------------------------------------------------
// Improved Perlin noise, based on Java reference code by Ken Perlin himself.
class newPerlin_t : public noiseGenerator_t
{
public:
	newPerlin_t() {}
	virtual ~newPerlin_t() {}
	virtual float operator() (const Point3 &pt) const;
private:
	float fade(float t) const { return t*t*t*(t*(t*6 - 15) + 10); }
	float grad(int hash, float x, float y, float z) const
	{
		int h = hash & 15;                     // CONVERT LO 4 BITS OF HASH CODE
		float u = h<8 ? x : y,                // INTO 12 GRADIENT DIRECTIONS.
					 v = h<4 ? y : h==12||h==14 ? x : z;
		return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
	}
};

//---------------------------------------------------------------------------
// Standard Perlin noise.
class stdPerlin_t : public noiseGenerator_t
{
public:
	stdPerlin_t() {}
	virtual ~stdPerlin_t() {}
	virtual float operator() (const Point3 &pt) const;
};

// Blender noise, similar to Perlin's
class blenderNoise_t : public noiseGenerator_t
{
public:
	blenderNoise_t() {}
	virtual ~blenderNoise_t() {}
	virtual float operator() (const Point3 &pt) const;
	// offset texture point coordinates by one
	virtual Point3 offset(const Point3 &pt) const { return pt+Point3(1.0, 1.0, 1.0); }
};

//---------------------------------------
// Voronoi, a.k.a. Worley/cellular basis

// distance metrics as functors
struct distanceMetric_t
{
  distanceMetric_t() {}
  virtual ~distanceMetric_t() {}
  virtual float operator() (float x, float y, float z, float e)=0;
};

struct dist_Real : public distanceMetric_t
{
	virtual float operator() (float x, float y, float z, float e) { return sqrt(x*x + y*y + z*z); }
};

struct dist_Squared : public distanceMetric_t
{
	virtual float operator() (float x, float y, float z, float e) { return (x*x + y*y + z*z); }
};

struct dist_Manhattan : public distanceMetric_t
{
	virtual float operator() (float x, float y, float z, float e) { return (fabs(x) + fabs(y) + fabs(z)); }
};

struct dist_Chebychev : public distanceMetric_t
{
	virtual float operator() (float x, float y, float z, float e)
	{
		x = fabs(x);
		y = fabs(y);
		z = fabs(z);
		float t = (x>y)?x:y;
		return ((z>t)?z:t);
	}
};

// minkovsky preset exponent 0.5
struct dist_MinkovskyH : public distanceMetric_t
{
	virtual float operator() (float x, float y, float z, float e)
	{
		float d = sqrt(fabs(x)) + sqrt(fabs(y)) + sqrt(fabs(z));
		return (d*d);
	}
};

// minkovsky preset exponent 4
struct dist_Minkovsky4 : public distanceMetric_t
{
	virtual float operator() (float x, float y, float z, float e)
	{
		x *= x;
		y *= y;
		z *= z;
		return sqrt(sqrt(x*x + y*y + z*z));
	}
};

// Minkovsky, general case, slow
struct dist_Minkovsky : public distanceMetric_t
{
	virtual float operator() (float x, float y, float z, float e)
	{
		return pow(pow(fabs(x), e) + pow(fabs(y), e) + pow(fabs(z), e), (float)1.0/e);
	}
};

class voronoi_t : public noiseGenerator_t
{
public:
	enum voronoiType {V_F1, V_F2, V_F3, V_F4, V_F2F1, V_CRACKLE};
	enum dMetricType {DIST_REAL, DIST_SQUARED, DIST_MANHATTAN, DIST_CHEBYCHEV,
				DIST_MINKOVSKY_HALF, DIST_MINKOVSKY_FOUR, DIST_MINKOVSKY};
	voronoi_t(voronoiType vt=V_F1, dMetricType dm=DIST_REAL, float mex=2.5);
	virtual ~voronoi_t()
	{
		if (distfunc) { delete distfunc;  distfunc=NULL; }
	}
	virtual float operator() (const Point3 &pt) const;
	float getDistance(int x) const { return da[x & 3]; }
	Point3 getPoint(int x) const { return pa[x & 3]; }
	void setMinkovskyExponent(float me) { mk_exp=me; }
	void getFeatures(const Point3 &pt) const;
	void setDistM(dMetricType dm);
protected:
	voronoiType vType;
	dMetricType dmType;
	float mk_exp, w1, w2, w3,w4;
	distanceMetric_t* distfunc;
	mutable float da[4];			// distance array
	mutable Point3 pa[4];	// feature point array
};

// cell noise
class cellNoise_t : public noiseGenerator_t
{
public:
	cellNoise_t() {}
	virtual ~cellNoise_t() {}
	virtual float operator() (const Point3 &pt) const;
};

//------------------
// Musgrave types

class musgrave_t
{
public:
	musgrave_t() {}
	virtual ~musgrave_t() {}
	virtual float operator() (const Point3 &pt) const=0;
};

class fBm_t : public musgrave_t
{
public:
	fBm_t(float _H, float _lacu, float _octs, const noiseGenerator_t* _nGen)
			: H(_H), lacunarity(_lacu), octaves(_octs), nGen(_nGen) {}
	virtual ~fBm_t() {}
	virtual float operator() (const Point3 &pt) const;
protected:
	float H, lacunarity, octaves;
	const noiseGenerator_t* nGen;
};

class mFractal_t : public musgrave_t
{
public:
	mFractal_t(float _H, float _lacu, float _octs, const noiseGenerator_t* _nGen)
			: H(_H), lacunarity(_lacu), octaves(_octs), nGen(_nGen) {}
	virtual ~mFractal_t() {}
	virtual float operator() (const Point3 &pt) const;
protected:
	float H, lacunarity, octaves;
	const noiseGenerator_t* nGen;
};

class heteroTerrain_t : public musgrave_t
{
public:
	heteroTerrain_t(float _H, float _lacu, float _octs, float _offs, const noiseGenerator_t* _nGen)
			: H(_H), lacunarity(_lacu), octaves(_octs), offset(_offs), nGen(_nGen) {}
	virtual ~heteroTerrain_t() {}
	virtual float operator() (const Point3 &pt) const;
protected:
	float H, lacunarity, octaves, offset;
	const noiseGenerator_t* nGen;
};

class hybridMFractal_t : public musgrave_t
{
public:
	hybridMFractal_t(float _H, float _lacu, float _octs, float _offs, float _gain, const noiseGenerator_t* _nGen)
			: H(_H), lacunarity(_lacu), octaves(_octs), offset(_offs), gain(_gain), nGen(_nGen) {}
	virtual ~hybridMFractal_t() {}
	virtual float operator() (const Point3 &pt) const;
protected:
	float H, lacunarity, octaves, offset, gain;
	const noiseGenerator_t* nGen;
};

class ridgedMFractal_t : public musgrave_t
{
public:
	ridgedMFractal_t(float _H, float _lacu, float _octs, float _offs, float _gain, const noiseGenerator_t* _nGen)
			: H(_H), lacunarity(_lacu), octaves(_octs), offset(_offs), gain(_gain), nGen(_nGen) {}
	virtual ~ridgedMFractal_t() {}
	virtual float operator() (const Point3 &pt) const;
protected:
	float H, lacunarity, octaves, offset, gain;
	const noiseGenerator_t* nGen;
};


// basic turbulence, half amplitude, double frequency defaults
// returns value in range (0,1)
float turbulence(const noiseGenerator_t* ngen, const Point3 &pt, int oct, float size, bool hard);
// noise cell color (used with voronoi)
Color cellNoiseColor(const Point3 &pt);

inline float getSignedNoise(const noiseGenerator_t* nGen, const Point3 &pt)
{
	return (float)2.0 * (*nGen)(pt) - (float)1.0;
}

__END_QDRENDER

// 1d noise from value
inline float Noise1D(unsigned int n)
{
  n ^= (n<<13);
  return (float)(n*(n*n*15731 + 789221) + 1376312589)*(1.f/4294967286.f);
}

// as above but returns integer (used for sample coord screen coordinate locking)
inline unsigned int Noise1D_int(unsigned int n)
{
  n ^= (n<<13);
  return n*(n*n*15731 + 789221) + 1376312589;
}

extern "C" {
// used for sampling as well
extern float hashpntf[768];
extern float hashvectf[768];
extern unsigned char hash[512];
}

//---------------------------------------------------------------------------
#endif  //_NOISE_H

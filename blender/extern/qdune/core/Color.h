#ifndef COLOR_H
#define COLOR_H

#include "QDRender.h"
__BEGIN_QDRENDER

class Color
{
public:
	// ctors
	Color():r(0), g(0), b(0) {}
	explicit Color(float i):r(i), g(i), b(i) {}
	explicit Color(float i, float j, float k):r(i), g(j), b(k) {}
	Color(const Color &c):r(c.r), g(c.g), b(c.b) {}
	// dtor
	~Color() {}
	// mtds
	void set(float i) { r=g=b=i; }
	void set(float i, float j, float k) { r=i;  g=j;  b=k; }
	// basic arith.
	Color operator+(const Color &c) const { return Color(r+c.r, g+c.g, b+c.b); }
	Color operator-(const Color &c) const { return Color(r-c.r, g-c.g, b-c.b); }
	Color operator*(float s) const { return Color(r*s, g*s, b*s); }
	friend Color operator*(float s, const Color &c) { return Color(c.r*s, c.g*s, c.b*s); }
	Color operator*(const Color &c) const { return Color(r*c.r, g*c.g, b*c.b); }
	Color operator/(float s) const { if (s!=0) s=1.f/s;  return Color(r*s, g*s, b*s);  }
	Color& operator+=(const Color &c) { r+=c.r;  g+=c.g;  b+=c.b;  return *this; }
	Color& operator-=(const Color &c) { r-=c.r;  g-=c.g;  b-=c.b;  return *this; }
	Color& operator*=(float s) { r*=s;  g*=s;  b*=s;  return *this; }
	Color& operator*=(const Color &c) { r*=c.r;  g*=c.g;  b*=c.b;  return *this; }
	Color& operator/=(float s) { if (s!=0) s=1.f/s;   r*=s;  g*=s;  b*=s;  return *this; }
	float& operator[](int i) { return rgb[i]; }	// no bounds check!
	float operator[](int i) const { return rgb[i]; }	// no bounds check!
	// data
	union {
		struct { float r, g, b; };
		float rgb[3];
	};
};

Color rgb2hsl(const Color& rgb);
Color hsl2rgb(const Color& hsl);

__END_QDRENDER

#endif // COLOR_H

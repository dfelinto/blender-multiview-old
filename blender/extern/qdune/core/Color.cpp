#include "Color.h"
#include "Mathutil.h"

__BEGIN_QDRENDER

// HSL to RGB conversion, based on examples from http://www.easyrgb.com

Color rgb2hsl(const Color& rgb)
{
	const float vmin = MIN2(rgb.r, MIN2(rgb.g, rgb.b));
	const float vmax = MAX2(rgb.r, MAX2(rgb.g, rgb.b));
	const float dmax = vmax - vmin;
	const float L = (vmax + vmin)*0.5f;
	float H = 0.f, S = 0.f;
	if (dmax != 0.f) {
		S = (L < 0.5f) ? (dmax / (vmax + vmin)) : dmax / (2.f - vmax - vmin);
		const float dh = dmax*0.5f, idm = 1.f/dmax;
		const float dR = (((vmax - rgb.r)*(1.f/6.f)) + dh)*idm;
		const float dG = (((vmax - rgb.g)*(1.f/6.f)) + dh)*idm;
		const float dB = (((vmax - rgb.b)*(1.f/6.f)) + dh)*idm;
		if (rgb.r == vmax) H = dB - dG;
		else if (rgb.g == vmax) H = (1.f/3.f) + dR - dB;
		else H = (2.f/3.f) + dG - dR;
		H = (H < 0.f) ? (H + 1.f) : ((H > 1.f) ? (H - 1.f) : H);
	}
	return Color(H, S, L);
}

inline float H2RGB(float v1, float v2, float vH)
{
	vH += ((vH < 0.f) ? 1.f : ((vH > 1.f) ? -1.f : 0.f));
	if ((6.f*vH) < 1.f) return v1 + (v2 - v1)*6.f*vH;
	if ((2.f*vH) < 1.f) return v2;
	if ((3.f*vH) < 2.f) return v1 + (v2 - v1)*((2.f/3.f) - vH)*6.f;
	return v1;
}

Color hsl2rgb(const Color& hsl)
{
	if (hsl[1] == 0.f) return Color(hsl[2]);
	const float v2 = (hsl[2] < 0.5f) ? (hsl[2] * (1.f + hsl[1])) : ((hsl[2] + hsl[1]) - hsl[1]*hsl[2]);
	const float v1 = 2.f*hsl[2] - v2;
	return Color(H2RGB(v1, v2, hsl[0] + (1.f / 3.f)), H2RGB(v1, v2, hsl[0]), H2RGB(v1, v2, hsl[0] - (1.f/3.f)));
}

__END_QDRENDER

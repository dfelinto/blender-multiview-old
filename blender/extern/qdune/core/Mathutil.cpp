#include "Mathutil.h"
#include "Transform.h"

__BEGIN_QDRENDER

RtVoid addMMM(RtMatrix r, const RtMatrix m1, const RtMatrix m2)
{
	r[0][0] = m1[0][0] + m2[0][0];
	r[0][1] = m1[0][1] + m2[0][1];
	r[0][2] = m1[0][2] + m2[0][2];
	r[0][3] = m1[0][3] + m2[0][3];
	r[1][0] = m1[1][0] + m2[1][0];
	r[1][1] = m1[1][1] + m2[1][1];
	r[1][2] = m1[1][2] + m2[1][2];
	r[1][3] = m1[1][3] + m2[1][3];
	r[2][0] = m1[2][0] + m2[2][0];
	r[2][1] = m1[2][1] + m2[2][1];
	r[2][2] = m1[2][2] + m2[2][2];
	r[2][3] = m1[2][3] + m2[2][3];
	r[3][0] = m1[3][0] + m2[3][0];
	r[3][1] = m1[3][1] + m2[3][1];
	r[3][2] = m1[3][2] + m2[3][2];
	r[3][3] = m1[3][3] + m2[3][3];
}

RtVoid subMMM(RtMatrix r, const RtMatrix m1, const RtMatrix m2)
{
	r[0][0] = m1[0][0] - m2[0][0];
	r[0][1] = m1[0][1] - m2[0][1];
	r[0][2] = m1[0][2] - m2[0][2];
	r[0][3] = m1[0][3] - m2[0][3];
	r[1][0] = m1[1][0] - m2[1][0];
	r[1][1] = m1[1][1] - m2[1][1];
	r[1][2] = m1[1][2] - m2[1][2];
	r[1][3] = m1[1][3] - m2[1][3];
	r[2][0] = m1[2][0] - m2[2][0];
	r[2][1] = m1[2][1] - m2[2][1];
	r[2][2] = m1[2][2] - m2[2][2];
	r[2][3] = m1[2][3] - m2[2][3];
	r[3][0] = m1[3][0] - m2[3][0];
	r[3][1] = m1[3][1] - m2[3][1];
	r[3][2] = m1[3][2] - m2[3][2];
	r[3][3] = m1[3][3] - m2[3][3];
}

RtVoid mulMMM(RtMatrix r, const RtMatrix m1, const RtMatrix m2)
{
	RtMatrix tr;
	tr[0][0] = m1[0][0]*m2[0][0] + m1[0][1]*m2[1][0] + m1[0][2]*m2[2][0] + m1[0][3]*m2[3][0];
	tr[0][1] = m1[0][0]*m2[0][1] + m1[0][1]*m2[1][1] + m1[0][2]*m2[2][1] + m1[0][3]*m2[3][1];
	tr[0][2] = m1[0][0]*m2[0][2] + m1[0][1]*m2[1][2] + m1[0][2]*m2[2][2] + m1[0][3]*m2[3][2];
	tr[0][3] = m1[0][0]*m2[0][3] + m1[0][1]*m2[1][3] + m1[0][2]*m2[2][3] + m1[0][3]*m2[3][3];

	tr[1][0] = m1[1][0]*m2[0][0] + m1[1][1]*m2[1][0] + m1[1][2]*m2[2][0] + m1[1][3]*m2[3][0];
	tr[1][1] = m1[1][0]*m2[0][1] + m1[1][1]*m2[1][1] + m1[1][2]*m2[2][1] + m1[1][3]*m2[3][1];
	tr[1][2] = m1[1][0]*m2[0][2] + m1[1][1]*m2[1][2] + m1[1][2]*m2[2][2] + m1[1][3]*m2[3][2];
	tr[1][3] = m1[1][0]*m2[0][3] + m1[1][1]*m2[1][3] + m1[1][2]*m2[2][3] + m1[1][3]*m2[3][3];

	tr[2][0] = m1[2][0]*m2[0][0] + m1[2][1]*m2[1][0] + m1[2][2]*m2[2][0] + m1[2][3]*m2[3][0];
	tr[2][1] = m1[2][0]*m2[0][1] + m1[2][1]*m2[1][1] + m1[2][2]*m2[2][1] + m1[2][3]*m2[3][1];
	tr[2][2] = m1[2][0]*m2[0][2] + m1[2][1]*m2[1][2] + m1[2][2]*m2[2][2] + m1[2][3]*m2[3][2];
	tr[2][3] = m1[2][0]*m2[0][3] + m1[2][1]*m2[1][3] + m1[2][2]*m2[2][3] + m1[2][3]*m2[3][3];

	tr[3][0] = m1[3][0]*m2[0][0] + m1[3][1]*m2[1][0] + m1[3][2]*m2[2][0] + m1[3][3]*m2[3][0];
	tr[3][1] = m1[3][0]*m2[0][1] + m1[3][1]*m2[1][1] + m1[3][2]*m2[2][1] + m1[3][3]*m2[3][1];
	tr[3][2] = m1[3][0]*m2[0][2] + m1[3][1]*m2[1][2] + m1[3][2]*m2[2][2] + m1[3][3]*m2[3][2];
	tr[3][3] = m1[3][0]*m2[0][3] + m1[3][1]*m2[1][3] + m1[3][2]*m2[2][3] + m1[3][3]*m2[3][3];
	memcpy(r, tr, sizeof(RtMatrix));
}

RtBoolean invertMatrix(RtMatrix minv, const RtMatrix m2)
{
	int indxc[4], indxr[4], ipiv[4] = {0,0,0,0};
	if (minv != m2) memcpy(minv, m2, sizeof(RtMatrix));
	for (int i = 0; i < 4; i++) {
		int irow = 0, icol = 0;
		float big = 0.f;
		for (int j = 0; j < 4; j++) {
			if (ipiv[j] != 1) {
				for (int k = 0; k < 4; k++) {
					if (ipiv[k] == 0) {
						if (ABS(minv[j][k]) >= big) {
							big = (float)(ABS(minv[j][k]));
							irow = j;
							icol = k;
						}
					}
					else if (ipiv[k] > 1) return RI_FALSE; // not invertable
				}
			}
		}
		++ipiv[icol];
		if (irow != icol) {
			for (int k = 0; k < 4; ++k) {
				const float tt = minv[irow][k];
				minv[irow][k] = minv[icol][k];
				minv[icol][k] = tt;
			}
		}
		indxr[i] = irow;
		indxc[i] = icol;
		if (minv[icol][icol] == 0.f) return RI_FALSE; // not invertable
		const float pivinv = 1.f / minv[icol][icol];
		minv[icol][icol] = 1.;
		for (int j = 0; j < 4; j++)
			minv[icol][j] *= pivinv;
		for (int j = 0; j < 4; j++) {
			if (j != icol) {
				const float save = minv[j][icol];
				minv[j][icol] = 0;
				for (int k = 0; k < 4; k++)
					minv[j][k] -= minv[icol][k]*save;
			}
		}
	}
	for (int j = 3; j >= 0; j--) {
		if (indxr[j] != indxc[j]) {
			for (int k = 0; k < 4; k++) {
				const float tt = minv[k][indxr[j]];
				minv[k][indxr[j]] = minv[k][indxc[j]];
				minv[k][indxc[j]] = tt;
			}
		}
	}
	return RI_TRUE;
}

// fresnel function, here the well-known Schlick approximation is used, good enough
void fresnel(const RtVector I, const RtVector N, const float eta, float& Kr, float& Kt, RtVector R, RtVector T)
{
	float fior = (eta - 1.f) / (eta + 1.f);
	fior *= fior;
	const float ndi = -N[0]*I[0] - N[1]*I[1] - N[2]*I[2];
	const float t = 1.f - ndi;
	const float t2 = t*t;
	Kr = fior + (1.f - fior) * t2*t2*t;
	Kt = 1.f - Kr;
	if (R) reflect(R, I, N);
	if (T) refract(T, I, N, eta);
}

__END_QDRENDER


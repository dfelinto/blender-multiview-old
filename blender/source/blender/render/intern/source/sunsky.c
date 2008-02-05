/**
 * 
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * Contributor(s): 
 *
 * ***** END GPL LICENSE BLOCK *****/


#include "sunsky.h"
#include "math.h"
#include "BLI_arithb.h"

/* Local Functions and Macros */

#define vec3opv(v1, v2, op, v3) \
	v1[0] = (v2[0] op v3[0]); \
	v1[1] = (v2[1] op v3[1]);\
	v1[2] = (v2[2] op v3[2]);

#define vec3opf(v1, v2, op, f1)\
	v1[0] = (v2[0] op (f1));\
	v1[1] = (v2[1] op (f1));\
	v1[2] = (v2[2] op (f1));

#define fopvec3(v1, f1, op, v2)\
	v1[0] = ((f1) op v2[0]);\
	v1[1] = ((f1) op v2[1]);\
	v1[2] = ((f1) op v2[2]);

float DegsToRads(float degrees)
{
    return degrees* M_PI / 180.0;
}

float RadsToDegs(float rads)
{ 
	return(rads * (180.0 / M_PI)); 
}


void ClipColour(float c[3])
{
    if (c[0] > 1.0) c[0] = 1.0;
    if (c[0] < 0.0) c[0] = 0.0;
    if (c[1] > 1.0) c[1] = 1.0;
    if (c[1] < 0.0) c[1] = 0.0;
    if (c[2] > 1.0) c[2] = 1.0;
    if (c[2] < 0.0) c[2] = 0.0;
}

/* All angles in radians, theta angles measured from normal */
float AngleBetween(float thetav, float phiv, float theta, float phi)
{
	float cospsi = sin(thetav) * sin(theta) * cos(phi - phiv) + cos(thetav) * cos(theta);

	if (cospsi > 1.0)
		return 0;
	if (cospsi < -1.0)
		return M_PI;

	return acos(cospsi);
}
void DirectionToThetaPhi(float *toSun, float *theta, float *phi)
{
    *theta = acos(toSun[2]);
    if (fabs(*theta) < 1e-5)
    	*phi = 0;
    else
    	*phi = atan2(toSun[1], toSun[0]);
}

void InitSunSky(struct SunSky *sunsky, float turb, float *toSun, float horizon_brightness, 
				float spread,float sun_brightness, float sun_size, float back_scatter)
{
    
   	float theta2;
	float theta3;
	float T;
	float T2;
	float chi;
        
	sunsky->turbidity = turb;

	sunsky->horizon_brightness = horizon_brightness;
	sunsky->spread = spread;
	sunsky->sun_brightness = sun_brightness;
	sunsky->sun_size = sun_size;
	sunsky->backscattered_light = back_scatter;
	
	/* Junge's exponent */
	sunsky->V = 4.0;  

	sunsky->toSun[0] = toSun[0];
    sunsky->toSun[1] = toSun[1];
    sunsky->toSun[2] = toSun[2];

    DirectionToThetaPhi(sunsky->toSun, &sunsky->theta, &sunsky->phi);

	ComputeAttenuatedSunlight(sunsky->theta, sunsky->turbidity, sunsky->sunSpectralRaddata);
	sunsky->sunSolidAngle = 0.25 * M_PI * 1.39 * 1.39 / (150 * 150);   // = 6.7443e-05

	theta2 = sunsky->theta*sunsky->theta;
	theta3 = theta2 * sunsky->theta;
	T = turb;
	T2 = turb*turb;

	chi = (4.0 / 9.0 - T / 120.0) * (M_PI - 2 * sunsky->theta);
	sunsky->zenith_Y = (4.0453 * T - 4.9710) * tan(chi) - .2155 * T + 2.4192;
	sunsky->zenith_Y *= 1000;   // conversion from kcd/m^2 to cd/m^2

	if (sunsky->zenith_Y<=0)
		sunsky->zenith_Y = 1e-6;
	
	sunsky->zenith_x =
	    ( + 0.00165 * theta3 - 0.00374 * theta2 + 0.00208 * sunsky->theta + 0) * T2 +
	    ( -0.02902 * theta3 + 0.06377 * theta2 - 0.03202 * sunsky->theta + 0.00394) * T +
	    ( + 0.11693 * theta3 - 0.21196 * theta2 + 0.06052 * sunsky->theta + 0.25885);

	sunsky->zenith_y =
	    ( + 0.00275 * theta3 - 0.00610 * theta2 + 0.00316 * sunsky->theta + 0) * T2 +
	    ( -0.04214 * theta3 + 0.08970 * theta2 - 0.04153 * sunsky->theta + 0.00515) * T +
	    ( + 0.15346 * theta3 - 0.26756 * theta2 + 0.06669 * sunsky->theta + 0.26688);

	
	sunsky->perez_Y[0] = 0.17872 * T - 1.46303;
	sunsky->perez_Y[1] = -0.35540 * T + 0.42749;
	sunsky->perez_Y[2] = -0.02266 * T + 5.32505;
	sunsky->perez_Y[3] = 0.12064 * T - 2.57705;
	sunsky->perez_Y[4] = -0.06696 * T + 0.37027;

	sunsky->perez_x[0] = -0.01925 * T - 0.25922;
	sunsky->perez_x[1] = -0.06651 * T + 0.00081;
	sunsky->perez_x[2] = -0.00041 * T + 0.21247;
	sunsky->perez_x[3] = -0.06409 * T - 0.89887;
	sunsky->perez_x[4] = -0.00325 * T + 0.04517;

	sunsky->perez_y[0] = -0.01669 * T - 0.26078;
	sunsky->perez_y[1] = -0.09495 * T + 0.00921;
	sunsky->perez_y[2] = -0.00792 * T + 0.21023;
	sunsky->perez_y[3] = -0.04405 * T - 1.65369;
	sunsky->perez_y[4] = -0.01092 * T + 0.05291;
	
    /* suggested by glome in 
     * http://projects.blender.org/tracker/?func=detail&atid=127&aid=8063&group_id=9*/
	sunsky->perez_Y[0] *= sunsky->horizon_brightness;
	sunsky->perez_x[0] *= sunsky->horizon_brightness;
	sunsky->perez_y[0] *= sunsky->horizon_brightness;
	
	sunsky->perez_Y[1] *= sunsky->spread;
	sunsky->perez_x[1] *= sunsky->spread;
	sunsky->perez_y[1] *= sunsky->spread;

	sunsky->perez_Y[2] *= sunsky->sun_brightness;
	sunsky->perez_x[2] *= sunsky->sun_brightness;
	sunsky->perez_y[2] *= sunsky->sun_brightness;
	
	sunsky->perez_Y[3] *= sunsky->sun_size;
	sunsky->perez_x[3] *= sunsky->sun_size;
	sunsky->perez_y[3] *= sunsky->sun_size;
	
	sunsky->perez_Y[4] *= sunsky->backscattered_light;
	sunsky->perez_x[4] *= sunsky->backscattered_light;
	sunsky->perez_y[4] *= sunsky->backscattered_light;
}
void ComputeAttenuatedSunlight(float theta, int turbidity, float spectrumdata[SPECTRUM_MAX_COMPONENTS])
{
    float data[91];  // (800 - 350) / 5  + 1
    float spectralsample;
    
    float beta;
    float tauR, tauA, tauO, tauG, tauWA;

    float m;
 
    int i;
    float lambda;
    
    const float alpha = 1.3;
    const float lOzone = .35;
    const float w = 2.0;
    
    beta = 0.04608365822050 * turbidity - 0.04586025928522;
    m = 1.0 / (cos(theta) + 0.000940 * pow(1.6386 - theta, -1.253)); 
    
    for (i = 0, lambda = 350; i < 91; i++, lambda += 5)
    {
        // Rayleigh Scattering
        // Results agree with the graph (pg 115, MI) */
		tauR = exp( -m * 0.008735 * pow(lambda/1000, (float)(-4.08)));

        // Aerosal (water + dust) attenuation
        // beta - amount of aerosols present
        // alpha - ratio of small to large particle sizes. (0:4,usually 1.3)
        // Results agree with the graph (pg 121, MI)
        
		tauA = exp(-m * beta * pow(lambda/1000, -alpha));  // lambda should be in um

        // Attenuation due to ozone absorption
        // lOzone - amount of ozone in cm(NTP)
        // Results agree with the graph (pg 128, MI)
        
        tauO = exp(-m * GetOzoneSample(lambda) * lOzone);

        // Attenuation due to mixed gases absorption
        // Results agree with the graph (pg 131, MI)
        spectralsample = GetGasSample(lambda);
        tauG = exp(-1.41 * spectralsample * m / pow(1.0 + 118.93 * spectralsample * m, 0.45));

        // Attenuation due to water vapor absorbtion
        // w - precipitable water vapor in centimeters (standard = 2)
        // Results agree with the graph (pg 132, MI)
		

		spectralsample = GetWaterSample(lambda);
		tauWA = exp(-0.2385 * spectralsample * w * m / pow(1.0 + 20.07 * spectralsample * w * m, 0.45));

        data[i] = 106 * GetSolarSample(lambda) * tauR * tauA * tauO * tauG * tauWA;
    }

    // Converts to Spectrum
    GetSpectrumDataFromRegularSpectralCurve(data, 350, 800, 91, spectrumdata);
}

void GetSkySpectralRadiancef(struct SunSky *sunsky, const float varg[3], float spectralData[SPECTRUM_MAX_COMPONENTS])
{
	float theta, phi;
	float v[3];
	int i;
	
	VecCopyf(v, (float*)varg);
	Normalize(v);
	if (v[2] < 0)
    {
		for(i=0; i<SPECTRUM_MAX_COMPONENTS; i++)
		{
			spectralData[i] = 0.0;
		}
		return;
    }
	if (v[2] < 0.001)
    {
            v[2] = 0.001;
            Normalize(v);
    }

	DirectionToThetaPhi(v, &theta, &phi);

	GetSkySpectralRadiance(sunsky, theta, phi, spectralData);
}

float PerezFunction(struct SunSky *sunsky, const float *lam, float theta, float gamma, float lvz)
{
    float den, num;
    den = ((1 + lam[0] * exp(lam[1])) *
                   (1 + lam[2] * exp(lam[3] * sunsky->theta) + lam[4] * cos(sunsky->theta) * cos(sunsky->theta)));

    num = ((1 + lam[0] * exp(lam[1] / cos(theta))) *
                   (1 + lam[2] * exp(lam[3] * gamma) + lam[4] * cos(gamma) * cos(gamma)));

    return(lvz * num / den);
}

void GetSkySpectralRadiance(struct SunSky *sunsky, float theta, float phi, float spectralData[SPECTRUM_MAX_COMPONENTS])
{
    float gamma;
    float x,y,Y;
    float epsilon;
    float color[3];
    
    int i;
        
    gamma = AngleBetween(theta, phi, sunsky->theta, sunsky->phi);
    epsilon = 1e-6;
    // Compute xyY values
    x = PerezFunction(sunsky, sunsky->perez_x, theta, gamma, sunsky->zenith_x);
    y = PerezFunction(sunsky, sunsky->perez_y, theta, gamma, sunsky->zenith_y);
    Y = PerezFunction(sunsky, sunsky->perez_Y, theta, gamma, sunsky->zenith_Y);

    GetSpectrumDataFromChromaticitySpectrum(x, y, spectralData);
    
    ConvertSpectrumDataToCIEXYZ(spectralData, color);
        
    for(i=0; i<SPECTRUM_MAX_COMPONENTS; i++)
    {
    	if(color[1]==0)
    		color[1] = epsilon;
    	spectralData[i] = (Y * spectralData[i])/color[1];
    }
}

void GetSkyXYZRadiancef(struct SunSky* sunsky, const float varg[3], float color_out[3])
{
    float	theta, phi;
    float	v[3];

	VecCopyf(v, (float*)varg);
	Normalize(v);

    if (v[2] < 0)
    {
        color_out[0]=0;
        color_out[1]=0;
        color_out[2]=0;
    }
    if (v[2] < 0.001){
        v[2] = 0.001;
        Normalize(v);
    }

    DirectionToThetaPhi(v, &theta, &phi);
    GetSkyXYZRadiance(sunsky, theta, phi, color_out);
}

void GetSkyXYZRadiance(struct SunSky* sunsky, float theta, float phi, float color_out[3])
{
    float gamma;
    float x,y,Y,X,Z;
    gamma = AngleBetween(theta, phi, sunsky->theta, sunsky->phi);
    // Compute xyY values
    x = PerezFunction(sunsky, sunsky->perez_x, theta, gamma, sunsky->zenith_x);
    y = PerezFunction(sunsky, sunsky->perez_y, theta, gamma, sunsky->zenith_y);
    Y = PerezFunction(sunsky, sunsky->perez_Y, theta, gamma, sunsky->zenith_Y);

    X = (x / y) * Y;
    Z = ((1 - x - y) / y) * Y;

    color_out[0] = X;
    color_out[1] = Y;
    color_out[2] = Z;
}


void FastComputeAttenuatedSunlight(float theta, int turbidity, float fTau[3])
{
    float fBeta ;
    float fTauR, fTauA;
    float m ;
    float fAlpha;
    
    int i;
    float fLambda[3]; 
	fLambda[0] = 0.65f;	// red (in um.)
	fLambda[1] = 0.57f;	// green (in um.)
	fLambda[2] = 0.475f;	// blue (in um.)

	fAlpha = 1.3f;
	fBeta = 0.04608365822050f * turbidity - 0.04586025928522f;
	
	m =  1.0/(cos(theta) + 0.15f*pow(93.885f-theta/M_PI*180.0f,-1.253f));  // Relative Optical Mass

    for(i = 0; i < 3; i++)
	{
		// Rayleigh Scattering
		// Results agree with the graph (pg 115, MI) */
		// lambda in um.
		fTauR = exp( -m * 0.008735f * pow(fLambda[i], (float)(-4.08f)));

		// Aerosal (water + dust) attenuation
		// beta - amount of aerosols present 
		// alpha - ratio of small to large particle sizes. (0:4,usually 1.3)
		// Results agree with the graph (pg 121, MI) 
		
		fTauA = exp(-m * fBeta * pow(fLambda[i], -fAlpha));  // lambda should be in um


		fTau[i] = fTauR * fTauA; 

    }
}

void InitAtmosphere(struct SunSky *sunSky, float mief, float rayf,
							float inscattf, float extincf, float disf, float sky_dist)
{
	const float pi = 3.14159265358f;
	const float n = 1.003f; // refractive index
	const float N = 2.545e25;
	const float pn = 0.035f;
	const float T = 2.0f;
	float fTemp, fTemp2, fTemp3, fBeta, fBetaDash;
	float c = (6.544*T - 6.51)*1e-17; // from page 57 of my thesis.
	float K[3] = {0.685f, 0.679f, 0.670f}; // from pg 64 of my thesis.
	float vBetaMieTemp[3];
	
	float fLambda[3],fLambda2[3], fLambda4[3];
	float vLambda2[3];
	float vLambda4[3];
	
	int i;

	sunSky->atm_BetaMieMultiplier  = mief;
	sunSky->atm_BetaRayMultiplier = rayf;
	sunSky->atm_InscatteringMultiplier = inscattf;
	sunSky->atm_ExtinctionMultiplier = extincf;
	sunSky->atm_DistanceMultiplier = disf;
	sunSky->atm_DistanceOverSky = sky_dist;
	
	sunSky->atm_HGg=0.8;

	fLambda[0]  = 1/650e-9f; // red // note 650e-9 m = 650nm.
	fLambda[1]  = 1/570e-9f;	// green
	fLambda[2]  = 1/475e-9f; // blue
	for (i=0; i < 3; i++)
	{
		fLambda2[i] = fLambda[i]*fLambda[i];
		fLambda4[i] = fLambda2[i]*fLambda2[i];
	}

	vLambda2[0] = fLambda2[0];
	vLambda2[1] = fLambda2[1];
	vLambda2[2] = fLambda2[2];
 
	vLambda4[0] = fLambda4[0];
	vLambda4[1] = fLambda4[1];
	vLambda4[2] = fLambda4[2];

	// Rayleigh scattering constants.
	fTemp = pi*pi*(n*n-1)*(n*n-1)*(6+3*pn)/(6-7*pn)/N;
	fBeta = 8*fTemp*pi/3;
		
	vec3opf(sunSky->atm_BetaRay, vLambda4, *, fBeta);
	fBetaDash = fTemp/2;
	vec3opf(sunSky->atm_BetaDashRay, vLambda4,*, fBetaDash);
	

	// Mie scattering constants.
	
	
	fTemp2 = 0.434*c*(2*pi)*(2*pi)*0.5f;
	vec3opf(sunSky->atm_BetaDashMie, vLambda2, *, fTemp2);
	
	
	fTemp3 = 0.434f*c*pi*(2*pi)*(2*pi);
	
	vec3opv(vBetaMieTemp, K, *, fLambda);
	vec3opf(sunSky->atm_BetaMie, vBetaMieTemp,*, fTemp3);
	
}

void atmosphere_pixle_shader( struct SunSky* sunSky, float view[3], float s, float sunColorIntensity, float rgb[3])
{
	float costheta;
	float Phase_1;
	float Phase_2;
	float sunColor[3];
	
	float E[3];
	float E1[3];
	float fReflectance = 0.10f;
	float vDiffuse[3]={0.138f,0.113f, 0.08f}; // Taken from soil's relectance spectrum data.
	
	float I[3];
	float fTemp;
	float vTemp1[3], vTemp2[3];

	float sunDirection[3];
	
	s *= sunSky->atm_DistanceMultiplier;
	
	sunDirection[0] = sunSky->toSun[0];
	sunDirection[1] = sunSky->toSun[1];
	sunDirection[2] = sunSky->toSun[2];
	
	costheta = Inpf(view, sunDirection); // cos(theta)
	Phase_1 = 1 + (costheta * costheta); // Phase_1
	
	vec3opf(sunSky->atm_BetaRay, sunSky->atm_BetaRay, *, sunSky->atm_BetaRayMultiplier);
	vec3opf(sunSky->atm_BetaMie, sunSky->atm_BetaMie, *, sunSky->atm_BetaMieMultiplier);
	vec3opv(sunSky->atm_BetaRM, sunSky->atm_BetaRay, +, sunSky->atm_BetaMie);
	
	//e^(-(beta_1 + beta_2) * s) = E1
	vec3opf(E1, sunSky->atm_BetaRM, *, -s/log(2));
	E1[0] = exp(E1[0]);
	E1[1] = exp(E1[1]);
	E1[2] = exp(E1[2]);
	vec3opf(vDiffuse, vDiffuse, *, fReflectance);
	vec3opv(E, E1, *, vDiffuse);
	vec3opf(E, E1, +, 0.0);
		
	//Phase2(theta) = (1-g^2)/(1+g-2g*cos(theta))^(3/2)
	fTemp = 1 + sunSky->atm_HGg - 2 * sunSky->atm_HGg * costheta;
	fTemp = fTemp * sqrt(fTemp);
	Phase_2 = (1 - sunSky->atm_HGg * sunSky->atm_HGg)/fTemp;
	
	vec3opf(vTemp1, sunSky->atm_BetaDashRay, *, Phase_1);
	vec3opf(vTemp2, sunSky->atm_BetaDashMie, *, Phase_2);	

	vec3opv(vTemp1, vTemp1, +, vTemp2);
	fopvec3(vTemp2, 1.0, -, E1);
	vec3opv(vTemp1, vTemp1, *, vTemp2);

	fopvec3(vTemp2, 1.0, / , sunSky->atm_BetaRM);

	vec3opv(I, vTemp1, *, vTemp2);
		
	vec3opf(I, I, *, sunSky->atm_InscatteringMultiplier);
	vec3opf(E, E, *, sunSky->atm_ExtinctionMultiplier);
		
	//scale to color sun
	FastComputeAttenuatedSunlight(sunSky->theta, sunSky->turbidity, sunColor);
	/*vec3opv(E, E, *, sunColor);*/
	vec3opf(E, E, *, sunColorIntensity);
		
	/*vec3opv(I, I, *, sunColor);*/
	vec3opf(I, I, *, sunColorIntensity);

	vec3opv(rgb, rgb, *, E);
	vec3opv(rgb, rgb, +, I);
}
#undef vec3opv
#undef vec3opf
#undef fopvec3

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
 * ***** END GPL LICENSE BLOCK *****
 */

#include "spectrum.h"

#ifndef SUNSKY_H_
#define SUNSKY_H_


typedef struct SunSky
{
	short effect_type;
    float turbidity;
    float theta, phi;
    
    float toSun[3];

    float sunSpectralRaddata[SPECTRUM_MAX_COMPONENTS];
    float sunSolidAngle;

    float zenith_Y, zenith_x, zenith_y;
    
    float perez_Y[5], perez_x[5], perez_y[5];

    float V;

    /* suggested by glome in 
     * http://projects.blender.org/tracker/?func=detail&atid=127&aid=8063&group_id=9*/
    float horizon_brightness;
    float spread;
    float sun_brightness;
    float sun_size;
    float backscattered_light;
    
    float atm_HGg;

    float atm_InscatteringMultiplier;
    float atm_ExtinctionMultiplier;
    float atm_BetaRayMultiplier;
    float atm_BetaMieMultiplier;
    float atm_DistanceMultiplier;
    float atm_DistanceOverSky;

    float atm_BetaRay[3];
    float atm_BetaDashRay[3];
    float atm_BetaMie[3];
    float atm_BetaDashMie[3];
    float atm_BetaRM[3];
}SunSky;

/* Sun and Sky functions, Use Spectral Power Distribution - correct approch */
void InitSunSky(struct SunSky *sunsky, float turb, float *toSun, float horizon_brightness, 
				float spread,float sun_brightness, float sun_size, float back_scatter);
void ComputeSunSkyDirection(float lat, float longi, int sm, int jd, float tOfDay, float *theta, float * phi);
void ComputeAttenuatedSunlight(float theta, int turbidity, float spectrumdata[SPECTRUM_MAX_COMPONENTS]);
float PerezFunction(struct SunSky *sunsky, const float *lam, float theta, float gamma, float lvz);
void GetSkySpectralRadiancef(struct SunSky *sunsky, const float varg[3], float spectralData[SPECTRUM_MAX_COMPONENTS]);
void GetSkySpectralRadiance(struct SunSky *sunsky, float theta, float phi, float spectralData[SPECTRUM_MAX_COMPONENTS]);
void GetSkyXYZRadiance(struct SunSky* sunsky, float theta, float phi, float color_out[3]);
void GetSkyXYZRadiancef(struct SunSky* sunsky, const float varg[3], float color_out[3]);

/* Atmosphere functions, Use RGB - fast approch*/
void InitAtmosphere(struct SunSky *sunSky, float mief, float rayf, float inscattf, float extincf, float disf, float sky_dist);
void atmosphere_pixle_shader( struct SunSky* sunSky, float view[3], float s, float sunColorIntensity, float rgb[3]);
void FastComputeAttenuatedSunlight(float theta, int turbidity, float fTau[3]);

/* */
float DegsToRads(float degrees);
float RadsToDegs(float rads);
void ClipColour(float c[3]);

#endif /*SUNSKY_H_*/

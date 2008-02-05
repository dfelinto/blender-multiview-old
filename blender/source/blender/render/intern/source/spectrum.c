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

#include "spectrum.h"
#include "spectrum_constants.h"

float GetOzoneSample(float lambda)
{
	return GetSampleFromIrregularSpectralCurve(k_oAmplitudes, k_oWavelengths, 64, lambda);
}

float GetWaterSample(float lambda)
{
	return GetSampleFromIrregularSpectralCurve(k_waAmplitudes, k_waWavelengths, 13, lambda);
}

float GetGasSample(float lambda)
{
	return GetSampleFromIrregularSpectralCurve(k_gAmplitudes, k_gWavelengths, 4, lambda);
}

float GetSolarSample(float lambda)
{
	return GetSampleFromRegularSpectralCurve(solAmplitudes, 380, 750, 38, lambda);
}

float mix(float a, float b, float s)
{ 
    return((1.0 - s) * a + s * b); 
}

float GetSampleFromIrregularSpectralCurve(float *amplitudes, float *wavelengths, int divisions, float lambda) 
{
    float 	x, result;
    int		i;

    if (divisions == 0)
       return(0.0);
    else if (divisions == 1)
        return(amplitudes[0]);

    if (lambda < wavelengths[0])
        return(amplitudes[0]);

    i = 1;
    while (i < divisions)
        if (lambda < wavelengths[i])
        {
            x = (lambda - wavelengths[i - 1]) / (wavelengths[i] - wavelengths[i - 1]);

            result = mix(amplitudes[i - 1], amplitudes[i], x);

            return(result);
        }
        else
            i++;

    return(amplitudes[divisions - 1]);
}

float GetSampleFromRegularSpectralCurve(float *amplitudes, int waveStart, int waveEnd, int divisions, float lambda) 
{
	float waveDelta;
    float result, x, dx;
    int bin1, bin2;

    waveDelta = (waveEnd - waveStart) / (divisions - 1);
    
    if (lambda < waveStart || lambda > waveEnd)
            return(0.0);

    x = (lambda - waveStart) / waveDelta;

    bin1 = (int) x;
    dx = x - bin1;
    if (dx > 1e-8)
            bin2 = bin1 + 1;
    else
            bin2 = bin1;
    if(bin1<0)
    	bin1 = 0;
    if(bin2<0)
    	bin2 = 0;
    if(bin1>=divisions)
    	bin1 = divisions - 1;
    if(bin2>=divisions)
    	bin2 = divisions - 1;

    result = mix(amplitudes[bin1], amplitudes[bin2], dx);

    return(result);    
}

void GetSpectrumDataFromRegularSpectralCurve(float *amplitudes, int waveStart, int waveEnd, int divisions,
												float spectrumData[SPECTRUM_MAX_COMPONENTS])
{
    float lambda, x;
    int i;

    for (i = 0; i < SPECTRUM_MAX_COMPONENTS; i++)
    {
        x = (i + 0.5) / SPECTRUM_MAX_COMPONENTS;
        lambda = SPECTRUM_START + x * (SPECTRUM_END - SPECTRUM_START);

        spectrumData[i] = GetSampleFromRegularSpectralCurve(amplitudes, waveStart, waveEnd, divisions, lambda);
    }
}

float GetSampleFromChromaticitySpectrum(float x, float y, float lambda)
{
	float M1, M2;
    M1 = (-1.3515 - 1.7703 * x +  5.9114 * y) / (0.0241 + 0.2562 * x - 0.7341 * y);
    M2 = ( 0.03   -31.4424 * x + 30.0717 * y) / (0.0241 + 0.2562 * x - 0.7341 * y);
    
	return( GetSampleFromRegularSpectralCurve(tS0Amplitudes, 300, 830, 54, lambda) +
			M1 * GetSampleFromRegularSpectralCurve(tS1Amplitudes, 300, 830, 54, lambda) +
			M2 * GetSampleFromRegularSpectralCurve(tS2Amplitudes, 300, 830, 54, lambda));
}

void GetSpectrumDataFromChromaticitySpectrum(float x, float y, float spectrumData[SPECTRUM_MAX_COMPONENTS])
{
    float		lambda, cx;
    int			i;

    for (i = 0; i < SPECTRUM_MAX_COMPONENTS; i++)
    {
        cx = (i + 0.5) / SPECTRUM_MAX_COMPONENTS;
        lambda = SPECTRUM_START + cx * (SPECTRUM_END - SPECTRUM_START);

        spectrumData[i] = GetSampleFromChromaticitySpectrum(x, y, lambda);
    }
}

void ConvertSpectrumDataToCIEXYZ(float spectrumData[SPECTRUM_MAX_COMPONENTS], float color[3])
{
    int i;
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
    for (i = 0; i < SPECTRUM_MAX_COMPONENTS; i++)
    {
        float x = (i + 0.5) / SPECTRUM_MAX_COMPONENTS;
        float lambda = mix(SPECTRUM_START, SPECTRUM_END, x);

        color[0] += GetSampleFromRegularSpectralCurve(tCIE_xbar, 360, 830, 95, lambda) * spectrumData[i];
        color[1] += GetSampleFromRegularSpectralCurve(tCIE_ybar, 360, 830, 95, lambda) * spectrumData[i];
        color[2] += GetSampleFromRegularSpectralCurve(tCIE_zbar, 360, 830, 95, lambda) * spectrumData[i];
    }
}



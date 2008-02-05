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

#ifndef SPECTRUM_H_
#define SPECTRUM_H_

#define SPECTRUM_MAX_COMPONENTS     100
#define SPECTRUM_START              350.0
#define SPECTRUM_END                800.0

float GetOzoneSample(float lambda);
float GetWaterSample(float lambda);
float GetGasSample(float lambda);
float GetSolarSample(float lambda);

float GetSampleFromIrregularSpectralCurve(float *amplitudes, float *wavelengths, int divisions, float lambda);
float GetSampleFromRegularSpectralCurve(float *amplitudes, int waveStart, int waveEnd, int divisions, float lambda);
float GetSampleFromChromaticitySpectrum(float x, float y, float lambda);
void GetSpectrumDataFromRegularSpectralCurve(float *amplitudes, int waveStart, int waveEnd, int divisions,
												float spectrumData[SPECTRUM_MAX_COMPONENTS]);
void GetSpectrumDataFromChromaticitySpectrum(float x, float y, float spectrumData[SPECTRUM_MAX_COMPONENTS]);
void ConvertSpectrumDataToCIEXYZ(float spectrumData[SPECTRUM_MAX_COMPONENTS], float color[3]);

float mix(float a, float b, float s);
#endif /*SPECTRUM_H_*/

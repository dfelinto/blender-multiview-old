/**
 * $Id$
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * The contents of this file may be used under the terms of either the GNU
 * General Public License Version 2 or later (the "GPL", see
 * http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
 * later (the "BL", see http://www.blender.org/BL/ ) which has to be
 * bought from the Blender Foundation to become active, in which case the
 * above mentioned GPL option does not apply.
 *
 * The Original Code is Copyright (C) 2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */
#ifndef SND_FMODDEVICE
#define SND_FMODDEVICE

#include "fmod.h"
#include "SND_AudioDevice.h"
#include "SoundDefines.h"

class SND_FmodDevice : public SND_AudioDevice
{
public:
	SND_FmodDevice();
	~SND_FmodDevice();

	SND_WaveSlot* LoadSample(const STR_String& samplename,
							 void* memlocation,
							 int size);

	void InitListener();
	void SetListenerGain(float gain) const;
	void SetDopplerVelocity(MT_Scalar dopplervelocity) const;
	void SetDopplerFactor(MT_Scalar dopplerfactor) const;
	void SetListenerRollOffFactor(MT_Scalar rollofffactor) const;

	void MakeCurrent() const;
	void NextFrame() const;
	void UseCD() const;

	void SetObjectBuffer(int id, unsigned int buffer); 
	int GetPlayState(int id);
	void PlayObject(int id);
	void StopObject(int id) const;
	void StopAllObjects();
	void PauseObject(int id) const;

	void SetObjectLoop(int id, unsigned int loopmode) const;
	void SetObjectLoopPoints(int id, unsigned int loopstart, unsigned int loopend) const;
	void SetObjectPitch(int id, MT_Scalar pitch) const;
	void SetObjectGain(int id, MT_Scalar gain) const;
	void SetObjectMinGain(int id, MT_Scalar mingain) const;
	void SetObjectMaxGain(int id, MT_Scalar maxgain) const;
	void SetObjectRollOffFactor(int id, MT_Scalar rolloff) const;
	void SetObjectReferenceDistance(int id, MT_Scalar distance) const;

	void SetObjectTransform(int id,
							const MT_Vector3& position,
							const MT_Vector3& velocity,
							const MT_Matrix3x3& orientation,	
							const MT_Vector3& lisposition,
							const MT_Scalar& rollofffactor) const;
	void ObjectIs2D(int id) const;

	void PlayCD(int track) const;
	void PauseCD(bool pause) const;
	void StopCD() const;
	void SetCDPlaymode(int playmode) const;
	void SetCDGain(MT_Scalar gain) const;

	void StartUsingDSP();
	float* GetSpectrum();
	void StopUsingDSP();

private:
	FSOUND_SAMPLE*	m_buffers[NUM_BUFFERS];
	FSOUND_SAMPLE*	m_sources[NUM_SOURCES];
	FSOUND_DSPUNIT*	m_dspunit;
	int				m_frequencies[NUM_SOURCES];
	int				m_max_channels;
	int				m_num_hardware_channels;
	int				m_num_software_channels;
	int				m_channels[NUM_SOURCES];
	float*			m_spectrum;
};

#endif //SND_FMODDEVICE

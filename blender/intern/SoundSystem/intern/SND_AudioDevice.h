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
#ifndef SND_AUDIODEVICE
#define SND_AUDIODEVICE

#include "SND_IAudioDevice.h"
#include "SoundDefines.h"
#include "SND_IdObject.h"


class SND_AudioDevice : public SND_IAudioDevice
{
public:
	SND_AudioDevice();
	virtual ~SND_AudioDevice();

	virtual bool IsInitialized();

	SND_WaveCache* GetWaveCache() const;

	bool GetNewId(SND_SoundObject* pObject);
	void ClearId(SND_SoundObject* pObject);

	void UseCD() const {};

	/* to be implemented in derived class

	virtual SND_WaveSlot* LoadSample(const STR_String& samplename,
									 void* memlocation,
									 int size) =0;
	*/
//	void RemoveSample(const char* filename);
	void RemoveAllSamples();

	/* to be implemented in derived class

	virtual void InitListener()=0;
	virtual void SetListenerGain(float gain) const =0;
	virtual void SetDopplerVelocity(MT_Scalar dopplervelocity) const =0;
	virtual void SetDopplerFactor(MT_Scalar dopplerfactor) const =0;
	virtual	void SetListenerRollOffFactor(MT_Scalar rollofffactor) const =0;
	
	virtual void MakeCurrent() const =0;

	virtual void UpdateDevice() const =0;

	virtual void SetObjectBuffer(int id, unsigned int buffer)=0; 
	virtual int GetPlayState(int id)=0;
	virtual void PlayObject(int id)=0;
	virtual void StopObject(int id) const =0;
	virtual void StopAllObjects()=0;
	virtual void PauseObject(int id) const =0;

	virtual void SetObjectLoop(int id, bool loop) const =0;
	virtual void SetObjectLoopPoints(int id, unsigned int loopstart, unsigned int loopend) const =0;
	virtual void SetObjectPitch(int id, MT_Scalar pitch) const =0;
	virtual void SetObjectGain(int id, MT_Scalar gain) const =0;
	virtual void SetObjectRollOffFactor(int id, MT_Scalar rolloff) const =0;
	virtual void SetObjectMinGain(int id, MT_Scalar mingain) const =0;
	virtual void SetObjectMaxGain(int id, MT_Scalar maxgain) const =0;
	virtual void SetObjectReferenceDistance(int id, MT_Scalar referencedistance) const =0;

	virtual void SetObjectTransform(int id,
									const MT_Vector3& position,
									const MT_Vector3& velocity,
									const MT_Matrix3x3& orientation,	
									const MT_Vector3& lisposition,
									const MT_Scalar& rollofffactor) const =0;
	virtual void ObjectIs2D(int id) const =0;

	virtual void PlayCD(int track) const =0;
	virtual void PauseCD(bool pause) const =0;
	virtual void StopCD() const =0;
	virtual void SetCDPlaymode(int playmode) const =0;
	virtual void SetCDGain(MT_Scalar gain) const =0;
	virtual float* GetSpectrum() =0;
	*/

protected:
	bool							m_audio;
	GEN_List						m_idObjectList;
	SND_IdObject*					m_idObjectArray[NUM_SOURCES];
	SND_WaveCache*					m_wavecache;

private:
	void		 RevokeSoundObject(SND_SoundObject* pObject);
};

#endif //SND_AUDIODEVICE

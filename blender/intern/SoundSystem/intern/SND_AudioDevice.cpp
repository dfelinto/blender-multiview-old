/**
 * $Id$
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

#include "SND_AudioDevice.h"

#include "SND_SoundObject.h"

#ifdef WIN32
// This warning tells us about truncation of __long__ stl-generated names.
// It can occasionally cause DevStudio to have internal compiler warnings.
#pragma warning( disable : 4786 )     
#endif


SND_AudioDevice::SND_AudioDevice()
{
	m_wavecache = NULL;
	m_audio = false;

	for (int i = 0; i < NUM_SOURCES; i++)
	{
		m_idObjectArray[i] = new SND_IdObject();
		m_idObjectArray[i]->SetId(i);
		m_idObjectArray[i]->SetSoundObject(NULL);
		m_idObjectList.addTail(m_idObjectArray[i]);
	}
}



SND_AudioDevice::~SND_AudioDevice()
{
	for (int i = 0; i < NUM_SOURCES; i++)
	{
		delete m_idObjectArray[i];
		m_idObjectArray[i] = NULL;
	}

	if (m_wavecache)
	{
		delete m_wavecache;
		m_wavecache = NULL;
	}
}



bool SND_AudioDevice::IsInitialized()
{
	return m_audio;
}



SND_WaveCache* SND_AudioDevice::GetWaveCache() const
{
	return m_wavecache;
}



/* seeks an unused id and returns it */
bool SND_AudioDevice::GetNewId(SND_SoundObject* pObject)
{
#ifdef ONTKEVER
	printf("SND_AudioDevice::GetNewId\n");
#endif

	bool result;

	// first, get the oldest (the first) idobject
	SND_IdObject* pIdObject = (SND_IdObject*)m_idObjectList.getHead();

	if (pIdObject->isTail())
	{
		result = false;
	}
	else
	{
		// find the first id object which doesn't have a high priority soundobject
		bool ThisSoundMustStay = false;
		bool OutOfIds = false;

		do
		{
			// if no soundobject present, it's seat may be taken
			if (pIdObject->GetSoundObject())
			{
				// and also if it ain't highprio
				if (pIdObject->GetSoundObject()->IsHighPriority())
				{
					ThisSoundMustStay = true;
					pIdObject = (SND_IdObject*)pIdObject->getNext();
					
					// if the last one is a priority sound too, then there are no id's left
					// and we won't add any new sounds
					if (pIdObject->isTail())
						OutOfIds = true;
				}
				else
				{
					ThisSoundMustStay = false;
				}
			}
			else
			{
				ThisSoundMustStay = false;
			}

		} while (ThisSoundMustStay && !OutOfIds);

		if (!OutOfIds)
		{
			SND_SoundObject* oldobject = oldobject = pIdObject->GetSoundObject();
			
			// revoke the old object if present
			if (oldobject)
			{
#ifdef ONTKEVER
				printf("oldobject: %x\n", oldobject);
#endif
				RevokeSoundObject(oldobject);
			}
			
			// set the new soundobject into the idobject
			pIdObject->SetSoundObject(pObject);
			
			// set the id into the soundobject
			int id = pIdObject->GetId();
			pObject->SetId(id);
			
			// connect the new id to the buffer the sample is stored in
			SetObjectBuffer(id, pObject->GetBuffer());
			
			// remove the idobject from the list and add it in the back again
			pIdObject->remove();
			m_idObjectList.addTail(pIdObject);
			
			result = true;
		}
	}

	return result;
}



void SND_AudioDevice::ClearId(SND_SoundObject* pObject)
{
#ifdef ONTKEVER
	printf("SND_AudioDevice::ClearId\n");
#endif

	if (pObject)
	{
		int id = pObject->GetId();
		
		if (id != -1)
		{
			// lets get the idobject belonging to the soundobject
			SND_IdObject* pIdObject = m_idObjectArray[id];
			SND_SoundObject* oldobject = pIdObject->GetSoundObject();
			
			if (oldobject)
			{
				RevokeSoundObject(oldobject);

				// clear the idobject from the soundobject
				pIdObject->SetSoundObject(NULL);
			}

			// remove the idobject and place it in front
			pIdObject->remove();
			m_idObjectList.addHead(pIdObject);
		}
	}
}



void SND_AudioDevice::RevokeSoundObject(SND_SoundObject* pObject)
{
#ifdef ONTKEVER
	printf("SND_AudioDevice::RevokeSoundObject\n");
#endif

	// stop the soundobject
	int id = pObject->GetId();

	if (id >= 0 && id < NUM_SOURCES)
	{
		StopObject(id);

		// remove the object from the 'activelist'
		pObject->SetActive(false);

#ifdef ONTKEVER
		printf("pObject->remove();\n");
#endif
	}

	// make sure its id is invalid
	pObject->SetId(-1);
}

/*
void SND_AudioDevice::RemoveSample(const char* filename)
{
	if (m_wavecache)
		m_wavecache->RemoveSample(filename);
}
*/

void SND_AudioDevice::RemoveAllSamples()
{
	if (m_wavecache)
		m_wavecache->RemoveAllSamples();
}


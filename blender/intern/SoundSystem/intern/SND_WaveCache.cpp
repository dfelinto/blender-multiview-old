/*
 * SND_WaveCache.cpp
 *
 * abstract wavecache, a way to organize samples
 *
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

#ifdef WIN32
#pragma warning (disable:4786) // Get rid of stupid stl-visual compiler debug warning
#endif //WIN32

#include "SND_WaveCache.h"
#include <stdio.h>

#ifndef __APPLE__
#include <malloc.h>
#else // __APPLE__
#include <sys/malloc.h>
#endif // __APPLE__


SND_WaveCache::SND_WaveCache()
{
	// do the buffer administration
	for (int i = 0; i < NUM_BUFFERS; i++)
		m_bufferList[i] = NULL;
}



SND_WaveCache::~SND_WaveCache()
{
	// clean up the mess
	FreeSamples();
	RemoveAllSamples();
}



SND_WaveSlot* SND_WaveCache::GetWaveSlot(const STR_String& samplename)
{
	SND_WaveSlot* waveslot = NULL;

	std::map<STR_String, SND_WaveSlot*>::iterator find_result = m_samplecache.find(samplename);
		
	// let's see if we have already loaded this sample
	if (find_result != m_samplecache.end())
	{
		waveslot = (*find_result).second;
	}
	else
	{
		// so the sample wasn't loaded, so do it here
		for (int bufnum = 0; bufnum < NUM_BUFFERS; bufnum++)
		{
			// find an empty buffer
			if (m_bufferList[bufnum] == NULL)
			{
				waveslot = new SND_WaveSlot();
				waveslot->SetSampleName(samplename);
				waveslot->SetBuffer(bufnum);
				m_bufferList[bufnum] = waveslot;
				break;
			}
		}
		m_samplecache.insert(std::pair<STR_String, SND_WaveSlot*>(samplename, waveslot));
	}

	return waveslot;
}



void SND_WaveCache::RemoveAllSamples()
{
	// remove all samples
	m_samplecache.clear();

	// reset the list of buffers
	for (int i = 0; i < NUM_BUFFERS; i++)
		m_bufferList[i] = NULL;
}



void SND_WaveCache::RemoveSample(const STR_String& samplename, int buffer)
{
	m_samplecache.erase(samplename);
	m_bufferList[buffer] = NULL;
}



void SND_WaveCache::FreeSamples()
{
	// iterate through the bufferlist and delete the waveslot if present
	for (int i = 0; i < NUM_BUFFERS; i++)
	{
		if (m_bufferList[i])
		{
			delete m_bufferList[i];
			m_bufferList[i] = NULL;
		}
	}
}

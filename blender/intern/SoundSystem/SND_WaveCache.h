/*
 * SND_WaveCache.h
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
#pragma warning (disable:4786) // get rid of stupid stl-visual compiler debug warning
#endif //WIN32

#ifndef __SND_WAVECACHE_H
#define __SND_WAVECACHE_H

#include "SND_WaveSlot.h"
#include "SoundDefines.h"
#include "SND_SoundObject.h"
#include <map>

class SND_WaveCache
{
public:
	SND_WaveCache();
	virtual ~SND_WaveCache();

	SND_WaveSlot*			GetWaveSlot(const STR_String& samplename);

	void					RemoveAllSamples();
	void					RemoveSample(const STR_String& samplename, int buffer);

private:
	std::map<STR_String, SND_WaveSlot*> m_samplecache;

	SND_WaveSlot*			m_bufferList[NUM_BUFFERS];

	void					FreeSamples();
};

#endif //__SND_WAVECACHE_H

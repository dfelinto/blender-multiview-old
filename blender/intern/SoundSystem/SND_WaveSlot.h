/*
 * SND_WaveSlot.cpp
 *
 * class for storing sample related information
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

#ifndef __SND_WAVESLOT_H
#define __SND_WAVESLOT_H

#include "STR_String.h"

class SND_WaveSlot
{
	STR_String		m_samplename;
	bool			m_loaded;
	void*			m_data;
	unsigned int	m_buffer;
	unsigned int	m_sampleformat;
	unsigned int	m_numberofchannels;
	unsigned int	m_samplerate;
	unsigned int	m_bitrate;
	unsigned int	m_numberofsamples;
	unsigned int	m_filesize;

public:

	SND_WaveSlot(): m_loaded(false),
					m_data(NULL),
					m_buffer(0),
					m_sampleformat(0),
					m_numberofchannels(0),
					m_samplerate(0),
					m_bitrate(0),
					m_numberofsamples(0),
					m_filesize(0)
					{};
	~SND_WaveSlot();

	void SetSampleName(STR_String samplename);
	void SetLoaded(bool loaded);
	void SetData(void* data);
	void SetBuffer(unsigned int buffer);
	void SetSampleFormat(unsigned int sampleformat);
	void SetNumberOfChannels(unsigned int numberofchannels);
	void SetSampleRate(unsigned int samplerate);
	void SetBitRate(unsigned int bitrate);
	void SetNumberOfSamples(unsigned int numberofsamples);
	void SetFileSize(unsigned int filesize);
	

	const STR_String&	GetSampleName();
	bool				IsLoaded() const;
	void*				GetData();
	unsigned int		GetBuffer() const;
	unsigned int		GetSampleFormat() const;
	unsigned int		GetNumberOfChannels() const;
	unsigned int		GetSampleRate() const;
	unsigned int		GetBitRate() const;
	unsigned int		GetNumberOfSamples() const;
	unsigned int		GetFileSize() const;

};

#endif //__SND_WAVESLOT_H

/*
 * SND_CDObject.cpp
 *
 * Implementation for CD playback
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

#include "SND_CDObject.h"


SND_CDObject* SND_CDObject::m_instance = NULL;

bool SND_CDObject::CreateSystem()
{
	bool result = false;

	if (!m_instance)
	{
		m_instance = new SND_CDObject();
		result = true;
	}

	return result;
}



bool SND_CDObject::DisposeSystem()
{
	bool result = false;

	if (m_instance)
	{
		delete m_instance;
		m_instance = NULL;
		result = true;
	}

	return result;
}



SND_CDObject* SND_CDObject::Instance()
{
	return m_instance;
}



SND_CDObject::SND_CDObject()
{
	m_gain = 1;
	m_playmode = SND_CD_ALL;
	m_track = 1;
	m_playstate = SND_STOPPED;
	m_used = false;

	// don't set the cd standard on modified:
	// if not used, we don't wanna touch it (performance)
	m_modified = false;
}



SND_CDObject::~SND_CDObject()
{
}



void SND_CDObject::SetGain(MT_Scalar gain)
{
	m_gain = gain;
	m_modified = true;
}



void SND_CDObject::SetPlaymode(int playmode)
{
	m_playmode = playmode;
}



void SND_CDObject::SetPlaystate(int playstate)
{
	m_playstate = playstate;
}



void SND_CDObject::SetTrack(int track)
{
	m_track = track;
}



int SND_CDObject::GetTrack() const
{
	return m_track;
}



MT_Scalar SND_CDObject::GetGain() const
{
	return m_gain;
}


int SND_CDObject::GetPlaystate() const
{
	return m_playstate;
}



bool SND_CDObject::IsModified() const
{
	return m_modified;
}



void SND_CDObject::SetModified(bool modified)
{
	m_modified = modified;
}



int SND_CDObject::GetPlaymode() const
{
	return m_playmode;
}



void SND_CDObject::SetUsed()
{
	m_used = true;
}



bool SND_CDObject::GetUsed()
{
	return m_used;
}


/*
 * SND_IdObject.cpp
 *
 * Object for storing runtime data, like id's, soundobjects etc
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

#include "SND_IdObject.h"

SND_IdObject::SND_IdObject()
{
}



SND_IdObject::~SND_IdObject()
{
}



SND_SoundObject* SND_IdObject::GetSoundObject()
{
	return m_soundObject;
}



void SND_IdObject::SetSoundObject(SND_SoundObject* pObject)
{
	m_soundObject = pObject;
}



int SND_IdObject::GetId()
{
	return m_id;
}



void SND_IdObject::SetId(int id)
{
	m_id = id;
}

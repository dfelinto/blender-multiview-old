/*
 * SND_SoundListener.cpp
 *
 * A SoundListener is for sound what a camera is for vision.
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

#include "SND_SoundListener.h"

SND_SoundListener::SND_SoundListener()
{
	m_modified = true;
	m_gain = 1.0;
	m_dopplerfactor = 1.0;
	m_dopplervelocity = 1.0;
	m_scale = 1.0;
	m_position[0] = 0.0;
	m_position[1] = 0.0;
	m_position[2] = 0.0;
	m_velocity[0] = 0.0;
	m_velocity[1] = 0.0;
	m_velocity[2] = 0.0;
	m_orientation[0][0] = 1.0;
	m_orientation[0][1] = 0.0;
	m_orientation[0][2] = 0.0;
	m_orientation[1][0] = 0.0;
	m_orientation[1][1] = 1.0;
	m_orientation[1][2] = 0.0;
	m_orientation[2][0] = 0.0;
	m_orientation[2][1] = 0.0;
	m_orientation[2][2] = 1.0;
}


SND_SoundListener::~SND_SoundListener()
{
	; /* intentionally empty */

}



void SND_SoundListener::SetGain(MT_Scalar gain)
{
	m_gain = gain;
	m_modified = true;
}



void SND_SoundListener::SetPosition (const MT_Vector3& pos)
{
	m_position = pos;
}



void SND_SoundListener::SetVelocity(const MT_Vector3& vel)
{
	m_velocity = vel;
}



void SND_SoundListener::SetOrientation(const MT_Matrix3x3& ori)
{
	m_orientation = ori;
}



void SND_SoundListener::SetDopplerFactor(MT_Scalar dopplerfactor)
{
	m_dopplerfactor = dopplerfactor;
	m_modified = true;
}



void SND_SoundListener::SetDopplerVelocity(MT_Scalar dopplervelocity)
{
	m_dopplervelocity = dopplervelocity;
	m_modified = true;
}



void SND_SoundListener::SetScale(MT_Scalar scale)
{
	m_scale = scale;
	m_modified = true;
}



MT_Scalar SND_SoundListener::GetGain() const
{
	return m_gain;
}



MT_Vector3 SND_SoundListener::GetPosition() const
{
	return m_position;
}



MT_Vector3 SND_SoundListener::GetVelocity() const
{
	return m_velocity;
}



MT_Matrix3x3 SND_SoundListener::GetOrientation()
{
	return m_orientation;
}



MT_Scalar SND_SoundListener::GetDopplerFactor() const
{
	return m_dopplerfactor;
}



MT_Scalar SND_SoundListener::GetDopplerVelocity() const
{
	return m_dopplervelocity;
}



MT_Scalar SND_SoundListener::GetScale() const
{
	return m_scale;
}



bool SND_SoundListener::IsModified() const
{
	return m_modified;	
}



void SND_SoundListener::SetModified(bool modified)
{
	m_modified = modified;
}

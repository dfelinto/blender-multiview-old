/*
 * SND_IdObject.h
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

#ifndef __SND_IDOBJECT_H
#define __SND_IDOBJECT_H

#include "SND_SoundObject.h"
#include "GEN_List.h"
#include "SoundDefines.h"

class SND_IdObject : public GEN_Link
{
	SND_SoundObject*	m_soundObject;
	int					m_id;

public:
	SND_IdObject();
	virtual ~SND_IdObject();

	SND_SoundObject*	GetSoundObject();
	void				SetSoundObject(SND_SoundObject* pObject);

	int					GetId();
	void				SetId(int id);
};
#endif //__SND_OBJECT_H

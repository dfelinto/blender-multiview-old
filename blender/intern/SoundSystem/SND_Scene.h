/*
 * SND_Scene.h
 *
 * The scene for sounds.
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

#ifndef __SND_SCENE_H
#define __SND_SCENE_H

#include "SoundDefines.h"
#include "SND_SoundObject.h"
#include "SND_CDObject.h"
#include "SND_SoundListener.h"
#include "SND_WaveSlot.h"

#include "MT_Vector3.h"
#include "MT_Matrix3x3.h"
#include "STR_String.h"

#include <set>


class SND_Scene
{
	std::set<class SND_SoundObject*>	m_soundobjects;

	GEN_List					m_activeobjects;
	class SND_IAudioDevice*		m_audiodevice;
	class SND_WaveCache*		m_wavecache;
	class SND_SoundListener		m_listener;
	bool						m_audio;			// to check if audio works
	bool						m_audioplayback;	// to check if audioplayback is wanted

	void					UpdateListener();
	void					BuildActiveList(MT_Scalar curtime);
	void					UpdateActiveObects();
	void					UpdateCD();

public:
	SND_Scene(SND_IAudioDevice* adi);
	~SND_Scene();

	bool				IsPlaybackWanted();

	void				AddActiveObject(SND_SoundObject* pObject, MT_Scalar curtime);
	void				RemoveActiveObject(SND_SoundObject* pObject);
	void				DeleteObjectWhenFinished(SND_SoundObject* pObject);

	void				Proceed();

	int					LoadSample(const STR_String& samplename,
								   void* memlocation,
								   int size);
	void				RemoveAllSamples();
	bool				CheckBuffer(SND_SoundObject* pObject);
	bool				IsSampleLoaded(STR_String& samplename);

	void				AddObject(SND_SoundObject* pObject);
	bool				SetCDObject(SND_CDObject* cdobject);
	void				DeleteObject(SND_SoundObject* pObject);
	void				RemoveAllObjects();
	void				StopAllObjects();
	int					GetObjectStatus(SND_SoundObject* pObject) const;

	void				SetListenerTransform(const MT_Vector3& pos,
											 const MT_Vector3& vel,
											 const MT_Matrix3x3& mat);

	SND_SoundListener*	GetListener();
};

#endif //__SND_SCENE_H

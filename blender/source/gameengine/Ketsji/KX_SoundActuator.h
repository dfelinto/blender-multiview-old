/**
 * KX_SoundActuator.h
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

#ifndef __KX_SOUNDACTUATOR
#define __KX_SOUNDACTUATOR

#include "SCA_IActuator.h"

class KX_SoundActuator : public SCA_IActuator
{
	Py_Header;
	bool					m_lastEvent;
	bool					m_isplaying;
	/* just some handles to the audio-data... */
	class SND_SoundObject*	m_soundObject;
	class SND_Scene*		m_soundScene;
	short					m_startFrame;
	short					m_endFrame;
	bool					m_pino;
public:

	enum KX_SOUNDACT_TYPE
	{
			KX_SOUNDACT_NODEF = 0,
			KX_SOUNDACT_PLAYSTOP,
			KX_SOUNDACT_PLAYEND,
			KX_SOUNDACT_LOOPSTOP,
			KX_SOUNDACT_LOOPEND,
			KX_SOUNDACT_LOOPBIDIRECTIONAL,
			KX_SOUNDACT_LOOPBIDIRECTIONAL_STOP,
			KX_SOUNDACT_MAX
	};

	KX_SOUNDACT_TYPE		m_type;

	KX_SoundActuator(SCA_IObject* gameobj,
					class SND_SoundObject* sndobj,
					class SND_Scene*	sndscene,
					KX_SOUNDACT_TYPE type,
					short start,
					short end,
					PyTypeObject* T=&Type);

	~KX_SoundActuator();

	void setSoundObject(class SND_SoundObject* soundobject);
	bool Update(double curtime,double deltatime);

	CValue* GetReplica();

	/* -------------------------------------------------------------------- */
	/* Python interface --------------------------------------------------- */
	/* -------------------------------------------------------------------- */

	PyObject*  _getattr(char *attr);

	KX_PYMETHOD(KX_SoundActuator,SetFilename);
	KX_PYMETHOD(KX_SoundActuator,GetFilename);
	KX_PYMETHOD(KX_SoundActuator,StartSound);
	KX_PYMETHOD(KX_SoundActuator,PauseSound);
	KX_PYMETHOD(KX_SoundActuator,StopSound);
	KX_PYMETHOD(KX_SoundActuator,SetGain);
	KX_PYMETHOD(KX_SoundActuator,GetGain);
	KX_PYMETHOD(KX_SoundActuator,SetPitch);
	KX_PYMETHOD(KX_SoundActuator,GetPitch);
	KX_PYMETHOD(KX_SoundActuator,SetRollOffFactor);
	KX_PYMETHOD(KX_SoundActuator,GetRollOffFactor);
	KX_PYMETHOD(KX_SoundActuator,SetLooping);
	KX_PYMETHOD(KX_SoundActuator,GetLooping);
	KX_PYMETHOD(KX_SoundActuator,SetPosition);
	KX_PYMETHOD(KX_SoundActuator,SetVelocity);
	KX_PYMETHOD(KX_SoundActuator,SetOrientation);
};
#endif //__KX_SOUNDACTUATOR


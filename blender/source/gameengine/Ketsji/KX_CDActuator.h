/**
 * KX_CDActuator.h
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

#ifndef __KX_CDACTUATOR
#define __KX_CDACTUATOR

#include "SCA_IActuator.h"
#include "SND_CDObject.h"


class KX_CDActuator : public SCA_IActuator
{
	Py_Header;
	bool					m_lastEvent;
	bool					m_isplaying;
	/* just some handles to the audio-data... */
	class SND_Scene*		m_soundscene;
	int						m_track;
	float					m_gain;
	short					m_startFrame;
	short					m_endFrame;

public:
	enum KX_CDACT_TYPE
	{
			KX_CDACT_NODEF = 0,
			KX_CDACT_PLAY_ALL,
			KX_CDACT_PLAY_TRACK,
			KX_CDACT_LOOP_TRACK,
			KX_CDACT_VOLUME,
			KX_CDACT_STOP,
			KX_CDACT_PAUSE,
			KX_CDACT_RESUME,
			KX_SOUNDACT_MAX
	};

	KX_CDACT_TYPE			m_type;

	KX_CDActuator(SCA_IObject* gameobject,
				  SND_Scene* soundscene,
				  KX_CDACT_TYPE type,
				  int track,
				  short start,
				  short end,
				  PyTypeObject* T=&Type);

	~KX_CDActuator();

	bool Update(double curtime,double deltatime);

	CValue* GetReplica();

	/* -------------------------------------------------------------------- */
	/* Python interface --------------------------------------------------- */
	/* -------------------------------------------------------------------- */

	PyObject*  _getattr(char *attr);

	KX_PYMETHOD(KX_CDActuator,StartCD);
	KX_PYMETHOD(KX_CDActuator,PauseCD);
	KX_PYMETHOD(KX_CDActuator,StopCD);
	KX_PYMETHOD(KX_CDActuator,SetGain);
	KX_PYMETHOD(KX_CDActuator,GetGain);
};
#endif //__KX_CDACTUATOR


/**
 * Generate random pulses
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

#ifndef __KX_RANDOMSENSOR
#define __KX_RANDOMSENSOR

#include "SCA_ISensor.h"
#include "BoolValue.h"
#include "SCA_RandomNumberGenerator.h"

class SCA_RandomSensor : public SCA_ISensor
{
	Py_Header;

	unsigned int m_currentDraw;
	int m_iteration;
	SCA_RandomNumberGenerator *m_basegenerator;
	bool m_lastdraw;
public:
	SCA_RandomSensor(class SCA_EventManager* rndmgr,
					SCA_IObject* gameobj,
					int startseed,
  					PyTypeObject* T=&Type);
	virtual ~SCA_RandomSensor();
	virtual CValue* GetReplica();
	virtual bool Evaluate(CValue* event);
	virtual bool IsPositiveTrigger();

	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

	virtual PyObject*  _getattr(char *attr);

	/* 1. setSeed                                                            */
	KX_PYMETHOD_DOC(SCA_RandomSensor,SetSeed);
	/* 2. getSeed                                                            */
	KX_PYMETHOD_DOC(SCA_RandomSensor,GetSeed);
	/* 3. getSeed                                                            */
	KX_PYMETHOD_DOC(SCA_RandomSensor,GetLastDraw);

};

#endif //__KX_RANDOMSENSOR

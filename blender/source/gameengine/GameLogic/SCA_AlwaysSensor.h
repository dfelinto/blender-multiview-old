/**
 * SCA_AlwaysSensor.h
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

#ifndef __KX_ALWAYSSENSOR
#define __KX_ALWAYSSENSOR
#include "SCA_ISensor.h"


class SCA_AlwaysSensor : public SCA_ISensor
{
	Py_Header;
	bool			m_alwaysresult;
public:
	SCA_AlwaysSensor(class SCA_EventManager* eventmgr,
					SCA_IObject* gameobj,
					PyTypeObject* T =&Type);
	virtual ~SCA_AlwaysSensor();
	virtual CValue* GetReplica();
	virtual bool Evaluate(CValue* event);
	virtual bool IsPositiveTrigger();

	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */
	
	virtual PyObject*  _getattr(char *attr);

	
};

#endif //__KX_ALWAYSSENSOR

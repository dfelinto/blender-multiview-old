/**
 * SCA_ANDController.h
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

#ifndef __KX_ANDCONTROLLER
#define __KX_ANDCONTROLLER

#include "SCA_IController.h"

class SCA_ANDController : public SCA_IController
{
	Py_Header;
	//virtual void Trigger(class SCA_LogicManager* logicmgr);
public:
	SCA_ANDController(SCA_IObject* gameobj,PyTypeObject* T=&Type);
	virtual ~SCA_ANDController();
	virtual CValue* GetReplica();
	virtual void Trigger(SCA_LogicManager* logicmgr);

	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

	virtual PyObject*  _getattr(char *attr);

};

#endif //__KX_ANDCONTROLLER

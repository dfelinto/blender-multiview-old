/**
 * KX_EXPRESSIONController.h
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

#ifndef __KX_EXPRESSIONCONTROLLER
#define __KX_EXPRESSIONCONTROLLER

#include "SCA_IController.h"


class SCA_ExpressionController : public SCA_IController
{
//	Py_Header;
	STR_String			m_exprText;

public:
	SCA_ExpressionController(SCA_IObject* gameobj,
							 const STR_String& exprtext,
							 PyTypeObject* T=&Type );

	virtual ~SCA_ExpressionController();
	virtual CValue* GetReplica();
	virtual void Trigger(SCA_LogicManager* logicmgr);
	virtual CValue*		FindIdentifier(const STR_String& identifiername);

	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

//	virtual PyObject*  _getattr(char *attr);

};

#endif //__KX_EXPRESSIONCONTROLLER

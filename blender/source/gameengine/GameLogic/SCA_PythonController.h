/**
 * Execute Python scripts
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

#ifndef KX_PYTHONCONTROLLER_H
#define KX_PYTHONCONTROLLER_H
	      
#include "SCA_IController.h"
#include "SCA_LogicManager.h"
#include "BoolValue.h"

class SCA_IObject;
class SCA_PythonController : public SCA_IController
{
	Py_Header;
	struct _object *		m_bytecode;
	bool					m_bModified;

 protected:
	STR_String				m_scriptText;
	STR_String				m_scriptName;
	PyObject*				m_pythondictionary;

 public: 
	static SCA_PythonController* m_sCurrentController; // protected !!!

	SCA_PythonController(SCA_IObject* gameobj,PyTypeObject* T = &Type);
	virtual ~SCA_PythonController();

	virtual CValue* GetReplica();
	virtual void  Trigger(class SCA_LogicManager* logicmgr);
  
	void	SetScriptText(const STR_String& text);
	void	SetScriptName(const STR_String& name);
	void	SetDictionary(PyObject*	pythondictionary);

	static char* sPyGetCurrentController__doc__;
	static PyObject* sPyGetCurrentController(PyObject* self, 
											 PyObject* args, 
											 PyObject* kwds);
	static char* sPyAddActiveActuator__doc__;
	static PyObject* sPyAddActiveActuator(PyObject* self, 
										  PyObject* args, 
										  PyObject* kwds);
	virtual PyObject*  _getattr(char *attr);

	KX_PYMETHOD_DOC(SCA_PythonController,GetSensors);
	KX_PYMETHOD_DOC(SCA_PythonController,GetSensor);
	KX_PYMETHOD_DOC(SCA_PythonController,GetActuator);
	KX_PYMETHOD_DOC(SCA_PythonController,GetActuators);
	KX_PYMETHOD(SCA_PythonController,SetScript);
	KX_PYMETHOD(SCA_PythonController,GetScript);
	

};

#endif //KX_PYTHONCONTROLLER_H


/**
 * $Id$
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

#include "SCA_ILogicBrick.h"



SCA_LogicManager* SCA_ILogicBrick::m_sCurrentLogicManager = NULL;

SCA_ILogicBrick::SCA_ILogicBrick(SCA_IObject* gameobj,
								 PyTypeObject* T)
	:
	CValue(T),
	m_gameobj(gameobj),
	m_Execute_Priority(0),
	m_Execute_Ueber_Priority(0),
	m_bActive(false),
	m_eventval(0)
{
	m_text = "KX_LogicBrick";
}



SCA_ILogicBrick::~SCA_ILogicBrick()
{
	RemoveEvent();
}



void SCA_ILogicBrick::SetExecutePriority(int execute_Priority)
{
	m_Execute_Priority = execute_Priority;
}



void SCA_ILogicBrick::SetUeberExecutePriority(int execute_Priority)
{
	m_Execute_Ueber_Priority = execute_Priority;
}



SCA_IObject* SCA_ILogicBrick::GetParent()
{
	return m_gameobj;
}



void SCA_ILogicBrick::ReParent(SCA_IObject* parent)
{
	m_gameobj = parent;
}



CValue* SCA_ILogicBrick::Calc(VALUE_OPERATOR op, CValue *val)
{
	CValue* temp = new CBoolValue(false,"");
	CValue* result = temp->Calc(op,val);
	temp->Release();

	return result;
} 



CValue* SCA_ILogicBrick::CalcFinal(VALUE_DATA_TYPE dtype,
								   VALUE_OPERATOR op,
								   CValue *val)
{
	// same as bool implementation, so...
	CValue* temp = new CBoolValue(false,"");
	CValue* result = temp->CalcFinal(dtype,op,val);
	temp->Release();

	return result;
}



const STR_String& SCA_ILogicBrick::GetText()
{ 
	if (m_name.Length())
		return m_name;

	return m_text;
}



float SCA_ILogicBrick::GetNumber()
{
	return -1;
}



STR_String SCA_ILogicBrick::GetName()
{
	return m_name;
}



void SCA_ILogicBrick::SetName(STR_String name)
{
	m_name = name;
}



void SCA_ILogicBrick::ReplicaSetName(STR_String name)
{
	m_name = name;
}
		


bool SCA_ILogicBrick::IsActive()
{
	return m_bActive;
}



bool SCA_ILogicBrick::LessComparedTo(SCA_ILogicBrick* other)
{
	return (this->m_Execute_Ueber_Priority < other->m_Execute_Ueber_Priority) 
		|| ((this->m_Execute_Ueber_Priority == other->m_Execute_Ueber_Priority) && 
		(this->m_Execute_Priority < other->m_Execute_Priority));
}



void SCA_ILogicBrick::SetActive(bool active)
{
	m_bActive=active;
	if (active)
	{
		//m_gameobj->SetDebugColor(GetDrawColor());
	} else
	{
		//m_gameobj->ResetDebugColor();
	}
}



void SCA_ILogicBrick::RegisterEvent(CValue* eventval)
{
	if (m_eventval)
		m_eventval->Release();

	m_eventval = eventval->AddRef();
}


void SCA_ILogicBrick::RemoveEvent()
{
	if (m_eventval)
	{
		m_eventval->Release();
		m_eventval = NULL;
	}
}



CValue* SCA_ILogicBrick::GetEvent()
{
	if (m_eventval)
	{
		return m_eventval->AddRef();
	}
	
	return NULL;
}




/* python stuff */

PyTypeObject SCA_ILogicBrick::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"SCA_ILogicBrick",
	sizeof(SCA_ILogicBrick),
	0,
	PyDestructor,
	0,
	__getattr,
	__setattr,
	0, //&MyPyCompare,
	__repr,
	0, //&cvalue_as_number,
	0,
	0,
	0,
	0
};



PyParentObject SCA_ILogicBrick::Parents[] = {
	&SCA_ILogicBrick::Type,
	&CValue::Type,
	NULL
};



PyMethodDef SCA_ILogicBrick::Methods[] = {
  {"getOwner", (PyCFunction) SCA_ILogicBrick::sPyGetOwner, METH_VARARGS},
  {"getExecutePriority", (PyCFunction) SCA_ILogicBrick::sPySetExecutePriority, METH_VARARGS},
  {"setExecutePriority", (PyCFunction) SCA_ILogicBrick::sPySetExecutePriority, METH_VARARGS},
  {NULL,NULL} //Sentinel
};



PyObject*
SCA_ILogicBrick::_getattr(char* attr)
{
  _getattr_up(CValue);
}



PyObject* SCA_ILogicBrick::PyGetOwner(PyObject* self, 
			       PyObject* args, 
			       PyObject* kwds)
{
	CValue* parent = GetParent();
	if (parent)
	{
		parent->AddRef();
		return parent;
	}

	printf("ERROR: Python scriptblock without owner\n");
	Py_INCREF(Py_None);
	return Py_None;//Int_FromLong(IsPositiveTrigger());
}



PyObject* SCA_ILogicBrick::PySetExecutePriority(PyObject* self, 
			       PyObject* args, 
			       PyObject* kwds)
{

	int priority=0;

    if (!PyArg_ParseTuple(args, "i", &priority)) {
		return NULL;
    }
	
	m_Execute_Ueber_Priority = priority;

	Py_Return;
}



PyObject* SCA_ILogicBrick::PyGetExecutePriority(PyObject* self, 
			       PyObject* args, 
			       PyObject* kwds)
{
	return PyInt_FromLong(m_Execute_Ueber_Priority);
}



/* Conversions for making life better. */
bool SCA_ILogicBrick::PyArgToBool(int boolArg)
{
	if (boolArg == KX_TRUE) {
		return true;
	} else if (boolArg == KX_FALSE){
		return false;
	} else {
		; /* error: bad conversion: how to catch this? */
		return false;
	}
}



PyObject* SCA_ILogicBrick::BoolToPyArg(bool boolarg)
{
	return PyInt_FromLong(boolarg? KX_TRUE: KX_FALSE);	
}

/**
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
#ifndef __KX_ILOGICBRICK
#define __KX_ILOGICBRICK

#include "Value.h"
#include "SCA_IObject.h"
#include "BoolValue.h"
class SCA_ILogicBrick : public CValue
{
	Py_Header;
	SCA_IObject*		m_gameobj;
	int					m_Execute_Priority;
	int					m_Execute_Ueber_Priority;

	bool				m_bActive;
	CValue*				m_eventval;
	STR_String			m_text;
	STR_String			m_name;
	//unsigned long		m_drawcolor;
protected:
	void RegisterEvent(CValue* eventval);
	void RemoveEvent();
	CValue* GetEvent();

public:
	SCA_ILogicBrick(SCA_IObject* gameobj,PyTypeObject* T );
	virtual ~SCA_ILogicBrick();

	void SetExecutePriority(int execute_Priority);
	void SetUeberExecutePriority(int execute_Priority);

	SCA_IObject*	GetParent();
	virtual void	ReParent(SCA_IObject* parent);

	// act as a BoolValue (with value IsPositiveTrigger)
	virtual CValue*	Calc(VALUE_OPERATOR op, CValue *val);
	virtual CValue*	CalcFinal(VALUE_DATA_TYPE dtype, VALUE_OPERATOR op, CValue *val);

	virtual const STR_String &	GetText();
	virtual float		GetNumber();
	virtual STR_String	GetName();
	virtual void		SetName(STR_String name);
	virtual void		ReplicaSetName(STR_String name);
		
	bool				IsActive();
	void				SetActive(bool active) ;

	virtual	bool		LessComparedTo(SCA_ILogicBrick* other);
	
	virtual PyObject*  _getattr(char *attr);

	static class SCA_LogicManager*	m_sCurrentLogicManager;


	// python methods

	KX_PYMETHOD(SCA_ILogicBrick,GetOwner);
	KX_PYMETHOD(SCA_ILogicBrick,SetExecutePriority);
	KX_PYMETHOD(SCA_ILogicBrick,GetExecutePriority);

	enum KX_BOOL_TYPE {
		KX_BOOL_NODEF = 0,
		KX_TRUE,
		KX_FALSE,
		KX_BOOL_MAX
	};


protected: 
	/* Some conversions to go with the bool type. */
	/** Convert a KX_TRUE, KX_FALSE in Python to a c++ value. */
	bool PyArgToBool(int boolArg);

	/** Convert a a c++ value to KX_TRUE, KX_FALSE in Python. */
	PyObject* BoolToPyArg(bool);

	
};


#endif

/**
 * Property sensor
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

#ifndef __KX_PROPERTYSENSOR
#define __KX_PROPERTYSENSOR

#include "SCA_ISensor.h"

class SCA_PropertySensor : public SCA_ISensor
{
	Py_Header;
	//class CExpression*	m_rightexpr;
	int				m_checktype;
	STR_String		m_checkpropval;
	STR_String		m_checkpropmaxval;
	STR_String		m_checkpropname;
	STR_String		m_previoustext;
	bool			m_lastresult;
	bool			m_recentresult;
	CExpression*	m_range_expr;

	/**
	 * Test whether this is a sensible value (type check)
	 */
	bool validValueForProperty(char *val, STR_String &prop);
 protected:

public:
	enum KX_PROPSENSOR_TYPE {
		KX_PROPSENSOR_NODEF = 0,
		KX_PROPSENSOR_EQUAL,
		KX_PROPSENSOR_NOTEQUAL,
		KX_PROPSENSOR_INTERVAL,
		KX_PROPSENSOR_CHANGED,
		KX_PROPSENSOR_EXPRESSION,
		KX_PROPSENSOR_MAX
	};

	const STR_String S_KX_PROPSENSOR_EQ_STRING;
	
	SCA_PropertySensor(class SCA_EventManager* eventmgr,
					  SCA_IObject* gameobj,
					  const STR_String& propname,
					  const STR_String& propval,
					  const STR_String& propmaxval,
					  KX_PROPSENSOR_TYPE checktype,
					  PyTypeObject* T=&Type );
	
	virtual ~SCA_PropertySensor();
	virtual CValue* GetReplica();
	void	PrecalculateRangeExpression();
	bool	CheckPropertyCondition();

	virtual bool Evaluate(CValue* event);
	virtual bool	IsPositiveTrigger();
	virtual CValue*		FindIdentifier(const STR_String& identifiername);

	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

	virtual PyObject*  _getattr(char *attr);

	/* 1. getType */
	KX_PYMETHOD_DOC(SCA_PropertySensor,GetType);
	/* 2. setType */
	KX_PYMETHOD_DOC(SCA_PropertySensor,SetType);
	/* 3. setProperty */
	KX_PYMETHOD_DOC(SCA_PropertySensor,SetProperty);
	/* 4. getProperty */
	KX_PYMETHOD_DOC(SCA_PropertySensor,GetProperty);
	/* 5. getValue */
	KX_PYMETHOD_DOC(SCA_PropertySensor,GetValue);
	/* 6. setValue */
	KX_PYMETHOD_DOC(SCA_PropertySensor,SetValue);
	
};
#endif

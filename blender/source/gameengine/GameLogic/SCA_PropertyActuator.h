/**
 * SCA_PropertyActuator.h
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

#ifndef __KX_PROPERTYACTUATOR
#define __KX_PROPERTYACTUATOR

#include "SCA_IActuator.h"

class SCA_PropertyActuator : public SCA_IActuator
{
	Py_Header;
	
	enum KX_ACT_PROP_MODE {
		KX_ACT_PROP_NODEF = 0,
		KX_ACT_PROP_ASSIGN,
		KX_ACT_PROP_ADD,
		KX_ACT_PROP_COPY,
		KX_ACT_PROP_MAX
	};
	
	/**check whether this value is valid */
	bool isValid(KX_ACT_PROP_MODE mode);
	
	int			m_type;
	STR_String	m_propname;
	STR_String	m_exprtxt;
	CValue*		m_sourceObj; // for copy property actuator

public:



	SCA_PropertyActuator(

		SCA_IObject* gameobj,

		CValue* sourceObj,

		const STR_String& propname,

		const STR_String& expr,

		int acttype,

		PyTypeObject* T=&Type

	);


	~SCA_PropertyActuator();


		CValue* 

	GetReplica(

	);


		bool 

	Update(

		double curtime,

		double deltatime

	);

	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

	PyObject*  _getattr(char *attr);

	// python wrapped methods
	KX_PYMETHOD_DOC(SCA_PropertyActuator,SetProperty);
	KX_PYMETHOD_DOC(SCA_PropertyActuator,GetProperty);
	KX_PYMETHOD_DOC(SCA_PropertyActuator,SetValue);
	KX_PYMETHOD_DOC(SCA_PropertyActuator,GetValue);
	
	/* 5. - ... setObject, getObject, setProp2, getProp2, setMode, getMode*/
	
};
#endif //__KX_PROPERTYACTUATOR_DOC

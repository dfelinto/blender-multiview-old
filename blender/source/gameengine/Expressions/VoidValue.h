/**
 * VoidValue.h: interface for the CVoidValue class.
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
#ifndef __VOIDVALUE_H__
#define __VOIDVALUE_H__

#include "Value.h"


//
// Void value, used to transport *any* type of data
//
class CVoidValue : public CPropValue  
{
	//PLUGIN_DECLARE_SERIAL (CVoidValue,CValue)

public:
	/// Construction/destruction
	CVoidValue()																: m_pAnything(NULL),	m_bDeleteOnDestruct(false) { }
	CVoidValue(void * voidptr, bool bDeleteOnDestruct, AllocationTYPE alloctype)	: m_pAnything(voidptr), m_bDeleteOnDestruct(bDeleteOnDestruct) { if (alloctype == STACKVALUE) CValue::DisableRefCount(); }
	virtual				~CVoidValue();											// Destruct void value, delete memory if we're owning it

	/// Value -> String or number
	virtual const STR_String &	GetText();												// Get string description of void value (unimplemented)
	virtual float		GetNumber()												{ return -1; }

	/// Value calculation
	virtual CValue*		Calc(VALUE_OPERATOR op, CValue *val);
	virtual CValue*		CalcFinal(VALUE_DATA_TYPE dtype, VALUE_OPERATOR op, CValue* val);

	/// Value replication
	virtual CValue*		GetReplica();
	
	/// Data members
	bool				m_bDeleteOnDestruct;
	void*				m_pAnything;
};

#endif // !defined _VOIDVALUE_H

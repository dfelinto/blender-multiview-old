/**
 * Do an object ipo
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

#ifndef __KX_IPOACTUATOR
#define __KX_IPOACTUATOR

#include "SCA_IActuator.h"

class KX_IpoActuator : public SCA_IActuator
{
	Py_Header;

	bool	m_bNegativeEvent;

	/** Begin frame of the ipo. */
	float	m_starttime;
	
	/** End frame of the ipo. */
	float   m_endtime;

	/** Include children in the transforms? */
	bool	m_recurse;

	/** Current active frame of the ipo. */
	float   m_localtime;

	/** play backwards or forwards? (positive means forward). */
	float	m_direction;

	/** Name of the property (only used in from_prop mode). */
	STR_String	m_propname;

	/** Interpret the ipo as a force? */
	bool    m_ipo_as_force;
	
	/** Apply a force-ipo locally? */
	bool    m_force_ipo_local;

public:
	enum IpoActType
	{
		KX_ACT_IPO_NODEF = 0,
		KX_ACT_IPO_PLAY,
		KX_ACT_IPO_PINGPONG,
		KX_ACT_IPO_FLIPPER,
		KX_ACT_IPO_LOOPSTOP,
		KX_ACT_IPO_LOOPEND,
		KX_ACT_IPO_KEY2KEY,
		KX_ACT_IPO_FROM_PROP,
		KX_ACT_IPO_MAX
	};

	static STR_String S_KX_ACT_IPO_PLAY_STRING;
	static STR_String S_KX_ACT_IPO_PINGPONG_STRING;
	static STR_String S_KX_ACT_IPO_FLIPPER_STRING;
	static STR_String S_KX_ACT_IPO_LOOPSTOP_STRING;
	static STR_String S_KX_ACT_IPO_LOOPEND_STRING;
	static STR_String S_KX_ACT_IPO_KEY2KEY_STRING;
	static STR_String S_KX_ACT_IPO_FROM_PROP_STRING;

	IpoActType string2mode(char* modename);
	
	IpoActType	m_type;

	KX_IpoActuator(SCA_IObject* gameobj,
				   const STR_String& propname,
				   float starttime,
				   float endtime,
				   bool recurse,
				   int acttype,
				   bool ipo_as_force, 
				   bool force_ipo_local, 
				   PyTypeObject* T=&Type);
	virtual ~KX_IpoActuator() {};

	virtual CValue* GetReplica() {
		KX_IpoActuator* replica = new KX_IpoActuator(*this);//m_float,GetName());
		replica->ProcessReplica();
		// this will copy properties and so on...
		CValue::AddDataToReplica(replica);
		return replica;
	};

	void		SetStart(float starttime);
	void		SetEnd(float endtime);
	virtual		bool Update(double curtime,double deltatime);

	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

	virtual PyObject*  _getattr(char *attr);
	//KX_PYMETHOD_DOC
	KX_PYMETHOD_DOC(KX_IpoActuator,Set);
	KX_PYMETHOD_DOC(KX_IpoActuator,SetProperty);
/*  	KX_PYMETHOD_DOC(KX_IpoActuator,SetKey2Key); */
	KX_PYMETHOD_DOC(KX_IpoActuator,SetStart);
	KX_PYMETHOD_DOC(KX_IpoActuator,GetStart);
	KX_PYMETHOD_DOC(KX_IpoActuator,SetEnd);
	KX_PYMETHOD_DOC(KX_IpoActuator,GetEnd);
	KX_PYMETHOD_DOC(KX_IpoActuator,SetIpoAsForce);
	KX_PYMETHOD_DOC(KX_IpoActuator,GetIpoAsForce);
	KX_PYMETHOD_DOC(KX_IpoActuator,SetType);
	KX_PYMETHOD_DOC(KX_IpoActuator,GetType);
	KX_PYMETHOD_DOC(KX_IpoActuator,SetForceIpoActsLocal);
	KX_PYMETHOD_DOC(KX_IpoActuator,GetForceIpoActsLocal);
	
};

#endif //__KX_IPOACTUATOR

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
 * Ketsji Logic Extenstion: Network Message Actuator class
 */
#ifndef __KX_NETWORKMESSAGEACTUATOR_H
#define __KX_NETWORKMESSAGEACTUATOR_H

#include "STR_String.h"

#include "SCA_IActuator.h"

#include "NG_NetworkMessage.h"

class KX_NetworkMessageActuator : public SCA_IActuator
{
	Py_Header;
	bool m_lastEvent;
	class NG_NetworkScene* m_networkscene;	// needed for replication
	STR_String m_toPropName;
	STR_String m_subject;
	int m_bodyType;
	STR_String m_body;
public:
	KX_NetworkMessageActuator(
		SCA_IObject* gameobj,
		NG_NetworkScene* networkscene,
		const STR_String &toPropName,
		const STR_String &subject,
		int bodyType,
		const STR_String &body,
		PyTypeObject* T=&Type);
	virtual ~KX_NetworkMessageActuator();

	virtual bool Update(double curtime, double deltatime);
	virtual CValue* GetReplica();

	/* ------------------------------------------------------------ */
	/* Python interface ------------------------------------------- */
	/* ------------------------------------------------------------ */

	virtual PyObject* _getattr(char *attr);

	KX_PYMETHOD(KX_NetworkMessageActuator, SetToPropName);
	KX_PYMETHOD(KX_NetworkMessageActuator, SetSubject);
	KX_PYMETHOD(KX_NetworkMessageActuator, SetBodyType);
	KX_PYMETHOD(KX_NetworkMessageActuator, SetBody);

};
#endif //__KX_NETWORKMESSAGEACTUATOR_H

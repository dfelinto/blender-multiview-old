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
 * Ketsji Logic Extenstion: Network Message Sensor class
 */
#ifndef __KX_NETWORKMESSAGE_SENSOR_H
#define __KX_NETWORKMESSAGE_SENSOR_H

#include "SCA_ISensor.h"

class KX_NetworkEventManager;
class NG_NetworkScene;

class KX_NetworkMessageSensor : public SCA_ISensor
{
	// note: Py_Header MUST BE the first listed here
	Py_Header;
	KX_NetworkEventManager *m_Networkeventmgr;
	NG_NetworkScene        *m_NetworkScene;

	// The subject we filter on.
	STR_String m_subject;

	// The number of messages caught since the last frame.
	int m_frame_message_count;

	bool m_IsUp;

	class CListValue* m_BodyList;
public:
	KX_NetworkMessageSensor(
		KX_NetworkEventManager* eventmgr,	// our eventmanager
		NG_NetworkScene *NetworkScene,		// our scene
		SCA_IObject* gameobj,				// the sensor controlling object
		const STR_String &subject,
		PyTypeObject* T=&Type
	);
	virtual ~KX_NetworkMessageSensor();

	virtual CValue* GetReplica();
	virtual bool Evaluate(CValue* event);
	virtual bool IsPositiveTrigger();
	void EndFrame();
	
	/* ------------------------------------------------------------- */
	/* Python interface -------------------------------------------- */
	/* ------------------------------------------------------------- */

	virtual PyObject*  _getattr(char *attr);

	KX_PYMETHOD_DOC(KX_NetworkMessageSensor, SetSubjectFilterText);
	KX_PYMETHOD_DOC(KX_NetworkMessageSensor, GetFrameMessageCount);
	KX_PYMETHOD_DOC(KX_NetworkMessageSensor, GetBodies);
	KX_PYMETHOD_DOC(KX_NetworkMessageSensor, GetSubject);


};
#endif //__KX_NETWORKMESSAGE_SENSOR_H

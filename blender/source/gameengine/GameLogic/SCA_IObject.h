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
 * An abstract object that has some logic, python scripting and
 * reference counting Note: transformation stuff has been moved to
 * SceneGraph
 */

#ifndef SCA_IOBJECT_H
#define SCA_IOBJECT_H

#include "Value.h"
#include <vector>

class SCA_ISensor;
class SCA_IController;
class SCA_IActuator;


typedef std::vector<SCA_ISensor *>       SCA_SensorList;
typedef std::vector<SCA_IController *>   SCA_ControllerList;
typedef std::vector<SCA_IActuator *>     SCA_ActuatorList;


class SCA_IObject :	public CValue
{
	
	Py_Header;
	
protected:
	SCA_SensorList         m_sensors;
	SCA_ControllerList     m_controllers;
	SCA_ActuatorList       m_actuators;
	static class MT_Point3 m_sDummy;

	/**
	 * Ignore activity culling requests?
	 */
	bool m_ignore_activity_culling;

	/**
	 * Ignore updates?
	 */
	bool m_suspended;
	
public:
	
	SCA_IObject(PyTypeObject* T=&Type);
	virtual ~SCA_IObject();

	SCA_ControllerList& GetControllers();
	SCA_SensorList& GetSensors();
	SCA_ActuatorList& GetActuators();

	void AddSensor(SCA_ISensor* act);
	void AddController(SCA_IController* act);
	void AddActuator(SCA_IActuator* act);
	
	SCA_ISensor* FindSensor(const STR_String& sensorname);
	SCA_IActuator* FindActuator(const STR_String& actuatorname);
	SCA_IController* FindController(const STR_String& controllername);

	void SetCurrentTime(float currentTime);

	void ReParentLogic();
	
	/**
	 * Set whether or not to ignore activity culling requests
	 */
	void SetIgnoreActivityCulling(bool b);

	/**
	 * Set whether or not this object wants to ignore activity culling
	 * requests
	 */
	bool GetIgnoreActivityCulling();

	/**
	 * Suspend all progress.
	 */
	void Suspend(void);
	
	/**
	 * Resume progress
	 */
	void Resume(void);
	
	const class MT_Point3&	ConvertPythonPylist(PyObject* pylist);
	const class MT_Point3&	ConvertPythonVectorArg(PyObject* args);
	
	// here come the python forwarded methods
	PyObject* _getattr(char* attr);
	
};
#endif //SCA_IOBJECT_H





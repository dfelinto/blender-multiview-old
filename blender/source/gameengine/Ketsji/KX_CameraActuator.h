/**
 * KX_CameraActuator.h
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

#ifndef __KX_CAMERAACTUATOR
#define __KX_CAMERAACTUATOR

#include "SCA_IActuator.h"
#include "MT_Scalar.h"


/**
 * The camera actuator does a Robbie Muller prespective for you. This is a 
 * weird set of rules that positions the camera sort of behind the object,
 * tracking, while avoiding any objects between the 'ideal' position and the
 * actor being tracked.
 */


class KX_CameraActuator : public SCA_IActuator
{

private :
	Py_Header;
	
	/** Object that will be tracked. */
	const CValue *m_ob;

	/** height (float), */
	const MT_Scalar m_height;
	
	/** min (float), */
	const MT_Scalar m_minHeight;
	
	/** max (float), */
	const MT_Scalar m_maxHeight;
	
	/** xy toggle (pick one): true == x, false == y */
	bool m_x;

	/* get the KX_IGameObject with this name */
	CValue *findObject(char *obName);

	/* parse x or y to a toggle pick */
	bool string2axischoice(const char *axisString);
	
 public:
	static STR_String X_AXIS_STRING;
	static STR_String Y_AXIS_STRING;
	
	/**
	 * Set the bool toggle to true to use x lock, false for y lock
	 */
	KX_CameraActuator(

		SCA_IObject *gameobj,
		const CValue *ob,
		MT_Scalar hght,
		MT_Scalar minhght,
		MT_Scalar maxhght,
		bool xytog,
		PyTypeObject* T=&Type

	);


	~KX_CameraActuator();



	/** Methods Inherited from  CValue */


	CValue* GetReplica();
	

	/** Methods inherited from SCA_IActuator */


	bool Update(

		double curtime,

		double deltatime

	);


	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

	virtual PyObject*  _getattr(char *attr);

	

};
#endif //__KX_CAMERAACTUATOR

/**
 * Senses mouse events
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

#ifndef __KX_MOUSESENSOR
#define __KX_MOUSESENSOR

#include "SCA_ISensor.h"
#include "BoolValue.h"
#include "SCA_IInputDevice.h"

class SCA_MouseSensor : public SCA_ISensor
{
	Py_Header;
	class SCA_MouseManager*	m_pMouseMgr;
	
	/**
	 * Use SCA_IInputDevice values to encode the mouse mode for now.
	 */
	short int m_mousemode;
	/**
	 * Triggermode true means all mouse events trigger. Useful mainly
	 * for button presses.
	 */
	bool m_triggermode;
	/**
	 * Remember the last state update 
	 */
	int m_val;

	SCA_IInputDevice::KX_EnumInputs m_hotkey;
	
	/**
	 * valid x coordinate
	 */
	short m_x;

	/**
	 * valid y coordinate
	 */
	short m_y;
	
 public:
	/**
	 * Allowable modes for the trigger status of the mouse sensor.
	 */
	enum KX_MOUSESENSORMODE {
		KX_MOUSESENSORMODE_NODEF = 0,
		KX_MOUSESENSORMODE_LEFTBUTTON,
		KX_MOUSESENSORMODE_MIDDLEBUTTON,
		KX_MOUSESENSORMODE_RIGHTBUTTON,
		KX_MOUSESENSORMODE_POSITION,
		KX_MOUSESENSORMODE_POSITIONX,
		KX_MOUSESENSORMODE_POSITIONY,
		KX_MOUSESENSORMODE_MOVEMENT,
		KX_MOUSESENSORMODE_MAX
	};

	bool isValid(KX_MOUSESENSORMODE);
	
	SCA_MouseSensor(class SCA_MouseManager* keybdmgr,
					int startx,int starty,
				   short int mousemode,
				   SCA_IObject* gameobj,
				   PyTypeObject* T=&Type );

	virtual ~SCA_MouseSensor();
	virtual CValue* GetReplica();
	virtual bool Evaluate(CValue* event);

	virtual bool IsPositiveTrigger();
	short int GetModeKey();
	SCA_IInputDevice::KX_EnumInputs GetHotKey();
	void setX(short x);
	void setY(short y);
	
	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

	virtual PyObject*  _getattr(char *attr);

	/* read x-coordinate */
	KX_PYMETHOD_DOC(SCA_MouseSensor,GetXPosition);
	/* read y-coordinate */
	KX_PYMETHOD_DOC(SCA_MouseSensor,GetYPosition);

};

#endif //__KX_MOUSESENSOR

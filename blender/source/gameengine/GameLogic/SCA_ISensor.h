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
 * Interface Class for all logic Sensors. Implements
 * pulsemode and pulsefrequency, and event suppression.
 */

#ifndef __SCA_ISENSOR
#define __SCA_ISENSOR

#include "SCA_ILogicBrick.h"



/**
 * Interface Class for all logic Sensors. Implements
 * pulsemode,pulsefrequency */
class SCA_ISensor : public SCA_ILogicBrick
{
	Py_Header;
	class SCA_EventManager* m_eventmgr;
	bool	m_triggered;

	/* Pulse positive  pulses? */
	bool m_pos_pulsemode;

	/** Pulse negative pulses? */
	bool m_neg_pulsemode;

	/** Repeat frequency in pulse mode. */
	int m_pulse_frequency;

	/** Number of ticks since the last positive pulse. */
	int m_pos_ticks;

	/** Number of ticks since the last negative pulse. */
	int m_neg_ticks;

	/* invert the output signal*/
	bool m_invert;

	/** Sensor must ignore updates? */
	bool m_suspended;

	/** Pass the activation on to the logic manager.*/
	void SignalActivation(class SCA_LogicManager* logicmgr);
	
public:
	SCA_ISensor(SCA_IObject* gameobj,
				class SCA_EventManager* eventmgr,
				PyTypeObject* T );;
	~SCA_ISensor();
	virtual void	ReParent(SCA_IObject* parent);

	/* Because we want sensors to share some behaviour, the Activate has     */
	/* an implementation on this level. It requires an evaluate on the lower */
	/* level of individual sensors. Mapping the old activate()s is easy.     */
	/* The IsPosTrig() also has to change, to keep things consistent.        */
	void Activate(class SCA_LogicManager* logicmgr,CValue* event);
	virtual bool Evaluate(CValue* event) = 0;
	virtual bool IsPositiveTrigger();
	
	virtual PyObject*  _getattr(char *attr);
	virtual CValue* GetReplica()=0;

	/** Set parameters for the pulsing behaviour.
	 * @param posmode Trigger positive pulses?
	 * @param negmode Trigger negative pulses?
	 * @param freq    Frequency to use when doing pulsing.
	 */
	void SetPulseMode(bool posmode,
					  bool negmode,
					  int freq);
	
	/** Set inversion of pulses on or off. */
	void SetInvert(bool inv);

	void RegisterToManager();
	virtual float GetNumber();

	/** Stop sensing for a while. */
	void Suspend();

	/** Is this sensor switched off? */
	bool IsSuspended();
	
	/** Resume sensing. */
	void Resume();

	/* Python functions: */
	KX_PYMETHOD_DOC(SCA_ISensor,IsPositive);
	KX_PYMETHOD_DOC(SCA_ISensor,GetUsePosPulseMode);
	KX_PYMETHOD_DOC(SCA_ISensor,SetUsePosPulseMode);
	KX_PYMETHOD_DOC(SCA_ISensor,GetFrequency);
	KX_PYMETHOD_DOC(SCA_ISensor,SetFrequency);
	KX_PYMETHOD_DOC(SCA_ISensor,GetUseNegPulseMode);
	KX_PYMETHOD_DOC(SCA_ISensor,SetUseNegPulseMode);
	KX_PYMETHOD_DOC(SCA_ISensor,GetInvert);
	KX_PYMETHOD_DOC(SCA_ISensor,SetInvert);

};

#endif //__SCA_ISENSOR

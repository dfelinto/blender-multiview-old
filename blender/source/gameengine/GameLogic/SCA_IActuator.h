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
 */
#ifndef __KX_IACTUATOR
#define __KX_IACTUATOR

#include "SCA_ILogicBrick.h"
#include <vector>

class SCA_IActuator : public SCA_ILogicBrick
{
protected:
	std::vector<CValue*> m_events;
	void RemoveAllEvents();

public:
	/**
	 * This class also inherits the default copy constructors
	 */

	SCA_IActuator(SCA_IObject* gameobj,
				  PyTypeObject* T =&Type); 

	/**
	 * Update(...)
	 * Update the actuator based upon the events received since 
	 * the last call to Update, the current time and deltatime the
	 * time elapsed in this frame ?
	 * It is the responsibility of concrete Actuators to clear
	 * their event's. This is usually done in the Update() method via 
	 * a call to RemoveAllEvents()
	 */


	virtual bool Update(double curtime,
						double deltatime);

	/** 
	 * Add an event to an actuator.
	 */ 
	void AddEvent(CValue* event);
	virtual void ProcessReplica();

	/** 
	 * Return true iff all the current events 
	 * are negative. The definition of negative event is
	 * not immediately clear. But usually refers to key-up events
	 * or events where no action is required.
	 */
	bool IsNegativeEvent() const;
	virtual ~SCA_IActuator();
};
#endif //__KX_IACTUATOR

/**
 * $Id$
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

/**

 * $Id$
 * Copyright (C) 2001 NaN Technologies B.V.
 * @author	Maarten Gribnau
 * @date	March 31, 2001
 */

#ifndef _H_ACT_ACTION_H_
#define _H_ACT_ACTION_H_

#include "MEM_RefCounted.h"
#include "STR_String.h"

/**
 * An action that can be applied or undone.
 * Virtual base class for actions with a name.
 * An action object inherits the shared object reference count mechanism.
 * @author	Maarten Gribnau
 * @date	March 31, 2001
 */

class ACT_Action : public MEM_RefCounted {
public:
	/**
	 * Constructs an action with the given name .
	 * @param	name	the name of the action.
	 * @param	applied	state of the action after creation.
	 */
	ACT_Action(const STR_String& name, bool applied = false);

	/** 
	 * Returns the name of the action.
	 * @param	name	the name of the action.
	 */
	inline virtual void getName(STR_String& name) const;

	/** 
	 * Returns the name of the action as a pointer to the data of the string.
	 * @return	pointer to the name of the action.
	 */
	inline virtual char* getName();

	/** 
	 * Sets the name of the action.
	 * @param	name	the name of the action.
	 */
	inline virtual void setName(const STR_String& name);

	/** 
	 * Returns whether this action is applied.
	 * @return whether this action is applied.
	 */
	inline virtual bool getIsApplied() const;


	/**
	 * Call this member to apply an action.
	 * Child classes should override the doApply member to implement apply functionality.
	 * @see doApply.
	 */
	virtual void apply();

	/**
	 * Call this member to undo an action .
	 * Child classes should override the doUndo member to implement undo functionality.
	 * @see doApply()
	 */
	virtual void undo();

protected:
	/**
	 * Implementations should overload this method to provide the apply functionality.
	 */
	virtual void doApply() = 0;

	/**
	 * Implementations should overload this method to provide the undo functionality.
	 */
	virtual void doUndo() = 0;

	/** Name of this action */
	STR_String m_name;
	/** Is this action already applied? */
	bool m_applied;
};


void ACT_Action::getName(STR_String& name) const
{
	name = m_name;
}

char* ACT_Action::getName()
{
	return m_name.Ptr();
}

void ACT_Action::setName(const STR_String& name)
{
	m_name = name;
}

inline bool ACT_Action::getIsApplied() const
{
	return m_applied;
}

#endif // _H_ACT_ACTION_H_
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
 * @date	May 15, 2001
 */

#ifndef _GHOST_BUTTONS_H_
#define _GHOST_BUTTONS_H_

#include "GHOST_Types.h"


struct GHOST_Buttons {
    /**
     * Constructor.
     */
    GHOST_Buttons();
    
    /**
     * Returns the state of a single button.
     * @param mask. Key button to return.
     * @return The state of the button (pressed == true).
     */
    virtual bool get(GHOST_TButtonMask mask) const;
    
    /**
     * Updates the state of a single button.
     * @param mask. Button state to update.
     * @param down. The new state of the button.
     */
    virtual void set(GHOST_TButtonMask mask, bool down);
    
    /**
     * Sets the state of all buttons to up.
     */
    virtual void clear(); 
    
    GHOST_TUns8 m_ButtonLeft		: 1;
    GHOST_TUns8 m_ButtonMiddle		: 1;
    GHOST_TUns8 m_ButtonRight		: 1;
};

#endif // _GHOST_BUTTONS_H_

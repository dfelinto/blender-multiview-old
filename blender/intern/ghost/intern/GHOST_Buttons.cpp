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
 * @date	May 31, 2001
 */

#include "GHOST_Buttons.h"


GHOST_Buttons::GHOST_Buttons()
{
	clear();
}


bool GHOST_Buttons::get(GHOST_TButtonMask mask) const
{
    switch (mask) {
    case GHOST_kButtonMaskLeft:
        return m_ButtonLeft;
    case GHOST_kButtonMaskMiddle:
        return m_ButtonMiddle;
    case GHOST_kButtonMaskRight:
        return m_ButtonRight;
    default:
        return false;
    }
}

void GHOST_Buttons::set(GHOST_TButtonMask mask, bool down)
{
    switch (mask) {
    case GHOST_kButtonMaskLeft:
        m_ButtonLeft = down; break;
    case GHOST_kButtonMaskMiddle:
        m_ButtonMiddle = down; break;
    case GHOST_kButtonMaskRight:
        m_ButtonRight = down; break;
    default:
        break;
    }
}

void GHOST_Buttons::clear()
{
    m_ButtonLeft = false;
    m_ButtonMiddle = false;
    m_ButtonRight = false;
}

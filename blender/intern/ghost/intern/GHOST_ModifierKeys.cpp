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

#include "GHOST_ModifierKeys.h"


GHOST_ModifierKeys::GHOST_ModifierKeys()
{
	clear();
}


GHOST_TKey GHOST_ModifierKeys::getModifierKeyCode(GHOST_TModifierKeyMask mask)
{
	GHOST_TKey key;
	switch (mask) {
	case GHOST_kModifierKeyLeftShift:		key = GHOST_kKeyLeftShift;		break;
	case GHOST_kModifierKeyRightShift:		key = GHOST_kKeyRightShift;		break;
	case GHOST_kModifierKeyLeftAlt:			key = GHOST_kKeyLeftAlt;		break;
	case GHOST_kModifierKeyRightAlt:		key = GHOST_kKeyRightAlt;		break;
	case GHOST_kModifierKeyLeftControl:		key = GHOST_kKeyLeftControl;	break;
	case GHOST_kModifierKeyRightControl:	key = GHOST_kKeyRightControl;	break;
	case GHOST_kModifierKeyCommand:			key = GHOST_kKeyCommand;		break;
	default:
		// Should not happen
		key = GHOST_kKeyUnknown;
		break;
	}
	return key;
}


bool GHOST_ModifierKeys::get(GHOST_TModifierKeyMask mask) const
{
    switch (mask) {
    case GHOST_kModifierKeyLeftShift:
        return m_LeftShift;
    case GHOST_kModifierKeyRightShift:
        return m_RightShift;
    case GHOST_kModifierKeyLeftAlt:
        return m_LeftAlt;
    case GHOST_kModifierKeyRightAlt:
        return m_RightAlt;
    case GHOST_kModifierKeyLeftControl:
        return m_LeftControl;
    case GHOST_kModifierKeyRightControl:
        return m_RightControl;
    case GHOST_kModifierKeyCommand:
        return m_Command;
    default:
        return false;
    }
}


void GHOST_ModifierKeys::set(GHOST_TModifierKeyMask mask, bool down)
{
    switch (mask) {
    case GHOST_kModifierKeyLeftShift:
        m_LeftShift = down; break;
    case GHOST_kModifierKeyRightShift:
        m_RightShift = down; break;
    case GHOST_kModifierKeyLeftAlt:
        m_LeftAlt = down; break;
    case GHOST_kModifierKeyRightAlt:
        m_RightAlt = down; break;
    case GHOST_kModifierKeyLeftControl:
        m_LeftControl = down; break;
    case GHOST_kModifierKeyRightControl:
        m_RightControl = down; break;
    case GHOST_kModifierKeyCommand:
        m_Command = down; break;
    default:
        break;
    }
}


void GHOST_ModifierKeys::clear()
{
    m_LeftShift = false;
    m_RightShift = false;
    m_LeftAlt = false;
    m_RightAlt = false;
    m_LeftControl = false;
    m_RightControl = false;
    m_Command = false;
}


bool GHOST_ModifierKeys::equals(const GHOST_ModifierKeys& keys) const
{
	return (m_LeftShift == keys.m_LeftShift) &&
		(m_RightShift == keys.m_RightShift) &&
		(m_LeftAlt == keys.m_LeftAlt) &&
		(m_RightAlt == keys.m_RightAlt) &&
		(m_LeftControl == keys.m_LeftControl) &&
		(m_RightControl == keys.m_RightControl) &&
        (m_Command == keys.m_Command);
}

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
#ifndef __KX_CLIENTOBJECT_INFO_H
#define __KX_CLIENTOBJECT_INFO_H

/**
 * Client Type and Additional Info. This structure can be use instead of a bare void* pointer, for safeness, and additional info for callbacks
 */

struct KX_ClientObjectInfo
{
	int			m_type;
	void*		m_clientobject;
	void*		m_auxilary_info;
};

#endif //__KX_CLIENTOBJECT_INFO_H

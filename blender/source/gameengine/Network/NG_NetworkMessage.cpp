/*
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
 * generic Network Message implementation
 */
#include "NG_NetworkMessage.h"
#include <assert.h>

int NG_NetworkMessage::s_nextID = 3; // just some number to start with

NG_NetworkMessage::NG_NetworkMessage(
	const STR_String& to,
	const STR_String& from,
	const STR_String& subject,
	const STR_String& body) :
	m_uniqueMessageID(s_nextID++),
	m_refcount(1),
	m_to(to),
	m_from(from),
	m_subject(subject),
	m_message(body)
{
}

NG_NetworkMessage::~NG_NetworkMessage()
{
	assert(m_refcount==0);
}

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
 * NetworkGame_NetworkMessage generic Network Message class
 */
#ifndef NG_NETWORKMESSAGE_H
#define NG_NETWORKMESSAGE_H

#include "STR_HashedString.h"

class NG_NetworkMessage
{
	static int			s_nextID;
	int					m_uniqueMessageID;	// intern counting MessageID
	unsigned int		m_ClientId;
	int					m_refcount;

	STR_String			m_to;			// receiver
	STR_String			m_from;			// sender
	STR_String			m_subject;		// empty or propName
	STR_String			m_message;		// message or propValue

protected:
	~NG_NetworkMessage();

public:
	NG_NetworkMessage(
		const STR_String& to,
		const STR_String& from,
		const STR_String& subject,
		const STR_String& body);

	void AddRef() {
		m_refcount++;
	}

	// This is not nice code you should'nt need to resort to
	// delete this.
	void Release()
	{
		if (! --m_refcount)
		{
 			delete this;
		}
	}

	/**
	 * set the content of this message
	 */
	void SetMessageText(const STR_String& msgtext) {
		m_message = msgtext;
	}

	/**
	 * get the (read-only) To part of this message
	 */
	const STR_String& GetDestinationName() { return m_to;};

	/**
	 * get the (read-only) From part of this message
	 */
	const STR_String& GetSenderName() { return m_from;};

	/**
	 * get the (read-only) Subject part of this message
	 */
	const STR_String& GetSubject() { return m_subject;};

	/**
	 * get the (read-only) Body part of this message
	 */
	const STR_String& GetMessageText() {
//cout << "GetMessageText " << m_message << "\n";
		return m_message;
	}
	const STR_String& GetMessageText() const {
//cout << "GetMessageText " << m_message << "\n";
		return m_message;
	}

	/**
	 * Set the NetworkMessage sender identifier
	 */
	void SetSender(unsigned int ClientId) {
		m_ClientId = ClientId;
	}

	/**
	 * Get the NetworkMessage sender identifier
	 */
	unsigned int GetSender(void) {
		return m_ClientId;
	}

	/**
	  * get the unique Network Message ID
	  */
	int GetMessageID() {
		return m_uniqueMessageID;
	}
};

#endif //NG_NETWORKMESSAGE_H

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
 * NetworkGameengine_NetworkDeviceInterface
 * Functions like (de)initialize network, get library version
 * To be derived by loopback and network libraries
 */
#ifndef NG_NETWORKDEVICEINTERFACE_H
#define NG_NETWORKDEVICEINTERFACE_H

#include "NG_NetworkMessage.h"

#include <vector>

class NG_NetworkDeviceInterface
{
private:
	// candidates for shared/common implementation class
	bool m_online;
public:
	NG_NetworkDeviceInterface() {};
	virtual ~NG_NetworkDeviceInterface() {};

	virtual void NextFrame()=0;

	/**
	  * Mark network connection online
	  */
	void Online(void) { m_online = true; }
	/**
	  * Mark network connection offline
	  */
	void Offline(void) { m_online = false; }
	/**
	  * Is the network connection established ?
	  */
	bool IsOnline(void) { return m_online; }

	virtual bool Connect(char *address, unsigned int port, char *password,
		     unsigned int localport, unsigned int timeout)=0;
	virtual bool Disconnect(void)=0;

	virtual void SendNetworkMessage(NG_NetworkMessage* msg)=0;
	/**
	  * read NG_NetworkMessage from library buffer, may be
	  * irrelevant for loopbackdevices
	  */
	
	virtual std::vector<NG_NetworkMessage*> RetrieveNetworkMessages()=0;

	/**
	  * number of messages in device hash for this frame
	  */

	virtual STR_String GetNetworkVersion(void)=0;
};

#endif //NG_NETWORKDEVICEINTERFACE_H

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
 * LoopbackNetworkDeviceInterface derived from NG_NetworkDeviceInterface
 */
#ifndef NG_LOOPBACKNETWORKDEVICEINTERFACE_H
#define NG_LOOPBACKNETWORKDEVICEINTERFACE_H

#include <deque>
#include "NG_NetworkDeviceInterface.h"

class NG_LoopBackNetworkDeviceInterface : public NG_NetworkDeviceInterface
{
	enum {
		LOOPBACK_NETWORK_VERSION=28022001
	};
	
	std::deque<NG_NetworkMessage*> m_messages[2];
	int		m_currentQueue;

public:
	NG_LoopBackNetworkDeviceInterface();
	virtual ~NG_LoopBackNetworkDeviceInterface();

	/**
	  * Clear message buffer
	  */
	virtual void NextFrame();

	bool Connect(char *address, unsigned int port, char *password,
		     unsigned int localport, unsigned int timeout) {
	    return true;}
	bool Disconnect(void) {return true;}

	virtual void SendNetworkMessage(class NG_NetworkMessage* msg);
	virtual vector<NG_NetworkMessage*>		RetrieveNetworkMessages();

	STR_String GetNetworkVersion();
};

#endif //NG_LOOPBACKNETWORKDEVICEINTERFACE_H

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
 * TerraplayNetworkDeviceInterface derived from NG_NetworkDeviceInterface
 */
#ifndef NG_TERRAPLAYNETWORKDEVICEINTERFACE_H
#define NG_TERRAPLAYNETWORKDEVICEINTERFACE_H

#include <deque>

#include "GASInterface.h"

#include "NG_NetworkDeviceInterface.h"

class NG_TerraplayNetworkDeviceInterface : public NG_NetworkDeviceInterface
{
	std::deque<NG_NetworkMessage*> m_messages;

	// Terraplay GAS stuff
	GASInterface *GAS;
	GASClientId group_id;
	GASRequestId group_id_request;
	int group_id_request_valid;

	void interface_error(char *str, GASResult error);
public:
	NG_TerraplayNetworkDeviceInterface();
	~NG_TerraplayNetworkDeviceInterface();

	bool Connect(char *GAS_address, unsigned int GAS_port,
		     char *GAS_password, unsigned int localport,
		     unsigned int timeout);
	bool Disconnect(void);

	void SendNetworkMessage(NG_NetworkMessage* nwmsg);
	vector<NG_NetworkMessage*> RetrieveNetworkMessages(void);
	
	STR_String GetNetworkVersion(void);

	int mytest(void);
};

#endif //NG_TERRAPLAYNETWORKDEVICEINTERFACE_H

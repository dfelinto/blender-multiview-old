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

#include "NG_LoopBackNetworkDeviceInterface.h"
#include "NG_NetworkMessage.h"

// temporary debugging printf's
#ifdef NAN_NET_DEBUG
  #include <stdio.h>
#endif

NG_LoopBackNetworkDeviceInterface::NG_LoopBackNetworkDeviceInterface()
{
	m_currentQueue=0;
	Online();   // LoopBackdevices are 'online' immediately
}

NG_LoopBackNetworkDeviceInterface::~NG_LoopBackNetworkDeviceInterface()
{
}

// perhaps this should go to the shared/common implementation too
void NG_LoopBackNetworkDeviceInterface::NextFrame()
{
	// Release reference to the messages while emptying the queue
	while (m_messages[m_currentQueue].size() > 0) {
		// Should do assert(m_events[0]);
		m_messages[m_currentQueue][0]->Release();
		m_messages[m_currentQueue].pop_front();
	}
	//m_messages[m_currentQueue].clear();

	m_currentQueue=1-m_currentQueue;
}

STR_String NG_LoopBackNetworkDeviceInterface::GetNetworkVersion()
{
	return LOOPBACK_NETWORK_VERSION;
}

void NG_LoopBackNetworkDeviceInterface::SendNetworkMessage(NG_NetworkMessage* nwmsg)
{
#ifdef NAN_NET_DEBUG
	printf("NG_LBNDI::SendNetworkMessage '%s'->'%s' '%s' '%s'\n",
		   nwmsg->GetDestinationName().ReadPtr(),
		   nwmsg->GetSenderName().ReadPtr(),
		   nwmsg->GetSubject().ReadPtr(),
		   nwmsg->GetMessageText().ReadPtr());
#endif
	int backqueue = 1-m_currentQueue;

	nwmsg->AddRef();
	m_messages[backqueue].push_back(nwmsg);
}

vector<NG_NetworkMessage*> NG_LoopBackNetworkDeviceInterface::RetrieveNetworkMessages()
{
	vector<NG_NetworkMessage*> messages;
	
	std::deque<NG_NetworkMessage*>::iterator mesit=m_messages[m_currentQueue].begin();
	for (; !(mesit == m_messages[m_currentQueue].end());mesit++)
	{

		// We don't increase the reference count for these messages. We
		// are passing a vector of messages in the interface and not
		// explicitily storing the messgaes for long term usage

		messages.push_back(*mesit);

#ifdef NAN_NET_DEBUG
		printf("NG_LBNDI::RetrieveNetworkMessages '%s'->'%s' '%s' '%s'\n",
			(*mesit)->GetDestinationName().ReadPtr(),
			(*mesit)->GetSenderName().ReadPtr(),
			(*mesit)->GetSubject().ReadPtr(),
			(*mesit)->GetMessageText().ReadPtr());
#endif
	}
	return messages;
}


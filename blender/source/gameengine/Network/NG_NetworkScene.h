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
 * NetworkSceneManagement generic class
 */
#ifndef __NG_NETWORKSCENE_H
#define __NG_NETWORKSCENE_H

#include "GEN_Map.h"
#include "STR_HashedString.h"
#include <vector>
class NG_NetworkDeviceInterface;

class NG_NetworkScene
{
	class NG_NetworkDeviceInterface *m_networkdevice;	
	GEN_Map<STR_HashedString, class NG_NetworkObject *> m_networkObjects;

	// GEN_Maps used as a 'Bloom' filter
	typedef GEN_Map<STR_HashedString, std::vector<class NG_NetworkMessage*>* > TMessageMap;
	TMessageMap m_messagesByDestinationName;
	TMessageMap m_messagesBySenderName;
	TMessageMap m_messagesBySubject;

public:	
	NG_NetworkScene(NG_NetworkDeviceInterface *nic);
	~NG_NetworkScene();

	/**
	 * progress one frame, handle all network traffic
	 */
	void proceed(double curtime, double deltatime);

	/**
	 * add a networkobject to the scene
	 */
	void AddObject(NG_NetworkObject* object);

	/**
	 * remove a networkobject to the scene
	 */
	void RemoveObject(NG_NetworkObject* object);

	/**
	 * remove all objects at once
	 */
	void RemoveAllObjects();

	/**
	 *	send a message (ascii text) over the network
	 */
	void SendMessage(const STR_String& to,const STR_String& from,const STR_String& subject,const STR_String& message);

	/**
	 * find an object by name
	 */
	NG_NetworkObject* FindNetworkObject(const STR_String& objname);

	bool	ConstraintsAreValid(const STR_String& from,const STR_String& subject,class NG_NetworkMessage* message);
	vector<NG_NetworkMessage*> FindMessages(const STR_String& to,const STR_String& from,const STR_String& subject,bool spamallowed);

protected:
	/**
	 * Releases messages in message map members.
	 */
	void ClearAllMessageMaps(void);

	/**
	 * Releases messages for the given message map.
	 * @param map	Message map with messages.
	 */
	void ClearMessageMap(TMessageMap& map);	
};

#endif //__NG_NETWORKSCENE_H

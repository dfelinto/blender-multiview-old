/*
 * SND_DeviceManager.h
 *
 * singleton for creating, switching and deleting audiodevices
 *
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

#ifndef __SND_DEVICEMANAGER_H
#define __SND_DEVICEMANAGER_H

#include "SND_IAudioDevice.h"

class SND_DeviceManager
{
public :

	/**
	 * a subscription is needed before instances are given away
	 * applications must call subscribe first, get an instance, and
	 * when they are finished with sound, unsubscribe
	 */
	static void Subscribe();
	static void Unsubscribe();

	static SND_IAudioDevice* Instance();
	static void	SetDeviceType(int device_type);

private :

	/**
	 * Private to enforce singleton
	 */
	SND_DeviceManager();
	SND_DeviceManager(const SND_DeviceManager&);
	~SND_DeviceManager();
	
	static SND_IAudioDevice* m_instance;

	/**
	 * The type of device to be created on a call
	 * to Instance().
	 */
	static int m_device_type;

	/**
	 * Remember the number of subscriptions.
	 * if 0, delete the device
	 */
	static int m_subscriptions;;
};

#endif //__SND_DEVICEMANAGER_H









		
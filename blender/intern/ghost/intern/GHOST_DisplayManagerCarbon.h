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
 * @date	September 21, 2001
 */

#ifndef _GHOST_DISPLAY_MANAGER_CARBON_H_
#define _GHOST_DISPLAY_MANAGER_CARBON_H_

#ifndef __APPLE__
#error Apple only!
#endif // __APPLE__

#include "GHOST_DisplayManager.h"

#include <Carbon/Carbon.h>

/**
 * Manages system displays  (Mac OSX/Carbon implementation).
 */

class GHOST_DisplayManagerCarbon : public GHOST_DisplayManager
{
public:
	/**
	 * Constructor.
	 */
	GHOST_DisplayManagerCarbon(void);

	/**
	 * Returns the number of display devices on this system.
	 * @param numDisplays The number of displays on this system.
	 * @return Indication of success.
	 */
	virtual GHOST_TSuccess getNumDisplays(GHOST_TUns8& numDisplays) const;

	/**
	 * Returns the number of display settings for this display device.
	 * @param display The index of the display to query with 0 <= display < getNumDisplays().
	 * @param setting The number of settings of the display device with this index.
	 * @return Indication of success.
	 */
	virtual GHOST_TSuccess getNumDisplaySettings(GHOST_TUns8 display, GHOST_TInt32& numSettings) const;

	/**
	 * Returns the current setting for this display device. 
	 * @param display The index of the display to query with 0 <= display < getNumDisplays().
	 * @param index	  The setting index to be returned.
	 * @param setting The setting of the display device with this index.
	 * @return Indication of success.
	 */
	virtual GHOST_TSuccess getDisplaySetting(GHOST_TUns8 display, GHOST_TInt32 index, GHOST_DisplaySetting& setting) const;

	/**
	 * Returns the current setting for this display device. 
	 * @param display The index of the display to query with 0 <= display < getNumDisplays().
	 * @param setting The current setting of the display device with this index.
	 * @return Indication of success.
	 */
	virtual GHOST_TSuccess getCurrentDisplaySetting(GHOST_TUns8 display, GHOST_DisplaySetting& setting) const;

	/**
	 * Changes the current setting for this display device. 
	 * @param display The index of the display to query with 0 <= display < getNumDisplays().
	 * @param setting The current setting of the display device with this index.
	 * @return Indication of success.
	 */
	virtual GHOST_TSuccess setCurrentDisplaySetting(GHOST_TUns8 display, const GHOST_DisplaySetting& setting);

protected:
	/**
	 * Returns a value from a dictionary.
	 * @param	values	Dictionary to return value from.
	 * @param	key	Key to return value for.
	 * @return The value for this key.
	 */
	long getValue(CFDictionaryRef values, CFStringRef key) const;
	
	/** Cached number of displays. */
	CGDisplayCount m_numDisplays;
	/** Cached display id's for each display. */
	CGDirectDisplayID* m_displayIDs;
};


#endif // _GHOST_DISPLAY_MANAGER_CARBON_H_

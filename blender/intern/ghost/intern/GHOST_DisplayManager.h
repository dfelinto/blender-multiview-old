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

#ifndef _GHOST_DISPLAY_MANAGER_H_
#define _GHOST_DISPLAY_MANAGER_H_

#include "GHOST_Types.h"

#ifdef WIN32
#pragma warning (disable:4786) // suppress stl-MSVC debug info warning
#endif // WIN32

#include <vector>

/**
 * Manages system displays  (platform independent implementation).
 */

class GHOST_DisplayManager
{
public:
	enum { kMainDisplay = 0 };
	/**
	 * Constructor.
	 */
	GHOST_DisplayManager(void);
	
	/**
	 * Destructor.
	 */
	virtual ~GHOST_DisplayManager(void);

	/**
	 * Initializes the list with devices and settings.
	 * @return Indication of success.
	 */
	virtual GHOST_TSuccess initialize(void);

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
	 * The setting given to this method is matched againts the available diplay settings.
	 * The best match is activated (@see findMatch()).
	 * @param display The index of the display to query with 0 <= display < getNumDisplays().
	 * @param setting The setting of the display device to be matched and activated.
	 * @return Indication of success.
	 */
	virtual GHOST_TSuccess setCurrentDisplaySetting(GHOST_TUns8 display, const GHOST_DisplaySetting& setting);

protected:
	typedef std::vector<GHOST_DisplaySetting> GHOST_DisplaySettings;

	/**
	 * Finds the best display settings match.
	 * @param display	The index of the display device.
	 * @param setting	The setting to match.
	 * @param match		The optimal display setting.
	 * @return Indication of success.
	 */
	GHOST_TSuccess findMatch(GHOST_TUns8 display, const GHOST_DisplaySetting& setting, GHOST_DisplaySetting& match) const;

	/**
	 * Retrieves settings for each display device and stores them.
	 * @return Indication of success.
	 */
	GHOST_TSuccess initializeSettings(void);
	
	/** Tells whether the list of display modes has been stored already. */
	bool m_settingsInitialized;
	/** The list with display settings for the main display. */
	std::vector<GHOST_DisplaySettings> m_settings;
};


#endif // _GHOST_DISPLAY_MANAGER_H_

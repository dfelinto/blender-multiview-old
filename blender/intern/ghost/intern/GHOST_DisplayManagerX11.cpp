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

#include "GHOST_DisplayManagerX11.h"

#include "GHOST_SystemX11.h"



GHOST_DisplayManagerX11::
GHOST_DisplayManagerX11(
	GHOST_SystemX11 *system
) :
	GHOST_DisplayManager(),
	m_system(system)
{
	//nothing to do.
}

	GHOST_TSuccess 
GHOST_DisplayManagerX11::
getNumDisplays(
	GHOST_TUns8& numDisplays
) const{	
	numDisplays =  m_system->getNumDisplays();
	return GHOST_kSuccess;
}


	GHOST_TSuccess 
GHOST_DisplayManagerX11::
getNumDisplaySettings(
	GHOST_TUns8 display,
	GHOST_TInt32& numSettings
) const{
	
	// We only have one X11 setting at the moment.
	GHOST_ASSERT(display < 1, "Only single display systems are currently supported.\n");	
	numSettings = GHOST_TInt32(1);
	return GHOST_kSuccess;
}

	GHOST_TSuccess 
GHOST_DisplayManagerX11::
getDisplaySetting(
	GHOST_TUns8 display,
	GHOST_TInt32 index,
	GHOST_DisplaySetting& setting
) const {
	
	GHOST_ASSERT(display < 1, "Only single display systems are currently supported.\n");	
	GHOST_ASSERT(index < 1, "Requested setting outside of valid range.\n");	
	
	Display * x_display = m_system->getXDisplay();

	if (x_display == NULL) {
		return GHOST_kFailure;
	}

	setting.xPixels  = DisplayWidth(x_display, DefaultScreen(x_display));
	setting.yPixels = DisplayHeight(x_display, DefaultScreen(x_display));
	setting.bpp = DefaultDepth(x_display,DefaultScreen(x_display));

	// Don't think it's possible to get this value from X!
	// So let's guess!!
	setting.frequency = 60;

	return GHOST_kSuccess;
}
	
	GHOST_TSuccess 
GHOST_DisplayManagerX11::
getCurrentDisplaySetting(
	GHOST_TUns8 display,
	GHOST_DisplaySetting& setting
) const {
	return getDisplaySetting(display,GHOST_TInt32(0),setting);
}


	GHOST_TSuccess 
GHOST_DisplayManagerX11::
setCurrentDisplaySetting(
	GHOST_TUns8 display,
	const GHOST_DisplaySetting& setting
){
	// This is never going to work robustly in X 
	// but it's currently part of the full screen interface

	// we fudge it for now.

	return GHOST_kSuccess;
}





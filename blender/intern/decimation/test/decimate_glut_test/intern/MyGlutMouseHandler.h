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

#ifndef NAN_INCLUDED_MyGlutMouseHandler_h

#define NAN_INCLUDED_MyGlutMouseHandler_h

#include "common/GlutMouseManager.h"
#include "GHOST_IWindow.h"

class MyGlutMouseHandler : public GlutMouseHandler
{

public :
 
	static 
		MyGlutMouseHandler *
	New(
	) {
		return new MyGlutMouseHandler();
	}

		void
	ButtonDown(
		GHOST_IWindow * window,
		GHOST_TButtonMask button_mask,
		int x,
		int y
	){
		if (button_mask == GHOST_kButtonMaskLeft) {
			m_moving = true;
			m_begin_x = x;
			m_begin_y = y;	
		}
		window->invalidate();
	}

		void
	ButtonUp(
		GHOST_IWindow * window,
		GHOST_TButtonMask button_mask,
		int x,
		int y
	) {
		if (button_mask == GHOST_kButtonMaskLeft) {
			m_moving = false;
		}
		window->invalidate();
	}

		void
	Motion(
		GHOST_IWindow * window,
		int x,
		int y
	){
		if (m_moving) {
			m_angle_x = m_angle_x + (x - m_begin_x);
			m_begin_x = x;

			m_angle_y = m_angle_y + (y - m_begin_y);
			m_begin_y = y;
		}
		window->invalidate();
	}

	const 
		float
	AngleX(
	) const {
		return m_angle_x;
	}

	const 
		float
	AngleY(
	) const {
		return m_angle_y;
	}

	
private :

	MyGlutMouseHandler (
	) :  
		m_angle_x(0),
		m_angle_y(0),
		m_begin_x(0),
		m_begin_y(0),
		m_moving (false)
	{
	};
		
	float m_angle_x;
	float m_angle_y;
	float m_begin_x;
	float m_begin_y;

	bool m_moving;
	
};

#endif


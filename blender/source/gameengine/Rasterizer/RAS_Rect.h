/**
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

#ifndef _RAS_RECT
#define _RAS_RECT


/** 
 * @section interface class.
 * RAS_Rect just encodes a simple rectangle.
 * Should be part of a generic library
 */

class RAS_Rect
{
public:  // todo: make a decent class, and make private
	int m_x1, m_y1;
	int m_x2, m_y2;

public:
	RAS_Rect() : m_x1(0), m_y1(0), m_x2(0), m_y2(0) {}
	int GetWidth(
	) const {
		return m_x2 - m_x1;
	}
	int GetHeight(
	) const {
		return m_y2 - m_y1;
	}
	int GetLeft(
	) const {
		return m_x1;
	}
	int GetRight(
	) const {
		return m_x2;
	}
	int GetBottom(
	) const {
		return m_y1;
	}
	int GetTop(
	) const {
		return m_y2;
	}

	void SetLeft(
		int x1)
	{
		m_x1 = x1;
	}
	void SetBottom(
		int y1)
	{
		m_y1 = y1;
	}
	void SetRight(
		int x2)
	{
		m_x2 = x2;
	}
	void SetTop(
		int y2)
	{
		m_y2 = y2;
	}
};


#endif // _RAS_RECT

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

#include "IMG_Rect.h"


TVisibility IMG_Rect::getVisibility(IMG_Rect& r) const
{
	bool lt = isInside(r.m_l, r.m_t);
	bool rt = isInside(r.m_r, r.m_t);
	bool lb = isInside(r.m_l, r.m_b);
	bool rb = isInside(r.m_r, r.m_b);
	TVisibility v;
	if (lt && rt && lb && rb) {
		// All points inside, rectangle is inside this
		v = kFullyVisible;		
	}
	else if (!(lt || rt || lb || rb)) {
		// None of the points inside
		// Check to see whether the rectangle is larger than this one
		if ((r.m_l < m_l) && (r.m_t < m_t) && (r.m_r > m_r) && (r.m_b > m_b)) {
			v = kPartiallyVisible;
		}
		else {
			v = kNotVisible;
		}
	}
	else {
		// Some of the points inside, rectangle is partially inside
		v = kPartiallyVisible;
	}
	return v;
}

TVisibility IMG_Rect::getVisibility(IMG_Line& l) const
{
	bool s = isInside(l.m_xs, l.m_ys);
	bool e = isInside(l.m_xe, l.m_ye);
	TVisibility v;
	if (s && e) {
		v = kFullyVisible;
	}
	else if (s || e) {
		v = kPartiallyVisible;
	}
	else {
		v = kNotVisible;
	}
	return v;
}

	
void IMG_Rect::setCenter(TInt32 cx, TInt32 cy)
{
	TInt32 offset = cx - (m_l + (m_r - m_l)/2);
	m_l += offset;
	m_r += offset;
	offset = cy - (m_t + (m_b - m_t)/2);
	m_t += offset;
	m_b += offset;
}

void IMG_Rect::setCenter(TInt32 cx, TInt32 cy, TInt32 w, TInt32 h)
{
	long w_2, h_2;
	
	w_2 = w >> 1;
	h_2 = h >> 1;
	m_l = cx - w_2;
	m_t = cy - h_2;
	m_r = m_l + w;
	m_b = m_t + h;
}

bool IMG_Rect::clip(IMG_Rect& r) const
{
	bool clipped = false;
	if (r.m_l < m_l) {
		r.m_l = m_l;
		clipped = true;
	}
	if (r.m_t < m_t) {
		r.m_t = m_t;
		clipped = true;
	}
	if (r.m_r > m_r) {
		r.m_r = m_r;
		clipped = true;
	}
	if (r.m_b > m_b) {
		r.m_b = m_b;
		clipped = true;
	}
	return clipped;
}

bool IMG_Rect::clip(IMG_Line& l) const
{
	bool clipped = false;
	return clipped;
}

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

/*

 * Copyright (c) 2000 Gino van den Bergen <gino@acm.org>
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Gino van den Bergen makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

#ifndef MT_POINT2_H
#define MT_POINT2_H

#include "MT_Vector2.h"

class MT_Point2 : public MT_Vector2 {
public:
    MT_Point2() {}
    MT_Point2(const float *v) : MT_Vector2(v) {} 
    MT_Point2(const double *v) : MT_Vector2(v) {}
    MT_Point2(MT_Scalar x, MT_Scalar y) : MT_Vector2(x, y) {}

    MT_Point2& operator+=(const MT_Vector2& v);
    MT_Point2& operator-=(const MT_Vector2& v);
    MT_Point2& operator=(const MT_Vector2& v);

    MT_Scalar  distance(const MT_Point2& p) const;
    MT_Scalar  distance2(const MT_Point2& p) const;

    MT_Point2  lerp(const MT_Point2& p, MT_Scalar t) const;
};

MT_Point2  operator+(const MT_Point2& p, const MT_Vector2& v);
MT_Point2  operator-(const MT_Point2& p, const MT_Vector2& v);
MT_Vector2 operator-(const MT_Point2& p1, const MT_Point2& p2);

MT_Scalar MT_distance(const MT_Point2& p1, const MT_Point2& p2);
MT_Scalar MT_distance2(const MT_Point2& p1, const MT_Point2& p2);

MT_Point2 MT_lerp(const MT_Point2& p1, const MT_Point2& p2, MT_Scalar t);

#ifdef GEN_INLINED
#include "MT_Point2.inl"
#endif

#endif

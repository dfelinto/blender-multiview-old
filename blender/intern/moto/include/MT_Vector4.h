/**
 * $Id$
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version. The Blender
 * Foundation also sells licenses for use in proprietary software under
 * the Blender License.  See http://www.blender.org/BL/ for information
 * about this.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
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

#ifndef MT_VECTOR4_H
#define MT_VECTOR4_H

#include <MT_assert.h>

#include "MT_Tuple4.h"

class MT_Vector4 : public MT_Tuple4 {
public:
    MT_Vector4() {}
    MT_Vector4(const float *v) : MT_Tuple4(v) {}
    MT_Vector4(const double *v) : MT_Tuple4(v) {}
    MT_Vector4(MT_Scalar x, MT_Scalar y, MT_Scalar z, MT_Scalar w) : 
        MT_Tuple4(x, y, z, w) {}
  
    MT_Vector4& operator+=(const MT_Vector4& v);
    MT_Vector4& operator-=(const MT_Vector4& v);
    MT_Vector4& operator*=(MT_Scalar s);
    MT_Vector4& operator/=(MT_Scalar s);

    MT_Scalar   dot(const MT_Vector4& v) const; 

    MT_Scalar   length2() const;
    MT_Scalar   length() const;

    MT_Vector4  absolute() const;

    void        normalize();
    MT_Vector4  normalized() const;

    void        scale(MT_Scalar x, MT_Scalar y, MT_Scalar z, MT_Scalar w); 
    MT_Vector4  scaled(MT_Scalar x, MT_Scalar y, MT_Scalar z, MT_Scalar w) const; 

    bool        fuzzyZero() const;
};

MT_Vector4 operator+(const MT_Vector4& v1, const MT_Vector4& v2);
MT_Vector4 operator-(const MT_Vector4& v1, const MT_Vector4& v2);
MT_Vector4 operator-(const MT_Vector4& v);
MT_Vector4 operator*(const MT_Vector4& v, MT_Scalar s);
MT_Vector4 operator*(MT_Scalar s, const MT_Vector4& v);
MT_Vector4 operator/(const MT_Vector4& v, MT_Scalar s);

MT_Scalar  MT_dot(const MT_Vector4& v1, const MT_Vector4& v2);

MT_Scalar  MT_length2(const MT_Vector4& v);
MT_Scalar  MT_length(const MT_Vector4& v);

bool       MT_fuzzyZero(const MT_Vector4& v);
bool       MT_fuzzyEqual(const MT_Vector4& v1, const MT_Vector4& v2);

#ifdef GEN_INLINED
#include "MT_Vector4.inl"
#endif

#endif


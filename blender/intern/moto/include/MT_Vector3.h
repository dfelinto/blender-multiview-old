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

#ifndef MT_VECTOR3_H
#define MT_VECTOR3_H

#include <MT_assert.h>
#include "MT_Tuple3.h"

class MT_Vector3 : public MT_Tuple3 {
public:
    MT_Vector3() {}
    MT_Vector3(const float *v) : MT_Tuple3(v) {}
    MT_Vector3(const double *v) : MT_Tuple3(v) {}
    MT_Vector3(MT_Scalar x, MT_Scalar y, MT_Scalar z) : MT_Tuple3(x, y, z) {}
  
    MT_Vector3& operator+=(const MT_Vector3& v);
    MT_Vector3& operator-=(const MT_Vector3& v);
    MT_Vector3& operator*=(MT_Scalar s);
    MT_Vector3& operator/=(MT_Scalar s);
  
    MT_Scalar   dot(const MT_Vector3& v) const; 

    MT_Scalar   length2() const;
    MT_Scalar   length() const;

    MT_Vector3  absolute() const;

    void        noiseGate(MT_Scalar threshold);

    void        normalize();
    MT_Vector3  normalized() const;
	MT_Vector3  safe_normalized() const;
	

    void        scale(MT_Scalar x, MT_Scalar y, MT_Scalar z); 
    MT_Vector3  scaled(MT_Scalar x, MT_Scalar y, MT_Scalar z) const; 
    
    bool        fuzzyZero() const; 

    MT_Scalar   angle(const MT_Vector3& v) const;
    MT_Vector3  cross(const MT_Vector3& v) const;
    MT_Scalar   triple(const MT_Vector3& v1, const MT_Vector3& v2) const;

    int         closestAxis() const;

    static MT_Vector3 random();
};

MT_Vector3 operator+(const MT_Vector3& v1, const MT_Vector3& v2);
MT_Vector3 operator-(const MT_Vector3& v1, const MT_Vector3& v2);
MT_Vector3 operator-(const MT_Vector3& v);
MT_Vector3 operator*(const MT_Vector3& v, MT_Scalar s);
MT_Vector3 operator*(MT_Scalar s, const MT_Vector3& v);
MT_Vector3 operator/(const MT_Vector3& v, MT_Scalar s);

MT_Vector3 operator*(const MT_Vector3& v1, const MT_Vector3& v2);

MT_Scalar  MT_dot(const MT_Vector3& v1, const MT_Vector3& v2);

MT_Scalar  MT_length2(const MT_Vector3& v);
MT_Scalar  MT_length(const MT_Vector3& v);

bool       MT_fuzzyZero(const MT_Vector3& v);
bool       MT_fuzzyEqual(const MT_Vector3& v1, const MT_Vector3& v2);

MT_Scalar  MT_angle(const MT_Vector3& v1, const MT_Vector3& v2);
MT_Vector3 MT_cross(const MT_Vector3& v1, const MT_Vector3& v2);
MT_Scalar  MT_triple(const MT_Vector3& v1, const MT_Vector3& v2, 
                     const MT_Vector3& v3);

#ifdef GEN_INLINED
#include "MT_Vector3.inl"
#endif

#endif

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

#ifndef MT_TUPLE4_H
#define MT_TUPLE4_H

#include "MT_Stream.h"
#include "MT_Scalar.h"

class MT_Tuple4 {
public:
    MT_Tuple4() {}
    MT_Tuple4(const float *v) { setValue(v); }
    MT_Tuple4(const double *v) { setValue(v); }
    MT_Tuple4(MT_Scalar x, MT_Scalar y, MT_Scalar z, MT_Scalar w) {
        setValue(x, y, z, w);
    }
    
    MT_Scalar&       operator[](int i)       { return m_co[i]; }
    const MT_Scalar& operator[](int i) const { return m_co[i]; }
    
    MT_Scalar&       x()       { return m_co[0]; } 
    const MT_Scalar& x() const { return m_co[0]; } 

    MT_Scalar&       y()       { return m_co[1]; }
    const MT_Scalar& y() const { return m_co[1]; } 

    MT_Scalar&       z()       { return m_co[2]; } 
    const MT_Scalar& z() const { return m_co[2]; } 

    MT_Scalar&       w()       { return m_co[3]; } 
    const MT_Scalar& w() const { return m_co[3]; } 

    MT_Scalar       *getValue()       { return m_co; }
    const MT_Scalar *getValue() const { return m_co; }
    

    void getValue(float *v) const { 
        v[0] = float(m_co[0]);
		v[1] = float(m_co[1]); 
		v[2] = float(m_co[2]); 
		v[3] = float(m_co[3]);
    }
    
    void getValue(double *v) const { 
        v[0] = double(m_co[0]); 
		v[1] = double(m_co[1]); 
		v[2] = double(m_co[2]); 
		v[3] = double(m_co[3]);
    }
    
    void setValue(const float *v) {
        m_co[0] = MT_Scalar(v[0]); 
        m_co[1] = MT_Scalar(v[1]); 
        m_co[2] = MT_Scalar(v[2]); 
        m_co[3] = MT_Scalar(v[3]);
    }
    
    void setValue(const double *v) {
        m_co[0] = MT_Scalar(v[0]); 
        m_co[1] = MT_Scalar(v[1]); 
        m_co[2] = MT_Scalar(v[2]); 
        m_co[3] = MT_Scalar(v[3]);
    }
    
    void setValue(MT_Scalar x, MT_Scalar y, MT_Scalar z, MT_Scalar w) {
        m_co[0] = x; m_co[1] = y; m_co[2] = z; m_co[3] = w;
    }
    
protected:
    MT_Scalar m_co[4];                            
};

inline bool operator==(const MT_Tuple4& t1, const MT_Tuple4& t2) {
    return t1[0] == t2[0] && t1[1] == t2[1] && t1[2] == t2[2] && t1[3] == t2[3];
}

inline MT_OStream& operator<<(MT_OStream& os, const MT_Tuple4& t) {
    return os << t[0] << ' ' << t[1] << ' ' << t[2] << ' ' << t[3];
}

#endif

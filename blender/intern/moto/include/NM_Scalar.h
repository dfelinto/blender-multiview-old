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

#include <math.h>

#include <iostream>

template <class T> 
class NM_Scalar {
public:    
    NM_Scalar() {}
    explicit NM_Scalar(T value, T error = 0.0) : 
        m_value(value), m_error(error) {}

    T getValue() const { return m_value; }
    T getError() const { return m_error; }

    operator T() const { return m_value; }

    NM_Scalar operator-() const {
        return NM_Scalar<T>(-m_value, m_error);
    }

    NM_Scalar& operator=(T value) {
        m_value = value;
        m_error = 0.0;
        return *this;
    }

    NM_Scalar& operator+=(const NM_Scalar& x) {
        m_value += x.m_value;
        m_error = (fabs(m_value) * (m_error + 1.0) + 
                   fabs(x.m_value) * (x.m_error + 1.0)) /
            fabs(m_value + x.m_value);
        return *this;
    }

    NM_Scalar& operator-=(const NM_Scalar& x) {
        m_value -= x.m_value;
        m_error = (fabs(m_value) * (m_error + 1.0) + 
                   fabs(x.m_value) * (x.m_error + 1.0)) /
            fabs(m_value - x.m_value);
        return *this;
    }

    NM_Scalar& operator*=(const NM_Scalar& x) {
        m_value *= x.m_value;
        m_error += x.m_error + 1.0;
        return *this;
    }

    NM_Scalar& operator/=(const NM_Scalar& x) {
        m_value /= x.m_value;
        m_error += x.m_error + 1.0;
        return *this;
    }

private:
    T m_value;
    T m_error;
};

template <class T>
inline NM_Scalar<T> operator+(const NM_Scalar<T>& x, const NM_Scalar<T>& y) {
    return x.getValue() == 0.0 && y.getValue() == 0.0 ?
        NM_Scalar<T>(0.0, 0.0) :
        NM_Scalar<T>(x.getValue() + y.getValue(), 
                     (fabs(x.getValue()) * (x.getError() + 1.0) + 
                      fabs(y.getValue()) * (y.getError() + 1.0)) /
                     fabs(x.getValue() + y.getValue()));
}

template <class T>
inline NM_Scalar<T> operator-(const NM_Scalar<T>& x, const NM_Scalar<T>& y) {
    return x.getValue() == 0.0 && y.getValue() == 0.0 ?
        NM_Scalar<T>(0.0, 0.0) :
        NM_Scalar<T>(x.getValue() - y.getValue(), 
                     (fabs(x.getValue()) * (x.getError() + 1.0) + 
                      fabs(y.getValue()) * (y.getError() + 1.0)) /
                     fabs(x.getValue() - y.getValue()));
}

template <class T>
inline NM_Scalar<T> operator*(const NM_Scalar<T>& x, const NM_Scalar<T>& y) {
    return NM_Scalar<T>(x.getValue() * y.getValue(), 
                        x.getError() + y.getError() + 1.0);
}

template <class T>
inline NM_Scalar<T> operator/(const NM_Scalar<T>& x, const NM_Scalar<T>& y) {
    return NM_Scalar<T>(x.getValue() / y.getValue(), 
                        x.getError() + y.getError() + 1.0);
}

template <class T>
inline std::ostream& operator<<(std::ostream& os, const NM_Scalar<T>& x) {
    return os << x.getValue() << '[' << x.getError() << ']';
}

template <class T>
inline NM_Scalar<T> sqrt(const NM_Scalar<T>& x) {
    return NM_Scalar<T>(sqrt(x.getValue()),
                        0.5 * x.getError() + 1.0);
}

template <class T>
inline NM_Scalar<T> acos(const NM_Scalar<T>& x) {
    return NM_Scalar<T>(acos(x.getValue()), x.getError() + 1.0);
}

template <class T>
inline NM_Scalar<T> cos(const NM_Scalar<T>& x) {
    return NM_Scalar<T>(cos(x.getValue()), x.getError() + 1.0);
}

template <class T>
inline NM_Scalar<T> sin(const NM_Scalar<T>& x) {
    return NM_Scalar<T>(sin(x.getValue()), x.getError() + 1.0);
}

template <class T>
inline NM_Scalar<T> fabs(const NM_Scalar<T>& x) {
    return NM_Scalar<T>(fabs(x.getValue()), x.getError());
}

template <class T>
inline NM_Scalar<T> pow(const NM_Scalar<T>& x, const NM_Scalar<T>& y) {
    return NM_Scalar<T>(pow(x.getValue(), y.getValue()), 
                        fabs(y.getValue()) * x.getError() + 1.0);
}









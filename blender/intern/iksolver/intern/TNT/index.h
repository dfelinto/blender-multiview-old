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

*
* Template Numerical Toolkit (TNT): Linear Algebra Module
*
* Mathematical and Computational Sciences Division
* National Institute of Technology,
* Gaithersburg, MD USA
*
*
* This software was developed at the National Institute of Standards and
* Technology (NIST) by employees of the Federal Government in the course
* of their official duties. Pursuant to title 17 Section 105 of the
* United States Code, this software is not subject to copyright protection
* and is in the public domain.  The Template Numerical Toolkit (TNT) is
* an experimental system.  NIST assumes no responsibility whatsoever for
* its use by other parties, and makes no guarantees, expressed or implied,
* about its quality, reliability, or any other characteristic.
*
* BETA VERSION INCOMPLETE AND SUBJECT TO CHANGE
* see http://math.nist.gov/tnt for latest updates.
*
*/



// Vector/Matrix/Array Index Module  

#ifndef INDEX_H
#define INDEX_H

#include "subscript.h"

namespace TNT
{

class Index1D
{
    Subscript lbound_;
    Subscript ubound_;

    public:

    Subscript lbound() const { return lbound_; }
    Subscript ubound() const { return ubound_; }

    Index1D(const Index1D &D) : lbound_(D.lbound_), ubound_(D.ubound_) {}
    Index1D(Subscript i1, Subscript i2) : lbound_(i1), ubound_(i2) {}

    Index1D & operator=(const Index1D &D)
    {
        lbound_ = D.lbound_;
        ubound_ = D.ubound_;
        return *this;
    }

};

inline Index1D operator+(const Index1D &D, Subscript i)
{
    return Index1D(i+D.lbound(), i+D.ubound());
}

inline Index1D operator+(Subscript i, const Index1D &D)
{
    return Index1D(i+D.lbound(), i+D.ubound());
}



inline Index1D operator-(Index1D &D, Subscript i)
{
    return Index1D(D.lbound()-i, D.ubound()-i);
}

inline Index1D operator-(Subscript i, Index1D &D)
{
    return Index1D(i-D.lbound(), i-D.ubound());
}

} // namespace TNT

#endif


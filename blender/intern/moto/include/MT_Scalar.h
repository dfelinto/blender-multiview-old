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

#ifndef MT_SCALAR_H
#define MT_SCALAR_H

#include <math.h>
#include <float.h>

#include "MT_random.h"
#include "NM_Scalar.h"

typedef double MT_Scalar;

const MT_Scalar  MT_DEGS_PER_RAD(57.29577951308232286465);
const MT_Scalar  MT_RADS_PER_DEG(0.01745329251994329547);
const MT_Scalar  MT_PI(3.14159265358979323846);
const MT_Scalar  MT_2_PI(6.28318530717958623200);
const MT_Scalar  MT_EPSILON(1.0e-10);
const MT_Scalar  MT_EPSILON2(1.0e-20);
const MT_Scalar  MT_INFINITY(1.0e50);

inline int       MT_sign(MT_Scalar x) {
    return x < 0.0 ? -1 : x > 0.0 ? 1 : 0;
}
 
inline MT_Scalar MT_abs(MT_Scalar x) { return fabs(x); }

inline bool      MT_fuzzyZero(MT_Scalar x) { return MT_abs(x) < MT_EPSILON; }
inline bool      MT_fuzzyZero2(MT_Scalar x) { return MT_abs(x) < MT_EPSILON2; }

inline MT_Scalar MT_radians(MT_Scalar x) { 
    return x * MT_RADS_PER_DEG;
}

inline MT_Scalar MT_degrees(MT_Scalar x) { 
    return x * MT_DEGS_PER_RAD;
}

inline MT_Scalar MT_random() { 
    return MT_Scalar(MT_rand()) / MT_Scalar(MT_RAND_MAX);
}


#endif


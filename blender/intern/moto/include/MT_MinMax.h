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

#ifndef MT_MINMAX_H
#define MT_MINMAX_H

template <class T>
inline const T& MT_min(const T& a, const T& b) {
  return b < a ? b : a;
}

template <class T>
inline const T& MT_max(const T& a, const T& b) {
  return  a < b ? b : a;
}

template <class T>
inline void MT_set_min(T& a, const T& b) {
    if (a > b) a = b;
}

template <class T>
inline void MT_set_max(T& a, const T& b) {
    if (a < b) a = b;
}

#endif

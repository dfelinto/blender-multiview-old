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

#ifndef GEN_STREAM_H

#define GEN_STREAM_H

#ifdef __CUSTOM_STREAM

class MT_OStream
{
public:
    inline  MT_OStream& operator<<(double);
    inline  MT_OStream& operator<<(int);
    inline  MT_OStream& operator<<(char*);
};

const char GEN_endl = '\n';

#else

#include <iostream>

typedef std::ostream MT_OStream;

inline MT_OStream& GEN_endl(MT_OStream& os) { return std::endl(os); }

#endif

#endif

/*
 * SND_Object.h
 *
 * Abstract sound object
 *
 * $Id$
 *
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

#ifndef __SND_OBJECT_H
#define __SND_OBJECT_H

#include "GEN_List.h"
#include "MT_Matrix3x3.h"
#include "SoundDefines.h"

/**
 *   SND_Object is an interface class for soundobjects, listeners and other
 *   kinds of sound related thingies. 
 */

class SND_Object : public GEN_Link
{
public:
	SND_Object() {};
	virtual ~SND_Object() {};
};
#endif //__SND_OBJECT_H

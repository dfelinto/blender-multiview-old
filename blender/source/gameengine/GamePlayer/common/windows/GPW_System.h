/**
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
 * Timing routine taken and modified from KX_BlenderSystem.cpp
 */

#ifndef _GPW_SYSTEM_H_
#define _GPW_SYSTEM_H_

#pragma warning (disable:4786) // suppress stl-MSVC debug info warning

#include "GPC_System.h"

#if defined(__CYGWIN32__)
#	define __int64 long long
#endif


class GPW_System : public GPC_System
{
public:
	GPW_System();

	virtual double GetTimeInSeconds();
protected:

  __int64 m_freq;
  __int64 m_lastCount;
  __int64 m_lastRest;
  long    m_lastTime;

};


#endif //_GPW_SYSTEM_H_

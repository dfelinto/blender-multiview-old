/*
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
 * System specific information / access.
 * Interface to the commandline arguments
 */
#include "SYS_System.h"
#include "SYS_SingletonSystem.h"

SYS_SystemHandle SYS_GetSystem()
{
	return (SYS_SystemHandle) SYS_SingletonSystem::Instance();
}

void SYS_DeleteSystem(SYS_SystemHandle sys)
{
	if (sys) {
		((SYS_SingletonSystem *) sys)->Destruct();
	}
}

int SYS_GetCommandLineInt(SYS_SystemHandle sys, const char *paramname, int defaultvalue)
{
	return ((SYS_SingletonSystem *) sys)->SYS_GetCommandLineInt(paramname, defaultvalue);
}

float SYS_GetCommandLineFloat(SYS_SystemHandle sys, const char *paramname, float defaultvalue)
{
	return ((SYS_SingletonSystem *) sys)->SYS_GetCommandLineFloat(paramname, defaultvalue);
}

const char *SYS_GetCommandLineString(SYS_SystemHandle sys, const char *paramname, const char *defaultvalue)
{
	return ((SYS_SingletonSystem *) sys)->SYS_GetCommandLineString(paramname, defaultvalue);
}

void SYS_WriteCommandLineInt(SYS_SystemHandle sys, const char *paramname, int value)
{
	((SYS_SingletonSystem *) sys)->SYS_WriteCommandLineInt(paramname, value);
}

void SYS_WriteCommandLineFloat(SYS_SystemHandle sys, const char *paramname, float value)
{
	((SYS_SingletonSystem *) sys)->SYS_WriteCommandLineFloat(paramname, value);
}

void SYS_WriteCommandLineString(SYS_SystemHandle sys, const char *paramname, const char *value)
{
	((SYS_SingletonSystem *) sys)->SYS_WriteCommandLineString(paramname, value);
}

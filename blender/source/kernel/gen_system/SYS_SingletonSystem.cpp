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
 * Unique instance of system class for system specific information / access
 * Used by SYS_System
 */
#include "SYS_SingletonSystem.h"
#include "GEN_DataCache.h"

SYS_SingletonSystem*	SYS_SingletonSystem::_instance = 0;

void SYS_SingletonSystem::Destruct()
{
	if (_instance) {
		delete _instance;
		_instance = NULL;
	}
}

SYS_SingletonSystem *SYS_SingletonSystem::Instance()
{
	if (!_instance) {
		_instance = new SYS_SingletonSystem();
	}
	return _instance;
}

int SYS_SingletonSystem::SYS_GetCommandLineInt(const char *paramname, int defaultvalue)
{
	int *result = m_int_commandlineparms[paramname];
	if (result)
		return *result;

	return defaultvalue;
}

float SYS_SingletonSystem::SYS_GetCommandLineFloat(const char *paramname, float defaultvalue)
{
	float *result = m_float_commandlineparms[paramname];
	if (result)
		return *result;

	return defaultvalue;
}

const char *SYS_SingletonSystem::SYS_GetCommandLineString(const char *paramname, const char *defaultvalue)
{
	STR_String *result = m_string_commandlineparms[paramname];
	if (result)
		return *result;

	return defaultvalue;
}

void SYS_SingletonSystem::SYS_WriteCommandLineInt(const char *paramname, int value)
{
	m_int_commandlineparms.insert(paramname, value);
}

void SYS_SingletonSystem::SYS_WriteCommandLineFloat(const char *paramname, float value)
{
	m_float_commandlineparms.insert(paramname, value);
}

void SYS_SingletonSystem::SYS_WriteCommandLineString(const char *paramname, const char *value)
{
	m_string_commandlineparms.insert(paramname, value);
}

SYS_SingletonSystem::SYS_SingletonSystem() : m_soundfilecache(NULL)
{
}

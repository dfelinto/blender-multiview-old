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
#ifndef __SINGLETONSYSTEM_H
#define __SINGLETONSYSTEM_H

#include "GEN_Map.h"
#include "STR_HashedString.h"
#include "GEN_DataCache.h"

class SYS_SingletonSystem
{
public:
	static		SYS_SingletonSystem*	Instance();
	static		void	Destruct();

	int		SYS_GetCommandLineInt(const char* paramname,int defaultvalue);
	float		SYS_GetCommandLineFloat(const char* paramname,float defaultvalue);
	const char*	SYS_GetCommandLineString(const char* paramname,const char* defaultvalue);

	void		SYS_WriteCommandLineInt(const char* paramname,int value);
	void		SYS_WriteCommandLineFloat(const char* paramname,float value);
	void		SYS_WriteCommandLineString(const char* paramname,const char* value);

	SYS_SingletonSystem();

private:
	static SYS_SingletonSystem*	_instance;
	GEN_Map<STR_HashedString,int>	m_int_commandlineparms;
	GEN_Map<STR_HashedString,float>	m_float_commandlineparms;
	GEN_Map<STR_HashedString,STR_String>	m_string_commandlineparms;
	void*	m_soundfilecache;
};

#endif //__SINGLETONSYSTEM_H

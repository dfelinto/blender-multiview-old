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
 */
#ifndef __GEN_DATACACHE_H
#define __GEN_DATACACHE_H

#include "STR_HashedString.h"
#include "GEN_Map.h"

template <class T> 
class GEN_DataCache
{
	GEN_Map<STR_HashedString,T*>	m_dataCache;
	virtual	T*	LoadData(const STR_String& name)=0;
	virtual void	FreeCacheObjects()=0;

public:
	GEN_DataCache() {};
	virtual ~GEN_DataCache() {};

	T*		GetData(const STR_String& paramname)
	{
		T* result=NULL;

		T** resultptr = m_dataCache[paramname];
		if (resultptr)
		{
			result = *resultptr;
		}
		
		else
		{
			result = LoadData(paramname);
			if (result)
			{
				m_dataCache.insert(paramname,result);
			}
		}

		return result;
	}
	
	virtual void	ClearCache()
	{
		FreeCacheObjects();
		m_dataCache.clear();
	}
};

#endif //__GEN_DATACACHE_H

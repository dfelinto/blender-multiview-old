/**
 * $Id$
 *
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */
#ifndef __MEMORY_RESOURCE_H__
#define __MEMORY_RESOURCE_H__

#include <windows.h>

class MemoryResource
{
public:
	/**
	 * Default constructor.
	 */
	MemoryResource();

	/**
	 * Destructor.
	 */
	virtual ~MemoryResource();

	/**
	 * Loads the data from the resource.
	 * @param hInstApp	The application's instance (location of the resources).
	 * @param lpName	Name of the resource.
	 * @param lpType	Type of the resource.
	 */
	virtual bool load(HINSTANCE hInstApp, LPCTSTR lpName, LPCTSTR lpType);

	/**
	 * Returns a pointer to the data loaded from the resource.
	 * @return A pointer to the data loaded from the resource.
	 */
	virtual LPVOID getData() const
	{
		return m_data;
	}

	/**
	 * Returns the size of the data loaded from the resource.
	 * @return The size of the  loaded from the resource.
	 */
	virtual DWORD getDataSize() const
	{
		return m_dataSize;
	}

protected:
	/**
	 * Allocates a data block and store it in the m_data member.
	 * @param	numBytes Size of the data block to allocate.
	 * @return	Indication of success.
	 */
	virtual bool allocateData(unsigned int numBytes);

	/**
	 * Disposes the data stored at the m_data member.
	 */
	virtual void disposeData(void);

	LPVOID	m_data;
	DWORD	m_dataSize;
};

#endif // __MEMORY_RESOURCE_H__


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

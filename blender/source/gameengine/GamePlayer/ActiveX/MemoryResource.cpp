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

#include "MemoryResource.h"


MemoryResource::MemoryResource()
	: m_data(0), m_dataSize(0)
{
}


MemoryResource::~MemoryResource()
{
	disposeData();
}


bool MemoryResource::load(HINSTANCE hInstApp, LPCTSTR lpName, LPCTSTR lpType)
{
	bool success = false;

	HRSRC hResInfo = ::FindResource(hInstApp, lpName, lpType);
	if (hResInfo) {
		m_dataSize = ::SizeofResource(hInstApp, hResInfo);
		if (m_dataSize) {
			success = allocateData(m_dataSize);
			if (success) {
				HGLOBAL hData = ::LoadResource(hInstApp, hResInfo);
				if (hData) {
					LPVOID pData = ::LockResource(hData);
					::memcpy(m_data, pData, m_dataSize);

				}
				else {
					success = false;
				}
			}
		}
	}

	return success;
}


bool MemoryResource::allocateData(unsigned int numBytes)
{
	disposeData();
	m_data = new char [m_dataSize];
	return m_data ? true : false;
}


void MemoryResource::disposeData(void)
{
	if (m_data) {
		delete [] m_data;
		m_data = 0;
	}
}

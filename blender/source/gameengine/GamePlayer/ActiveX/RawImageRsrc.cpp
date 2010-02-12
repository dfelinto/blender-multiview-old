/**
 * $Id$
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
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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

#include "RawImageRsrc.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

RawImageRsrc::RawImageRsrc()
	: m_width(0), m_height(0)
{
}

bool RawImageRsrc::load(
	HINSTANCE hInstApp, LPCTSTR lpName, LPCTSTR lpType,
	int srcWidth, int srcHeight,
	int width, int height,
	TImageAlignment alignment, int offsetX, int offsetY)
{
	bool success = MemoryResource::load(hInstApp, lpName, lpType);

	if (success) {
		void* tempData = m_data;

		int numBytes = width*height*4;
		m_data = new char [numBytes];
		if (m_data) {
			::memset(m_data, 0x00000000, numBytes);
			m_width = width;
			m_height = height;

			int srcBytesWidth = srcWidth * 4;
			int dstBytesWidth = width * 4;
			int numRows = (srcHeight + offsetY) < height ? srcHeight : height - offsetY;
			numBytes = (srcWidth + offsetX) < width ? srcBytesWidth : (width - offsetX)*4;

			if ((offsetX < width) && (offsetY < height)) {
				unsigned char* src = (unsigned char*)tempData;
				unsigned char* dst = (unsigned char*)m_data;
				if (alignment == alignTopLeft) {
					// Put original in upper left corner

					// Add vertical offset
					dst += offsetY * dstBytesWidth;	
					// Add horizontal offset
					dst += offsetX * 4;
					for (int row = 0; row < numRows; row++) {
						::memcpy(dst, src, numBytes);
						src += srcBytesWidth;
						dst += dstBytesWidth;
					}
				}
				else {
					// Put original in lower right corner

					// Add vertical offset
					dst += (height - (srcHeight + offsetY)) * dstBytesWidth;
					// Add horizontal offset
					if (width > (srcWidth + offsetX)) {
						dst += (width - (srcWidth + offsetX)) * 4;
					}
					else {
						src += (srcWidth + offsetX - width) * 4;
					}
					for (int row = 0; row < numRows; row++) {
						::memcpy(dst, src, numBytes);
						src += srcBytesWidth;
						dst += dstBytesWidth;
					}
				}
			}
			delete [] tempData;
		}
		else {
			// Allocation failed, restore old data
			m_data = tempData;
			success = false;
		}
	}
	
	return success;
}


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
   
#include <iostream>
#include <string.h>

#include "GPC_RawImage.h"

#include "GPC_RawLogoArrays.h"


GPC_RawImage::GPC_RawImage()
		: m_data(0), m_dataSize(0), m_width(0), m_height(0)
{
}


bool GPC_RawImage::Load(
		char *srcName,
		int destWidth, int destHeight,
		TImageAlignment alignment, int offsetX, int offsetY)
{
	int srcWidth, srcHeight;
	bool success = true;
	if(strcmp(srcName, "BlenderLogo") == 0)
		GetRawBlenderLogo(&m_data, &srcWidth, &srcHeight);
	else
		if(strcmp(srcName, "Blender3DLogo") == 0)
			GetRawBlender3DLogo(&m_data, &srcWidth, &srcHeight);
#if 0
	else
		if(strcmp(srcName, "NaNLogo") == 0)
			GetRawNaNLogo(&m_data, &srcWidth, &srcHeight);
#endif
		else  // unknown image
			success = false;

	if(success)
	{
		unsigned char *tempData = m_data;

		int numBytes = destWidth * destHeight * 4;
		m_data = new unsigned char[numBytes];  // re-use m_data ('unsigned char' was 'char')
		if(m_data)
		{
			::memset(m_data, 0x00000000, numBytes);
			m_width = destWidth;
			m_height = destHeight;

			int srcBytesWidth = srcWidth * 4;
			int dstBytesWidth = m_width * 4;
			int numRows = (srcHeight + offsetY) < m_height ? srcHeight : m_height - offsetY;
			numBytes = (srcWidth + offsetX) < m_width ? srcBytesWidth : (m_width - offsetX) * 4;

			if((offsetX < m_width) && (offsetY < m_height))
			{
				unsigned char* src = (unsigned char*)tempData;
				unsigned char* dst = (unsigned char*)m_data;
				if(alignment == alignTopLeft)
				{
					// Put original in upper left corner

					// Add vertical offset
					dst += offsetY * dstBytesWidth;	
					// Add horizontal offset
					dst += offsetX * 4;
					for (int row = 0; row < numRows; row++)
					{
						::memcpy(dst, src, numBytes);
						src += srcBytesWidth;
						dst += dstBytesWidth;
					}
				}
				else
				{
					// Put original in lower right corner

					// Add vertical offset
					dst += (m_height - (srcHeight + offsetY)) * dstBytesWidth;
					// Add horizontal offset
					if (m_width > (srcWidth + offsetX)) {
						dst += (m_width - (srcWidth + offsetX)) * 4;
					}
					else {
						src += (srcWidth + offsetX - m_width) * 4;
					}
					for (int row = 0; row < numRows; row++) {
						::memcpy(dst, src, numBytes);
						src += srcBytesWidth;
						dst += dstBytesWidth;
					}
				}
			}
// doesn't compile under Linux			delete [] tempData;
			delete tempData;
		}
		else {
			// Allocation failed, restore old data
			m_data = tempData;
			success = false;
		}
	}
	
	return success;
}


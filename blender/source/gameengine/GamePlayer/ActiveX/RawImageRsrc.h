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
#ifndef __RAW_IMAGE_RSRC_H__
#define __RAW_IMAGE_RSRC_H__

#include "MemoryResource.h"

/**
 * This memory resource reads images from the application resources.
 * Images are expected to be stored as raw RGBA bytes.
 * You can generate these images by exporting images with an alpha 
 * channel from Photoshop in RAW format with interleaved channels.
 */

class RawImageRsrc : public MemoryResource
{
public:
	RawImageRsrc();

	typedef enum {
		alignTopLeft,
		alignBottomRight
	} TImageAlignment;

	/**
	 * Loads the image form the resource into memory.
	 * Converts size of the image and places it with given alignment.
	 * @param hInstApp	The application's instance (location of the resources).
	 * @param lpName	Name of the resource.
	 * @param lpType	Type of the resource.
	 * @param srcWidth	The width of the resource image.
	 * @param srcHeight	The height of the resource image.
	 * @param width		The width of the image created.
	 * @param height	The height of the image created.
	 * @param alignment How the resource image is located in the image created.
	 * @param offsetX	Amount of horzontal offset applied to the resource image.
	 * @param offsetY	Amount of vertical offset applied to the resource image.
	 */
	virtual bool load(
		HINSTANCE hInstApp, LPCTSTR lpName, LPCTSTR lpType,
		int srcWidth, int srcHeight,
		int width, int height,
		TImageAlignment alignment = alignTopLeft,
		int offsetX = 0, int offsetY = 0);

	/**
	 * Returns the width of the image.
	 * @return The width of the image.
	 */
	virtual int getWidth() const
	{
		return m_width;
	}

	/**
	 * Returns the height of the image.
	 * @return The height of the image.
	 */
	virtual int getHeight() const
	{
		return m_height;
	}

protected:
	int m_width;
	int m_height;
};

#endif // __RAW_IMAGE_RSRC_H__


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

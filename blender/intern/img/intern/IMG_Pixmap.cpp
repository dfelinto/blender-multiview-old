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

/**

 * $Id$
 * Copyright (C) 2001 NaN Technologies B.V.
 * Base class for pixmaps.
 * @author	Maarten Gribnau
 * @date	March 6, 2001
 */


#include "../extern/IMG_Pixmap.h"


IMG_Pixmap::IMG_Pixmap()
{
	m_width = 0;
	m_height = 0;
	m_rowBytes = 0;
	m_pixelSize = 0;
	m_pixelType = kPixelTypeUnknown;
/*
#if OS_MACINTOSH
	bitOrder = kQ3EndianBig;
	byteOrder = kQ3EndianBig;
#else 
	bitOrder = kQ3EndianLittle;
	byteOrder = kQ3EndianLittle;
#endif
*/
}


IMG_Pixmap::IMG_Pixmap(GEN_TUns32 width, GEN_TUns32 height)
{
	m_width = width;
	m_height = height;
	m_rowBytes = 0;
	m_pixelSize = 0;
	m_pixelType = kPixelTypeUnknown;
/*
#if OS_MACINTOSH
	bitOrder = kQ3EndianBig;
	byteOrder = kQ3EndianBig;
#else 
	bitOrder = kQ3EndianLittle;
	byteOrder = kQ3EndianLittle;
#endif
*/
}


IMG_Pixmap::~IMG_Pixmap()
{
}
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

#include "IMG_Pixmap.h"




IMG_Pixmap::IMG_Pixmap()
{
	m_image = 0;
	m_width = 0;
	m_height = 0;
	m_rowBytes = 0;
	m_pixelSize = 0;
	m_pixelType = kPixelTypeRGB32;
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
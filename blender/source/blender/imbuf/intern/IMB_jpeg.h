/**
 * IMB_jpeg.h
 *
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

#ifndef IMB_JPEG_H
#define IMB_JPEG_H

struct ImBuf;
struct jpeg_compress_struct;

int imb_save_jpeg(char * name, struct ImBuf * ibuf, int flags);
struct ImBuf * imb_ibJpegImageFromFilename (char * filename, int flags);
struct ImBuf * imb_ibJpegImageFromMemory (unsigned char * buffer, int size, int flags);

#endif

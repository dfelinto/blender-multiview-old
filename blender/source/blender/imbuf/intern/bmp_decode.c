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

#ifdef _WIN32
#include "BLI_winstuff.h"
#endif
#include "BLI_blenlib.h"

#include "imbuf.h"
#include "imbuf_patch.h"

#include "IMB_imbuf_types.h"
#include "IMB_imbuf.h"

#include "IMB_allocimbuf.h"
#include "IMB_cmap.h"
#include "IMB_bmp.h"


// some code copied from article on microsoft.com, copied
// here for enhanced BMP support in the future
// http://www.microsoft.com/msj/defaultframe.asp?page=/msj/0197/mfcp1/mfcp1.htm&nav=/msj/0197/newnav.htm

/*
LPBYTE CDib::GetBits()
 {
   return (LPBYTE)m_pbmih + // start of bitmap +
     m_pbmih->biSize +      // size of header +
     GetNumPaletteColors()  // (num colors *
       *sizeof(RGBQUAD);    // size each entry)
 }

UINT CDib::GetNumPaletteColors()
 {
    UINT nColors=m_pbmih->biClrUsed;
    if (nColors==0 && m_pbmih->biBitCount<=8)
       nColors = 1<<m_pbmih->biBitCount;
    return nColors;
 }

*/

typedef struct BMPINFOHEADER{
	unsigned int	biSize;
	int				biWidth;
	int				biHeight;
	unsigned short	biPlanes;
	unsigned short	biBitCount;
	unsigned int	biCompression;
	unsigned int	biSizeImage;
	int				biXPelsPerMeter;
	int				biYPelsPerMeter;
	unsigned int	biClrUsed;
	unsigned int	biClrImportant;
} BMPINFOHEADER;

#define BMP_FILEHEADER_SIZE 14

static int checkbmp(unsigned char *mem)
{
	int ret_val = 0;
	BMPINFOHEADER bmi;
	unsigned int u;

	if (mem) {
		if ((mem[0] == 'B') && (mem[1] == 'M')) {
			// skip fileheader
			mem += BMP_FILEHEADER_SIZE;
		}

		// for systems where an int needs to be 4 bytes aligned
		memcpy(&bmi, mem, sizeof(bmi));

		u = LITTLE_LONG(bmi.biSize);
		// we only support uncompressed 24 or 32 bits images for now
		if (u >= sizeof(BMPINFOHEADER)) {
			if ((bmi.biCompression == 0) && (bmi.biClrUsed == 0)) {
				u = LITTLE_SHORT(bmi.biBitCount);
				if (u >= 16) {
					ret_val = 1;
				}
			}
		}
	}

	return(ret_val);
}

int imb_is_a_bmp(void *buf) {
	
	return checkbmp(buf);
}

struct ImBuf *imb_bmp_decode(unsigned char *mem, int size, int flags)
{
	struct ImBuf *ibuf = 0;
	BMPINFOHEADER bmi;
	int x, y, depth, skip, i;
	unsigned char *bmp, *rect;
	unsigned short col;

	if (checkbmp(mem) == 0) return(0);

	if ((mem[0] == 'B') && (mem[1] == 'M')) {
		// skip fileheader
		mem += BMP_FILEHEADER_SIZE;
	}

	// for systems where an int needs to be 4 bytes aligned
	memcpy(&bmi, mem, sizeof(bmi));

	skip = LITTLE_LONG(bmi.biSize);
	x = LITTLE_LONG(bmi.biWidth);
	y = LITTLE_LONG(bmi.biHeight);
	depth = LITTLE_SHORT(bmi.biBitCount);

	// printf("skip: %d, x: %d y: %d, depth: %d (%x)\n", skip, x, y, depth, bmi.biBitCount);
	if (flags & IB_test) {
		ibuf = IMB_allocImBuf(x, y, depth, 0, 0);
	} else {
		ibuf = IMB_allocImBuf(x, y, depth, IB_rect, 0);
		bmp = mem + skip;
		rect = (unsigned char *) ibuf->rect;

		if (depth == 16) {
			for (i = x * y; i > 0; i--) {
				col = bmp[0] + (bmp[1] << 8);
				rect[0] = ((col >> 10) & 0x1f) << 3;
				rect[1] = ((col >>  5) & 0x1f) << 3;
				rect[2] = ((col >>  0) & 0x1f) << 3;
				
				rect[3] = 255;
				rect += 4; bmp += 2;
			}

		} else if (depth == 24) {
			for (i = x * y; i > 0; i--) {
				rect[0] = bmp[2];
				rect[1] = bmp[1];
				rect[2] = bmp[0];
				
				rect[3] = 255;
				rect += 4; bmp += 3;
			}
		} else if (depth == 32) {
			for (i = x * y; i > 0; i--) {
				rect[0] = bmp[0];
				rect[1] = bmp[1];
				rect[2] = bmp[2];
				rect[3] = bmp[3];
				rect += 4; bmp += 4;
			}
		}
	}

	if (ibuf) {
		ibuf->ftype = BMP;
	}
	
	return(ibuf);
}


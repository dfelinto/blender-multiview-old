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
#ifndef AVI_INTERN_H
#define AVI_INTERN_H

#include <stdio.h> /* for FILE */

#include "MEM_guardedalloc.h"

unsigned int GET_FCC (FILE *fp);
unsigned int GET_TCC (FILE *fp);

#define PUT_FCC(ch4, fp) putc(ch4[0],fp); putc(ch4[1],fp); putc(ch4[2],fp); putc(ch4[3],fp)
#define PUT_FCCN(num, fp) putc((num>>0)&0377,fp); putc((num>>8)&0377,fp); putc((num>>16)&0377,fp); putc((num>>24)&0377,fp)
#define PUT_TCC(ch2, fp) putc(ch2[0],fp); putc(ch2[1],fp)

void *avi_format_convert (AviMovie *movie, int stream, void *buffer, AviFormat from, AviFormat to, int *size);

int avi_get_data_id(AviFormat format, int stream);
int avi_get_format_type(AviFormat format);
int avi_get_format_fcc(AviFormat format);
int avi_get_format_compression(AviFormat format);

#endif

/**
 * Compatibility-like things for windows.
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
#pragma warning(once: 4761 4305 4244 4018)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef near
#undef far
#undef rad
#undef rad1
#undef rad2
#undef rad3
#undef vec
#undef rect
#undef rct1
#undef rct2

#define near clipsta
#define far clipend

#undef small

#ifndef __WINSTUFF_H__
#define __WINSTUFF_H__

#define M_PI		3.14159265358979323846
#define M_PI_2		1.57079632679489661923
#define M_SQRT2		1.41421356237309504880
#define M_SQRT1_2	0.70710678118654752440

#define MAXPATHLEN MAX_PATH

#define S_ISREG(x) ((x&S_IFMT) == S_IFREG)
#define S_ISDIR(x) ((x&S_IFMT) == S_IFDIR)

typedef unsigned int mode_t;

struct dirent {
	int d_ino;
	int d_off;
	unsigned short d_reclen;
	char *d_name;
};

typedef struct _DIR {
	HANDLE handle;
	WIN32_FIND_DATA data;
	char path[MAX_PATH];
	long dd_loc;
	long dd_size;
	char dd_buf[4096];
	void *dd_direct;
	
	struct dirent direntry;
} DIR;

void RegisterBlendExtension(char * str);
int strcasecmp (char *s1, char *s2);
int strncasecmp (char *s1, char *s2, int n);
DIR *opendir (const char *path);
struct dirent *readdir(DIR *dp);
int closedir (DIR *dp);

#endif /* __WINSTUFF_H__ */

/**
 * blenlib/BLI_storage_types.h
 *
 * Some types for dealing with directories
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
#ifndef BLI_STORAGE_TYPES_H
#define BLI_STORAGE_TYPES_H

#include <sys/stat.h>

#define HDRSIZE 512
#define NAMSIZE 200

struct header{
	char	name[NAMSIZE];
	unsigned int	size;
	unsigned int	chksum;
	char	fill[HDRSIZE-NAMSIZE-2*sizeof(unsigned int)];
};

#ifdef WIN32
typedef unsigned int mode_t;
#endif

struct direntry{
	char	*string;
	mode_t	type;
	char	*relname;
	struct	stat s;
	unsigned int	flags;
	char	size[16];
	char	mode1[4];
	char	mode2[4];
	char	mode3[4];
	char	owner[16];
	char	time[8];
	char	date[16];
	char	extra[16];
	void	*poin;
	int		nr;
};

struct dirlink
{
	struct dirlink *next,*prev;
	char *name;
};

#endif /* BLI_STORAGE_TYPES_H */

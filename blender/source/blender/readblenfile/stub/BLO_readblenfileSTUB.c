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
 *
 */

#include <stdio.h>
#include "GEN_messaging.h"

	int
BLO_readblenfilememory(
	char *fromBuffer, int fromBufferSize)
{
#ifndef NDEBUG
	fprintf(GEN_errorstream,
			"Error BLO_readblenfilename is a stub\n");
#endif
	return(1);
}

	int
BLO_readblenfilename(
	char *fileName)
{
#ifndef NDEBUG
	fprintf(GEN_errorstream,
			"Error BLO_readblenfilename is a stub\n");
#endif
	return(1);
}

	int
BLO_readblenfilehandle(
	int fileHandle)
{
#ifndef NDEBUG
	fprintf(GEN_errorstream,
			"Error BLO_readblenfilehandle is a stub\n");
#endif
	return(1);
}

	int
BLO_is_a_runtime(
	char *file)
{
#ifndef NDEBUG
	fprintf(GEN_errorstream,
			"Error BLO_is_a_runtime is a stub\n");
#endif
	return 0;
}

	int
BLO_read_runtime(
	char *file) 
{
#ifndef NDEBUG
	fprintf(GEN_errorstream,
			"Error BLO_read_runtime is a stub\n");
#endif
	return 0;
}

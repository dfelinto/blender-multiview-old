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
 * A little test to see how our type defines behave. 
 */

#include "../../../readstreamglue/BLO_sys_types.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
	int verbose       = 0;
	int error_status  = 0;
	int char_size     = 0;
	int short_size     = 0;
	int int_size     = 0;
	int long_size     = 0;
	
   	switch (argc) {
	case 2:		
		verbose = atoi(argv[1]);
		if (verbose < 0) verbose = 0;
		break;		
	case 1:
	default:
		verbose = 0;
	}

	/* ----------------------------------------------------------------- */
	if (verbose > 0) {
		printf("*** Type define size test\n|\n");
	}
	/* Check if these exist, and show their sizes. */
	

	char_size     = sizeof(uint8_t);
	short_size    = sizeof(uint16_t);
	int_size      = sizeof(uint32_t);
	long_size     = sizeof(uint64_t);

	if (verbose > 1) {
		printf("|- uint8_t  : \t%4d, expected 1.\n", char_size);
		printf("|- uint16_t : \t%4d, expected 2.\n", short_size);
		printf("|- uint32_t : \t%4d, expected 4.\n", int_size);
		printf("|- uint64_t : \t%4d, expected 8.\n\n", long_size);
	}

	if ((char_size != 1)
		|| (short_size != 2)
		|| (int_size != 4)
		|| (long_size != 8)
		) {
		error_status = 1;
	}

	if (verbose > 0) {
		if (error_status) {
			printf("|-- Size mismatch detected !!!\n|\n");
		} else {
			printf("|-- Sizes are correct.\n");
		}
		printf("|\n*** End of type define size test\n");
	}
	
	exit(error_status);
}

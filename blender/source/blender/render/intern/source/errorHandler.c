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
 * Error handler for the rendering code. Maybe also useful elsewhere?
 */

#include "GEN_messaging.h"
#include "stdio.h"
#include "errorHandler.h"

#include "render_intern.h"

/* ------------------------------------------------------------------------- */

/* counters for error handling */
static int lastError;         /* code of last encountered error              */
static int errorCount;        /* count how many time it occured              */
/* ------------------------------------------------------------------------- */

char errorStrings[RE_MAX_ERROR][100] = {
    "0: No error",
    "1: recalculated depth falls outside original range",
    "2: invalid face/halo type",
    "3: invalid face index",
    "4: invalid data pointer",
	"5: generic trace counter",
	"6: overflow on z buffer depth",
	"7: write outside edgerender buffer",
	"8: cannot allocate memory",
	"9: write outside colour target buffer",
};

/* ------------------------------------------------------------------------- */

void RE_errortrace_reset(void)
{
	lastError = RE_NO_ERROR;
	errorCount = 0;
}

void RE_error(int errType, char* fname)
{
	/*
	 * This memory behaviour should move to the generic stream...
	 */
	
    if (lastError == errType) {
        int teller;
        errorCount++;
        for (teller = 0; teller < 12; teller++)
            fprintf(GEN_errorstream, "%c", 0x08); /* backspaces */
        fprintf(GEN_errorstream, "( %8u )", errorCount);
    } else {
        fprintf(GEN_errorstream, "\n*** %s: %s             ", 
                fname, errorStrings[errType]);
        lastError = errType;
        errorCount = 1;
    }    
} /* end of void RE_error(int errType, char* errText) */

/* ------------------------------------------------------------------------- */
/* note: non-repeating */
void RE_error_int(int errType, char* fname, int value)
{
	fprintf(GEN_errorstream, "\n*** %s: %s : %d", 
			fname, errorStrings[errType], value);
	lastError = RE_NO_ERROR;
} /* end of void RE_error_int(int errType, char* errText, int value) */

/* ------------------------------------------------------------------------- */

/* eof */

/*
 * errorHandler.h
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

#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H 

/* ------------------------------------------------------------------------- */
/* error codes */
enum RE_RENDER_ERROR {
	RE_NO_ERROR,
	RE_DEPTH_MISMATCH,     /* 1.  conflict resolution detects a bad z value  */
	RE_BAD_FACE_TYPE,      /* 2. a face type switch fails                    */
	RE_BAD_FACE_INDEX,     /* 3. tried to do an operation with a bad index   */
	RE_BAD_DATA_POINTER,
	RE_TRACE_COUNTER,
	RE_TOO_MANY_FACES,     /* 6. overflow on z-buffer depth                  */
	RE_EDGERENDER_WRITE_OUTSIDE_BUFFER, /* 7. write value outside buffer     */
	RE_CANNOT_ALLOCATE_MEMORY, /* 8. no memory for malloc                    */
	RE_WRITE_OUTSIDE_COLOUR_BUFFER, /* 9. write outside colour target buffer */
	RE_MAX_ERROR
};

/**
 * Reset all counters for the error trace
 */
void RE_errortrace_reset(void);

/**
 * Signals an error to screen. Counts repetitive errors
 */
void RE_error(int errType, char* fname);

/**
 * Signals an error, and prints an integer argument
 */
void RE_error_int(int errType, char* fname, int valye);

#endif /* ERRORHANDLER_H */

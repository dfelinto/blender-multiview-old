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
#ifndef __EXPRESSION_INCLUDE
#define __EXPRESSION_INCLUDE


#define EXP_DECLARE_HANDLE(name) typedef struct name##__ { int unused; } *name

EXP_DECLARE_HANDLE(EXP_ValueHandle);
EXP_DECLARE_HANDLE(EXP_ExpressionHandle);


#ifdef __cplusplus
extern "C" {
#endif

extern EXP_ValueHandle		EXP_CreateInt(int innie);
extern EXP_ValueHandle		EXP_CreateBool(int innie);
extern EXP_ValueHandle		EXP_CreateString(const char* str);
extern void					EXP_SetName(EXP_ValueHandle,const char* newname);

/* calculate expression from inputtext */
extern EXP_ValueHandle		EXP_ParseInput(const char* inputtext);
extern void					EXP_ReleaseValue(EXP_ValueHandle);
extern int					EXP_IsValid(EXP_ValueHandle);

/* assign property 'propval' to 'destinationval' */
extern void					EXP_SetProperty(EXP_ValueHandle propval,EXP_ValueHandle destinationval);

/* returns NULL if property doesn't exist */
extern EXP_ValueHandle		EXP_GetProperty(EXP_ValueHandle inval,const char* propname);

const char*					EXP_GetText(EXP_ValueHandle);

#ifdef __cplusplus
}
#endif

#endif //__EXPRESSION_INCLUDE

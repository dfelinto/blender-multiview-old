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
#include "EXP_C-Api.h"
#include "IntValue.h"
#include "BoolValue.h"
#include "StringValue.h"
#include "ErrorValue.h"
#include "InputParser.h"


EXP_ValueHandle EXP_CreateInt(int innie)
{
	return (EXP_ValueHandle) new CIntValue(innie);
}



EXP_ValueHandle EXP_CreateBool(int innie)
{
	return (EXP_ValueHandle) new CBoolValue(innie!=0);
}



EXP_ValueHandle EXP_CreateString(const char* str)
{
	
	return (EXP_ValueHandle) new CStringValue(str,"");
}



void EXP_SetName(EXP_ValueHandle inval,const char* newname)
{
	((CValue*) inval)->SetName(newname);
}



/* calculate expression from inputtext */
EXP_ValueHandle EXP_ParseInput(const char* inputtext)
{
	CValue* resultval=NULL;
	CParser parser;
	CExpression* expr = parser.ProcessText(inputtext);
	if (expr)
	{
		resultval = expr->Calculate();
		expr->Release();
	}
	else
	{
		resultval = new CErrorValue("couldn't parsetext");
	}

	return (EXP_ValueHandle) resultval;
}



void EXP_ReleaseValue(EXP_ValueHandle inval)
{
	((CValue*) inval)->Release();
}



int EXP_IsValid(EXP_ValueHandle inval)
{
	return !((CValue*) inval)->IsError();
}



/* assign property 'propval' to 'destinationval' */
void EXP_SetProperty(EXP_ValueHandle destinationval,
					 const char* propname,
					 EXP_ValueHandle propval)
{
	((CValue*) destinationval)->SetProperty(propname,(CValue*)propval);
}



const char* EXP_GetText(EXP_ValueHandle inval)
{
	return ((CValue*) inval)->GetText();
}



EXP_ValueHandle EXP_GetProperty(EXP_ValueHandle inval,const char* propname)
{
	return (EXP_ValueHandle) ((CValue*)inval)->GetProperty(propname);
}

/**
 * $Id$
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

/**

 * $Id$
 * Copyright (C) 2001 NaN Technologies B.V.
 * @author	Maarten Gribnau
 * @date	April, 25, 2001
 */

#include "ACT_ActionC-Api.h"

#include "ACT_ActionStack.h"
#include "ACT_CallbackAction.h"


ACT_ActionPtr ACT_ActionCreate(
	char* name,
	int isApplied,
	ACT_ActionUserDataPtr data,
	ACT_ActionApplyProcPtr applyProc,
	ACT_ActionUndoProcPtr undoProc,
	ACT_ActionDisposeProcPtr disposeProc)
{
	STR_String tmp (name);
	ACT_CallbackAction* action = new ACT_CallbackAction(tmp, isApplied != 0, data, applyProc, undoProc, disposeProc);
	return (ACT_ActionPtr) action;
}


char* ACT_ActionGetName(ACT_ActionPtr action)
{
	return action ? ((ACT_Action*)action)->getName() : 0;
}


ACT_ActionStackPtr ACT_ActionStackCreate(unsigned int stackSize)
{
	return ((ACT_ActionStackPtr) (new ACT_ActionStack (stackSize)));
}


void ACT_ActionStackDispose(ACT_ActionStackPtr stack)
{
	if (stack) {
		delete (ACT_ActionStack*) stack;
	}
}


unsigned int ACT_ActionStackGetStackDepth(ACT_ActionStackPtr stack)
{
	return stack ? ((ACT_ActionStack*)stack)->getStackDepth() : 0;
}

unsigned int ACT_ActionStackGetMaxStackDepth(ACT_ActionStackPtr stack)
{
	return stack ? ((ACT_ActionStack*)stack)->getMaxStackDepth() : 0;
}

void ACT_ActionStackSetMaxStackDepth(ACT_ActionStackPtr stack, unsigned int maxStackDepth)
{
	if (stack) {
		((ACT_ActionStack*)stack)->setMaxStackDepth(maxStackDepth);
	}
}

void ACT_ActionStackPush(ACT_ActionStackPtr stack, ACT_ActionPtr action)
{
	if (stack && action) {
		((ACT_ActionStack*)stack)->push(*((ACT_Action*)action));
	}
}


ACT_ActionStackPtr ACT_ActionStackPeekUndo(ACT_ActionStackPtr stack)
{
	return (ACT_ActionStackPtr) (stack ? ((ACT_ActionStack*)stack)->peekUndo() : 0);
}


ACT_ActionStackPtr ACT_ActionStackPeekRedo(ACT_ActionStackPtr stack)
{
	return (ACT_ActionStackPtr) (stack ? ((ACT_ActionStack*)stack)->peekRedo() : 0);
}


void ACT_ActionStackUndo(ACT_ActionStackPtr stack)
{
	if (stack) {
		((ACT_ActionStack*)stack)->undo();
	}
}


void ACT_ActionStackRedo(ACT_ActionStackPtr stack)
{
	if (stack) {
		((ACT_ActionStack*)stack)->redo();
	}
}

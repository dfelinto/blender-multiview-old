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
 * @date	March 31, 2001
 */

#include "ACT_ActionC-Api.h"
#include "TestAction.h"

int main(int argc, char *argv[])
{
	ACT_ActionStackPtr stack = ACT_ActionStackCreate (3);
	ACT_ActionPtr action = ACT_ActionCreate("action1", 0, 0, printApplied, printUndone, printDisposed);
	ACT_ActionStackPush(stack, action);
	MEM_RefCountedDecRef(action);
	action = ACT_ActionCreate("action2", 0, 0, printApplied, printUndone, printDisposed);
	ACT_ActionStackPush(stack, action);
	MEM_RefCountedDecRef(action);
	action = ACT_ActionCreate("action3", 0, 0, printApplied, printUndone, printDisposed);
	ACT_ActionStackPush(stack, action);
	MEM_RefCountedDecRef(action);

	ACT_ActionStackUndo(stack);
	ACT_ActionStackUndo(stack);
	ACT_ActionStackUndo(stack);
	ACT_ActionStackRedo(stack);
	ACT_ActionStackRedo(stack);
	ACT_ActionStackRedo(stack);

	ACT_ActionStackSetMaxStackDepth(stack, 1);
	ACT_ActionStackUndo(stack);
	ACT_ActionStackRedo(stack);
	ACT_ActionStackSetMaxStackDepth(stack, 5);
	ACT_ActionStackUndo(stack);
	ACT_ActionStackRedo(stack);

	action = ACT_ActionCreate("action4", 0, 0, printApplied, printUndone, printDisposed);
	ACT_ActionStackPush(stack, action);
	MEM_RefCountedDecRef(action);
	ACT_ActionStackUndo(stack);
	action = ACT_ActionCreate("action5", 0, 0, printApplied, printUndone, printDisposed);
	ACT_ActionStackPush(stack, action);
	MEM_RefCountedDecRef(action);
	ACT_ActionStackUndo(stack);
	ACT_ActionStackUndo(stack);
	ACT_ActionStackRedo(stack);
	ACT_ActionStackRedo(stack);

	return 0;
}
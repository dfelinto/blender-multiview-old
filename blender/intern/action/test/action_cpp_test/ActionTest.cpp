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

#include "ACT_ActionStack.h"
#include "TestAction.h"

int main()
{
    ACT_ActionStack testStack (3);
    TestAction* testAction = new TestAction (STR_String("action1"));
    testStack.push(*testAction);
    testAction->decRef();
    testAction = new TestAction (STR_String("action2"));
    testStack.push(*testAction);
    testAction->decRef();
    testAction = new TestAction (STR_String("action3"));
    testStack.push(*testAction);
    testAction->decRef();

    testStack.undo();
    testStack.undo();
    testStack.undo();
    testStack.redo();
    testStack.redo();
    testStack.redo();

    testStack.setMaxStackDepth(1);
    testStack.undo();
    testStack.redo();
    testStack.setMaxStackDepth(5);
    testStack.undo();
    testStack.redo();

    testAction = new TestAction (STR_String("action4"));
    testStack.push(*testAction);
    testAction->decRef();
    testStack.undo();
    testAction = new TestAction (STR_String("action5"));
    testStack.push(*testAction);
    testAction->decRef();
    testStack.undo();
    testStack.undo();
    testStack.redo();
    testStack.redo();

	return 0;
}
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

#include "MEM_guardedalloc.h"


#include "KX_BlenderSceneConverter.h"
#include "KX_ConvertControllers.h"
#include "KX_Python.h"

// Controller
#include "SCA_ANDController.h"
#include "SCA_ORController.h"
#include "SCA_PythonController.h"
#include "SCA_ExpressionController.h"

#include "SCA_LogicManager.h"
#include "KX_GameObject.h"
#include "IntValue.h"

/* This little block needed for linking to Blender... */
#ifdef WIN32
#include "BLI_winstuff.h"
#endif

#include "DNA_object_types.h"
#include "DNA_controller_types.h"
#include "DNA_text_types.h"

#include "BKE_text.h"

#include "BLI_blenlib.h"

/* end of blender include block */


	static void
LinkControllerToActuators(
	SCA_IController *game_controller,
	bController* bcontr,	
	SCA_LogicManager* logicmgr,
	KX_BlenderSceneConverter* converter
) {
	// Iterate through the actuators of the game blender
	// controller and find the corresponding ketsji actuator.

	for (int i=0;i<bcontr->totlinks;i++)
	{
		bActuator* bact = (bActuator*) bcontr->links[i];
		SCA_IActuator *game_actuator = converter->FindGameActuator(bact);
		if (game_actuator) {
			logicmgr->RegisterToActuator(game_controller, game_actuator);
		}
	}
}


void BL_ConvertControllers(
	struct Object* blenderobject,
	class KX_GameObject* gameobj,
	SCA_LogicManager* logicmgr, 
	PyObject* pythondictionary,
	int &executePriority,
	int activeLayerBitInfo,
	bool isInActiveLayer,
	KX_BlenderSceneConverter* converter
) {
	int uniqueint=0;
	bController* bcontr = (bController*)blenderobject->controllers.first;
	while (bcontr)
	{
		SCA_IController* gamecontroller = NULL;
		switch(bcontr->type)
		{
			case CONT_LOGIC_AND:
			{
				gamecontroller = new SCA_ANDController(gameobj);
				LinkControllerToActuators(gamecontroller,bcontr,logicmgr,converter);
				break;
			}
			case CONT_LOGIC_OR:
			{
				gamecontroller = new SCA_ORController(gameobj);
				LinkControllerToActuators(gamecontroller,bcontr,logicmgr,converter);
				break;
			}
			case CONT_EXPRESSION:
			{
				bExpressionCont* bexpcont = (bExpressionCont*) bcontr->data;
				STR_String expressiontext = STR_String(bexpcont->str);
				if (expressiontext.Length() > 0)
				{
					gamecontroller = new SCA_ExpressionController(gameobj,expressiontext);
					LinkControllerToActuators(gamecontroller,bcontr,logicmgr,converter);

				}
				break;
			}
			case CONT_PYTHON:
			{
					
				// we should create a Python controller here
							
				SCA_PythonController* pyctrl = new SCA_PythonController(gameobj);
				gamecontroller = pyctrl;
					
				bPythonCont* pycont = (bPythonCont*) bcontr->data;
				pyctrl->SetDictionary(pythondictionary);
					
				if (pycont->text)
				{
					char *buf;
					// this is some blender specific code
					buf= txt_to_buf(pycont->text);
					if (buf)
					{
						pyctrl->SetScriptText(STR_String(buf));
						pyctrl->SetScriptName(pycont->text->id.name+2);
						MEM_freeN(buf);
					}
					
				}
					
				LinkControllerToActuators(gamecontroller,bcontr,logicmgr,converter);
				break;
			}
			default:
			{
				
			}
		}

		if (gamecontroller)
		{
			gamecontroller->SetExecutePriority(executePriority++);
			STR_String uniquename = bcontr->name;
			uniquename += "#CONTR#";
			uniqueint++;
			CIntValue* uniqueval = new CIntValue(uniqueint);
			uniquename += uniqueval->GetText();
			uniqueval->Release();
			gamecontroller->SetName(uniquename);
			gameobj->AddController(gamecontroller);
			
			converter->RegisterGameController(gamecontroller, bcontr);
		}
		
		bcontr = bcontr->next;
	}

}

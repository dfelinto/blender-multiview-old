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

#include "KX_ConvertProperties.h"


/* This little block needed for linking to Blender... */
#ifdef WIN32
#include "BLI_winstuff.h"
#endif

#include "DNA_object_types.h"
#include "DNA_property_types.h"
/* end of blender include block */

#include "Value.h"
#include "VectorValue.h"
#include "BoolValue.h"
#include "StringValue.h"
#include "FloatValue.h"
#include "KX_GameObject.h"
//#include "ListValue.h"
#include "IntValue.h"
#include "SCA_TimeEventManager.h"
#include "SCA_IScene.h"


void BL_ConvertProperties(Object* object,KX_GameObject* gameobj,SCA_TimeEventManager* timemgr,SCA_IScene* scene, bool isInActiveLayer)
{
	
	bProperty* prop = (bProperty*)object->prop.first;
	CValue* propval;	
	bool show_debug_info;
	while(prop)
	{
	
		propval = NULL;
		show_debug_info = bool (prop->flag & PROP_DEBUG);

		switch(prop->type) {
		case PROP_BOOL:
		{
			propval = new CBoolValue((bool)(prop->data != 0));
			gameobj->SetProperty(prop->name,propval);
			//promp->poin= &prop->data;
			break;
		}
		case PROP_INT:
		{
			propval = new CIntValue((int)prop->data);
			gameobj->SetProperty(prop->name,propval);
			break;
		}
		case PROP_FLOAT:
		{
			//prop->poin= &prop->data;
			float floatprop = *((float*)&prop->data);
			propval = new CFloatValue(floatprop);
			gameobj->SetProperty(prop->name,propval);
		}
		break;
		case PROP_STRING:
		{
			//prop->poin= callocN(MAX_PROPSTRING, "property string");
			propval = new CStringValue((char*)prop->poin,"");
			gameobj->SetProperty(prop->name,propval);
			break;
		}
		case PROP_TIME:
		{
			float floatprop = *((float*)&prop->data);

			CValue* timeval = new CFloatValue(floatprop);
			// set a subproperty called 'timer' so that 
			// we can register the replica of this property 
			// at the time a game object is replicated (AddObjectActuator triggers this)

			timeval->SetProperty("timer",new CBoolValue(true));
			if (isInActiveLayer)
			{
				timemgr->AddTimeProperty(timeval);
			}
			
			propval = timeval;
			gameobj->SetProperty(prop->name,timeval);

		}
		default:
		{
			// todo make an assert etc.
		}
		}
		
		if (propval)
		{
			if (show_debug_info)
			{
				scene->AddDebugProperty(gameobj,STR_String(prop->name));
			}
		}

		prop = prop->next;
	}

	
}

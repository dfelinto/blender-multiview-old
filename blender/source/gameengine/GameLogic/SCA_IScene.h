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
#ifndef __KX_ISCENE_H
#define __KX_ISCENE_H

#include <vector>

#include "STR_String.h"

struct SCA_DebugProp
{
	class CValue*	m_obj;
	STR_String 		m_name;
};

class SCA_IScene
{
	std::vector<SCA_DebugProp*> m_debugList;
public:
	SCA_IScene();
	virtual ~SCA_IScene();
	virtual class SCA_IObject* AddReplicaObject(class CValue* gameobj,
												class CValue* locationobj,
												int lifespan=0)=0;
	virtual void	RemoveObject(class CValue* gameobj)=0;
	virtual void	DelayedRemoveObject(class CValue* gameobj)=0;
	virtual void	ReplaceMesh(class CValue* gameobj,
								void* meshobj)=0;
	std::vector<SCA_DebugProp*>& GetDebugProperties();
	void			AddDebugProperty(class CValue* debugprop,
									 const STR_String &name);
};
#endif //__KX_ISCENE_H

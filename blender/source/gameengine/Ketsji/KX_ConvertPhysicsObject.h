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
#ifndef KX_CONVERTPHYSICSOBJECTS
#define KX_CONVERTPHYSICSOBJECTS




//#define USE_SUMO_SOLID
//solid is not available yet

//#define USE_ODE
//ode is not available yet


class RAS_MeshObject;
class KX_Scene;


struct KX_Bounds
{
	float m_center[3];
	float m_extends[3];
};

struct KX_ObjectProperties
{
	bool	m_dyna;
	double m_radius;
	bool	m_angular_rigidbody;
	bool	m_in_active_layer;
	bool	m_ghost;
	class KX_GameObject*	m_dynamic_parent;
	bool	m_isactor;
	bool	m_concave;
	bool	m_isdeformable;
	bool	m_implicitsphere ;
	bool	m_implicitbox;
	KX_Bounds	m_boundingbox;
};

#ifdef USE_ODE


void	KX_ConvertODEEngineObject(KX_GameObject* gameobj,
							 RAS_MeshObject* meshobj,
							 KX_Scene* kxscene,
							struct	PHY_ShapeProps* shapeprops,
							struct	PHY_MaterialProps*	smmaterial,
							struct	KX_ObjectProperties*	objprop);


#endif //USE_ODE


void	KX_ConvertDynamoObject(KX_GameObject* gameobj,
							 RAS_MeshObject* meshobj,
							 KX_Scene* kxscene,
							struct	PHY_ShapeProps* shapeprops,
							struct	PHY_MaterialProps*	smmaterial,
							struct	KX_ObjectProperties*	objprop);



#ifdef USE_SUMO_SOLID





void	KX_ConvertSumoObject(	class	KX_GameObject* gameobj,
							class	RAS_MeshObject* meshobj,
							class	KX_Scene* kxscene,
							struct	PHY_ShapeProps* shapeprops,
							struct	PHY_MaterialProps*	smmaterial,
							struct	KX_ObjectProperties*	objprop);
#endif



#endif //KX_CONVERTPHYSICSOBJECTS

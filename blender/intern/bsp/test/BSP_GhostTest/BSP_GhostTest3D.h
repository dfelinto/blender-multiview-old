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

#ifndef BSP_GhostTest_h

#define BSP_GhostTest_h



#include "GHOST_IEventConsumer.h"
#include "MT_Vector3.h"
#include "BSP_TMesh.h"
#include "BSP_MeshDrawer.h"

#include <vector>

class GHOST_IWindow;
class GHOST_ISystem;


class BSP_GhostTestApp3D :
public GHOST_IEventConsumer
{
public :
	// Construct an instance of the application;

	BSP_GhostTestApp3D(
	);

	// initialize the applicaton

		bool
	InitApp(
	);

	// Run the application untill internal return.
		void
	Run(
	);
	
	~BSP_GhostTestApp3D(
	);

		void
	SetMesh(
		MEM_SmartPtr<BSP_TMesh> mesh
	);

private :
	
	struct BSP_RotationSetting {
		MT_Scalar m_angle_x;
		MT_Scalar m_angle_y;
		int x_old;
		int y_old;
		bool m_moving;
	};

	struct BSP_TranslationSetting {
		MT_Scalar m_t_x;
		MT_Scalar m_t_y;
		MT_Scalar m_t_z;
		int x_old;
		int y_old;
		bool m_moving;
	};

	// Return the transform of object i

		MT_Transform
	GetTransform(
		int active_object
	);

	// Perform an operation between the first two objects in the
	// list
	
		void
	Operate(
		int type
	);
	
	// Swap mesh i and settings with the last mesh in list.

		void
	Swap(
		int i
	);

		void
	DrawPolies(
	);

		void
	UpdateFrame(
	);
	
		MT_Vector3
	UnProject(
		const MT_Vector3 & vec
	);

	// Create a frustum and projection matrix to
	// look at the bounding box 

		void
	InitOpenGl(
		const MT_Vector3 &min,
		const MT_Vector3 &max
	);


	// inherited from GHOST_IEventConsumer
		bool 
	processEvent(
		GHOST_IEvent* event
	);

	GHOST_IWindow *m_window;
	GHOST_ISystem *m_system;

	bool m_finish_me_off;

	// List of current meshes.
	std::vector< MEM_SmartPtr<BSP_TMesh> > m_meshes;

	std::vector< BSP_RotationSetting> m_rotation_settings;
	std::vector< BSP_TranslationSetting> m_translation_settings;
	std::vector< MT_Scalar> m_scale_settings;
	std::vector< int> m_render_modes;

	int m_current_object;


};

#endif
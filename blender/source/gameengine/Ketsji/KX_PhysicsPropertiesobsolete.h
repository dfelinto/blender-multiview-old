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
#ifndef KX_PROPSH
#define KX_PROPSH

#include <MT_Scalar.h>

// Properties of dynamic objects
struct KX_ShapeProps {
	MT_Scalar  m_mass;                  // Total mass
	MT_Scalar  m_inertia;               // Inertia, should be a tensor some time 
	MT_Scalar  m_lin_drag;              // Linear drag (air, water) 0 = concrete, 1 = vacuum 
	MT_Scalar  m_ang_drag;              // Angular drag
	MT_Scalar  m_friction_scaling[3];   // Scaling for anisotropic friction. Component in range [0, 1]   
	bool       m_do_anisotropic;        // Should I do anisotropic friction? 
	bool       m_do_fh;                 // Should the object have a linear Fh spring?
	bool       m_do_rot_fh;             // Should the object have an angular Fh spring?
};


// Properties of collidable objects (non-ghost objects)
struct KX_MaterialProps {
	MT_Scalar m_restitution;           // restitution of energie after a collision 0 = inelastic, 1 = elastic
	MT_Scalar m_friction;              // Coulomb friction (= ratio between the normal en maximum friction force)
	MT_Scalar m_fh_spring;             // Spring constant (both linear and angular)
	MT_Scalar m_fh_damping;            // Damping factor (linear and angular) in range [0, 1]
	MT_Scalar m_fh_distance;           // The range above the surface where Fh is active.    
	bool      m_fh_normal;             // Should the object slide off slopes?
};

#endif //KX_PROPSH

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
#ifndef __PHY_DYNAMIC_TYPES
#define __PHY_DYNAMIC_TYPES

/// PHY_ScalarType enumerates possible scalar types.
/// See the PHY_IMeshInterface for its use
typedef enum PHY_ScalarType {
	PHY_FLOAT,
	PHY_DOUBLE,
	PHY_INTEGER,
	PHY_SHORT,
	PHY_FIXEDPOINT88
} PHY_ScalarType;

/// PHY_PhysicsType enumerates all possible Physics Entities.
/// It is mainly used to create/add Physics Objects

typedef enum PHY_PhysicsType {
	PHY_CONVEX_RIGIDBODY=16386,
	PHY_CONCAVE_RIGIDBODY=16399,
	PHY_CONVEX_FIXEDBODY=16388,//'collision object'
	PHY_CONCAVE_FIXEDBODY=16401,
	PHY_CONVEX_KINEMATICBODY=16387,// 
	PHY_CONCAVE_KINEMATICBODY=16400,
	PHY_CONVEX_PHANTOMBODY=16398,
	PHY_CONCAVE_PHANTOMBODY=16402
} PHY_PhysicsType;

/// PHY_ConstraintType enumerates all supported Constraint Types
typedef enum PHY_ConstraintType {
	PHY_POINT2POINT_CONSTRAINT=1,
	PHY_LINEHINGE_CONSTRAINT,

} PHY_ConstraintType;

typedef float	PHY_Vector3[3];

#endif //__PHY_DYNAMIC_TYPES

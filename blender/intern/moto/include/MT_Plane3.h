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

#ifndef MT_PLANE3

#define MT_PLANE3

#include "MT_Tuple4.h"
#include "MT_Point3.h"

/**
 * A simple 3d plane class.
 *
 * This class represents a plane in 3d. The internal parameterization used
 * is n.x + d =0 where n is a unit vector and d is a scalar.
 *
 * It inherits data from MT_Tuple4 please see this class for low level
 * access to the internal representation.
 * 
 */

class MT_Plane3 : public MT_Tuple4
{
public :
	/**
	 * Constructor from 3 points
	 */

	MT_Plane3(
		const MT_Vector3 &a,
		const MT_Vector3 &b,
		const MT_Vector3 &c
	);
	/**
	 * Construction from vector and a point.
	 */

	MT_Plane3(
		const MT_Vector3 &n,
		const MT_Vector3 &p
	);

	/**
	 * Default constructor
	 */
	MT_Plane3(
	);

	/**
	 * Default constructor
	 */

	MT_Plane3(
		const MT_Plane3 & p
	):
		MT_Tuple4(p)
	{
	}

	/**
	 * Return plane normal
	 */
	
		MT_Vector3
	Normal(
	) const;

	/**
	 * Return plane scalar i.e the d from n.x + d = 0
	 */

		MT_Scalar
	Scalar(
	) const ; 

	/**
	 * Invert the plane - just swaps direction of normal.
	 */
		void
	Invert(
	);
	
	/**
	 * Assignment operator
	 */

		MT_Plane3 &
	operator = (
		const MT_Plane3 & rhs
	);

	/**
	 * Return the signed perpendicular distance from a point to the plane
	 */

		MT_Scalar
	signedDistance(
		const MT_Vector3 &
	) const;
		
	
};

#ifdef GEN_INLINED
#include "MT_Plane3.inl"
#endif


#endif



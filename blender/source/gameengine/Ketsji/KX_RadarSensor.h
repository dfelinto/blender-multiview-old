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
#ifndef __KX_RADAR_SENSOR_H
#define __KX_RADAR_SENSOR_H

#include "KX_NearSensor.h"
#include "MT_Point3.h"

/**
* Radar 'cone' sensor. Very similar to a near-sensor, but instead of a sphere, a cone is used.
*/
class KX_RadarSensor : public KX_NearSensor
{
 protected:
	Py_Header;
		
	MT_Scalar		m_coneradius;

	/**
	 * Height of the cone.
	 */
	MT_Scalar		m_coneheight;
	int				m_axis;

	/**
	 * The previous position of the origin of the cone.
	 */
	MT_Point3       m_cone_origin;

	/**
	 * The previous direction of the cone (origin to bottom plane).
	 */
	MT_Point3       m_cone_target;
	
public:
	KX_RadarSensor(class SCA_EventManager* eventmgr,
		class KX_GameObject* gameobj,
			double coneradius,
			double coneheight,
			int	axis,
			double margin,
			double resetmargin,
			class SM_Object* sumoObj,
			bool bFindMaterial,
			const STR_String& touchedpropname,
			class SM_Scene* sumoscene,
			PyTypeObject* T=&Type);
	KX_RadarSensor();
	virtual ~KX_RadarSensor();
	virtual void SynchronizeTransform();

	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */
	
	virtual PyObject*  _getattr(char *attr);

	KX_PYMETHOD_DOC(KX_RadarSensor,GetConeOrigin);
	KX_PYMETHOD_DOC(KX_RadarSensor,GetConeTarget);
	KX_PYMETHOD_DOC(KX_RadarSensor,GetConeHeight);

};

#endif //__KX_RADAR_SENSOR_H

/*
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
 * Camera in the gameengine. Cameras are also used for views.
 */

#ifndef __KX_CAMERA
#define __KX_CAMERA


#include "MT_Transform.h"
#include "MT_Matrix3x3.h"
#include "MT_Matrix4x4.h"
#include "MT_Vector3.h"
#include "MT_Point3.h"
#include "KX_GameObject.h"
#include "IntValue.h"
#include "RAS_CameraData.h"


class KX_Camera : public KX_GameObject
{

	/** Camera parameters (clips distances, focal lenght). These
	 * params are closely tied to BLender. In the gameengine, only the
	 * projection and modelview matrices are relevant. There's a
	 * conversion being done in the engine class. Why is it stored
	 * here? It doesn't really have a function here. */
	RAS_CameraData	m_camdata;

	// Never used, I think...
//	void MoveTo(const MT_Point3& movevec)
//	{
		/*MT_Transform camtrans;
		camtrans.invert(m_trans1);
		MT_Matrix3x3 camorient = camtrans.getBasis();
		camtrans.translate(camorient.inverse()*movevec);
		m_trans1.invert(camtrans);
		*/
//	}

	/**
	 * Storage for the projection matrix that is passed to the
	 * rasterizer. */
	MT_Matrix4x4 m_projection_matrix;

	/**
	 * Storage for the modelview matrix that is passed to the
	 * rasterizer. */
	MT_Matrix4x4 m_modelview_matrix;

public:

	KX_Camera(void* sgReplicationInfo,SG_Callbacks callbacks,const RAS_CameraData& camdata);
	virtual ~KX_Camera();
	
	MT_Transform		GetWorldToCamera() const;
	MT_Transform		GetCameraToWorld() const;

	void				CorrectLookUp(MT_Scalar speed);
	const MT_Point3		GetCameraLocation();

	/* I want the camera orientation as well. */
	const MT_Quaternion GetCameraOrientation();
		
	/** Sets the projection matrix that is used by the rasterizer. */
	void				SetProjectionMatrix(const MT_Matrix4x4 & mat);

	/** Sets the modelview matrix that is used by the rasterizer. */
	void				SetModelviewMatrix(const MT_Matrix4x4 & mat);
		
	/** Gets the projection matrix that is used by the rasterizer. */
	void				GetProjectionMatrix(MT_Matrix4x4 & mat);
	
	/** Gets the modelview matrix that is used by the rasterizer. */
	void				GetModelviewMatrix(MT_Matrix4x4 & mat);

	/** Gets the focal lenght. */
	float				GetLens();
	/** Gets the near clip distance. */
	float				GetCameraNear();
	/** Gets the far clip distance. */
	float				GetCameraFar();
	/** Gets all camera data. */
	RAS_CameraData*		GetCameraData();
};
#endif //__KX_CAMERA

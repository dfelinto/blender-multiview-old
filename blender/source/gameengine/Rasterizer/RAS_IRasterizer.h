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
#ifndef __RAS_IRASTERIZER
#define __RAS_IRASTERIZER

#ifdef WIN32
#pragma warning (disable:4786)
#endif

#include "MT_CmMatrix4x4.h"
#include "MT_Matrix4x4.h"

class RAS_ICanvas;
class RAS_IPolyMaterial;
#include "RAS_MaterialBucket.h"

/**
 * 3D rendering device context interface. 
 */

class RAS_IRasterizer
{

public:

	RAS_IRasterizer(RAS_ICanvas* canv){};
	virtual ~RAS_IRasterizer(){};
	enum	{
			RAS_RENDER_3DPOLYGON_TEXT = 16384
	};
	enum	{
			KX_BOUNDINGBOX = 1,
			KX_WIREFRAME,
			KX_SOLID,
			KX_SHADED,
			KX_TEXTURED 
	};

	enum	{
			KX_DEPTHMASK_ENABLED =1,
			KX_DEPTHMASK_DISABLED,
	};

	enum	{
			RAS_STEREO_NOSTEREO = 1,
			RAS_STEREO_QUADBUFFERED,
			RAS_STEREO_ABOVEBELOW,
			RAS_STEREO_INTERLACED,
	};
	enum	{
			RAS_STEREO_LEFTEYE = 1,
			RAS_STEREO_RIGHTEYE,
	};

	virtual void	SetDepthMask(int depthmask)=0;
	virtual void	SetMaterial(const RAS_IPolyMaterial& mat)=0;
	virtual bool	Init()=0;
	virtual void	Exit()=0;
	virtual bool	BeginFrame(int drawingmode, double time)=0;
	virtual void	ClearDepthBuffer()=0;
	virtual void	ClearCachingInfo(void)=0;
	virtual void	EndFrame()=0;
	virtual void	SetRenderArea()=0;

	virtual void	SetStereoMode(const int stereomode)=0;
	virtual bool	Stereo()=0;
	virtual void	SetEye(const int eye)=0;
	virtual void	SetEyeSeparation(const float eyeseparation)=0;
	virtual void	SetFocalLength(const float focallength)=0;

	virtual void	SwapBuffers()=0;
	virtual void	IndexPrimitives( const vecVertexArray& vertexarrays,
							const vecIndexArrays & indexarrays,
							int mode,
							class RAS_IPolyMaterial* polymat,
							class RAS_IRenderTools* rendertools,
							bool useObjectColor,
							const MT_Vector4& rgbacolor)=0;
	virtual void	IndexPrimitives_Ex( const vecVertexArray& vertexarrays,
							const vecIndexArrays & indexarrays,
							int mode,
							class RAS_IPolyMaterial* polymat,
							class RAS_IRenderTools* rendertools,
							bool useObjectColor,
							const MT_Vector4& rgbacolor)=0;
	virtual void	IndexPrimitives_3DText( const vecVertexArray& vertexarrays,
							const vecIndexArrays & indexarrays,
							int mode,
							class RAS_IPolyMaterial* polymat,
							class RAS_IRenderTools* rendertools,
							bool useObjectColor,
							const MT_Vector4& rgbacolor)=0;

	virtual void	SetProjectionMatrix(MT_CmMatrix4x4 & mat)=0;
	/* This one should become our final version, methinks. */
	/**
	 * Set the projection matrix for the rasterizer. This projects
	 * from camera coordinates to window coordinates.
	 * @param mat The projection matrix.
	 */
	virtual void	SetProjectionMatrix(MT_Matrix4x4 & mat)=0;
	virtual void	SetViewMatrix(const MT_Matrix4x4 & mat,
						const MT_Vector3& campos,
						const MT_Point3 &camLoc,
						const MT_Quaternion &camOrientQuat)=0;
	virtual const	MT_Point3& GetCameraPosition()=0;
	virtual void	LoadViewMatrix()=0;
	
	virtual void	SetFog(float start,
						   float dist,
						   float r,
						   float g,
						   float b)=0;
	
	virtual void	SetFogColor(float r,
								float g,
								float b)=0;

	virtual void	SetFogStart(float start)=0;
	virtual void	SetFogEnd(float end)=0;

	virtual void	DisplayFog()=0;
	virtual void	DisableFog()=0;

	virtual void	SetBackColor(float red,
								 float green,
								 float blue,
								 float alpha)=0;
	
	virtual void	SetDrawingMode(int drawingmode)=0;
	virtual int		GetDrawingMode()=0;

	virtual void	EnableTextures(bool enable)=0;
	
	virtual void	SetCullFace(bool enable)=0;

	virtual double	GetTime()=0;

	virtual MT_Matrix4x4 GetFrustumMatrix(
		float left,
		float right,
		float bottom,
		float top,
		float frustnear,
		float frustfar
	)=0;

	virtual void	SetSpecularity(float specX,
								   float specY,
								   float specZ,
								   float specval)=0;

	virtual void	SetShinyness(float shiny)=0;
	virtual void	SetDiffuse(float difX,
							   float difY,
							   float difZ,
							   float diffuse)=0;
	
};

#endif //__RAS_IRASTERIZER


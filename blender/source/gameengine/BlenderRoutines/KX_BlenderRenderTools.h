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
#ifndef __KX_BLENDERRENDERTOOLS
#define __KX_BLENDERRENDERTOOLS

#ifdef WIN32
// don't show stl-warnings
#pragma warning (disable:4786)
#endif

#include "RAS_IRenderTools.h"

/**
BlenderRenderTools are a set of tools to apply 2D/3D graphics effects, which are not
part of the (polygon) Rasterizer. 
Effects like 2D text, 3D (polygon) text, lighting.
*/

class KX_BlenderRenderTools  : public RAS_IRenderTools
{
	bool	m_lastblenderlights;
	void*	m_lastblenderobject;
	int		m_lastlayer;
	bool	m_lastlighting;

public:
	
						KX_BlenderRenderTools();
	virtual				~KX_BlenderRenderTools();	

	virtual void		EndFrame(class RAS_IRasterizer* rasty);
	virtual void		BeginFrame(class RAS_IRasterizer* rasty);
	void				DisableOpenGLLights();
	void				EnableOpenGLLights();
	int					ProcessLighting(int layer);

	virtual void	    RenderText2D(RAS_TEXT_RENDER_MODE mode,
									 const char* text,
									 int xco,
									 int yco,
									 int width,
									 int height);
	virtual void		RenderText(int mode,
								   class RAS_IPolyMaterial* polymat,
								   float v1[3],
								   float v2[3],
								   float v3[3],
								   float v4[3]);
	void				applyTransform(class RAS_IRasterizer* rasty,
									   double* oglmatrix,
									   int objectdrawmode );
	int					applyLights(int objectlayer);
	virtual void		PushMatrix();
	virtual void		PopMatrix();

	virtual class RAS_IPolyMaterial* CreateBlenderPolyMaterial(const STR_String &texname,
															   bool ba,
															   const STR_String& matname,
															   int tile,
															   int tilexrep,
															   int tileyrep,
															   int mode,
															   int transparant,
															   int lightlayer,
															   bool bIsTriangle,
															   void* clientobject,
															   void* tface);
};


#endif //__KX_BLENDERRENDERTOOLS

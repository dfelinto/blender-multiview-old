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
#ifndef __KX_BLENDERCANVAS
#define __KX_BLENDERCANVAS

#ifdef WIN32
#include <windows.h>
#endif 
#include <GL/gl.h>

#include "RAS_ICanvas.h"
#include "RAS_Rect.h"

#include "KX_BlenderGL.h"

struct ScrArea;

/**
 * 2D Blender device context abstraction. 
 * The connection from 3d rendercontext to 2d Blender surface embedding.
 */

class KX_BlenderCanvas : public RAS_ICanvas
{
private:
	/** Rect that defines the area used for rendering,
	    relative to the context */
	RAS_Rect m_displayarea;

public:
	/* Construct a new canvas.
	 * 
	 * @param area The Blender ScrArea to run the game within.
	 */
	KX_BlenderCanvas(struct ScrArea* area);
	~KX_BlenderCanvas();

		void 
	Init(
	);
	
		void 
	SwapBuffers(
	);
		void 
	Resize(
		int width,
		int height
	);

		void
	BeginFrame(
	);

		void 
	EndFrame(
	);

		void 
	ClearColor(
		float r,
		float g,
		float b,
		float a
	);

		void 
	ClearBuffer(
		int type
	);

		int 
	GetWidth(
	) const ;

		int 
	GetHeight(
	) const ;

	const
		RAS_Rect &
	GetDisplayArea(
	) const {
		return m_displayarea;
	};

		RAS_Rect &
	GetDisplayArea(
	) {
		return m_displayarea;
	};

		void
	SetViewPort(
		int x1, int y1,
		int x2, int y2
	);

		void 
	SetMouseState(
		RAS_MouseState mousestate
	);

		void 
	SetMousePosition(
		int x,
		int y
	);

		void 
	MakeScreenShot(
		const char* filename
	);
	
	/**
	 * Nothing needs be done for BlenderCanvas
	 * Begin/End Draw, as the game engine GL context
	 * is always current/active.
	 */

		bool 
	BeginDraw(
	) {
			return true;
	};

		void 
	EndDraw(
	) {
	};

private:
	/** Blender area the game engine is running within */
	struct ScrArea* m_area;
};
#endif // __KX_BLENDERCANVAS

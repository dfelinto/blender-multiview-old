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

#include "BIF_scrarea.h"


#include "KX_BlenderCanvas.h"


KX_BlenderCanvas::KX_BlenderCanvas(struct ScrArea *area) :
m_area(area)
{
}

KX_BlenderCanvas::~KX_BlenderCanvas()
{
}

void KX_BlenderCanvas::Init()
{
	glDepthFunc(GL_LEQUAL);
}	


void KX_BlenderCanvas::SwapBuffers()
{
	BL_SwapBuffers();
}

void KX_BlenderCanvas::BeginFrame()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

}


void KX_BlenderCanvas::EndFrame()
{
		// this is needed, else blender distorts a lot
	glPopAttrib();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
		
	glDisable(GL_FOG);
}



void KX_BlenderCanvas::ClearColor(float r,float g,float b,float a)
{
	glClearColor(r,g,b,a);
}



void KX_BlenderCanvas::ClearBuffer(int type)
{
	int ogltype = 0;

	if (type & RAS_ICanvas::COLOR_BUFFER )
		ogltype |= GL_COLOR_BUFFER_BIT;

	if (type & RAS_ICanvas::DEPTH_BUFFER )
		ogltype |= GL_DEPTH_BUFFER_BIT;
	glClear(ogltype);
}

int KX_BlenderCanvas::GetWidth(
) const {
	return scrarea_get_win_width(m_area);
}

int KX_BlenderCanvas::GetHeight(
) const {
	return scrarea_get_win_height(m_area);
}

	void
KX_BlenderCanvas::
SetViewPort(
	int x1, int y1,
	int x2, int y2
){
	int vp_width = (x2 - x1) + 1;
	int vp_height = (y2 - y1) + 1;
	int minx = scrarea_get_win_x(m_area);
	int miny = scrarea_get_win_y(m_area);

	glViewport(minx + x1, miny + y1, vp_width, vp_height);
	glScissor(minx + x1, miny + y1, vp_width, vp_height);
}


void KX_BlenderCanvas::SetMouseState(RAS_MouseState mousestate)
{
	switch (mousestate)
	{
	case MOUSE_INVISIBLE:
		{
			BL_HideMouse();
			break;
		}
	case MOUSE_WAIT:
		{
			BL_WaitMouse();
			break;
		}
	case MOUSE_NORMAL:
		{
			BL_NormalMouse();
			break;
		}
	default:
		{
		}
	}
}



//	(0,0) is top left, (width,height) is bottom right
void KX_BlenderCanvas::SetMousePosition(int x,int y)
{
	int winX = scrarea_get_win_x(m_area);
	int winY = scrarea_get_win_y(m_area);
	int winH = scrarea_get_win_height(m_area);
	
	BL_warp_pointer(winX + x, winY + (winH-y-1));
}



void KX_BlenderCanvas::MakeScreenShot(const char* filename)
{
	BL_MakeScreenShot(m_area, filename);
}

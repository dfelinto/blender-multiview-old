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
   
//#include <iostream>

#include "GPU_Canvas.h"

GPU_Canvas::GPU_Canvas(KXH_plugin_handle p, int width, int height)
	: GPC_Canvas(width, height), m_plugin(p)
{
	/* intentionally empty */
}


GPU_Canvas::~GPU_Canvas(void)
{
	/* intentionally empty */
}

void GPU_Canvas::Init(void)
{
	/* intentionally empty */
}

void GPU_Canvas::SwapBuffers(void)
{
	if (m_plugin) KXH_swap_buffers(m_plugin);
}

bool 
GPU_Canvas::BeginDraw(void) 
{
	if (m_plugin) {
		return KXH_begin_draw(m_plugin);
	} else {
		return false;
	}
}

void GPU_Canvas::EndDraw(void) 
{
	if (m_plugin) KXH_end_draw(m_plugin);
}

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

#ifndef __GPU_POLYGONMATERIAL_H
#define __GPU_POLYGONMATERIAL_H


#include "BP_PolygonMaterial.h"


class GPU_PolygonMaterial : public BP_PolygonMaterial
{
public:
	GPUPolygonMaterial(const STR_String& texname, bool ba,const STR_String& matname,
			int tile, int tileXrep, int tileYrep, int mode, int transparant,
			int lightlayer,bool bIsTriangle,void* clientobject,void* tpage) :
			BP_PolygonMaterial(texname, ba,matname, tile, tileXrep, tileYrep,
					mode, transparant, lightlayer, bIsTriangle, clientobject),
			m_tface(tpage)
	{
	}
    
	virtual ~GPU_PolygonMaterial()
	{
	}
};

#endif  // __GPU_POLYGONMATERIAL_H

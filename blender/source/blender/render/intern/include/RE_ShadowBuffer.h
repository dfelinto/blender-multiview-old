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

#ifndef RE_SHADOWBUFFER_H
#define RE_SHADOWBUFFER_H

#include <iostream.h>

struct ShadBuf;
struct LampRen;

class RE_ShadowBuffer {

 public:

	virtual ~RE_ShadowBuffer(void){};
	/**
	 * Place this scene in the buffer
	 */
	virtual void importScene(struct LampRen* lar) = 0;

	/**
	 * Test the shadow factor at a location in the buffer
	 * @param shadowResult a vector of 3 floats with rgb shadow values
	 */
	virtual void readShadowValue(struct ShadBuf *shb,
								 float inp,
								 float* shadowResult) = 0;
	
};

#endif /*  RE_SHADOWBUFFER_H */

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

#ifndef RE_DUMMYSHADOWBUFFER_H
#define RE_DUMMYSHADOWBUFFER_H

#include "RE_ShadowBuffer.h"

struct LampRen;

class RE_DummyShadowBuffer : public RE_ShadowBuffer {

 public:
	/**
	 * Make an empty shadow buffer
	 */
	RE_DummyShadowBuffer(void);

	/**
	 * Delete and clear this buffer
	 */
	virtual ~RE_DummyShadowBuffer(void);
	
	/**
	 * Place this scene in the buffer
	 */
	virtual void importScene(struct LampRen* lar);

	/**
	 * Always return a fixed shadow factor.
	 * @param inp ignored
	 * @param shb ignored
	 * @param shadowResult a vector of 3 floats with rgb shadow values
	 */
	virtual void readShadowValue(struct ShadBuf *shb,
								 float inp,
								 float* shadowResult);
	
};

#endif /*  RE_SHADOWBUFFER_H */

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

#ifndef RE_BASICSHADOWBUFFER_H
#define RE_BASICSHADOWBUFFER_H

#include "RE_ShadowBuffer.h"

struct LampRen;
struct Lamp;

class RE_BasicShadowBuffer : public RE_ShadowBuffer {
	
 private:
	
	void lrectreadRectz(int x1, int y1, int x2, int y2, char *r1);
	int sizeoflampbuf(struct ShadBuf *shb);
	int firstreadshadbuf(struct ShadBuf *shb, int xs, int ys, int nr);
	float readshadowbuf(struct ShadBuf *shb, int xs, int ys, int zs);
	float readshadowbuf_halo(struct ShadBuf *shb, int xs, int ys, int zs);
	float *give_jitter_tab(int samp);
	
	int bias;
	
 public:
	/**
	 * Make a shadow buffer from these settings
	 */
	RE_BasicShadowBuffer(struct LampRen *lar, float mat[][4]);

	/**
	 * Delete and clear this buffer
	 */
	virtual ~RE_BasicShadowBuffer(void);
	
	/**
	 * Calculates shadowbuffers for a vector of shadow-giving lamps
	 * @param lar The vector of lamps
	 */
	void importScene(LampRen *lar);

	/**
	 * Determines the shadow factor for a face and lamp. There is some
	 * communication with global variables here.
	 * @param shadres The RGB shadow factors: 1.0 for no shadow, 0.0 for complete
	 *                shadow. There must be a float[3] to write the result to.
	 * @param shb The shadowbuffer to find the shadow factor in.
	 * @param inp The inproduct between viewvector and ?
	 *
	 */
	virtual void readShadowValue(struct ShadBuf *shb,
								 float inp,
								 float* shadowResult);

	/**
	 * Determines the shadow factor for lamp <lar>, between <p1>
	 * and <p2>. (Which CS?)
	 */
	float shadow_halo(LampRen *lar, float *p1, float *p2);

};

#endif /* RE_BASICSHADOWBUFFER_H */



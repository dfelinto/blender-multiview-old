/*
 * shadbuf_ext.h
 *
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

#ifndef SHADBUF_EXT_H
#define SHADBUF_EXT_H

#include "render_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Init memory for old-style shodow buffers. */
void initshadowbuf(struct LampRen *lar, float mat[][4]);


/**
 * Calculates shadowbuffers for a vector of shadow-giving lamps
 * @param lar The vector of lamps
 */
void makeshadowbuf(LampRen *lar);

/**
 * Determines the shadow factor for a face and lamp. There is some
 * communication with global variables here.
 * @returns The shadow factors: 1.0 for no shadow, 0.0 for complete
 *          shadow.
 * @param shb The shadowbuffer to find the shadow factor in.
 * @param inp The inproduct between viewvector and ?
 *
 */
float testshadowbuf(struct ShadBuf *shb, float inp);	

/**
 * Determines the shadow factor for lamp <lar>, between <p1>
 * and <p2>. (Which CS?)
 */
float shadow_halo(LampRen *lar, float *p1, float *p2);

#ifdef __cplusplus
}
#endif

#endif /* SHADBUF_EXT_H */

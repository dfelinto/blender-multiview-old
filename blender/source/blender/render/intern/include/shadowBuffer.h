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

#ifndef SHADOWBUFFER_H
#define SHADOWBUFFER_H

#ifdef __cplusplus
extern "C" { 
#endif

#ifndef RE_SHADOWBUFFERHANDLE
#define RE_SHADOWBUFFERHANDLE
#define RE_DECLARE_HANDLE(name) typedef struct name##__ { int unused; } *name
RE_DECLARE_HANDLE(RE_ShadowBufferHandle);
#endif
	
	struct ShadBuf;
	struct LampRen;
	
/**
 * Calculates shadowbuffers for a vector of shadow-giving lamps
 * @param lar The vector of lamps
 * @returns a handle to the buffer
 */
	extern void RE_buildShadowBuffer(RE_ShadowBufferHandle dsbh,
									 struct LampRen *lar);
	
/**
 * Determines the shadow factor for a face and lamp. There is some
 * communication with global variables here? Should be made explicit...
 * @param shadres The RGB shadow factors: 1.0 for no shadow, 0.0 for complete
 *                shadow. There must be a float[3] to write the result to.
 * @param shb The shadowbuffer to find the shadow factor in.
 * @param inp The inproduct between viewvector and ?
 *
 */
	void RE_testshadowbuf(RE_ShadowBufferHandle dsbh,
						  struct ShadBuf* shbp,
						  float inp,
						  float* shadres);	

/**
 * Determines a shadow factor for halo-shadows.
 */
	
#ifdef __cplusplus
}
#endif

#endif /* SHADOWBUFFER_H */

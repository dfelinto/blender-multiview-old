/*
 * renderhelp_ext.h
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

#ifndef RENDERHELP_EXT_H
#define RENDERHELP_EXT_H 

#ifdef __cplusplus
extern "C" { 
#endif

	/* Push-pop, because this sometimes is necessary... */
	void pushTempPanoPhi(float p);
	void popTempPanoPhi(void);
	
	float getPanoPhi(void);
	float getPanovCo(void);
	float getPanovSi(void);
	void setPanoRot(int part);

	/** Set clip flags on all data entries, using the given projection
	 * function */
	void setzbufvlaggen( void (*projectfunc)(float *, float *) );

/* external for the time being, since the converter calls it. */
/** Recalculate all normals on renderdata. */
/*  	void set_normalflags(void); */

#ifdef __cplusplus
}
#endif

#endif

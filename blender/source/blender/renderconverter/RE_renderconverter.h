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
 * Interface to transform the Blender scene into renderable data.
 *
 * @mainpage RE - Blender RE-converter external interface
 *
 * @section about About the RE-converter module
 *
 * The converter takes a Blender scene, and transforms the data into
 * renderer-specific data.
 *
 * Conversions:
 * 
 * halos: (world settings) stars ->
 *        some particle effects  ->
 *        meshes with halo prop  ->  
 *                                  HaloRen (made by inithalo)
 *
 *
 *                                  VlakRen (face render data)
 * Each vlakren needs several VertRens to make sense.
 *                                  VertRen (vertex render data)
 *
 * @section issues Known issues with RE-converter
 *
 *
 * @section dependencies Dependencies
 *
 *
 * */

#ifndef RE_RENDERCONVERTER_H
#define RE_RENDERCONVERTER_H

#ifdef __cplusplus
extern "C" {
#endif

	struct LampRen;
	struct Object;
	struct Lamp;
	
	/** Transform a blender scene to render data. */
	void RE_rotateBlenderScene(void);

	/** Free all memory used for the conversion. */
	void RE_freeRotateBlenderScene(void);

	/**
	 * Used by the preview renderer.
	 */
	void RE_add_render_lamp(struct Object *ob, int doshadbuf);

	/**
	 * Strange support for star rendering for drawview.c... For
	 * rendering purposes, these function pointers should be NULL.
	 */
	void RE_make_stars(void (*initfunc)(void),
					   void (*vertexfunc)(float*),
					   void (*termfunc)(void));
		
#ifdef __cplusplus
}
#endif
#endif

  

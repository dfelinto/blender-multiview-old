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

#ifndef BDR_DRAWMESH_H
#define BDR_DRAWMESH_H

struct Image;
struct TFace;
struct Object;
struct Mesh;

/**
 * Enables or disable mipmapping for realtime images (textures).
 * Note that this will will destroy all texture bindings in OpenGL.
 * @see free_realtime_image()
 * @param mipmap Turn mipmapping on (mipmap!=0) or off (mipmap==0).
 */
void set_mipmap(int mipmap);

/**
 * Returns the current setting for mipmapping.
 */
int get_mipmap(void);

/**
 * Enables or disable linear mipmap setting for realtime images (textures).
 * Note that this will will destroy all texture bindings in OpenGL.
 * @see free_realtime_image()
 * @param mipmap Turn linear mipmapping on (linear!=0) or off (linear==0).
 */
void set_linear_mipmap(int linear);

/**
 * Returns the current setting for linear mipmapping.
 */
int get_linear_mipmap(void);

/**
 * Resets the realtime image cache variables.
 */
void clear_realtime_image_cache();

void free_realtime_image(struct Image *ima);
void free_all_realtime_images(void);
void make_repbind(struct Image *ima);
int set_tpage(struct TFace *tface);
void spack(unsigned int ucol);
void draw_tfaces3D(struct Object *ob, struct Mesh *me);
void draw_tface_mesh(struct Object *ob, struct Mesh *me, int dt);
void init_realtime_GL(void); 

#endif /* BDR_DRAWMESH_H */

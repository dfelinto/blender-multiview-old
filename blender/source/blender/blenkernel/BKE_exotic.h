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
 * dxf/vrml/videoscape external file io function prototypes
 */

#ifndef BKE_EXOTIC_H
#define BKE_EXOTIC_H

struct Mesh;

void mcol_to_rgba(unsigned int col, float *r, float *g, float *b, float *a);
unsigned int *mcol_to_vcol(struct Mesh *me); // used in py_main.c

/**
 * Reads all 3D fileformats other than Blender fileformat
 * @retval 0 The file could not be read.
 * @retval 1 The file was read succesfully.
 * @attention Used in filesel.c
 */
int BKE_read_exotic(char *name);

void write_dxf(char *str);
void write_vrml(char *str);
void write_videoscape(char *str);

#endif

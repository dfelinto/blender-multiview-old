/*
 * zbufferdatastruct_types.h
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

#ifndef ZBUFFERDATASTRUCT_TYPES_H
#define ZBUFFERDATASTRUCT_TYPES_H

#define  RE_ZBUFLEN 64       /* number of lines in the accumulation buffer  */

/**
 * Primitive data structure for zbuffering. One struct 
 * stores data for 4 entries. This struct has been extended
 * for the render pipeline overhaul.
 */
typedef struct RE_APixstrExt {
    unsigned short mask[4]; /* jitter masks                   */
    int zmin[4];    /* min. distance of all samples   */
    int zmax[4];    /* max. distance of all samples   */
    int p[4];       /* index                          */
    int t[4];       /* entry type: ZB_POLY or ZB_HALO */
    struct RE_APixstrExt *next;
} RE_APixstrExt;

/* For now I'll stick to the Blender convention of hand made defines */
/* but this should definitely be done in a better way. An enum may   */
/* be some help, but masking is still a nice feature...              */
/* object types to buffer in the z buffer */
#define RE_NONE 0
#define RE_POLY 1
#define RE_HALO 2
#define RE_SKY  4

/* unique indices for each field */
#define RE_ZMIN           0
#define RE_INDEX          1
#define RE_MASK           2
#define RE_TYPE           3
#define RE_ZMAX           4
#define RE_PIXELFIELDSIZE 5

typedef struct RE_APixstrExtMain
{
    struct RE_APixstrExt *ps;
    struct RE_APixstrExtMain *next;
} RE_APixstrExtMain;

#endif

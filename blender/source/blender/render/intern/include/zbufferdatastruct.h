/*
 * zbufferdatastruct_ext.h
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

#ifndef ZBUFFERDATASTRUCT_EXT_H
#define ZBUFFERDATASTRUCT_EXT_H

#include "zbufferdatastruct_types.h"

/**
 * Set memory and counters for a fresh z buffer
 */
void initZbuffer(int linewidth);

/**
 * Release memory for the current z buffer
 */
void freeZbuffer(void);

/**
 * Release previous buffer and initialise new buffer. 
 */
void resetZbuffer(void);

/**
 * Make a root for a memory block (internal)
 */
RE_APixstrExt  *addpsemainA(void);

/**
 * Release a memory chunk
 */
void            freepseA(void);

/**
 * Add a structure
 */
RE_APixstrExt  *addpseA(void);

/**
 * Add an object to a zbuffer entry.
 */
void insertObject(int teller,
/*  				  int opaque, */
				  int obindex,
				  int obtype, 
				  int dist, 
				  int mask);

/**
 * Add a flat object to a zbuffer entry.
 */
void insertFlatObject(RE_APixstrExt* ap, 
					  int obindex,
					  int obtype, 
					  int dist, 
					  int mask);

/**
 * Add a flat object to a zbuffer entry, but don't do OSA entry testing.
 */
void insertFlatObjectNoOsa(RE_APixstrExt* ap, 
						   int obindex,
						   int obtype, 
						   int dist, 
						   int mask);


#endif

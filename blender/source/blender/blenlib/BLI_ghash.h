/**
 * A general (pointer -> pointer) hash table ADT
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
 
#ifndef BLI_GHASH_H
#define BLI_GHASH_H

struct GHash;
typedef struct GHash GHash;

typedef unsigned int	(*GHashHashFP)		(void *key);
typedef int				(*GHashCmpFP)		(void *a, void *b);
typedef	void			(*GHashKeyFreeFP)	(void *key);
typedef void			(*GHashValFreeFP)	(void *val);

GHash*	BLI_ghash_new		(GHashHashFP hashfp, GHashCmpFP cmpfp);
void	BLI_ghash_free		(GHash *gh, GHashKeyFreeFP keyfreefp, GHashValFreeFP valfreefp);

void	BLI_ghash_insert	(GHash *gh, void *key, void *val);
void*	BLI_ghash_lookup	(GHash *gh, void *key);
int		BLI_ghash_haskey	(GHash *gh, void *key);

unsigned int	BLI_ghashutil_ptrhash	(void *key);
int				BLI_ghashutil_ptrcmp	(void *a, void *b);

unsigned int	BLI_ghashutil_strhash	(void *key);
int				BLI_ghashutil_strcmp	(void *a, void *b);

#endif

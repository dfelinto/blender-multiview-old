/**
 * @file BLI_dynstr.h
 * 
 * A dynamically sized string ADT.
 * This ADT is designed purely for dynamic string creation
 * through appending, not for general usage, the intent is
 * to build up dynamic strings using a DynStr object, then
 * convert it to a c-string and work with that.
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
 
#ifndef BLI_DYNSTR_H
#define BLI_DYNSTR_H

struct DynStr;

	/** The abstract DynStr type */
typedef struct DynStr DynStr;

	/**
	 * Create a new DynStr.
	 * 
	 * @return Pointer to a new DynStr.
	 */
DynStr*	BLI_dynstr_new					(void);

	/**
	 * Append a c-string to a DynStr.
	 * 
	 * @param ds The DynStr to append to.
	 * @param cstr The c-string to append.
	 */
void	BLI_dynstr_append				(DynStr *ds, char *cstr);

	/**
	 * Find the length of a DynStr.
	 * 
	 * @param ds The DynStr of interest.
	 * @return The length of @a ds.
	 */
int		BLI_dynstr_get_len				(DynStr *ds);

	/**
	 * Get a DynStr's contents as a c-string.
	 * <i> The returned c-string should be free'd
	 * using BLI_freeN. </i>
	 * 
	 * @param ds The DynStr of interest.
	 * @return The contents of @a ds as a c-string.
	 */
char*	BLI_dynstr_get_cstring			(DynStr *ds);

	/**
	 * Free the DynStr
	 * 
	 * @param ds The DynStr to free.
	 */
void	BLI_dynstr_free					(DynStr *ds);

#endif

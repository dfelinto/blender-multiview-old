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
 * 
 */

#ifndef BSE_FSMENU_H
#define BSE_FSMENU_H

	/** Returns the number of entries in the Fileselect Menu */
int		fsmenu_get_nentries		(void);

	/** Returns true if the fsmenu entry at @a index exists and
	 * is a seperator.
	 */
char*	fsmenu_is_entry_a_seperator	(int index);

	/** Returns the fsmenu entry at @a index (or NULL if a bad index)
     * or a seperator.
	 */
char*	fsmenu_get_entry		(int index);

	/** Returns a new menu description string representing the
	 * fileselect menu. Should be free'd with MEM_freeN.
	 */
char*	fsmenu_build_menu		(void);

	/** Append a seperator to the FSMenu, inserts always follow the
	 * last seperator.
	 */
void	fsmenu_append_seperator	(void);

	/** Inserts a new fsmenu entry with the given @a path.
	 * Duplicate entries are not added.
	 * @param sorted Should entry be inserted in sorted order?
	 */
void	fsmenu_insert_entry		(char *path, int sorted);

	/** Removes the fsmenu entry at the given @a index. */
void	fsmenu_remove_entry		(int index);

	/** Free's all the memory associated with the fsmenu */
void	fsmenu_free				(void);

#endif

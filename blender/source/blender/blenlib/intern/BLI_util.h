/**
 * blenlib/BLI_storage_types.h
 *
 * Some types for dealing with directories
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
#ifndef BLI_UTIL_H
#define BLI_UTIL_H

#define mallocstructN(x,y,name) (x*)MEM_mallocN((y)* sizeof(x),name)
#define callocstructN(x,y,name) (x*)MEM_callocN((y)* sizeof(x),name)


struct ListBase;

/*  void addlisttolist(struct ListBase *list1, struct ListBase *list2); */

#endif

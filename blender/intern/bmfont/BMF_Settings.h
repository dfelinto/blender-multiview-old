/**
 * $Id$
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

/**

 * $Id$
 * Copyright (C) 2001 NaN Technologies B.V.
 * Allows you to determine which fonts to include in the library.
 */

#ifndef __BMF_SETTINGS_H
#define __BMF_SETTINGS_H

/* This font is included always */
#define BMF_INCLUDE_HELV10 1

#ifndef BMF_MINIMAL

/* These fonts are included with the minimal setting defined */
#define BMF_INCLUDE_HELV12	1
#define BMF_INCLUDE_HELVB8	1
#define BMF_INCLUDE_HELVB10	1
#define BMF_INCLUDE_HELVB12	1
#define BMF_INCLUDE_HELVB14	1
#define BMF_INCLUDE_SCR12	1
#define BMF_INCLUDE_SCR14	1
#define BMF_INCLUDE_SCR15	1

#else /* BMF_MINIMAL */
#define BMF_INCLUDE_HELV12	0
#define BMF_INCLUDE_HELVB8	0
#define BMF_INCLUDE_HELVB10	0
#define BMF_INCLUDE_HELVB12	0
#define BMF_INCLUDE_HELVB14	0
#define BMF_INCLUDE_SCR12	0
#define BMF_INCLUDE_SCR14	0
#define BMF_INCLUDE_SCR15	0

#endif /* BMF_MINIMAL */

#endif /* __BMF_SETTINGS_H */

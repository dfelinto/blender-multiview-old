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

#ifndef BSE_EDITACTION_TYPES_H
#define BSE_EDITACTION_TYPES_H

#define	CHANNELHEIGHT	16
#define	CHANNELSKIP		2
#define ACTWIDTH	128

#define CHANNEL_FILTER_LOC		0x00000001	/* Show location keys */
#define CHANNEL_FILTER_ROT		0x00000002	/* Show rotation keys */
#define CHANNEL_FILTER_SIZE		0x00000004	/* Show size keys */
#define CHANNEL_FILTER_CON		0x00000008	/* Show constraint keys */
#define CHANNEL_FILTER_RGB		0x00000010	/* Show object color keys */

#define CHANNEL_FILTER_CU		0x00010000	/* Show curve keys */
#define CHANNEL_FILTER_ME		0x00020000	/* Show mesh keys */
#define CHANNEL_FILTER_LA		0x00040000	/* Show lamp keys */

#endif /* BSE_EDITACTION_TYPES_H */

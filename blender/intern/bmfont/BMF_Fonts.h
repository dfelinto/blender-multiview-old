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
 * Defines the names of the fonts in the library.
 */

#ifndef __BMF_FONTS_H
#define __BMF_FONTS_H

#include "BMF_Settings.h"

typedef enum
{
	BMF_kHelvetica10 = 0,
#if BMF_INCLUDE_HELV12
	BMF_kHelvetica12,
#endif
#if BMF_INCLUDE_HELVB8
	BMF_kHelveticaBold8,
#endif
#if BMF_INCLUDE_HELVB10
	BMF_kHelveticaBold10,
#endif
#if BMF_INCLUDE_HELVB12
	BMF_kHelveticaBold12,
#endif
#if BMF_INCLUDE_HELVB14
	BMF_kHelveticaBold14,
#endif
#if BMF_INCLUDE_SCR12
	BMF_kScreen12,
#endif
#if BMF_INCLUDE_SCR14
	BMF_kScreen14,
#endif
#if BMF_INCLUDE_SCR15
	BMF_kScreen15,
#endif
	BMF_kNumFonts
} BMF_FontType;

typedef struct BMF_Font BMF_Font;

#endif /* __BMF_FONTS_H */

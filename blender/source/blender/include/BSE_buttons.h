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

#ifndef BSE_BUTTONS_H
#define BSE_BUTTONS_H

struct ID;

void test_meshpoin_but(char *name, struct ID **idpp);
void test_obpoin_but(char *name, struct ID **idpp);
void test_scenepoin_but(char *name, struct ID **idpp);
void test_matpoin_but(char *name, struct ID **idpp);
void test_scriptpoin_but(char *name, struct ID **idpp);
void test_actionpoin_but(char *name, ID **idpp);	/* __ NLA */
void clever_numbuts_buts();

#endif  /*  BSE_BUTTONS_H */

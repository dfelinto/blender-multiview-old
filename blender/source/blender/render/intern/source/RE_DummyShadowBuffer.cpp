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

//  #include <iostream.h>

#include "render_intern.h"

#include "RE_DummyShadowBuffer.h"
struct LampRen;

RE_DummyShadowBuffer::RE_DummyShadowBuffer(void)
{
	/* empty for now */
//	cout << "Constructing dummy SB\n";
}

RE_DummyShadowBuffer::~RE_DummyShadowBuffer(void)
{
	/* empty for now */
//	cout << "Deconstructing dummy SB\n";
}

void RE_DummyShadowBuffer::importScene(struct LampRen* lar)
{
	/* empty for now */
//	cout << "Importing scene in dummy SB\n";
}

void RE_DummyShadowBuffer::readShadowValue(struct ShadBuf *shb,
										   float inp,
										   float* shadowResult)
{
	/* a sort of puple-ish colour */
	shadowResult[0] = 1.0;
	shadowResult[1] = 0.0;
	shadowResult[2] = 0.5;
}

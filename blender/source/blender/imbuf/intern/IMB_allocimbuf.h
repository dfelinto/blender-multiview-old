/**
 * allocimbuf.h
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

#ifndef IMB_ALLOCIMBUF_H
#define IMB_ALLOCIMBUF_H

struct ImBuf;

short imb_addrectImBuf(struct ImBuf * ibuf);
short imb_addplanesImBuf(struct ImBuf *ibuf);

short imb_addencodedbufferImBuf(struct ImBuf *ibuf);
short imb_enlargeencodedbufferImBuf(struct ImBuf *ibuf);

void imb_freerectImBuf(struct ImBuf *ibuf);
void imb_freeplanesImBuf(struct ImBuf *ibuf);

short imb_addcmapImBuf(struct ImBuf *ibuf);

#endif

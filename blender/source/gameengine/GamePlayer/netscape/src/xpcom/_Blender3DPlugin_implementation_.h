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
 * Implementation of a peer for a Blender plugin.
 */

#include "nsClassInfoMixin.h"

class _Blender3DPlugin_Implementation_ : public Blender3DPlugin,
					 public nsClassInfoMixin
{
private:
	void* pluginref;

public:
	NS_DECL_ISUPPORTS
	NS_DECL_BLENDER3DPLUGIN
	
	_Blender3DPlugin_Implementation_();
	virtual ~_Blender3DPlugin_Implementation_();

	/* additional members */
	void
	set_plugin_reference(
		void* ref
		);
	
/* 	NS_IMETHODIMP  */
/* 	BlenderURL( */
/* 		const char *url */
/* 		); */

/* 	NS_IMETHODIMP */
/* 	SendMessage( */
/* 		const char *to,  */
/* 		const char *from,  */
/* 		const char *subject,  */
/* 		const char *body */
/* 		); */
};

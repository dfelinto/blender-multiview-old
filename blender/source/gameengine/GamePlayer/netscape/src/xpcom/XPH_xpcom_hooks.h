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
 * Bindings from the c plugin api wrapper to XPCOM service requests.
 */

#ifndef XPH_XCOM_HOOKS_H
#define XPH_XCOM_HOOKS_H

#ifdef __cplusplus
extern "C" {
#endif

	/** Set the peer reference*/
	void
	XPH_set_peer(
		void** p_peer, 
		void *value,
		void *pluginref
		);

	/** Release our own peer reference. */
	void 
	XPH_release_peer(
		void* peer
		);

	/** Set interface ID of the Blender3DPlugin interface. */
	void
	XPH_set_IID(
		void *idloc,
		void *store
		);

	/** Retrieve size of the IID. */
	int 
	XPH_IID_size(
		void
		);

	
#ifdef __cplusplus
}
#endif

#endif

	

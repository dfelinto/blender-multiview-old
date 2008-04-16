/**
 * $Id$
 *
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
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


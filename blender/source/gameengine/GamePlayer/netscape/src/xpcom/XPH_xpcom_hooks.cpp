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
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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

#include "XPH_xpcom_hooks.h"
#include "Blender3DPlugin.h"
#include "_Blender3DPlugin_implementation_.h"

/** Set the peer reference*/
void
XPH_set_peer(
	void** p_peer, 
	void *value,
	void *pluginref
	)
{
	_Blender3DPlugin_Implementation_ *pluginpeer 
		= (_Blender3DPlugin_Implementation_ *) (*p_peer);

	if (pluginpeer) {
		pluginpeer = new _Blender3DPlugin_Implementation_();
		if (pluginpeer) {
			// Hopefully we get a peer per plugin,
			// otherwise we need an extra trick here...
			pluginpeer->set_plugin_reference(pluginref);
			// addref for ourself, don't forget to release
			// on shutdown to trigger its destruction
			NS_ADDREF(pluginpeer); 
			/* remember reference in the plugin */
			*p_peer = pluginpeer;
		}
	}
	
	NS_ADDREF(pluginpeer);
	// do it like this, because of the ambivalence of the cast
	Blender3DPlugin* foo = pluginpeer;
 	*(nsISupports **)value = foo;
// 	*(nsISupports **)value = pluginpeer;
}


void 
XPH_release_peer(void* p)
{
	/* Check the level of indirections on this one!!! */
	nsISupports * peer = (nsISupports *) p;
	NS_IF_RELEASE(peer);
}


/** Set interface ID of the Blender3DPlugin interface. */
void
XPH_set_IID(void *value, void* store)
{
	static nsIID thisiid = BLENDER3DPLUGIN_IID;
	nsIID* ptr = (nsIID *) store;
        *ptr = thisiid;
        *(nsIID **)value = ptr;
}

int 
XPH_IID_size(
	void
	)
{
	static nsIID thisiid = BLENDER3DPLUGIN_IID;
	return sizeof(thisiid);
}	

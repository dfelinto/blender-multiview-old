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

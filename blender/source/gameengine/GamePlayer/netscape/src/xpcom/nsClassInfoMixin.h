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
 * Implementation of a helper implementation for the browser plugin peer. 
 */

#ifndef NSCLASSINFOMIXIN_H
#define NSCLASSINFOMIXIN_H

#include "nsIClassInfo.h"


// helper class to implement all necessary nsIClassInfo method stubs
// and to set flags used by the security system
class nsClassInfoMixin : public nsIClassInfo
{
 public:

	// These flags are used by the DOM and security systems to signal that 
	// JavaScript callers are allowed to call this object's scritable methods.
	NS_IMETHOD GetFlags(PRUint32 *aFlags) {
		*aFlags = nsIClassInfo::PLUGIN_OBJECT | nsIClassInfo::DOM_OBJECT;
		return NS_OK;
	}
	
	NS_IMETHOD GetImplementationLanguage(PRUint32 *aImplementationLanguage) {
		*aImplementationLanguage = nsIProgrammingLanguage::CPLUSPLUS;
		return NS_OK;
	}

	// The rest of the methods can safely return error codes...
	NS_IMETHOD GetInterfaces(PRUint32 *count, nsIID * **array)
		{return NS_ERROR_NOT_IMPLEMENTED;}
	NS_IMETHOD GetHelperForLanguage(PRUint32 language, nsISupports **_retval)
		{return NS_ERROR_NOT_IMPLEMENTED;}
	NS_IMETHOD GetContractID(char * *aContractID)
		{return NS_ERROR_NOT_IMPLEMENTED;}
	NS_IMETHOD GetClassDescription(char * *aClassDescription)
		{return NS_ERROR_NOT_IMPLEMENTED;}
	NS_IMETHOD GetClassID(nsCID * *aClassID)
		{return NS_ERROR_NOT_IMPLEMENTED;}
	NS_IMETHOD GetClassIDNoAlloc(nsCID *aClassIDNoAlloc)
		{return NS_ERROR_NOT_IMPLEMENTED;}

};

#endif

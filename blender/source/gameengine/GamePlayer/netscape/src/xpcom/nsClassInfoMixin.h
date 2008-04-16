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


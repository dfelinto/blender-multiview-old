/**
 * $Id$
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

#include "stdafx.h"


#include "SafeControl.h"


////////////////////////////////////////////////////////////////
// Copied from the ActiveX SDK
// This code is used to register and unregister a
// control as safe for initialization and safe for scripting
HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription)
{
	ICatRegister* pcr = NULL ;
	HRESULT hr = S_OK ;
	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
		 NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (FAILED(hr))
		 return hr;
	// Make sure the HKCR\Component Categories\{..catid...} 
	// key is registered
	CATEGORYINFO catinfo;
	catinfo.catid = catid;
	catinfo.lcid = 0x0409 ; // english
	// Make sure the provided description is not too long.
	// Only copy the first 127 characters if it is
	int len = wcslen(catDescription);
	if (len>127)
		len = 127;
	wcsncpy(catinfo.szDescription, catDescription, len);
	// Make sure the description is null terminated
	catinfo.szDescription[len] = 0; //`\0';
	hr = pcr->RegisterCategories(1, &catinfo);
	pcr->Release();
	return hr;
}

HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
    // Register your component categories information.
    ICatRegister* pcr = NULL ;
    HRESULT hr = S_OK ;
    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
            NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
    if (SUCCEEDED(hr))
    {
           // Register this category as being "implemented" by
           // the class.
           CATID rgcatid[1] ;
           rgcatid[0] = catid;
           hr = pcr->RegisterClassImplCategories(clsid, 1, rgcatid);
    }
    if (pcr != NULL)
          pcr->Release();
    return hr;
}

HRESULT UnRegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
    ICatRegister* pcr = NULL ;
    HRESULT hr = S_OK ;
    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
             NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
    if (SUCCEEDED(hr))
    {
        // Unregister this category as being "implemented" by
        // the class.
        CATID rgcatid[1] ;
        rgcatid[0] = catid;
        hr = pcr->UnRegisterClassImplCategories(clsid, 1, rgcatid);
    }
    if (pcr != NULL)
    pcr->Release();
    return hr;
}


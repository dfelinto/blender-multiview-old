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
#include <stdlib.h>

#include "../PIL_dynlib.h"

/*
 * XXX, should use mallocN so we can see
 * handle's not being released. fixme zr
 */
 
#ifdef WIN32

#include <windows.h>

struct PILdynlib {
	void *handle;
};

PILdynlib *PIL_dynlib_open(char *name) {
	void *handle= LoadLibrary(name);

	if (handle) {	
		PILdynlib *lib= malloc(sizeof(*lib));
		lib->handle= handle;
		
		return lib;
	} else {
		return NULL;
	}
}

void *PIL_dynlib_find_symbol(PILdynlib* lib, char *symname) {
	return GetProcAddress(lib->handle, symname);
}

char *PIL_dynlib_get_error_as_string(PILdynlib* lib) {
	int err= GetLastError();

	if (err) {
		static char buf[1024];

		if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, 
					NULL, 
					err, 
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
					buf, 
					sizeof(buf), 
					NULL))
			return buf;
	}
	
	return NULL;
}

void PIL_dynlib_close(PILdynlib *lib) {
	FreeLibrary(lib->handle);
	
	free(lib);
}

#else
#ifdef __APPLE__

PILdynlib *PIL_dynlib_open(char *name) {
	return NULL;
}

void *PIL_dynlib_find_symbol(PILdynlib* lib, char *symname) {
	return NULL;
}

char *PIL_dynlib_get_error_as_string(PILdynlib* lib) {
	return "Plugins are currently unsupported on OSX";
}
	
void PIL_dynlib_close(PILdynlib *lib) {
	;
}

#else	/* Unix */

#include <dlfcn.h>

struct PILdynlib {
	void *handle;
};

PILdynlib *PIL_dynlib_open(char *name) {
	void *handle= dlopen(name, RTLD_LAZY);

	if (handle) {	
		PILdynlib *lib= malloc(sizeof(*lib));
		lib->handle= handle;
		
		return lib;
	} else {
		return NULL;
	}
}

void *PIL_dynlib_find_symbol(PILdynlib* lib, char *symname) {
	return dlsym(lib->handle, symname);
}

char *PIL_dynlib_get_error_as_string(PILdynlib* lib) {
	return dlerror();
}
	
void PIL_dynlib_close(PILdynlib *lib) {
	dlclose(lib->handle);
	
	free(lib);
}

#endif
#endif

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
 * Native implementation of the plugin-class methods, accessible
 * through the classic Netscape JRI bindings.
 */

#include <stdio.h>

#include "netscape_plugin_Plugin.h"
#define IMPLEMENT_Blender3DPlugin
#include "Blender3DPlugin.h"

#include "npapi.h"
#include "blender_plugin_types.h"

#include "PLB_script_bindings.h"

/* #define GENERATE_LOG */
static void
B3D_log_entry(char* msg);


/* Java bindings: */
JRI_PUBLIC_API(void)
	native_Blender3DPlugin_blenderURL(
		JRIEnv* env, 
		Blender3DPlugin* self,
		struct java_lang_String * url
		)
{
	
	NPP npp = (NPP)netscape_plugin_Plugin_getPeer(env, self);
	BlenderPluginInstance* inst = NULL;
	char* tmp;

	B3D_log_entry("native_Blender3DPlugin_blenderURL");
	
	inst = (BlenderPluginInstance*) npp->pdata;

	if (inst) {
		/* use UTF because we get ascii text only  */
		tmp = NPN_MemAlloc(JRI_GetStringUTFLength(env, url) + 1);
		strcpy(inst->new_url, JRI_GetStringUTFChars(env, url));
		PLB_native_blenderURL_func(inst, tmp);
		NPN_MemFree(tmp);
	}
}


JRI_PUBLIC_API(void)
	native_Blender3DPlugin_SendMessage(
		JRIEnv* env, 
		Blender3DPlugin* self,
		struct java_lang_String * to,
		struct java_lang_String * from,
		struct java_lang_String * subject,
		struct java_lang_String * body
		)
{
	
	NPP npp = (NPP)netscape_plugin_Plugin_getPeer(env, self);
	BlenderPluginInstance* inst  = NULL;
	char *to_p, *from_p, *subject_p, *body_p;

	B3D_log_entry("native_Blender3DPlugin_SendMessage");

	inst = (BlenderPluginInstance*) npp->pdata;

	fprintf(stderr, "Doing java stuff for instance %p\n",
		inst);
	fflush(stderr);
	
	to_p = NPN_MemAlloc(JRI_GetStringUTFLength(env, to) + 1);
	from_p = NPN_MemAlloc(JRI_GetStringUTFLength(env, from) + 1);
	subject_p = NPN_MemAlloc(JRI_GetStringUTFLength(env, subject) + 1);
	body_p = NPN_MemAlloc(JRI_GetStringUTFLength(env, body) + 1);

	strcpy(to_p, JRI_GetStringUTFChars(env, to));
	strcpy(from_p, JRI_GetStringUTFChars(env, from));
	strcpy(subject_p, JRI_GetStringUTFChars(env, subject));
	strcpy(body_p, JRI_GetStringUTFChars(env, body));

	PLB_native_SendMessage_func(inst, 
				    to_p, 
				    from_p, 
				    subject_p, 
				    body_p);

	NPN_MemFree(to_p);
	NPN_MemFree(from_p);
	NPN_MemFree(subject_p);
	NPN_MemFree(body_p);
}


static void
B3D_log_entry(char* msg)
{
#ifdef GENERATE_LOG
	FILE* fp = fopen("/tmp/plugin_log","a");
	if (!fp) return;
	fprintf(fp, "--> KXH_ketsji_hooks::%s\n", msg); 
	fflush(fp);
	fclose (fp);
#endif
}



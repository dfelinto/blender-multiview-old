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



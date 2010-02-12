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
 * NS api template, adapted to link to our own internals.
 */

#define UNIXSH_VERSION "$Id$"

/* -*- Mode: C; tab-width: 8; c-set-style: bsd -*- */

/* UnixShell.c was adapted from the template in the Netscape API. */

/* System: */
#include <string.h>

/* All nsapi stuff. nsapi now needs FILE, so include stdio as well. */
#include <stdio.h>
#include "npapi.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* Javascript through JRI: */
#include "netscape_plugin_Plugin.h"
#define IMPLEMENT_Blender3DPlugin
#include "Blender3DPlugin.h"

/* Javascript through XPCOM */
#if defined(__linux__) && defined(__i386__)
#include "XPH_xpcom_hooks.h"
#endif

/* Native hooks: */
#include "npapi.h"

/* Threading the NSPR way: */
#include "prthread.h"
#include "prlock.h"

/* Our own stuff: */
#include "blender_plugin_types.h"
#include "NZC_application_hooks.h"
#include "NZC_application_callback.h"
#include "PLB_script_bindings.h"

#if defined(_WIN32)
#include "PLG_MSWindows_windowing.h"
#elif defined(__APPLE__)
/* No apple inplementation yet. */
#else
/* Unix system: */
#include <unistd.h>
#include "PLG_X11_windowing.h"
#endif

/* --------------------------------------------------------------------- */

/** If defined: write to the plugin log file */
#define NZC_GENERATE_LOG

/* --------------------------------------------------------------------- */

/** Mutex for access to the GLX rendering context. Before gl commands
 * are issues, a thread needs to switch to the right glx
 * context. During its operations, no other threads may attempt to
 * access gl. */
PRLock* gl_lock;

/** To ensure correct initialisation of mutexes and such, this goes
 * into the instance creation. It was in the loading code before, but
 * that gives strange effects. */

/* --------------------------------------------------------------------- */

/* move to header... */
extern int32 STREAMBUFSIZE;
/* Local functions------------------------------------------------------- */

/** Generate a log file. */
static void
log_entry(char* msg);

/** This call does mutex init and such. */
void
initialization_call(void);

/** Do a single redraw. */
void
step_application(BlenderPluginInstance* inst);

/* --------------------------------------------------------------------- */
/* Implementations:                                                      */
/* --------------------------------------------------------------------- */

/* Huh? Not on win? */
#ifdef XP_UNIX

/* NPP_GetMIMEDescription() and NPP_GetValue() are called to determine
 * the mime types supported by this plugin. */
char*
NPP_GetMIMEDescription(
	void
	)
{
	log_entry("NPP_GetMIMEDescription");
	return("application/x-blender-plugin:blend:Blender 3D content");
}

NPError
NPP_GetValue(
	NPP instance,
	NPPVariable variable,
	void *value
	)
{
	NPError err = NPERR_NO_ERROR;
#if defined(__linux__) && defined(__i386__)
	BlenderPluginInstance* inst;
	void *tmp;
#endif
	
	log_entry("NPP_GetValue");

	switch (variable) {
	case NPPVpluginNameString:
		log_entry("NPP_GetValue::NPPVpluginNameString");
		*((char **)value) = "Blender";
		break;
	case NPPVpluginDescriptionString:
		log_entry("NPP_GetValue::NPPVpluginDescriptionString");
		*((char **)value) =
			"Player for interactive 3D content";
		break;
	case NPPVpluginScriptableInstance:
		/* This cast must succeed... otherwise we'll get nasty
		 * crashes :/ */
		log_entry("NPP_GetValue::NPPVpluginScriptableInstance");
#if defined(__linux__) && defined(__i386__)
		inst = (BlenderPluginInstance*) instance->pdata;
		if (inst) {
			XPH_set_peer(&inst->xpcom_peer,
				     value,
				     inst);
		}
#endif
		break;
	case NPPVpluginScriptableIID:
		log_entry("NPP_GetValue::NPPVpluginScriptableIID");
#if defined(__linux__) && defined(__i386__)
		tmp = NPN_MemAlloc(XPH_IID_size());
		XPH_set_IID(value, tmp);
#endif
		break;		
	default:
		err = NPERR_GENERIC_ERROR;
	}
	return err;
}

#endif /* XP_UNIX */

/* --------------------------------------------------------------------- */
/* Mozilla: NPP_Initialize() and NPP_GetJavaClass() are called when
 * starting the browser, and then every time the plugin is started*/
NPError
NPP_Initialize(
	void
	)
{
	log_entry("NPP_Initialize");
	return NPERR_NO_ERROR;
}


jref
NPP_GetJavaClass(
	void
	)
{
	JRIEnv* env;
	log_entry("NPP_GetJavaClass");

	env = NPN_GetJavaEnv();
	
	if (env) {
		use_netscape_plugin_Plugin(env);
		return use_Blender3DPlugin(env);
	} else {
		fprintf(stderr, "Blender3DPlugin: No JRI available, "
			"awaiting XPConnect queries.\n");
		fflush(stderr);
		return NULL;
	}
}

/* --------------------------------------------------------------------- */

void
NPP_Shutdown(
	void
	)
{
	JRIEnv* env = NPN_GetJavaEnv();

	log_entry("NPP_Shutdown");

	if (env) {
		unuse_netscape_plugin_Plugin(env);
		unuse_Blender3DPlugin(env);
	} 
}


NPError 
NPP_New(
	NPMIMEType pluginType,
	NPP instance,
	uint16 mode,
	int16 argc,
	char* argn[],
	char* argv[],
	NPSavedData* saved
	)
{
	BlenderPluginInstance* This  = NULL;
	int i = 0;
	int retval = 0;
	JRIEnv* env;

	log_entry("NPP_New");
	
	/* -sigh- ... another non-existing function... */
	/* Always windowless. */
/* 	NPN_SetValue(instance, */
/* 		     NPPVpluginWindowBool, */
/* 		     0); */
	
	if (instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;
	
	instance->pdata = NPN_MemAlloc(sizeof(BlenderPluginInstance));
	
	This = (BlenderPluginInstance*) instance->pdata;

	This->event_lock = PR_NewLock();
	This->window_ready_lock = PR_NewLock();

	This->framerate = -1;
	
	This->loading_animation = NULL;
	This->blend_file = NULL;
	This->loading_anim_store = NULL;
	This->main_file_store = NULL;
	
	This->browser_instance = instance;

	This->main_file_stream = NULL;
	This->loading_anim_stream = NULL;
	
	This->foreground_colour[0] = 0.0;
	This->foreground_colour[1] = 0.0;
	This->foreground_colour[2] = 0.0;
	This->background_colour[0] = 0.0;
	This->background_colour[1] = 0.0;
	This->background_colour[2] = 0.0;

	/** Some default? */
	This->framerate = 50;

	/* This call kicks the interpreter to create the actual object */
	env = NPN_GetJavaEnv();
	if (env) {
		This->jri_or_xp = 0;
		This->java_peer = NPN_GetJavaPeer(instance);
		This->xpcom_peer = NULL;
			} else {
		This->jri_or_xp = 1;
		This->java_peer = NULL;
		This->xpcom_peer = NULL;
	}
	
	/* Parse the options from the file. Should I do this in the
	 * implementation file maybe? Now we do a lot with
	 * instance-specific data. */
	while (i <argc ) {
		if (!strcmp(argn[i], "loadingURL")
		    ||!strcmp(argn[i], "loadingurl")) {
			/* A custom loading animation. */
			int url_len = strlen(argv[i]);
			if ((url_len > 0) && (url_len < 4096) ) {
				This->loading_animation = NPN_MemAlloc(url_len + 1);
				strcpy(This->loading_animation, argv[i]);

				/* Since there are some problems
				 * getting this threaded, do it
				 * here */
				retval = NPN_GetURL(This->browser_instance,
						    This->loading_animation,
						    NULL);
				if (retval) {
					/* error? Revert to default again. */
					This->loading_animation = NULL;
				}
			}
		} else if (!strcmp(argn[i],"frameRate")) {
			/* The desired framerate. */
			This->framerate = atoi(argv[i]);

			/** the framerate is clipped to [1,100] for
			 * now. Are there specs for this? */
			if (This->framerate < 1 ) This->framerate = 1;
			if (This->framerate > 100 ) This->framerate = 100;
			
		} else if (!strcmp(argn[i],"src")) {
			/* The blend file to load. */
			int url_len = strlen(argv[i]);
			if ((url_len > 0) && (url_len < 4096) ) {
				This->blend_file = NPN_MemAlloc(url_len + 1);
				strcpy(This->blend_file, argv[i]);
			}
		} else if (!strcmp(argn[i],"ForeColor")) {
			/* Foreground colour: in rgb */
			int colnum = atoi(argv[i]);
			
			This->foreground_colour[2] = (float) (colnum / 65536) / 256.0;
			colnum = colnum % 65535;
			This->foreground_colour[1] = (float) ((colnum) / 256) / 256.0;
			colnum = colnum % 256;
			This->foreground_colour[0] = (float) colnum / 256.0;
		} else if (!strcmp(argn[i],"BackColor")) {
			/* Background colour: in rgb */
			int colnum = atoi(argv[i]);
			
			This->background_colour[2] = (float) (colnum / 65536) / 256.0;
			colnum = colnum % 65535;
			This->background_colour[1] = (float) ((colnum) / 256) / 256.0;
			colnum = colnum % 256;
			This->background_colour[0] = (float) colnum / 256.0;
		} else if (!strcmp(argn[i],"useFileBackColor")) {
			/* Choose whether to use the background colour
			 * or the file-specified colour for the side
			 * areas. */
			This->use_file_background_colour = atoi(argv[i]);
		} 		
		i++;
	}
		
	if (This != NULL) {
		NPError err_status = NPERR_NO_ERROR;
		/* needs to become a once...?*/
		initialization_call();		
		PLG_plugin_new_instance_init(This);
		
		/* Lock now, and don't release until the window is
		 * ready. */
		PR_Lock(This->window_ready_lock);
		
		This->application  = APH_create_application((PLA_plugin_handle)This);
		if (!This->application) err_status = NPERR_GENERIC_ERROR;

		return err_status;
	} else
		return NPERR_OUT_OF_MEMORY_ERROR;
}


NPError 
NPP_Destroy(
	NPP instance,
	NPSavedData** save
	)
{
	BlenderPluginInstance* This;

	log_entry("NPP_Destroy");

	if (instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	This = (BlenderPluginInstance*) instance->pdata;

	/* PLUGIN DEVELOPERS:
	 *	If desired, call NP_MemAlloc to create a
	 *	NPSavedDate structure containing any state information
	 *	that you want restored if this plugin instance is later
	 *	recreated.
	 */
	
	if (This != NULL) {
		if (This->jri_or_xp) {
#if defined(__linux__) && defined(__i386__)
			if (This->xpcom_peer) {
				XPH_release_peer(This->xpcom_peer);
			}
#endif
		}

		

		APH_terminate_application(This->application);
		
		/* Terminate the locks... */
		PR_DestroyLock(gl_lock);
		PR_DestroyLock(This->event_lock);
		PR_DestroyLock(This->window_ready_lock);
		
		if (This->blend_file) NPN_MemFree(This->blend_file);
		if (This->main_file_store) NPN_MemFree(This->main_file_store);
		if (This->loading_animation) NPN_MemFree(This->loading_animation);
		if (This->loading_anim_store) NPN_MemFree(This->loading_anim_store);
		NPN_MemFree(instance->pdata);
		instance->pdata = NULL;
	}

	return NPERR_NO_ERROR;
}



NPError 
NPP_SetWindow(
	NPP instance,
	NPWindow* window
	)
{
	BlenderPluginInstance* This;

	log_entry("NPP_SetWindow");

	if (instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	/* window handle */
	if ((window == NULL)
		|| (window->window == NULL)) {
		return NPERR_NO_ERROR;
	}
	
	This = (BlenderPluginInstance*) instance->pdata;

	if (This) {
		/* get error from here: */
		PLG_set_platform_window(instance,
					window,
					This);
		return NPERR_NO_ERROR;
	} else {
		return NPERR_INVALID_INSTANCE_ERROR;
	}
}


NPError 
NPP_NewStream(
	NPP instance,
	NPMIMEType type,
	NPStream *stream, 
	NPBool seekable,
	uint16 *stype
	)
{
	//NPByteRange range;
	BlenderPluginInstance* This;

	log_entry("NPP_NewStream");
	
	if (instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	This = (BlenderPluginInstance*) instance->pdata;

	/* Only two now, but if there are more, this should become
	 * more general.*/
	if (This->loading_animation
	    && (strcmp(stream->url, This->loading_animation) == 0)) {
		This->loading_anim_total = stream->end;
		This->loading_anim_retrieved = 0;
		This->loading_anim_store = NPN_MemAlloc(stream->end*sizeof(unsigned char));
		This->loading_anim_stream = stream;
		if (!This->loading_anim_store) {
			fprintf(stderr, "Blender plugin: Out of memory! "
				"Cannot get chunk for main file.\n");
			exit(1);
		}
	} else {
		This->stream_total = stream->end;
		This->stream_retrieved = 0;
		This->main_file_store = NPN_MemAlloc(stream->end*sizeof(unsigned char));
		This->main_file_stream = stream;
		if (!This->main_file_store) {
			fprintf(stderr, "Blender plugin: Out of memory! "
				"Cannot get chunk for loading animation.\n");
			exit(1);
		}
	}
	
	/* Careful with this flag: it will cause the
	 * browser to stops interaction with the plugin!!*/
	/*
	  Hangs the browser. Also doesn't give nice progress updates.
	  *stype = NP_ASFILEONLY;
	  write requests as well as the NPP_streamasfile call
	  *stype = NP_ASFILE; 
	  */
	
	/* Normal streaming. On Linux, you still get a call to
	 * NPP_StreamAsFile, though. On FreeBSD, you _never_ get
	 * it, even with NP_ASFILE. */
	*stype = NP_NORMAL;

	return NPERR_NO_ERROR;
}


/* PLUGIN DEVELOPERS:
 *	These next 2 functions are directly relevant in a plug-in which
 *	handles the data in a streaming manner. If you want zero bytes
 *	because no buffer space is YET available, return 0. As long as
 *	the stream has not been written to the plugin, Navigator will
 *	continue trying to send bytes.  If the plugin doesn't want them,
 *	just return some large number from NPP_WriteReady(), and
 *	ignore them in NPP_Write().  For a NP_ASFILE stream, they are
 *	still called but can safely be ignored using this strategy.
 */

int32 STREAMBUFSIZE = 0X0FFFFFFF; /* If we are reading from a file in NPAsFile
				   * mode so we can take any size stream in our
				   * write call (since we ignore it) */

int32 
NPP_WriteReady(
	NPP instance,
	NPStream *stream
	)
{
	BlenderPluginInstance* This = NULL;
	int acceptable = 0;
	
/* 	log_entry("NPP_WriteReady"); */

	if (instance != NULL)
		This = (BlenderPluginInstance*) instance->pdata;

	/* Check whether buffers already exist: */

	if ((This->main_file_stream && This->main_file_store)
	    || (This->loading_anim_stream && This->loading_anim_store)) {
		acceptable = STREAMBUFSIZE;
	}
	
	return acceptable;
}


int32 
NPP_Write(
	NPP instance,
	NPStream *stream,
	int32 offset,
	int32 len,
	void *buffer
	)
{
	BlenderPluginInstance* This = NULL;
	int accepted = 0;
	
/* 	log_entry("NPP_Write"); */
	
	This = (BlenderPluginInstance*) instance->pdata;
	
	if (instance != NULL)
	{
		if (stream == This->main_file_stream) {
			memcpy(((unsigned char*)This->main_file_store)
			       + This->stream_retrieved,
			       buffer,
			       len);
			accepted = len;
			This->stream_retrieved += len;
			if (This->stream_retrieved >= This->stream_total) {
				fflush(stderr);
				APH_main_file_loaded(This->application,
						     This->main_file_store,
						     This->stream_total);
			}
		} else if (stream == This->loading_anim_stream) {
			memcpy(((unsigned char*)This->loading_anim_store)
			       + This->loading_anim_retrieved,
			       buffer,
			       len);
			accepted = len;
			This->loading_anim_retrieved += len;
			if (This->loading_anim_retrieved >= This->loading_anim_total) {
				APH_loading_anim_loaded(This->application,
							This->loading_anim_store,
							This->loading_anim_total);
			}
		} else {
			/* the stream ref wasn't set yet..*/
			accepted = 0;
		}
	}

	return accepted;
}


NPError 
NPP_DestroyStream(
	NPP instance,
	NPStream *stream,
	NPError reason
	)
{
	BlenderPluginInstance* This = NULL;

	log_entry("NPP_DestroyStream");

	if (instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;
	This = (BlenderPluginInstance*) instance->pdata;

	if (This) {
		if (reason != NPRES_DONE) {
			if (stream == This->loading_anim_stream) {
				APH_loading_anim_failed(This->application);
			} else {
				APH_main_file_failed(This->application);
			}
		}
		return NPERR_NO_ERROR;
	} else {
		return NPERR_INVALID_INSTANCE_ERROR;
	}
}


/* Not supposed to be called anymore... Anyway, we don't need the
 * results. Some Moz implementations will call this one regardless the
 * desired transfer mode! */
void 
NPP_StreamAsFile(
	NPP instance,
	NPStream *stream,
	const char* fname
	)
{
/* 	log_entry("NPP_StreamAsFile"); */
}


void 
NPP_Print(
	NPP instance,
	NPPrint* printInfo
	)
{

	log_entry("NPP_Print");

	if(printInfo == NULL)
		return;

	if (instance != NULL) {
/*  		BlenderPluginInstance* This = (BlenderPluginInstance*) instance->pdata; */
	
		if (printInfo->mode == NP_FULL) {
		    /*
		     * PLUGIN DEVELOPERS:
		     *	If your plugin would like to take over
		     *	printing completely when it is in full-screen mode,
		     *	set printInfo->pluginPrinted to TRUE and print your
		     *	plugin as you see fit.  If your plugin wants Netscape
		     *	to handle printing in this case, set
		     *	printInfo->pluginPrinted to FALSE (the default) and
		     *	do nothing.  If you do want to handle printing
		     *	yourself, printOne is true if the print button
		     *	(as opposed to the print menu) was clicked.
		     *	On the Macintosh, platformPrint is a THPrint; on
		     *	Windows, platformPrint is a structure
		     *	(defined in npapi.h) containing the printer name, port,
		     *	etc.
		     */

/*  			void* platformPrint = */
/*  				printInfo->print.fullPrint.platformPrint; */
/*  			NPBool printOne = */
/*  				printInfo->print.fullPrint.printOne; */
			
			/* Do the default*/
			printInfo->print.fullPrint.pluginPrinted = FALSE;
		}
		else {	/* If not fullscreen, we must be embedded */
		    /*
		     * PLUGIN DEVELOPERS:
		     *	If your plugin is embedded, or is full-screen
		     *	but you returned false in pluginPrinted above, NPP_Print
		     *	will be called with mode == NP_EMBED.  The NPWindow
		     *	in the printInfo gives the location and dimensions of
		     *	the embedded plugin on the printed page.  On the
		     *	Macintosh, platformPrint is the printer port; on
		     *	Windows, platformPrint is the handle to the printing
		     *	device context.
		     */

/*  			NPWindow* printWindow = */
/*  				&(printInfo->print.embedPrint.window); */
/*  			void* platformPrint = */
/*  				printInfo->print.embedPrint.platformPrint; */
		}
	}
}

/* --------------------------------------------------------------------- */

void
initialization_call(void)
{

	log_entry("initialization_call");

	PLG_global_instance_init();
	
	/* Mutexes need to be initialized: event and window ready
         * locks are instance specific. */
	gl_lock = PR_NewLock();
	
	if (!gl_lock) {
		fprintf(stderr, "Blender3DPlugin: Failure "
			"creating nspr threads, bailing\n");
		exit(1);
	}
	
}

/* ---------------------------------------------------------------------- */

/** Hook for Javascript SendMessage function */
void
PLB_native_SendMessage_func(
	void* i,
	const char* to,
	const char* from,
	const char* subject,
	const char* body
	)
{
	BlenderPluginInstance* inst =  (BlenderPluginInstance*) i;
	APH_send_message(inst->application, 
			 to, 
			 from, 
			 subject, 
			 body);
}



void
PLB_native_blenderURL_func(
	void* i,
	const char* url
	)
{

	BlenderPluginInstance* inst =  (BlenderPluginInstance*) i;
	NPError retval;
	
	inst->new_url = NPN_MemAlloc(strlen(url) + 1);
	if (!inst->new_url) return;
	
	strcpy(inst->new_url, url);
	
	if (inst->browser_instance) {
		NPN_Status(inst->browser_instance,
			   "native_Blender3DPlugin_blenderURL");
	}
	
	fprintf(stderr, "Got javascript request "
		"for new url: %s\n", inst->new_url);
	fflush(stderr);

	/* rewire to PLA_request_url, need to change the headers */

	/* Restart with a new url: i should make an url queue. also, the
	   char* cast is not very good */
	retval = NPN_GetURL(inst->browser_instance,
			    inst->new_url,
			    NULL);
	
	if (retval != NPERR_NO_ERROR) {
		fprintf(stderr, " java initiated geturl failed with %d\n", retval);
		exit(1);
	} else {
		fprintf(stderr, " java initiated geturl returned %d\n", retval);
	}


	/* need to adapt the state diagram */
	APH_restart_with_url(inst->application);

}


/* --------------------------------------------------------------------- */
/* For the plugin:                                                       */
/* --------------------------------------------------------------------- */

void 
PLA_await_window(
	PLA_plugin_handle i
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	PR_Lock(inst->window_ready_lock);
	PR_Unlock(inst->window_ready_lock);
}

void 
PLA_lock_events(
	PLA_plugin_handle i
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	PR_Lock(inst->event_lock);
}

void 
PLA_unlock_events(
	PLA_plugin_handle i
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	PR_Unlock(inst->event_lock);
}



int
PLA_request_replacing_url(PLA_plugin_handle i,
			  char* url)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	NPError retval = NPERR_NO_ERROR;

	log_entry("PLA_request_replacing_url");
	
	retval =  NPN_GetURL(inst->browser_instance,
			     url,
			     "_current");

	if (retval != NPERR_NO_ERROR) {
		fprintf(stderr, " Blender plugin: replacing "
			"url failed, exiting...\n");
		exit(1);
	}
	return retval;

}


/* some getters: */

/* char* */
/* PLA_get_blend_file_url(PLA_plugin_handle i){ */
/* 	BlenderPluginInstance* inst = (BlenderPluginInstance*) i; */
/* //	log_entry("PLG_get_blend_file_url"); */
/* 	return inst->blend_file; */
/* } */

char*
PLA_get_loading_animation_url(PLA_plugin_handle i){
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
//	log_entry("PLG_get_loading_animation_url");
	return inst->loading_animation;
}


int
PLA_stream_retrieved(PLA_plugin_handle i)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	return inst->stream_retrieved;
}

int
PLA_stream_total(PLA_plugin_handle i)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	return inst->stream_total;
}

float*
PLA_get_foreground_colour(PLA_plugin_handle i)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	log_entry("PLA_get_foreground_colour");
	return inst->foreground_colour;
}

float*
PLA_get_background_colour(PLA_plugin_handle i)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	return inst->background_colour;
}

float
PLA_requested_framerate(PLA_plugin_handle i)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	return (float) inst->framerate;
}

/* --------------------------------------------------------------------- */

static void
log_entry(char* msg)
{
#ifdef NZC_GENERATE_LOG
	FILE* fp = fopen("/tmp/plugin_log","a");
	if (!fp) return;
  	fprintf(fp, "--> Unixshell::(%p) %s\n", 
		PR_GetCurrentThread(),
		msg); 
	fflush(fp);
	fclose (fp);
#endif
}

/* --------------------------------------------------------------------- */

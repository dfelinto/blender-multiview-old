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
 * Implementation of the application hooks between browser and app, 
 * where app = Ketjsi.
 */

/*
 * Application handle refers to ketsji_engine_data!!!
 */

#include <stdio.h>

/* Moz infra: */
#include "npapi.h"
#include "prthread.h"

/* Plugin related */
#include "NZC_application_hooks.h"
#include "NZC_application_callback.h"
#include "KXH_ketsji_hooks.h"

/* Util */
#include "STR_String.h"

/* Blender system */
#include "GEN_messaging.h"
#include "SYS_System.h"

/* Gameplayer interfacing */
#include "SCA_IInputDevice.h"
//#include "SCA_ISystem.h"
#include "GPC_System.h"
#include "GPC_Canvas.h"
#include "GPC_KeyboardDevice.h"
#include "GPC_MouseDevice.h"
#include "GPC_RenderTools.h"
#include "GPC_RawImage.h"
#include "GPC_RawLoadDotBlendArray.h"
#include "GPC_RawLogoArrays.h"
#include "SND_DeviceManager.h"
#include "RAS_OpenGLRasterizer.h"
#include "KX_KetsjiEngine.h"
#include "KX_BlenderSceneConverter.h"
#include "NG_LoopBackNetworkDeviceInterface.h"
#include "KX_PythonInit.h"
#include "KX_PyConstraintBinding.h"

#include "KXH_engine_data_wraps.h"

#if defined(_WIN32)

#include "KXH_MSWindows_services.h"

#elif defined(__APPLE__)

#else
#include <unistd.h>
#include <vector.h>
#include "KXH_unix_event_sinks.h"
#include "KXH_unix_services.h"
#endif

/* --------------------------------------------------------------------- */
/* Some handy defines...                                                 */
/* --------------------------------------------------------------------- */
/* if defined: log function entry */
#define KXH_GENERATE_LOG

/* --------------------------------------------------------------------- */

/** Cast the application handle to local data. */
ketsji_engine_data*
handle_to_data(
	APH_application_handle handle
	);

/** main() for the plugin thread */
void
plugin_mainloop(
	void* arg
	);

/* Let the gameengine do one step. */
void 
tick(
	ketsji_engine_data* k
	);

/** Make a gameengine and give it devices. */
void
initialize_gameengine(
	ketsji_engine_data* k,
	bool for_main_file
	);

/** Check for network messages posted by the current blend file */
void
check_for_messages(
	ketsji_engine_data* k
	);

/** Request transition to completely other url.  */
void
request_replacing_url(
	ketsji_engine_data* k,
	const STR_String& url
	);

/* Use the default .blend loader as loading animation. */
void 
open_default_loader(
	ketsji_engine_data* k
	);

/* Message the loading animation to up the progress. */
void 
update_loading_progress(
	ketsji_engine_data* k
	);

/** Stop, and drop resources from the current running engine. */
void 
release_resources(
	ketsji_engine_data* k
	);

/** Set the canvas to a plain fore and background color. */
void
set_plain_color(
	ketsji_engine_data* k
	);

/** Check whether there is a valid loading anim. */
bool 
loading_anim_file_valid(
	ketsji_engine_data* k
	);

/** Test the type/status of the main file. */
KXH_MAIN_FILE_STATE
main_file_state(
	ketsji_engine_data* k
	);

/** Little looper to enable all kinds of logging. */
static void 
to_state(
	ketsji_engine_data* k, 
	KXH_STATE i
	);

/** Redraw the canvas with a plain foreground color. */
void 
set_plain_color_redraw_func(
	ketsji_engine_data* k
	);

static void
KXH_log_entry(
	char* msg
	);


/* --------------------------------------------------------------------- */
/* Drawing functions                                                     */
/* --------------------------------------------------------------------- */

/** Acquire drawing context: blocking */
bool
KXH_begin_draw(KXH_plugin_handle h)
{
	ketsji_engine_data* k = (ketsji_engine_data*) h;
//	KXH_log_entry("KXH_begin_draw");
	if((!k) || (!k->kx_initialized)) return false;
	
	return (PLA_acquire_gl_context(k->plugin) ? true : false);
}

/** Swap buffers in the drawing context. This does not release the
 * context! */
void
KXH_swap_buffers(KXH_plugin_handle h)
{
	ketsji_engine_data* k = (ketsji_engine_data*) h;
//	KXH_log_entry("KXH_swap_buffers");
	if((!k) || (!k->kx_initialized)) return;
	PLA_swap_gl_context(k->plugin);	
}

/** Release drawing context. */
void
KXH_end_draw(KXH_plugin_handle h)
{
	ketsji_engine_data* k = (ketsji_engine_data*) h;
//	KXH_log_entry("KXH_end_draw");
	if((!k) || (!k->kx_initialized)) return;
	PLA_release_gl_context(k->plugin);
}

/* --------------------------------------------------------------------- */
/* Engine control functions                                              */
/* --------------------------------------------------------------------- */

void
APH_redraw(
	APH_application_handle handle
	)
{
	ketsji_engine_data* k = (ketsji_engine_data*) handle;
// 	KXH_log_entry("APH_redraw");
	if(!k) return;
	// make one step progress
	tick(k);
}

void
APH_reset_viewport(
	APH_application_handle handle, 
	int width, 
	int height
	)
{
	ketsji_engine_data* ked = (ketsji_engine_data*) handle;
// 	KXH_log_entry("APH_reset_viewport");
	if((!ked) || (!ked->kx_initialized)) return;

	/* Only sets some width/height (GPC_Canvas), so no lock
	 * needed, I think. */
	ked->canvas_device->Resize(width, height);
}

void
APH_main_file_failed(APH_application_handle h)
{
	ketsji_engine_data* ked = (ketsji_engine_data*) h;
	KXH_log_entry("APH_main_file_failed");
	ked->blendfile_failed = true;
}

void
APH_loading_anim_failed(APH_application_handle h)
{
	ketsji_engine_data* ked = (ketsji_engine_data*) h;
	KXH_log_entry("APH_loading_anim_failed");
	ked->loading_anim_failed = true;
}

void
APH_main_file_loaded(APH_application_handle h,
		     unsigned char * buffer,
		     int size)
{
	ketsji_engine_data* k = (ketsji_engine_data*) h;
	ReportList reports;
	KXH_log_entry("APH_main_file_loaded");

	BKE_reports_init(&reports, 0);
	k->blendfile = BLO_read_from_memory(buffer, 
					    size, 
					    &reports);
	BKE_reports_clear(&reports);

	if (!k->blendfile) 
		k->blendfile_failed = true;
}

void
APH_loading_anim_loaded(APH_application_handle h,
			unsigned char * buffer,
			int size)
{
	ketsji_engine_data* k = (ketsji_engine_data*) h;
	ReportList reports;
	KXH_log_entry("APH_loading_anim_loaded");

	BKE_reports_init(&reports, 0);
	k->loading_anim = BLO_read_from_memory(buffer, 
					       size, 
					       &reports);
	BKE_reports_clear(&reports);

	if (!k->blendfile) 
		k->loading_anim_failed = true;
}


/* --------------------------------------------------------------------- */
/* Implemetation of javascript hooks:                                    */
/* --------------------------------------------------------------------- */
/** send a message to the current gameworld*/
void
APH_send_message(
	APH_application_handle h, 
	char* to, 
	char* from, 
	char* subject, 
	char* body
	)
{
  
	ketsji_engine_data* k = handle_to_data(h);
	STR_String s_to, s_from, s_subject, s_body;

	KXH_log_entry("APH_send_message");

	s_to.Format("%s", to);
	s_from.Format("%s", from);
	s_subject.Format("%s", subject);
	s_body.Format("%f", body);

	if(k->net_dev) {
		// Store a progress message in the network device.



		NG_NetworkMessage* msg = new NG_NetworkMessage(s_to, s_from, s_subject, s_body);
		k->net_dev->SendNetworkMessage(msg);
		msg->Release();
	}

}


/** Prepare to restart with a new url. Streaming is started elsewhere. */
void
APH_restart_with_url(APH_application_handle h)
{
	ketsji_engine_data* k = handle_to_data(h);
	
	KXH_log_entry("APH_restart_with_url");

	std::cerr << " request restart for url " <<  endl;

	k->new_url_requested = true;
	to_state(k, KXH_AWAITING_NEW_STREAM);
	
}

/* --------------------------------------------------------------------- */
/* The progress function:                                                */
/* --------------------------------------------------------------------- */
void
KXH_proceed(ketsji_engine_data* k)
{
//   	KXH_log_entry("KXH_proceed");
  
	// Proceed with a game step. The stand-alone player switches on
	// the drawing context here, but doesn't deactivate it
	// afterwards... Isn't this what the canvas is for? Why are extra
	// context switches needed?
	if (k && k->kx_initialized && k->kx_engine) { 
// 		KXH_log_entry("KXH_proceed:: proceeding");
		k->kx_engine->NextFrame();
		k->kx_engine->Render();
	}

}



/* --------------------------------------------------------------------- */
/* Initialization and other crud                                         */
/* --------------------------------------------------------------------- */
// Note that the engine itself is initialised inside the thread! This
// is necessary to have a valid glx context when initialisation takes
// place. Without that, the rasterizer init runs into problems.
APH_application_handle
APH_create_application(PLA_plugin_handle p_handle)
{
	thread_data_wrap *t;

	KXH_log_entry("APH_create_application");

	t = (thread_data_wrap*) malloc(sizeof(thread_data_wrap));
	t->ked = NULL;
	t->ph = p_handle;

	t->t = PR_CreateThread(PR_USER_THREAD,
			       plugin_mainloop,
			       t,
			       PR_PRIORITY_NORMAL,
			       PR_LOCAL_THREAD,
			       PR_JOINABLE_THREAD, 
			       0);
	if (!t->t) { 
		std::cerr << "Blender3DPlugin: Failed to create "
			"thread for game, bailing." << endl; 
		exit(1); 
	} 

	/* make cond var from this: */
	while (!t->ked) {
#ifdef _WIN32
	  Sleep(20000);
#else
	  usleep(20000);
#endif
	}

	t->ked->thread_data = t;
	
	return (APH_application_handle) t->ked;
	
}


ketsji_engine_data*
init_thread(PLA_plugin_handle p_handle)
{
	ketsji_engine_data* ked = NULL;

	KXH_log_entry("init_thread");
	
	ked = (ketsji_engine_data*) malloc(sizeof(ketsji_engine_data));
	if(!ked) return NULL;	
	ked->plugin = p_handle;
	ked->kx_initialized = false;
	ked->terminate = false;
	ked->termination_request = false;
	ked->replacing_url_requested = false;
	ked->new_url_requested = false;

	ked->blendfile = NULL;
	ked->loading_anim = NULL;
	ked->blendfile_failed = false;
	ked->loading_anim_failed = false;

	ked->state = KXH_INIT;
	ked->termination_request = false;

	ked->mousedevice = NULL;
	ked->keyboarddevice = NULL;
	ked->net_dev = NULL;
	ked->canvas_device = NULL;
	ked->rasterizer = NULL;
	ked->rendertools = NULL;
	ked->audiodevice = NULL;
	ked->converter = NULL;
	
	return ked;

}

void
APH_terminate_application(APH_application_handle handle)
{
	ketsji_engine_data* k = (ketsji_engine_data*) handle;
	PRStatus st;
	
	KXH_log_entry("APH_terminate_application");

	// The request is looped through the cleanup... then the
	// terminate is set, and the thread can end. Maybe add a
	// timeout here?
	k->termination_request = true;

#ifndef _WIN32
	KXH_log_entry("APH_terminate_application:: wait for join");
	st = PR_JoinThread(k->thread_data->t);
	KXH_log_entry("APH_terminate_application:: join done");
	if (st != PR_SUCCESS) {
		std::cerr << "Blender3DPlugin: Critical failure "
			"joining gameengine thread, bailing." << endl;
		exit(1);
	}
	free(k);
#endif

}

void 
open_default_loader(ketsji_engine_data* k)
{
	ReportList reports;
	unsigned char * data;
	int size;

	KXH_log_entry("open_default_loader");

	GetRawLoadingAnimation(&data, &size);
	BKE_reports_init(&reports, 0);
	k->loading_anim = BLO_read_from_memory(data, size, &reports);
	BKE_reports_clear(&reports);
}

void
initialize_gameengine(ketsji_engine_data* k, struct BlendFileData * active_file)
{
	KXH_log_entry("initialize_gameengine");

	// kx does some hidden gl things:
	PLA_acquire_gl_context(k->plugin);

	// underlying blender systems
	GEN_init_messaging_system();
	initglobals();	/* blender.c */

	// This is the platform dependence: 
	KXH_create_devices(k);

	// create the engine
	k->kx_engine = new KX_KetsjiEngine(k->logic_system);

	if (k->kx_engine)
	{	
		k->converter = new
			KX_BlenderSceneConverter(active_file->main, k->kx_engine);
		
		PyObject* dictionaryobject 
			= initGamePlayerPythonScripting("Ketsji", 
						  psl_Highest, 0, NULL);
		if (k->keyboarddevice 
		    && k->mousedevice 
		    && k->net_dev 
		    && k->canvas_device 
		    && k->rasterizer 
		    && k->rendertools 
		    && k->audiodevice 
		    && k->converter 
		    && dictionaryobject) {
			// only continue when everything is ok. 
			KXH_log_entry("initialize_gameengine:: devices ok, connecting...");
			
			k->kx_engine->SetKeyboardDevice(k->keyboarddevice);
			k->kx_engine->SetMouseDevice(k->mousedevice);		
			k->kx_engine->SetNetworkDevice(k->net_dev);
			k->kx_engine->SetCanvas(k->canvas_device);
			k->kx_engine->SetRasterizer(k->rasterizer);
			k->kx_engine->SetRenderTools(k->rendertools);
 			k->kx_engine->SetAudioDevice(k->audiodevice);
			k->kx_engine->SetSceneConverter(k->converter);
			k->kx_engine->SetPyNamespace(dictionaryobject);
		} else {
			// do some error trap;
			std::cerr << "Blender3DPlugin:: could not initialize "
				"gameengine, bailing..." << endl;
			exit(1);
		}

		// do the startscenestuff
		Scene *scene = active_file->curscene;
		char *startsc = scene->id.name + 2;
		STR_String* startSceneName = new STR_String(startsc);
		
		KX_Scene* startscene 
			= new KX_Scene((SCA_IInputDevice*)k->keyboarddevice,
				       (SCA_IInputDevice*)k->mousedevice, k->net_dev,
				       k->audiodevice,
				       startSceneName->Ptr(), scene);
		
		initRasterizer(k->rasterizer, k->canvas_device);;
		PyDict_SetItemString(dictionaryobject, "GameLogic", initGameLogic(k->kx_engine, startscene)); // Same as importing the module
		initGameKeys();
		initPythonConstraintBinding();
		initMathutils();
		initGeometry();
		initBGL();
		
		KXH_log_entry("APH_initialize_gameengine:: will enter kx engine");
		
		k->converter->ConvertScene(
			startSceneName->Ptr(), 
			startscene,
			dictionaryobject,
			k->rendertools,
			k->canvas_device);

		k->kx_engine->AddScene(startscene);
		
		// Inside, the rasterizer is initialised! This
		// requires gl calls...
		k->rasterizer->Init();
		k->kx_engine->StartEngine( true );

		// that's it:
		k->kx_initialized = true;
		KXH_log_entry("APH_initialize_gameengine:: mainloop locked");
	}
	
	PLA_release_gl_context(k->plugin);
}

void 
tick(ketsji_engine_data* k)
{
 	KXH_log_entry("tick");

	if (!k) {
		KXH_log_entry("Engine data not defined!");
		return;
	}

	switch (k->state) {
	case KXH_INIT:
		KXH_log_entry("tick: init");
		set_plain_color(k);
			
		if (PLA_get_loading_animation_url(k->plugin)) {
			// A loading animation url was given:
			// use it. Loading was already
			// initiated elsewhere.
			KXH_log_entry("tick: show fg");
			to_state(k, KXH_SHOW_FG_COLOR);
		} else {
			KXH_log_entry("tick: default loader ");
			open_default_loader(k);
			initialize_gameengine(k, k->loading_anim);
			to_state(k, KXH_PLAY_LOADING_ANIM);
		}
		break;
	case KXH_SHOW_FG_COLOR:
		/* load the animation. */
		set_plain_color(k);			

		// next state: 
		if (k->loading_anim) {
			to_state(k, KXH_TEST_LOADING_FILE_TYPE);
		} else if (k->loading_anim_failed) {
			to_state(k, KXH_GL_ERROR_STATE);
		}
		break;
	case KXH_TEST_LOADING_FILE_TYPE:
		// valid means: must be a publisher file
		if (loading_anim_file_valid(k)){
			initialize_gameengine(k, k->loading_anim);
			to_state(k, KXH_PLAY_LOADING_ANIM);
		} else {
			to_state(k, KXH_GL_ERROR_STATE);
		}
		break;
	case KXH_PLAY_LOADING_ANIM:
		// send a msg to the loading anim :
		update_loading_progress(k);
		KXH_proceed(k);
		// next state:
		if (k->blendfile) {
			// clean up the running engine
			release_resources(k);
			to_state(k, KXH_TEST_MAIN_FILE);
		} else if (k->blendfile_failed) {
			release_resources(k);
			to_state(k, KXH_GL_ERROR_STATE);
		}
		break;
	case KXH_TEST_MAIN_FILE:
		switch (main_file_state(k)) {
		case KXH_MAIN_FILE_IS_PUBLISHER_FILE:
			initialize_gameengine(k, k->blendfile);
			to_state(k, KXH_PLAY_MAIN_FILE);
			break;
		case KXH_MAIN_FILE_IS_NOT_PUBLISHER_FILE:
			initialize_gameengine(k, k->blendfile);
			to_state(k, KXH_ADD_LOGOS);
			break;
		case KXH_MAIN_FILE_CORRUPT:
			to_state(k, KXH_GL_ERROR_STATE);
			break;
		default:
			to_state(k, KXH_GL_ERROR_STATE);
		}
		break;
	case KXH_ADD_LOGOS:
		KXH_add_banners(k);
		to_state(k, KXH_PLAY_MAIN_FILE);
		break;
	case KXH_PLAY_MAIN_FILE:
		// The gameengine's event table bookkeeping
		// interferes with the event chain. Mutex it.
			
		PLA_lock_events(k->plugin);
		KXH_proceed(k);
		PLA_unlock_events(k->plugin);
		
		if (k->replacing_url_requested) {
			to_state(k, KXH_AWAITING_NEW_STREAM);
		}

		break;
	case KXH_AWAITING_NEW_STREAM:
		/* exit, and request transition from javascript */
		/* the exit itself should be effectuated by the
		 * browser? or do we exit ourselves? */
		std::cerr << "Early cleanup awaiting new url" << endl;
		
		to_state(k, KXH_CLEAN_UP);
		

		break;
	case KXH_GL_ERROR_STATE:
		// state transition here is forced from the
		// outside, and caught by the general
		// termination criterium
			
		// for now, just color cycle a bit
		set_plain_color(k);
		break;
	case KXH_CLEAN_UP:
		// this is the actual TERM signal:
		release_resources(k);
			
		if (k->blendfile) {
			BLO_blendfiledata_free(k->blendfile);
			k->blendfile = NULL;
		}

		if (k->loading_anim) {
			BLO_blendfiledata_free(k->loading_anim);
			k->loading_anim = NULL;
		}
		// we may want to add termination of file streaming as well
		k->terminate = true;
		break;
	default: 
		std::cerr << "Blender Plugin entered bad state!!! Exiting before we dump core... " << endl;
		exit(1);
	}

	KXH_log_entry("tick done");

}


void
plugin_mainloop(void* arg)
{
	thread_data_wrap* tdw = (thread_data_wrap*) arg;

	KXH_log_entry("plugin_mainloop");

	// This should let the init_ continue, so we can go on and
	// await the glx context.
	tdw->ked = init_thread(tdw->ph);

	// Wait for window:: 
	PLA_await_window(tdw->ph);

	if (PLA_create_gl_context(tdw->ph)) {
		
		/* This needs to become more sophisticated. */
		tdw->ked->sleep_time = 
			(int) (1000000.0 
			       / (float)PLA_requested_framerate(tdw->ph));
		
		while (!tdw->ked->terminate) {
			
			// we keep it routed through the browser api
			// connection layer, so it is still possible
			// to do async update requests. Threading
			// should be changed a bit for that maybe :/
			// This is looped through to the state engine
			// stepping
			PLA_request_application_progress(tdw->ked->plugin);
			
			// we poll the networkdevice for messages posted by
			// the blend file:
			check_for_messages(tdw->ked);
			
			// adapt for framerate... should be
			// improved...
#ifdef _WIN32 
			Sleep(30 * tdw->ked->sleep_time);
#else
			usleep(tdw->ked->sleep_time);
#endif
		
			// always loop through the cleanup. This also
			// overrides any pending state changes. The cleanup
			// state sets the final termination criterium.
			if (tdw->ked->termination_request) {
				KXH_log_entry(" --- term request seen --- ");
				to_state(tdw->ked, KXH_CLEAN_UP);
			}
		}

		PLA_delete_gl_context(tdw->ph);
		
	}
	KXH_log_entry("Leaving plugin thread");

#ifdef _WIN32
	free(tdw->ked);
#endif

	// After this return, the thread will terminate: 
	return;

}

// ---------------------------------------------------------------------
// Helper functions:
// ---------------------------------------------------------------------

ketsji_engine_data*
handle_to_data(APH_application_handle handle)
{
	return (ketsji_engine_data*) handle;
}


void
check_for_messages(ketsji_engine_data* k)
{
  
	if (k->net_dev) {
		vector<NG_NetworkMessage*> msgs;
		msgs = k->net_dev->RetrieveNetworkMessages();
		if (msgs.size() > 0) {
			vector<NG_NetworkMessage*>::iterator it;
			/* At the moment, we look for messages for the "host_application"
			 * with subject "load_url" only.
			 */
			for (it = msgs.begin(); it != msgs.end(); it++) {
				NG_NetworkMessage* msg = *it;
	
// 				std::cerr << "Message received:" << msg->GetDestinationName()
// 					  << ", " << msg->GetSubject() << endl;

				if (msg->GetDestinationName() == STR_String("host_application")) {
					if (msg->GetSubject() == STR_String("load_url")) {
						request_replacing_url(k, msg->GetMessageText());
					}
				}
			}
		}
	}
}


void
request_replacing_url(ketsji_engine_data* k,
		      const STR_String& url)
{
	int retval;
	char c_url[4096];

	if (url.Length() > 4000) {
		return;
	}

	// this should become an strncpy. And there should be some
	// error handling
	strcpy(c_url, url);

	retval = PLA_request_replacing_url(k->plugin,
					   c_url);

	if (retval) {
		fprintf(stderr, "geturl failed with %d\n", retval);
	}

}

void
set_plain_color(ketsji_engine_data* k) 
{
//	KXH_log_entry("set_plain_color");
	set_plain_color_redraw_func(k);
}

void 
set_plain_color_redraw_func(ketsji_engine_data* k)
{
	
  //	KXH_log_entry("set_plain_color_redraw_func");
	PLA_acquire_gl_context(k->plugin);

#ifndef _WIN32
	float *c = PLA_get_foreground_colour(k->plugin);
 	glClearColor(c[0], c[1], c[2], 1.0);
 	glClear(GL_COLOR_BUFFER_BIT);
#endif

	PLA_swap_gl_context(k->plugin);
	PLA_release_gl_context(k->plugin);

}

bool 
loading_anim_file_valid(ketsji_engine_data* k)
{
	KXH_log_entry("loading_anim_file_valid");

	// no file: always bad
	if (!k->loading_anim) return false;

	// not a publisher file: also bad
	if (k->loading_anim->type != BLENFILETYPE_PUB) return false;

	// valid publisher file: good
	return true;
}

KXH_MAIN_FILE_STATE
main_file_state(ketsji_engine_data* k)
{
	KXH_log_entry("main_file_state");
	if (!k->blendfile) return KXH_MAIN_FILE_CORRUPT;

	if (k->blendfile->type == BLENFILETYPE_PUB) { 
		return KXH_MAIN_FILE_IS_PUBLISHER_FILE;
	} else { 
		return KXH_MAIN_FILE_IS_NOT_PUBLISHER_FILE;
	}
}

void 
update_loading_progress(ketsji_engine_data* k)
{
	float progress = 
		(float) PLA_stream_retrieved(k->plugin) 
		/ (float) PLA_stream_total(k->plugin);

//	KXH_log_entry("update_loading_progress");

	// Communicate with a net-message: 
	STR_String to = "";
	STR_String from = "";
	STR_String subject = "progress";
	STR_String body;
	body.Format("%f", progress);  // a number between 0.0 and 1.0

	if(k->net_dev) {
		// Store a progress message in the network device.
		NG_NetworkMessage* msg = new NG_NetworkMessage(to, from, subject, body);
		k->net_dev->SendNetworkMessage(msg);
		msg->Release();
	}
}

void 
release_resources(ketsji_engine_data* k)
{
	KXH_log_entry("release_resources");

	if (k) {
		if (k->net_dev) {
			k->net_dev->Disconnect();
			delete k->net_dev;
			k->net_dev = NULL;
		}
		if (k->kx_engine) {
			// needs to be guarded if there are other KX-s
			// running!
			exitGamePythonScripting();
			k->kx_engine->StopEngine();
			delete k->kx_engine;
			k->kx_engine = NULL;
		}
		if (k->logic_system) {
			delete k->logic_system;
			k->logic_system = NULL;
		}
		if (k->converter) {
			delete k->converter;
			k->converter = NULL;
		}
		if (k->rasterizer) {
			delete k->rasterizer;
			k->rasterizer = NULL;
		}
		if (k->rendertools) {
			delete k->rendertools;
			k->rendertools = NULL;
		}
		if (k->canvas_device) {
			delete k->canvas_device;
			k->canvas_device = NULL;
		}
		if (k->mousedevice) {
			delete k->mousedevice;
			k->mousedevice = NULL;
		}
		if (k->keyboarddevice) {
			delete k->keyboarddevice;
			k->keyboarddevice = NULL;
		}
 		SND_DeviceManager::Unsubscribe();

		k->kx_initialized = false;
	}
}



// ---------------------------------------------------------------------

void 
static to_state(ketsji_engine_data* k, KXH_STATE i)
{
#ifdef KXH_GENERATE_LOG
	FILE* fp = fopen("/tmp/plugin_log","a");
	if (!fp) return;
	fprintf(fp, "--> KXH_ketsji_hooks:: moving from state %d to state %d \n", k->state, i); 
	fflush(fp);
	fclose (fp);
#endif
	k->state = i;
}

static void
KXH_log_entry(char* msg)
{
#ifdef KXH_GENERATE_LOG
	FILE* fp = fopen("/tmp/plugin_log","a");
	if (!fp) return;
  	fprintf(fp, "--> KXH_ketsji_hooks::(%p) %s\n", 
		PR_GetCurrentThread(),
		msg); 
	fflush(fp);
	fclose (fp);
#endif
}



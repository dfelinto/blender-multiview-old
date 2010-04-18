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
 * X11 windowing for the browser plugin
 */

#if !defined(_WIN32) && !defined(__APPLE__)

#include "PLG_X11_windowing.h"
#include "NZC_application_hooks.h"    /* only for redraw */
#include "NZC_application_callback.h"
#include "KXH_unix_event_sinks.h"

/** Log message */
static void
PLG_log_entry(char* msg);

/* #define PLG_GENERATE_LOG */

/** Register event handlers for key presses, mouse buttons and mouse
 * movement to this widget. Does its own gl and x locking. */
void
register_event_handlers(
	BlenderPluginInstance* inst,
	Widget w
	);
/** Make a GLX context. Return True if it succeeded, False
    otherwise. Does not handle locks. */
Bool
create_gl_context(
	BlenderPluginInstance* inst
	);


/** Release the GLX context, if it exists, and reset the
    reference. Does not handle locks. */
void
delete_glx_context(
	BlenderPluginInstance* inst
	);

/** 1 if GLX is available, 0 if not. Does not handle locks. */
int
check_glx_availability(
	BlenderPluginInstance* inst
	);

/** Redraw the window. */
void
PLB_redraw_callback(
	Widget w,
	XtPointer userData,
	XEvent *ev,
	Boolean *b
	);

/** Register a mouse pointer movement event. */
void
PLB_pointer_motion_callback(
	Widget w,
	XtPointer userData,
	XEvent *ev,
	Boolean *b
	);

/** Register a mouse button press event. */
void
PLB_buttonpress_callback(
	Widget w,
	XtPointer userData,
	XEvent *ev,
	Boolean *b
	);

/** Register a mouse button release event. */
void
PLB_buttonrelease_callback(
	Widget w,
	XtPointer userData,
	XEvent * ev,
	Boolean *b
	);

/** Draw some error strings in the widget. */
void 
PLB_draw_simple_strings_callback(
	Widget w,
	XtPointer userData,
	XEvent *ev,
	Boolean *b
	);

/** Register a key press or release event. */
void 
PLB_keyboard_key_callback(
	Widget w,
	XtPointer userData,
	XEvent *ev,
	Boolean *b
	);


/** Double buffered, with alpha and 16-bit color, please. */
int GL_capabilities[] =
{
	GLX_DOUBLEBUFFER,   // front and back buffers
	GLX_RGBA,           // rgba display
	GLX_DEPTH_SIZE, 8,  // at least 8 bits deep
	GLX_RED_SIZE, 1,    // of which at least 1 red
	GLX_GREEN_SIZE, 1,  // of which at least 1 green
	GLX_BLUE_SIZE, 1,   // of which at least 1 blue
	None
};

extern PRLock* gl_lock;

/* ---------------------------------------------------------------------- */

void
PLG_global_instance_init(
	void
	)
{

	Bool res;
	/* These two calls have to come in conjunction. Presumably,
	 * XtToolkitInitialize() was already called, but doing it
	 * again is harmless. */
	XtToolkitInitialize();
	res = XtToolkitThreadInitialize();
	
	if (res == False) {
		fprintf(stderr,
			"Blender3DPlugin::XtToolkitThreadInitialize: failure\n"
			"Either your OS does not support threading, or\n"
			"your X11 Intrinsics are nor thread-safe.\n");
		exit(1);
	}
	
}

void
PLG_plugin_new_instance_init(
	BlenderPluginInstance * inst
	)
{

	inst->initialized         = 0;
	inst->window_invalidated  = 0;
	inst->display_invalidated = 0;

	inst->x            = 0;
	inst->y            = 0;
	inst->width        = 0;
	inst->height       = 0;
	inst->display_ref  = 0;
	inst->window_ref   = 0;
	
	/* Glx related: */
	inst->ns_widget    = NULL;
	inst->ctx          = NULL;
	inst->vi           = NULL;
	inst->def_vi       = NULL;
	inst->appctx       = NULL;
	inst->gl_version_minor = 0;
	inst->gl_version_major = 0;
	inst->cmap = 0;
	
	inst->have_glx             = 0;
	inst->wrl_locked           = 1;
	inst->reset_event_handlers = 0;

}

/* Sometimes, when this function is called, the event handlers should
 * be reinstated. This function needs to flag the game engine to do
 * so. */
NPError 
PLG_set_platform_window(
	NPP instance,
	NPWindow* window,
	BlenderPluginInstance * inst
	)
{
	XtAppContext ac;
	Widget new_widget;

	PLG_log_entry("PLG_set_platform_window");

	if (((Window) window->window) != inst->window_ref) {
		inst->window_ref = (Window) window->window;
		inst->width = window->width;
		inst->height = window->height;
		inst->window_invalidated = 1;

	}
	if (((NPSetWindowCallbackStruct *) window->ws_info)->display != inst->display_ref) {
		inst->display_ref = ((NPSetWindowCallbackStruct *) window->ws_info)->display;
		inst->width = window->width;
		inst->height = window->height;
		inst->display_invalidated = 1;
	}
	
	if ( (!inst->initialized)
	     || inst->window_invalidated
	     || inst->display_invalidated) {
		/* Case 1 & 2: initialize/ window invalidated  */
		
		
		/* Get a new app context, and see whether it changed
		 * wrt. what it was. I think it should be constant for
		 * one app. This is potentially dangerous, since these
		 * are X calls outside the lock. */
		new_widget = XtWindowToWidget(inst->display_ref,
					      (Window) window->window);

		ac = XtWidgetToApplicationContext(new_widget);

		if (inst->appctx
		    && (inst->appctx != ac ) ) {
			fprintf(stderr, "Application context "
				"changed unexpectedly!!! Bailing...");
			fflush(stderr);
			exit(1);
		}
		
		PR_Lock(gl_lock);
		inst->appctx = ac;
		XtAppLock(inst->appctx);

		
		PR_Lock(inst->event_lock);
		inst->ns_widget = XtWindowToWidget(inst->display_ref, 
						   inst->window_ref);
		inst->reset_event_handlers = 1;
		PR_Unlock(inst->event_lock);


		if (!inst->ns_widget) {
			fprintf(stderr, "PLG_SetWindow:: ns widget "
				"was null! Bailing...\n");
			exit(1);
		}

		if (!inst->ctx && inst->wrl_locked) {
			/* ready to start drawing */
			PR_Unlock(inst->window_ready_lock);
			inst->wrl_locked = 0;
		}

		XMapWindow(inst->display_ref, 
			   inst->window_ref);

		inst->initialized = 1;
		inst->window_invalidated = 0;
		inst->display_invalidated = 0;

		XtAppUnlock(inst->appctx);
		PR_Unlock(gl_lock);  
		
	} else {
		PLG_log_entry("PLG_set_platform_window:: nothing to do");
	}

	return NPERR_NO_ERROR;
}



/* The redraw() can be dangerous at times.... I'll keep it for now,
 * but it may have to go for the threaded version. It seems it's
 * called _after_ the destruction of the plugin! */
void
PLB_redraw_callback(
	Widget w,
	XtPointer userData,
	XEvent * ev,
	Boolean *b
	)
{
#ifdef PLG_UNTHREADED_TEST
  	BlenderPluginInstance* inst = (BlenderPluginInstance*) userData; 
#endif
	
	PLG_log_entry("PLG_redraw_callback");
  	PLG_log_entry("suppressed"); 
	
	
#ifdef PLG_UNTHREADED_TEST
	/* no redraw if there's the slightest hint of something amiss */
	if ((w != inst->ns_widget) 
	    || (!inst->ctx) 
	    || inst->display_invalidated 
	    || inst->window_invalidated) {
		PLG_log_entry("suppressed due to bad display refs"); 
		return;
	}
	
  	APH_redraw(inst->application);
#endif

}

void
PLB_pointer_motion_callback(
	Widget w,
	XtPointer userData,
	XEvent * ev,
	Boolean *b
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) userData;
	XMotionEvent *xme = &(ev->xmotion);
/* 	PLG_log_entry("PLG_pointer_motion_callback"); */
	
	PR_Lock(inst->event_lock);
  	APH_mouse_movement(inst->application,
			   xme->x,
			   xme->y);
	PR_Unlock(inst->event_lock);

}


void
PLB_buttonpress_callback(
	Widget w,
	XtPointer userData,
	XEvent * ev,
	Boolean *b
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) userData;
  	XButtonEvent *xbe = &(ev->xbutton);

/* 	PLG_log_entry("PLG_buttonpress_callback"); */
	PR_Lock(inst->event_lock);
	APH_mousekey_press(inst->application, xbe->button); 
	PR_Unlock(inst->event_lock);
	
}

void
PLB_buttonrelease_callback(
	Widget w,
	XtPointer userData,
	XEvent * ev,
	Boolean *b
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) userData;  
	XButtonEvent *xbe = &(ev->xbutton);

/* 	PLG_log_entry("PLG_buttonrelease_callback"); */
	PR_Lock(inst->event_lock);
  	APH_mousekey_release(inst->application, xbe->button);
	PR_Unlock(inst->event_lock);
	
}

void
PLB_keyboard_key_callback(
	Widget w,
	XtPointer userData,
	XEvent * ev,
	Boolean *b
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) userData;
	XEvent pending_event;
/*  	XtAppContext widget_context;  */
	XKeyEvent *xke = &(ev->xkey);
	XKeyEvent *xpke;
	KeySym key_sym = XLookupKeysym(xke,0);
	
/* 	PLG_log_entry("PLG_keyboard_key_callback"); */
	
	if (xke->type == KeyPress) {
		PR_Lock(inst->event_lock);
		APH_keyboard_press(inst->application, key_sym); 
		PR_Unlock(inst->event_lock);
	} else if (xke->type == KeyRelease) {
		/* This is a hack to work around X's stupid key
		 * repeat. A key repeat is done by emitting two events
		 * on the same timestamp: a key release, and the new
		 * key press. */

/*  		widget_context = XtWidgetToApplicationContext(inst->ns_widget);  */
		
 		if (XtAppPending(inst->appctx)) { 
 			XtAppPeekEvent(inst->appctx, &pending_event); 
			if (pending_event.type == KeyPress) {
				xpke = &(pending_event.xkey);
				if ((xpke->time == xke->time)
				    && (XLookupKeysym(xpke, 0) == key_sym) ) {
					return; /* just drop out... */
				}
			}
		}
		PR_Lock(inst->event_lock);
		APH_keyboard_release(inst->application, key_sym);
		PR_Unlock(inst->event_lock);
	}
}

/* This redraw callback handles the case of not having gl access. */
void
PLB_draw_simple_strings_callback(
	Widget w,
	XtPointer userData,
	XEvent * ev,
	Boolean *b
	)
{
	GC gc;
	XGCValues gcv;
	char *text2  = "This display does not seem to support GLX.";
	char *text   = "We cannot render without OpenGL support.";
	BlenderPluginInstance* inst;
	/* ev is ignored here */

	PLG_log_entry("PLG_draw_simple_strings_callback");

	inst = (BlenderPluginInstance*) userData;

	/* Lock X! */
	XtAppLock(inst->appctx);
	
	XtVaGetValues (w,
		       XtNbackground,
		       &gcv.background,
		       XtNforeground,
		       &gcv.foreground,
		       0);
	gc = XCreateGC (inst->display_ref,
			inst->window_ref,
			GCForeground | GCBackground,
			&gcv);

	XDrawRectangle (inst->display_ref,
			inst->window_ref,
			gc,
			2,
			2,
			inst->width - 4,
			inst->height - 4);
	XDrawString (inst->display_ref, 
		     inst->window_ref, 
		     gc,
		     10,
		     40,
		     text, 
		     strlen (text));
	XDrawString (inst->display_ref, 
		     inst->window_ref, 
		     gc,
		     10,
		     20,
		     text2, 
		     strlen (text2));
	/* and release... */
	XtAppUnlock(inst->appctx);

}


/* There can be only one current gl context, so we lock access to
 * it. Although it is good for the app to handle not being able to
 * acquire the context itself, sometimes a blocking call is really
 * needed. Note that failure from this function means there is no
 * usable gl context! There is only one lock so far, and that needs to
 * change for multiple plugins. */
int
PLA_acquire_gl_context(
	PLA_plugin_handle i
	)
{

	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	Bool result = False;
/*  	PLG_log_entry("PLG_acquire_glx_context"); */

	PR_Lock(gl_lock);
	XtAppLock(inst->appctx);
	PLG_log_entry("PLG_acquire_gl_context:: gl and app locked ");
	
	if ((!inst->display_ref)
	    || (!inst->window_ref)
	    || (!inst->ctx)) {
		fprintf(stderr, "Blender plugin encountered fatal "
			"windowing error! Exiting... \n");
		exit(1); /* this shouldn't happen, really. */
	}

	/* This one still fails sometimes on startup? gives glx bad
	 * drawables. Why? Some errors arise from the NVidia
	 * drivers... Setting __GL_SINGLE_THREADED helps. */
	result = glXMakeCurrent(inst->display_ref,  // Display*
				inst->window_ref,   // GLXDrawable
				inst->ctx);         // GLXContext

	
	if (result == False) {
		fprintf(stderr, "Blender plugin encountered fatal "
			"glx error! Exiting... \n");
		exit(1); /* this shouldn't happen, really. */
	}
	/* For now, we always return 1. Any error condition will cause
	 * an exit(). */
	return 1;
}


void
PLA_swap_gl_context(
	PLA_plugin_handle i
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	
/* 	PLG_log_entry("PLG_swap_glx_context"); */
	
	glXSwapBuffers(inst->display_ref,
		       inst->window_ref);
}

void
PLA_release_gl_context(
	PLA_plugin_handle i
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	Bool result = False;
	
/* 	PLG_log_entry("PLG_release_glx_context"); */

	result = glXMakeCurrent(inst->display_ref,
				None,
				NULL);
	if (result == False) {
		PLG_log_entry("PLG_acquire_glx_context:: releasing glx failed");
	} 

	XtAppUnlock(inst->appctx);
	PR_Unlock(gl_lock);
}

/** Get the width of the display window. */
int
PLA_get_display_width(
	PLA_plugin_handle i
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	PLG_log_entry("PLG_get_display_width");
	return inst->width;
}

/** Get the height of the display window. */
int
PLA_get_display_height(PLA_plugin_handle i)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	PLG_log_entry("PLG_get_display_height");
	return inst->height;
}


void
PLA_request_application_progress(
	PLA_plugin_handle i
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;

	/* xtapp locks may be nested, which is nice */
	XtAppLock(inst->appctx);
	
	if (inst->reset_event_handlers) {
		PR_Lock(inst->event_lock);
		register_event_handlers(inst,
					inst->ns_widget);
		inst->reset_event_handlers = 0;
		PR_Unlock(inst->event_lock);

	}
	
	/* 	log_entry("step_application"); */
	if (inst->window_ref != XtWindow(inst->ns_widget)) {
		fprintf(stderr, "window not correct?\n");
		fflush(stderr);
/* 		APH_redraw(inst->application); */
	} else {
		APH_redraw(inst->application);
	}

	XtAppUnlock(inst->appctx);
	
}

int
PLA_create_gl_context(
	PLA_plugin_handle i
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
	Bool retval;
	
	PR_Lock(gl_lock);
	XtAppLock(inst->appctx);
	
	retval = create_gl_context(inst);

	XtAppUnlock(inst->appctx);
	PR_Unlock(gl_lock);

	return ((retval==True)? 1 : 0);

}

void 
PLA_delete_gl_context(
	PLA_plugin_handle i
	)
{
	BlenderPluginInstance* inst = (BlenderPluginInstance*) i;
  
	PR_Lock(gl_lock);
	XtAppLock(inst->appctx);

	/* X should do this when the parent is destroyed, but some drivers
	 * don't handle that properly. */
	delete_glx_context(inst);
	
	XtAppUnlock(inst->appctx);
	PR_Unlock(gl_lock);

}


void
register_event_handlers(
	BlenderPluginInstance* inst,
	Widget w
	)
{
	PLG_log_entry("register_event_handlers");

/* 		XtAddEventHandler(w, */
/* 				  ExposureMask, */
/* 				  FALSE, */
/* 				  PLB_redraw_callback, */
/* 				  (XtPointer) inst); */

	/* key handler: both press and release */
	XtAddEventHandler(w,
			  KeyPressMask + KeyReleaseMask,
			  FALSE,
			  PLB_keyboard_key_callback,
			  (XtPointer) inst);
	
	/* mouse keys: down*/
	XtAddEventHandler(w,
			  ButtonPressMask,
			  FALSE,
			  PLB_buttonpress_callback,
			  (XtPointer) inst);
	
	/* mouse keys: up */
	XtAddEventHandler(w,
			  ButtonReleaseMask,
			  FALSE,
			  PLB_buttonrelease_callback,
			  (XtPointer) inst);

	/* mouse movement: */
	XtAddEventHandler(w,
			  PointerMotionMask,
			  FALSE,
			  PLB_pointer_motion_callback,
			  (XtPointer) inst);

}

Bool
create_gl_context(
	BlenderPluginInstance* inst
	)
{
	int matches = 0;
	long mask = VisualNoMask;

	PLG_log_entry("PLG_implementation::create_gl_context");

	/* Before we continue, we must be certain GLX is supported. */
	if (!check_glx_availability(inst)) {
		inst->have_glx = 0;
		/* No gl! That should be told to the engine! */
		XtAddEventHandler(
			inst->ns_widget,
			ExposureMask,
			FALSE,
			PLB_draw_simple_strings_callback,
			(XtPointer) inst
			);
		return False; 
	} else {
		inst->have_glx = 1;
	}

	/* This is what we'd like to have... How about allowing
         * different GL props? */
	inst->vi= glXChooseVisual(inst->display_ref,
				  DefaultScreen(inst->display_ref),
				  GL_capabilities); 

	if (!inst->vi) {
		fprintf(stderr, "Blender3DPlugin: "
			"Could not get glX visual, bailing.\n");
		exit(1);
	}
	
	/* ... and this is what we can get. How many matches are needed
	 * for a usable context? If the vi doesn't match, a bad match
	 * error arises, which terminates the app! */
	inst->def_vi = XGetVisualInfo(inst->display_ref,
				      mask,
				      inst->vi, 
				      &matches);

	if (!inst->def_vi) {
		fprintf(stderr, "Blender3DPlugin: "
			"Could not match glX visual to X, bailing.\n");
		exit(1);
	}
	
	/* The last arg determines whether or not to allow direct access,
	 * or only through the X server. Arg 3 determines whether we can
	 * share displaylists. It may be set to NULL if sharing gives
	 * problems.*/
	inst->ctx = glXCreateContext(inst->display_ref,
				     inst->def_vi,
				     NULL, //DefaultScreen(inst->display_ref), 
				     False); // use direct connection if possible?

	/* Do the event handlers here. This means they only get done
	 * when we have gl. We should do the draw handler when this
	 * returns false: */
	PR_Lock(inst->event_lock);
	register_event_handlers(inst,
				inst->ns_widget);
	inst->reset_event_handlers = 0;
	PR_Unlock(inst->event_lock);

	/* Not used? It has side effects! But it does not seem to be
	 * missed. Better leave it in, because non-16 bit displays
	 * will likely need it.*/
	inst->cmap = XCreateColormap(inst->display_ref,
				     inst->window_ref,
				     inst->vi->visual,
				     AllocNone);

	XFree(inst->vi);
	XFree(inst->def_vi);
	return (inst->ctx) ? True: False;
}

void
delete_glx_context(
	BlenderPluginInstance* inst
	)
{
	PLG_log_entry("PLG_implementation::delete_glx_context");

	if(inst->ctx && inst->display_ref) {
		/* This is only done when the plugin exits. If this
		 * fails, we cannot do anything about it, really. */
		glXMakeCurrent(inst->display_ref,
			       None,
			       NULL);

		glXDestroyContext(inst->display_ref,
				  inst->ctx);
		inst->ctx = NULL;
	}
}

/* Simple check. We need a valid XVisualInfo for this test.  */
int
check_glx_availability(
	BlenderPluginInstance* inst
	)
{
	Bool res;

	
	PLG_log_entry("PLG_implementation::check_glx_availability"); 

	if (!inst || !inst->display_ref || !inst->window_ref) return 0;
	
	/* This should move to the other thread as well. */
	res = glXQueryExtension(inst->display_ref,
				NULL,
				NULL);

	if (res == True) {
		/* Also retrieve versions: */
		PLG_log_entry("PLG_implementation:: have gl"); 
		res = glXQueryVersion(inst->display_ref,
				      &(inst->gl_version_major),
				      &(inst->gl_version_minor));
		return 1;
	} else {
		return 0;
	};
}

static void
PLG_log_entry(
	char* msg
	)
{
#ifdef PLG_GENERATE_LOG
	FILE* fp;

	fp = fopen("/tmp/plugin_log","a");
	if (!fp) return;
  	fprintf(fp, "--> PLG_implementation::(%p) %s\n", 
		PR_GetCurrentThread(),
		msg); 
	fflush(fp);
	fclose (fp);
#endif
}

/* ---------------------------------------------------------------------- */
#endif /* !defined(_win32) && !defined(__APPLE__) */

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
 * The ubiquitous gears, threaded as test for the plugin frame. 
 */


/*
 * 3-D gear wheels.  This program is in the public domain.
 *
 * Command line options:
 *    -info      print GL implementation information
 *
 *
 * Brian Paul
 */

/* Conversion to GLUT by Mark J. Kilgard */

/*
 * $Log$
 * Revision 1.5  2007/04/04 13:18:38  campbellbarton
 * moved source and text to american spelling
 * * colour -> color
 * * centre -> center
 * * normalise -> normalize
 * * modelling -> modeling
 *
 * Revision 1.4  2004/03/22 22:01:50  jesterking
 * [GameEngine] Commit all Kester's changes made to the gameengine to restore 2.25 like physics.
 * [SCons] Build with Solid as default when enabling the gameengine in the build process
 * [SCons] Build solid and qhull from the extern directory and link statically against them
 *
 * That was about it.
 *
 * There are a few things that needs double checking:
 *
 * * Makefiles
 * * Projectfiles
 * * All the other systems than Linux and Windows on which the build (with scons) has been successfully tested.
 *
 * Revision 1.1.1.1  2003/08/10 20:22:32  desaster
 * Import of Blender/cvs 2003/08/10
 *
 * Revision 1.3  2002/11/25 12:02:14  mein
 *
 *
 * updated .c files to include:
 * #ifdef HAVE_CONFIG_H
 * #include <config.h>
 * #endif
 *
 * Just need to finish cpp files now :)
 *
 * Kent
 * --
 * mein@cs.umn.edu
 *
 * Revision 1.2  2002/10/13 15:57:16  hans
 * - mention the GPL the right way
 * - extend NaN Holding copyright to its proper startup
 *
 * Revision 1.1.1.1  2002/10/12 11:37:45  hans
 * For the open source Blender project we start with this fresh repository.
 * These are the NaN 2.25 Blender sources, slightly cleaned up, and stripped
 * from parts that could not be open sourced. Enjoy!
 *
 * Revision 1.7  2002/01/31 13:23:41  nzc
 * - Removed old threading remnants.
 * - Split off KXH windowing-specific parts.
 * - Split off browser windowing specific parts.
 *
 * Revision 1.6  2002/01/29 12:40:24  nzc
 * - Make Irix compiler happy.
 *
 * Revision 1.5  2002/01/29 11:57:31  nzc
 * - Switched plugin over to NSPR for threading.
 * - Cleaned up some more dead code.
 * - Cleaned up redundant header inclusion.
 * - Adapted tests to the new situation.
 *
 * Revision 1.4  2002/01/04 17:02:56  nzc
 * - Removed double mutex unlock from plain color drawing func.
 * - Renamed functions to better reflect the grouping. See comments in
 *   UnixShell.c for details.
 *
 * Revision 1.3  2001/12/12 11:32:21  nzc
 * - Added simple tag argument parser.
 * - Cleaned up code a bit.
 *
 * Revision 1.2  2001/12/07 10:49:43  nzc
 * - Switched mime type to application/x-blender-plugin, which is the same as the
 *   windows plugin.
 * - Concentrated drawing in the game engine in a more compact chunk. You can
 *   now skip drawing for a certain frame easily. Audio is also wrapped in
 *   that chunk. Janco: maybe separate loop for the audioo scenes?
 * - Adapted plugin-application interface to match use by Blender canvases.
 * - Adapted canvas to support error reporting on acquiring canvases.
 * - Adapted tests to run with the new apis.
 *
 * Revision 1.1  2001/12/03 12:21:07  nzc
 * - Added threading to the gears test. Works on FreeBSD and Linux :)
 *
 * Revision 1.1  2001/11/29 13:39:51  nzc
 * - Added more x fixes.
 * - Split up the tests into distinct parts.
 * - Use make ptest to make a gears plugin test on linux or freebsd.
 * - Works on Linux and FreeBSD now :)
 *
 * Revision 1.5  2001/11/23 15:24:42  nzc
 * - Ironed out a dozen little details for X/glX.
 * - Added more tracing hooks.
 *
 * Revision 1.4  2001/11/23 12:41:31  nzc
 * - Changed stupid common and unix lib names to plugin_common and
 *   plugin_common_unix.
 * - Abstracted plugin to application wrapper.
 * - Adapted test to comply to wrapper interface.
 *
 * Revision 1.3  2001/11/21 18:20:26  nzc
 * - Applied some ifdef's to tracing code for ease of use.
 * - Removed some unnecessary/unused variables and code.
 *
 * Revision 1.2  2001/11/21 13:33:22  nzc
 * - Some cleanup of comments, includes and headers.
 * - Added first part of X to KX event conversion.
 *
 * Revision 1.1  2001/11/20 13:08:23  nzc
 * - Added new ns plugin frame. This one has a gears-test.
 *
 * Revision 1.2  1999/10/21 16:39:06  brianp
 * added -info command line option
 *
 * Revision 1.1.1.1  1999/08/19 00:55:40  jtg
 * Imported sources
 *
 * Revision 3.2  1999/06/03 17:07:36  brianp
 * an extra quad was being drawn in front and back faces
 *
 * Revision 3.1  1998/11/03 02:49:10  brianp
 * added fps output
 *
 * Revision 3.0  1998/02/14 18:42:29  brianp
 * initial rev
 *
 */

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <GL/glut.h>
#include <X11/keysym.h>
#include <X11/Intrinsic.h>


#include "NZC_application_hooks.h"
#include "NZC_application_callback.h"
#include "prthread.h"
#include "prlock.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265
#endif

/* if defined: generate a log file */
#define NZC_GENERATE_LOG

struct _thread_data_wrap;

typedef struct _gears_data {

	int terminate;
	PLA_plugin_handle plugin;

	struct _thread_data_wrap* thread_data;
	
	GLfloat pos[4];
	GLfloat red[4];
	GLfloat green[4];
	GLfloat blue[4];

	
	GLint Frames;

	GLfloat view_rotx, view_roty, view_rotz;
	GLint gear1, gear2, gear3;
	GLfloat angle;
	
} gears_data;

typedef struct _thread_data_wrap {
	PRThread* t;
	gears_data* gd;
	PLA_plugin_handle ph;
} thread_data_wrap;

/** Draw a set of gears. */
void
gear(GLfloat inner_radius,
	 GLfloat outer_radius,
	 GLfloat width,
	 GLint teeth,
	 GLfloat tooth_depth);

/** Make progress */
void
TST_proceed(gears_data* h);

/** Add an entry to the log. */
static void
TST_log_entry(char* msg);
static void
TST_log_pointer(void* p);

/** Prepare the display lists. Needs glx resource lock */
void make_display_list(gears_data* h);

/** plugin main */
void
plugin_mainloop(void* arg);

gears_data*
init_thread(PLA_plugin_handle p_handle);

/**

  Draw a gear wheel.  You'll probably want to call this function when
  building a display list since we do a lot of trig here.
 
  Input:  inner_radius - radius of hole at center
          outer_radius - radius at center of teeth
          width - width of gear
          teeth - number of teeth
          tooth_depth - depth of tooth

 **/

void
gear(GLfloat inner_radius,
	 GLfloat outer_radius,
	 GLfloat width,
	 GLint teeth,
	 GLfloat tooth_depth)
{
	GLint i;
	GLfloat r0, r1, r2;
	GLfloat angle, da;
	GLfloat u, v, len;

	TST_log_entry("gear");
	
	r0 = inner_radius;
	r1 = outer_radius - tooth_depth / 2.0;
	r2 = outer_radius + tooth_depth / 2.0;

	da = 2.0 * M_PI / teeth / 4.0;

	glShadeModel(GL_FLAT);

	glNormal3f(0.0, 0.0, 1.0);

	/* draw front face */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;
		glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
		glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
		if (i < teeth) {
			glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
			glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
		}
	}
	glEnd();

  /* draw front sides of teeth */
	glBegin(GL_QUADS);
	da = 2.0 * M_PI / teeth / 4.0;
	for (i = 0; i < teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;

		glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
	}
	glEnd();

	glNormal3f(0.0, 0.0, -1.0);

	/* draw back face */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;
		glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
		if (i < teeth) {
			glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
			glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
		}
	}
	glEnd();

  /* draw back sides of teeth */
	glBegin(GL_QUADS);
	da = 2.0 * M_PI / teeth / 4.0;
	for (i = 0; i < teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;

		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
		glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
	}
	glEnd();

  /* draw outward faces of teeth */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i < teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;

		glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
		glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
		u = r2 * cos(angle + da) - r1 * cos(angle);
		v = r2 * sin(angle + da) - r1 * sin(angle);
		len = sqrt(u * u + v * v);
		u /= len;
		v /= len;
		glNormal3f(v, -u, 0.0);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
		glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
		glNormal3f(cos(angle), sin(angle), 0.0);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
		glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
		u = r1 * cos(angle + 3 * da) - r2 * cos(angle + 2 * da);
		v = r1 * sin(angle + 3 * da) - r2 * sin(angle + 2 * da);
		glNormal3f(v, -u, 0.0);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
		glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
		glNormal3f(cos(angle), sin(angle), 0.0);
	}

	glVertex3f(r1 * cos(0), r1 * sin(0), width * 0.5);
	glVertex3f(r1 * cos(0), r1 * sin(0), -width * 0.5);

	glEnd();

	glShadeModel(GL_SMOOTH);

	/* draw inside radius cylinder */
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= teeth; i++) {
		angle = i * 2.0 * M_PI / teeth;
		glNormal3f(-cos(angle), -sin(angle), 0.0);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
		glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
	}
	glEnd();

}

void make_display_list(gears_data* h)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();
	glRotatef(h->view_rotx, 1.0, 0.0, 0.0);
	glRotatef(h->view_roty, 0.0, 1.0, 0.0);
	glRotatef(h->view_rotz, 0.0, 0.0, 1.0);

	glPushMatrix();
	glTranslatef(-3.0, -2.0, 0.0);
	glRotatef(h->angle, 0.0, 0.0, 1.0);
	glCallList(h->gear1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3.1, -2.0, 0.0);
	glRotatef(-2.0 * h->angle - 9.0, 0.0, 0.0, 1.0);
	glCallList(h->gear2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-3.1, 4.2, 0.0);
	glRotatef(-2.0 * h->angle - 25.0, 0.0, 0.0, 1.0);
	glCallList(h->gear3);
	glPopMatrix();

	glPopMatrix();

}


void
APH_redraw(APH_application_handle handle)
{
	gears_data* h = (gears_data*) handle;
	
	TST_log_entry("NZC_redraw");
	TST_log_entry("NZC_redraw:: plugin handle");
	TST_log_pointer(h->plugin);

	if(PLA_acquire_glx_context(h->plugin)) {
		make_display_list(h);
		PLA_swap_glx_context(h->plugin);
		PLA_release_glx_context(h->plugin);
	}
	h->Frames++;
}




void
TST_proceed(gears_data* h)
{
	TST_log_entry("TST_proceed");
	h->angle += 2.0;
}

/** Accept an X key release event */
void APH_keyboard_release(APH_application_handle handle, KeySym key)
{
	gears_data* h = (gears_data*) handle;

	TST_log_entry("NZC_keyboard_release");
	h->angle += 2.0;
}

/** Accept an X mousebutton press event */
void APH_mousekey_press(APH_application_handle handle, int xbut)
{
	gears_data* h = (gears_data*) handle;

	TST_log_entry("NZC_mousekey_press");
	h->angle += 2.0;
}

/** Accept an X mousebutton release event */
void APH_mousekey_release(APH_application_handle handle, int xbut)
{
	gears_data* h = (gears_data*) handle;

	TST_log_entry("NZC_mousekey_release");
	h->angle += 2.0;
}

/** Accept an X mouse movement event */
void APH_mouse_movement(APH_application_handle handle, int x, int y)
{
	gears_data* h = (gears_data*) handle;

	TST_log_entry("NZC_mouse_movement");
	h->angle += 2.0;

}


void
APH_keyboard_press(APH_application_handle handle, KeySym key)
{
	gears_data* h = (gears_data*) handle;

	TST_log_entry("NZC_keyboard_press");
	
	switch (key) {
	case XK_Up:
		h->view_rotx += 5.0;
		break;
	case XK_Down:
		h->view_rotx -= 5.0;
		break;
	case XK_Left:
		h->view_roty += 5.0;
		break;
	case XK_Right:
		h->view_roty -= 5.0;
		break;
	case XK_z:
		h->view_rotz += 5.0;
		break;
	case XK_x:
		h->view_rotz -= 5.0;
		break;
	case XK_Escape: 
		h->terminate = 1;
		break;
	default:
		/* nothing*/
		;
	}
}


/* new context: do inits  */
void
APH_reset_viewport(APH_application_handle handle, int width, int height)
{
	gears_data* gd = (gears_data*) handle;
	GLfloat h = (GLfloat) height / (GLfloat) width;

	TST_log_entry("NZC_set_viewport");

	if(!PLA_acquire_glx_context(gd->plugin)) return;

	glViewport(0, 0, (GLint) width, (GLint) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -40.0);

	// redo the displaylists
	glLightfv(GL_LIGHT0, GL_POSITION, gd->pos);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	/* make the gears */
	gd->gear1 = glGenLists(1);
	glNewList(gd->gear1, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gd->red);
	gear(1.0, 4.0, 1.0, 20, 0.7);
	glEndList();

	gd->gear2 = glGenLists(1);
	glNewList(gd->gear2, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gd->green);
	gear(0.5, 2.0, 2.0, 10, 0.7);
	glEndList();

	gd->gear3 = glGenLists(1);
	glNewList(gd->gear3, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gd->blue);
	gear(1.3, 2.0, 0.5, 10, 0.7);
	glEndList();

	glEnable(GL_NORMALIZE);

	PLA_release_glx_context(gd->plugin);

}

APH_application_handle
APH_create_application(PLA_plugin_handle p_handle)
{
	thread_data_wrap *t;
	
	TST_log_entry("NZC_create_application");

	t = (thread_data_wrap*) malloc(sizeof(thread_data_wrap));
	
	t->gd = NULL;
	t->ph = p_handle;

//	TST_log_entry("NZC_create_application:: starting thread");
	
	t->t = PR_CreateThread(PR_USER_THREAD,
			       plugin_mainloop,
			       t,
			       PR_PRIORITY_NORMAL,
			       PR_LOCAL_THREAD,
			       PR_JOINABLE_THREAD, 
			       0);

	if (!t->t) { 
		fprintf(stderr, "Blender3DPlugin: Failed to create "
			"thread for game, bailing."); 
		exit(1); 
	} 

//	TST_log_entry("NZC_create_application:: will wait for data");

	while (!t->gd) {
		/* stupid wait.... for now.... Later on, this can be synced with a mutex */
//		TST_log_entry("NZC_create_application:: not found");
		usleep(20000);
	}

//	TST_log_entry("NZC_create_application:: done: unlock and exit");

	t->gd->thread_data = t;
	
	return (APH_application_handle) t->gd;
	
}




gears_data*
init_thread(PLA_plugin_handle p_handle)
{
	gears_data* gd = NULL;

	TST_log_entry("NZC_create_application");
	
	gd = (gears_data*) malloc(sizeof(gears_data));
	if(!gd) return NULL;
	
	gd->terminate = 0;

	gd->plugin = p_handle;

	gd->Frames = 0;
	gd->view_rotx = 20.0;
	gd->view_roty = 30.0;
	gd->view_rotz = 0.0;
	gd->angle = 0.0;

	gd->pos[0]   = 5.0;
	gd->pos[1]   = 5.0;
	gd->pos[2]   = 10.0;
	gd->pos[3]   = 0.0;

	gd->red[0]   = 0.8;
	gd->red[1]   = 0.1;
	gd->red[2]   = 0.0;
	gd->red[3]   = 1.0;

	gd->green[0]   = 0.0;
	gd->green[1]   = 0.8;
	gd->green[2]   = 0.2;
	gd->green[3]   = 1.0;

	gd->blue[0]   = 0.2;
	gd->blue[1]   = 0.2;
	gd->blue[2]   = 1.0;
	gd->blue[3]   = 1.0;

	return  gd;

}

void
APH_terminate_application(APH_application_handle handle)
{
	gears_data* h = (gears_data*) handle;
	int res;
	PRStatus st;
	
	TST_log_entry("NZC_terminate_application");

	/* cancel the thread first: */


	h->terminate = 1;
	
	/* and join it? */
	fprintf(stderr, "Joining %p\n", h->thread_data->t);
	st = PR_JoinThread(h->thread_data->t);
	if (st != PR_SUCCESS)
	{
		fprintf(stderr, "Bad join\n");
		exit(1);
	}
	
	TST_log_entry("NZC_terminate_application:: joined");
	free(h);

}

void
plugin_mainloop(void* arg)
{
	thread_data_wrap* tdw = (thread_data_wrap*) arg;

	TST_log_entry("plugin_mainloop");

	/* this should let the init_ continue... */
	tdw->gd = init_thread(tdw->ph);

	TST_log_entry("plugin_mainloop:: sleep a while (3)");
	sleep(3);
	
  	while (!tdw->gd->terminate) { 
		TST_log_entry("plugin_mainloop -- tick");
		TST_proceed(tdw->gd);
		APH_redraw((APH_application_handle)tdw->gd);
		TST_log_entry("plugin_mainloop -- hibernate");
  		sleep(2); 
	}
	
}


static void
TST_log_entry(char* msg)
{
#ifdef NZC_GENERATE_LOG
	FILE* fp = fopen("plugin_log","a");
	if (!fp) return;
  	fprintf(fp, "--> TST_gears::%s\n", msg); 
	fflush(fp);
	fclose (fp);
#endif
}

static void
TST_log_pointer(void* p)
{
#ifdef NZC_GENERATE_LOG
	FILE* fp = fopen("plugin_log","a");
	if (!fp) return;
  	fprintf(fp, "--> TST_gears::pointer::%p\n", p); 
	fflush(fp);
	fclose (fp);
#endif
}

/* eof */


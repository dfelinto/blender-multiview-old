
/*
 * Python Blender Window module
 *
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


#include "Python.h"
#include "BPY_macros.h"
#include "b_interface.h"
#include "BPY_tools.h"
#include "BPY_main.h"
#include "BPY_window.h"

#include "BSE_headerbuttons.h"

#include "BIF_screen.h"               // curarea
#include "BIF_space.h"                // allqueue()
#include "BIF_drawtext.h"             // pop_space_text
#include "mydevice.h"                 // for all the event constants 

#include "opy_datablock.h"
#include "opy_nmesh.h"

#include "DNA_view3d_types.h"
#include "DNA_space_types.h"


/*********************************/
/* helper routines               */


/** update current camera view */

void window_update_curCamera(Object *camera)
{
	copy_view3d_lock(REDRAW);
}

char Windowmodule_QRedrawAll_doc[]= "() - Redraw all windows by queue event";

/* hack to flag that window redraw has happened inside slider callback: */
int g_window_redrawn = 0;

static PyObject *Windowmodule_QRedrawAll(PyObject *self, PyObject *args) 
{
	int wintype = 0;
	BPY_TRY(PyArg_ParseTuple(args, "|i", &wintype));

	allqueue(REDRAWALL, 0);
	RETURN_INC(Py_None);
}
char Windowmodule_Redraw_doc[]= "() - Force a redraw of a specific Window Type; see Window.Const";

PyObject *Windowmodule_Redraw(PyObject *self, PyObject *args) 
{
	ScrArea *tempsa, *sa;
	SpaceText *st;
	int wintype = SPACE_VIEW3D;
	short redraw_all = 0;
	
	BPY_TRY(PyArg_ParseTuple(args, "|i", &wintype));

	g_window_redrawn = 1;
	
	if (wintype < 0) 
		redraw_all = 1;
	if (!during_script()) {
		tempsa= curarea;
		sa= getGlobal()->curscreen->areabase.first;
		while(sa) {
	
			if (sa->spacetype== wintype || redraw_all) {
				/* don't force-redraw Text window (Python GUI) when
				   redraw is called out of a slider update */
				if (sa->spacetype == SPACE_TEXT) {
					st = sa->spacedata.first;
					if (st->text->flags & TXT_FOLLOW) // follow cursor display	
						pop_space_text(st);
					if (disable_force_draw) {
						scrarea_queue_redraw(sa);
					}


				} else {
					scrarea_do_windraw(sa);
					if (sa->headwin) scrarea_do_headdraw(sa);
				}
			}

			sa= sa->next;
		}
		if(curarea!=tempsa) areawinset(tempsa->win);

		if (curarea->headwin) scrarea_do_headdraw(curarea);
		screen_swapbuffers();
	}

	RETURN_INC(Py_None);
}

char Windowmodule_RedrawAll_doc[]= "() - Redraw all windows";
static PyObject *Windowmodule_RedrawAll(PyObject *self, PyObject *args) 
{
	return Windowmodule_Redraw(self, Py_BuildValue("(i)", -1));
}

char Windowmodule_draw_progressbar_doc[]= "(done, text) - Draw a progressbar.\n\
'done' is a float value <= 1.0, 'text' contains info about what is currently\n\
being done";

static PyObject *Windowmodule_draw_progressbar(PyObject *self, PyObject *args) 
{
	float done;
	char *info = 0;
	int retval;

	BPY_TRY(PyArg_ParseTuple(args, "fs", &done, &info));
	retval = progress_bar(done, info);
	return Py_BuildValue("i", retval);
}

#undef METHODDEF
#define METHODDEF(func) {#func, Windowmodule_##func, METH_VARARGS, Windowmodule_##func##_doc}

static struct PyMethodDef Windowmodule_methods[] = {
	METHODDEF(Redraw),
	METHODDEF(QRedrawAll),
	METHODDEF(RedrawAll),
	METHODDEF(draw_progressbar),
	
	{NULL, NULL}
};


#undef BPY_ADDCONST
#define BPY_ADDCONST(dict, name) insertConst(dict, #name, PyInt_FromLong(SPACE_##name))

PyObject *INITMODULE(Window)(void) 
{
	PyObject *d;
	PyObject *mod= Py_InitModule(SUBMODULE(Window), Windowmodule_methods);
	PyObject *dict= PyModule_GetDict(mod);

/* from DNA_screen.types.h */
	d = ConstObject_New();
	PyDict_SetItemString(dict, "Types" , d);

	BPY_ADDCONST(d, VIEW3D);
	BPY_ADDCONST(d, IPO);
	BPY_ADDCONST(d, OOPS);
	BPY_ADDCONST(d, BUTS);
	BPY_ADDCONST(d, FILE);
	BPY_ADDCONST(d, IMAGE);
	BPY_ADDCONST(d, INFO);
	BPY_ADDCONST(d, SEQ);
	BPY_ADDCONST(d, IMASEL);
	BPY_ADDCONST(d, SOUND);
	BPY_ADDCONST(d, ACTION);
	BPY_ADDCONST(d, TEXT);
	BPY_ADDCONST(d, NLA);
/*	BPY_ADDCONST(d, LOGIC); */

	return mod;
}

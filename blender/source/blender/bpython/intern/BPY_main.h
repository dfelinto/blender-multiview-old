/* BlenderPython Main routine header *
   $Id$
  
   ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
  
   The contents of this file may be used under the terms of either the GNU
   General Public License Version 2 or later (the "GPL", see
   http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
   later (the "BL", see http://www.blender.org/BL/ ) which has to be
   bought from the Blender Foundation to become active, in which case the
   above mentioned GPL option does not apply.
  
   The Original Code is Copyright (C) 2002 by NaN Holding BV.
   All rights reserved.
  
   The Original Code is: all of this file.
  
   Contributor(s): none yet.
  
   ***** END GPL/BL DUAL LICENSE BLOCK *****
 */
//   Note: Functions prefixed with BPY_ are called from blenkernel routines */


#include "Python.h" /* The python includes themselves. */
#include "compile.h" /* to give us PyCodeObject */
#include "eval.h" /*  for PyEval_EvalCode.h */

/* blender stuff */
#include "MEM_guardedalloc.h"
#include "BLI_blenlib.h"
#include "BLI_editVert.h"
#include "BLI_fileops.h" /* string handling of filenames */

#include "BKE_bad_level_calls.h"
// #include "BKE_editmesh.h"

#include "BKE_global.h"
#include "BKE_main.h"

#include "BLO_genfile.h" // for BLO_findstruct_offset only
#include "BKE_text.h"
#include "BKE_displist.h"
#include "BKE_mesh.h"
#include "BKE_material.h"
#include "BKE_object.h"
#include "BKE_screen.h"
#include "BKE_scene.h"
#include "BKE_library.h"
#include "BKE_text.h"

#include "b_interface.h"

/* prototypes of externally used functions are HERE */
#include "BPY_extern.h"

	/* I just chucked some prototypes
	 * here... not sure where they should
	 * really be. -zr
	 */
extern struct ID * script_link_id;

extern PyObject *g_blenderdict;
extern int g_window_redrawn;
extern int disable_force_draw;

void window_update_curCamera(Object *);
PyObject *ConstObject_New(void);
void insertConst(PyObject *dict, char *name, PyObject *item);
PyObject *Windowmodule_Redraw(PyObject *self, PyObject *args);

char *event_to_name(short event);
void syspath_append(PyObject *dir);
void init_syspath(void);
void set_scriptlinks(ID *id, short event);
void release_scriptlinks(ID *id);

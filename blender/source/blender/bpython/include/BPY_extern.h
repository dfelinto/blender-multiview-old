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
struct Text;
struct ID;
struct ScriptLink;
struct ListBase;
struct SpaceText;
struct _object;  // forward declaration for PyObject !


void BPY_start_python(void);
void BPY_end_python(void);
int BPY_Err_getLinenumber(void);
const char *BPY_Err_getFilename(void);
void BPY_Err_Handle(struct Text *text);
struct _object *BPY_txt_do_python(struct SpaceText* st);
void BPY_free_compiled_text(struct Text* text);
void BPY_clear_bad_scriptlink(struct ID *id, struct Text *byebye);
void BPY_clear_bad_scriptlinks(struct Text *byebye);
void BPY_clear_bad_scriptlist(struct ListBase *, struct Text *byebye);
void BPY_do_all_scripts(short event);
void BPY_do_pyscript(struct ID *id, short event);
void BPY_free_scriptlink(struct ScriptLink *slink);
void BPY_copy_scriptlink(struct ScriptLink *scriptlink);

/* format importer hook */
int BPY_call_importloader(char *name);

int BPY_spacetext_is_pywin(struct SpaceText *st);
void BPY_spacetext_do_pywin_draw(struct SpaceText *st);
void BPY_spacetext_do_pywin_event(struct SpaceText *st, unsigned short event, short val);

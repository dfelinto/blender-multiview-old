/**
 * blenlib/BKE_bad_level_calls.h (mar-2001 nzc)
 *	
 * Stuff that definitely does not belong in the kernel! These will all
 * have to be removed in order to restore sanity.
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
#ifndef BKE_BAD_LEVEL_CALLS_H
#define BKE_BAD_LEVEL_CALLS_H

/* readfile.c */
struct PluginSeq;
void open_plugin_seq(struct PluginSeq *pis, char *seqname);
struct SpaceButs;
void set_rects_butspace(struct SpaceButs *buts);
struct SpaceImaSel;
void check_imasel_copy(struct SpaceImaSel *simasel);
struct ScrArea;
struct bScreen;
void unlink_screen(struct bScreen *sc);
void freeAllRad(void);
void free_editText(void);
void setscreen(struct bScreen *sc);
void force_draw_all(void);
  /* otherwise the WHILE_SEQ doesn't work */
struct Sequence;
void build_seqar(struct ListBase *seqbase, struct Sequence  ***seqar, int *totseq);

struct ID;
void BPY_do_pyscript (struct ID *id, short int event);

/* writefile.c */
struct Oops;
void free_oops(struct Oops *oops);
void error(char *str, ...);

/* anim.c */
extern struct ListBase editNurb;

/* displist.c */
#include "DNA_world_types.h"	/* for render_types */
#include "render_types.h"
extern struct RE_Render R;
float RE_Spec(float,  int);
void waitcursor(int);
void allqueue(unsigned short event, short val);
#define REDRAWVIEW3D	0x4010
struct Material;
extern struct Material defmaterial;

/* effect.c */
void    RE_jitterate1(float *jit1, float *jit2, int num, float rad1);
void    RE_jitterate2(float *jit1, float *jit2, int num, float rad2);

/* exotic.c */
void load_editMesh(void);
void make_editMesh(void);
void free_editMesh(void);
void free_editArmature(void);
void docentre_new(void);
int saveover(char *str);

/* image.c */
#include "DNA_image_types.h"
void free_realtime_image(Image *ima); // has to become a callback, opengl stuff
void RE_make_existing_file(char *name); // from render, but these funcs should be moved anyway 

/* ipo.c */
void copy_view3d_lock(short val);	// was a hack, to make scene layer ipo's possible

/* library.c */
void allspace(unsigned short event, short val) ;
#define OOPS_TEST             2

/* mball.c */
extern ListBase editelems;

/* object.c */
/*  void BPY_free_scriptlink(ScriptLink *slink); */
/*  void BPY_copy_scriptlink(ScriptLink *scriptlink); */
float *give_cursor(void);  // become a callback or argument
void exit_posemode(int freedata);

/* packedFile.c */
short pupmenu(char *instr);  // will be general callback

/* sca.c */
#define LEFTMOUSE    0x001	// because of mouse sensor

/* scene.c */
#include "DNA_sequence_types.h"
void free_editing(struct Editing *ed);	// scenes and sequences problem...

/* texture.c */
#define FLO 128
#define INT	96
struct EnvMap;
struct Tex;
void    RE_free_envmap(struct EnvMap *env);      
struct EnvMap *RE_copy_envmap(struct EnvMap *env);
void    RE_free_envmapdata(struct EnvMap *env);
int     RE_envmaptex(struct Tex *tex, float *texvec, float *dxt, float *dyt);
void    RE_calc_R_ref(void);
extern char texstr[15][8];	/* buttons.c */

/* memory for O is declared in the render module... */
#include "BKE_osa_types.h"
extern Osa O;

/* editsca.c */
void make_unique_prop_names(char *str);

#endif


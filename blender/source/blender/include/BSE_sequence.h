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
 * 
 */

#ifndef BSE_SEQUENCE_H
#define BSE_SEQUENCE_H

struct PluginSeq;
struct StripElem;
struct Strip;
struct Sequence;
struct ListBase;
struct Editing;
struct ImBuf;
struct Scene;

void open_plugin_seq(struct PluginSeq *pis, char *seqname);
struct PluginSeq *add_plugin_seq(char *str, char *seqname);
void free_plugin_seq(struct PluginSeq *pis);
void free_stripdata(int len, struct StripElem *se);
void free_strip(struct Strip *strip);
void new_stripdata(struct Sequence *seq);
void free_sequence(struct Sequence *seq);
void do_seq_count(struct ListBase *seqbase, int *totseq);
void do_build_seqar(struct ListBase *seqbase, struct Sequence ***seqar, int depth);
void build_seqar(struct ListBase *seqbase, struct Sequence  ***seqar, int *totseq);
void free_editing(struct Editing *ed);
void calc_sequence(struct Sequence *seq);
void sort_seq(void);
void clear_scene_in_allseqs(struct Scene *sce);
void do_alphaover_effect(float facf0,
						 float facf1,
						 int x, int y,
						 unsigned int *rect1,
						 unsigned int *rect2,
						 unsigned int *out);
void do_alphaunder_effect(float facf0, float facf1,
						  int x, int y,
						  unsigned int *rect1, unsigned int *rect2,
						  unsigned int *out);
void do_cross_effect(float facf0, float facf1,
					 int x, int y,
					 unsigned int *rect1, unsigned int *rect2,
					 unsigned int *out);
void do_gammacross_effect(float facf0, float facf1,
						  int x, int y,
						  unsigned int *rect1, unsigned int *rect2,
						  unsigned int *out);
void do_add_effect(float facf0, float facf1,
				   int x, int y,
				   unsigned int *rect1, unsigned int *rect2,
				   unsigned int *out);
void do_sub_effect(float facf0, float facf1,
				   int x, int y,
				   unsigned int *rect1, unsigned int *rect2,
				   unsigned int *out);
void do_drop_effect(float facf0, float facf1,
					int x, int y,
					unsigned int *rect2i, unsigned int *rect1i,
					unsigned int *outi);
void do_drop_effect2(float facf0, float facf1,
					 int x, int y,
					 unsigned int *rect2, unsigned int *rect1,
					 unsigned int *out);
void do_mul_effect(float facf0, float facf1,
				   int x, int y,
				   unsigned int *rect1, unsigned int *rect2,
				   unsigned int *out);
void make_black_ibuf(struct ImBuf *ibuf);
void multibuf(struct ImBuf *ibuf, float fmul);
void do_effect(int cfra, struct Sequence *seq, struct StripElem *se);
int evaluate_seq_frame(int cfra);
struct StripElem *give_stripelem(struct Sequence *seq, int cfra);
void set_meta_stripdata(struct Sequence *seqm);
void do_seq_count_cfra(struct ListBase *seqbase, int *totseq, int cfra);
void do_build_seqar_cfra(struct ListBase *seqbase, struct Sequence ***seqar, int cfra);
struct ImBuf *give_ibuf_seq(int cfra);
void free_imbuf_effect_spec(int cfra);
void free_imbuf_seq_except(int cfra);
void free_imbuf_seq(void);
void do_render_seq(void);


#endif

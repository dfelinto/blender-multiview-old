/**
 * blenlib/DNA_sequence_types.h (mar-2001 nzc)
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
#ifndef DNA_SEQUENCE_TYPES_H
#define DNA_SEQUENCE_TYPES_H

#include "DNA_listBase.h"

struct Ipo;
struct Scene;

typedef struct StripElem {
	char name[40];
	struct ImBuf *ibuf;
	struct StripElem *se1, *se2, *se3;
	short ok, nr;
	int pad;
		
} StripElem;

typedef struct Strip {
	struct Strip *next, *prev;
	short rt, len, us, done;
	StripElem *stripdata;
	char dir[80];
	short orx, ory;
	int pad;
		
} Strip;


typedef struct PluginSeq {
	char name[80];
	void *handle;
	
	char *pname;
	
	int vars, version;

	void *varstr;
	float *cfra;
	
	float data[32];

	void (*doit)(void);

	void (*callback)(void);
} PluginSeq;


/* LET OP: eerste stuk identiek aan ID (ivm ipo's) */

typedef struct Sequence {

	struct Sequence *next, *prev, *newseq;
	void *lib;
	char name[24];
	
	short flag, type;
	int len;
	int start, startofs, endofs;
	int startstill, endstill;
	int machine, depth;
	int startdisp, enddisp;
	float mul, handsize;
	int sfra;
	
	Strip *strip;
	StripElem *curelem;
	
	struct Ipo *ipo;
	struct Scene *scene;
	struct anim *anim;
	float facf0, facf1;
	
	PluginSeq *plugin;

	/* pointers voor effecten: */
	struct Sequence *seq1, *seq2, *seq3;
	
	/* meta */
	ListBase seqbase;
	
} Sequence;


#
#
typedef struct MetaStack {
	struct MetaStack *next, *prev;
	ListBase *oldbasep;
	Sequence *parseq;
} MetaStack;

typedef struct Editing {
	ListBase *seqbasep;
	ListBase seqbase;
	ListBase metastack;
	short flag, rt;
	int pad;
} Editing;

/* ***************** SEQUENCE ****************** */

/* seq->flag */
#define SEQ_LEFTSEL		2
#define SEQ_RIGHTSEL	4
#define SEQ_OVERLAP		8
#define SEQ_FILTERY		16

/* seq->type LET OP BITJE 3!!! */
#define SEQ_IMAGE		0
#define SEQ_META		1
#define SEQ_SCENE		2
#define SEQ_MOVIE		3

#define SEQ_EFFECT		8
#define SEQ_CROSS		8
#define SEQ_ADD			9
#define SEQ_SUB			10
#define SEQ_ALPHAOVER	11
#define SEQ_ALPHAUNDER	12
#define SEQ_GAMCROSS	13
#define SEQ_MUL			14
#define SEQ_OVERDROP	15
#define SEQ_PLUGIN		24

#endif

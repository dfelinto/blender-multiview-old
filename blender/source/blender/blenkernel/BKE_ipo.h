/**
 * blenlib/BKE_ipo.h (mar-2001 nzc)
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
#ifndef BKE_IPO_H
#define BKE_IPO_H

typedef struct CfraElem {
	struct CfraElem *next, *prev;
	float cfra;
	int sel;
} CfraElem;

struct Ipo;
struct IpoCurve;
struct MTex;
struct Material;
struct Object;
struct Sequence;
struct ListBase;
struct BezTriple;
/*  struct IPO_Channel; */

float frame_to_float(int cfra);
void free_ipo(struct Ipo *ipo);
struct Ipo *add_ipo(char *name, int idcode);
struct Ipo *copy_ipo(struct Ipo *ipo);
void make_local_obipo(struct Ipo *ipo);
void make_local_matipo(struct Ipo *ipo);
void make_local_keyipo(struct Ipo *ipo);
void make_local_ipo(struct Ipo *ipo);
void calchandles_ipocurve(struct IpoCurve *icu);
void testhandles_ipocurve(struct IpoCurve *icu);
void sort_time_ipocurve(struct IpoCurve *icu);
int test_time_ipocurve(struct IpoCurve *icu);
void correct_bezpart(float *v1, float *v2, float *v3, float *v4);
int findzero(float x, float q0, float q1, float q2, float q3, float *o);
void berekeny(float f1, float f2, float f3, float f4, float *o, int b);
void berekenx(float *f, float *o, int b);
float eval_icu(struct IpoCurve *icu, float ipotime);
void calc_icu(struct IpoCurve *icu, float ctime);
float calc_ipo_time(struct Ipo *ipo, float ctime);
void calc_ipo(struct Ipo *ipo, float ctime);
void write_ipo_poin(void *poin, int type, float val);
float read_ipo_poin(void *poin, int type);
void *give_mtex_poin(struct MTex *mtex, int adrcode );
void *get_ipo_poin(struct ID *id, struct IpoCurve *icu, int *type);
void set_icu_vars(struct IpoCurve *icu);
void execute_ipo(struct ID *id, struct Ipo *ipo);
void do_ipo_nocalc(struct Ipo *ipo);
void do_ipo(struct Ipo *ipo);
void do_mat_ipo(struct Material *ma);
void do_ob_ipo(struct Object *ob);
void do_seq_ipo(struct Sequence *seq);
int has_ipo_code(struct Ipo *ipo, int code);
void do_all_ipos(void);
int calc_ipo_spec(struct Ipo *ipo, int adrcode, float *ctime);
void clear_delta_obipo(struct Ipo *ipo);
void add_to_cfra_elem(struct ListBase *lb, struct BezTriple *bezt);
void make_cfra_list(struct Ipo *ipo, struct ListBase *elems);
/* the sort is an IPO_Channel... */
int IPO_GetChannels(struct Ipo *ipo, short *channels);
void test_ipo_get(void);                             

float IPO_GetFloatValue(struct Ipo *ipo,
/*  						struct IPO_Channel channel, */
						/* channels are shorts... bit ugly for now*/
						short c,
						float ctime);


#endif

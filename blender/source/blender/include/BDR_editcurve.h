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

#ifndef BDR_EDITCURVE_H
#define BDR_EDITCURVE_H

struct Curve;
struct Nurb;
struct BezTriple;
struct BPoint;
struct BezTripleNurb;

short isNurbsel(struct Nurb *nu);
int isNurbsel_count(struct Nurb *nu);
void printknots(void);
void load_editNurb(void);
void make_editNurb(void);
void remake_editNurb(void);
void separate_nurb(void);
short isNurbselUV(struct Nurb *nu, int *u, int *v, int flag);
void setflagsNurb(short flag);
void rotateflagNurb(short flag, float *cent, float rotmat[][3]);
void translateflagNurb(short flag, float *vec);
void weightflagNurb(short flag, float w, int mode);
void deleteflagNurb(short flag);
short extrudeflagNurb(int flag);
void adduplicateflagNurb(short flag);
void switchdirectionNurb2(void);
void switchdirection_knots(float *base, int tot);
void deselectall_nurb(void);
void hideNurb(int swap);
void revealNurb(void);
void selectswapNurb(void);
void subdivideNurb(void);
short findnearestNurbvert(short sel, struct Nurb **nurb, 
						  struct BezTriple **bezt, struct BPoint **bp);
void findselectedNurbvert(struct Nurb **nu, struct BezTriple **bezt, 
						  struct BPoint **bp);
void setsplinetype(short type);
void rotate_direction_nurb(struct Nurb *nu);
int is_u_selected(struct Nurb *nu, int u);
void make_selection_list_nurb(void);
void merge_2_nurb(struct Nurb *nu1, struct Nurb *nu2);
void merge_nurb(void);
void addsegment_nurb(void);
void mouse_nurb(void);
void spinNurb(float *dvec, short mode);
void addvert_Nurb(int mode);
void extrude_nurb(void);
void makecyclicNurb(void);
void selectconnected_nurb(void);
void selectrow_nurb(void);
void adduplicate_nurb(void);
void delNurb(void);
void join_curve(int type);
struct Nurb *addNurbprim(int type, int stype, int newname);
void default_curve_ipo(struct Curve *cu);
void add_primitiveCurve(int stype);
void add_primitiveNurb(int type);
void clear_tilt(void);
void clever_numbuts_curve(void);         
int bezt_compare (const void *e1, const void *e2);

#endif  /*  BDR_EDITCURVE_H */

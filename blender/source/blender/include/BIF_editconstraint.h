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

#ifndef BIF_EDITCONSTRAINT_H
#define BIF_EDITCONSTRAINT_H

struct ListBase;
struct Object;
struct bConstraint;
struct bConstraintChannel;

typedef struct ConstraintElement{
	struct ConstraintElement *next, *prev;
	struct ConstraintElement *parent;
	Object		*ob;
	int			flag;
	const char	*substring;
	void		*subdata;
} ConstraintElement;

struct bConstraintChannel *add_new_constraint_channel(const char *name);
struct bConstraint * add_new_constraint(void);
struct ListBase *get_constraint_client_channels (int forcevalid);
struct ListBase *get_constraint_client(char *name, short *clienttype, void** clientdata);
int test_constraints (struct Object *owner, const char *substring, int disable);
void test_scene_constraints (void);
void unique_constraint_name (struct bConstraint *con, struct ListBase *list);
void *new_constraint_data (short type);

/*  void unique_constraint_name (struct bConstraint *con, struct ListBase *list); */

#endif


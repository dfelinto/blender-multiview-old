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

#ifndef BSE_EDITACTION_H
#define BSE_EDITACTION_H

struct bAction;
struct bActionChannel;
struct bPoseChannel;
struct Object;
struct Ipo;

struct bActionChannel* get_hilighted_action_channel(struct bAction* action);
void set_exprap_action(int mode);
void free_posebuf(void);
void copy_posebuf (void);
void paste_posebuf (int flip);
void set_action_key (struct bAction *act, struct bPoseChannel *chan, int adrcode, short makecurve);
struct bAction *add_empty_action(void);
void deselect_actionchannel_keys (struct bAction *act, int test);
void deselect_actionchannels (struct bAction *act, int test);
void winqreadactionspace(unsigned short event, short val, char ascii);
void remake_action_ipos(struct bAction *act);
void select_actionchannel_by_name (struct bAction *act, char *name, int select);
struct bAction *bake_action_with_client (struct bAction *act, struct Object *arm, float tolerance);

#endif /* BSE_EDITACTION_H */

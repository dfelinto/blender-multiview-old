/**
 * blenlib/BKE_sca.h (mar-2001 nzc)
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
#ifndef BKE_SCA_H
#define BKE_SCA_H

struct Text;
struct bSensor;
struct Object;
struct bController;
struct bActuator;

void unlink_controller(struct bController *cont);
void unlink_controllers(struct ListBase *lb);
void free_controller(struct bController *cont);
void free_controllers(struct ListBase *lb);

void unlink_actuator(struct bActuator *act);
void unlink_actuators(struct ListBase *lb);
void free_actuator(struct bActuator *act);
void free_actuators(struct ListBase *lb);

void free_text_controllers(struct Text *txt);
void free_sensor(struct bSensor *sens);
void free_sensors(struct ListBase *lb);
struct bSensor *copy_sensor(struct bSensor *sens);
void copy_sensors(struct ListBase *lbn, struct ListBase *lbo);
void init_sensor(struct bSensor *sens);
struct bSensor *new_sensor(int type);
struct bController *copy_controller(struct bController *cont);
void copy_controllers(struct ListBase *lbn, struct ListBase *lbo);
void init_controller(struct bController *cont);
struct bController *new_controller(int type);
struct bActuator *copy_actuator(struct bActuator *act);
void copy_actuators(struct ListBase *lbn, struct ListBase *lbo);
void init_actuator(struct bActuator *act);
struct bActuator *new_actuator(int type);
void clear_sca_new_poins_ob(struct Object *ob);
void clear_sca_new_poins(void);
void set_sca_new_poins_ob(struct Object *ob);
void set_sca_new_poins(void);
void sca_remove_ob_poin(struct Object *obt, struct Object *ob);                    

#endif

/**
 * $Id: IDProp.h
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version. The Blender
 * Foundation also sells licenses for use in proprietary software under
 * the Blender License.  See http://www.blender.org/BL/ for information
 * about this.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * Contributor(s): Joseph Eagar
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */
 
#include <Python.h>

struct ID;
struct IDProperty;
struct BPyIDGroupIterObject;

typedef struct BPyIDPropertyObject {
	PyObject_VAR_HEAD
	struct ID *id;
	struct IDProperty *prop, *parent;
	PyObject *data_wrap;
} BPyIDPropertyObject;

typedef struct BPyIDArrayObject {
	PyObject_VAR_HEAD
	struct ID *id;
	struct IDProperty *prop;
} BPyIDArrayObject;

typedef struct BPyIDGroupIterObject {
	PyObject_VAR_HEAD
	BPyIDPropertyObject *group;
	struct IDProperty *cur;
	int mode;
} BPyIDGroupIterObject;

PyObject *IDGroupType_Init( void );
PyObject *IDGroupIterType_Init( void );
PyObject *IDArrayType_Init( void );

PyObject *IDProperty_CreatePyObject(struct ID *id, struct IDProperty *prop, struct IDProperty *parent);

#define IDPROP_ITER_KEYS	0
#define IDPROP_ITER_ITEMS	1

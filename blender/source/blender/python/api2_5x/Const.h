/* 
 * $Id: Const.h,v 1.11 2006/03/18 15:23:02 campbellbarton Exp $
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
 * This is a new part of Blender.
 *
 * Contributor(s): Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

#ifndef EXPP_const_H
#define EXPP_const_H

#undef CONST_BITFIELDS		/* define to enable constant bitfield handling */
#include <Python.h>

/*-------------------TYPE OBJECT-------------------------------*/
extern PyTypeObject Const_Type;
extern PyTypeObject ConstCategoty_Type;
/*-------------------TYPE CHECKS-------------------------------*/
#define BPyConst_Check(v) PyObject_TypeCheck(v, &Const_Type)
#define BPyConstType_Check(d,v) (BPyConst_Check(v)&&(v)->defn==d)
#define BPyConstCategory_Check(v) PyObject_TypeCheck(v, &ConstCategoty_Type)
#define BPyConstCategoryType_Check(d,v) (BPyConstCategory_Check(v)&&(v)->defn==d)

/*-------------------TYPE ENUMS--------------------------------*/
enum const_types {
	EXPP_CONST_INT = 0,
	EXPP_CONST_FLOAT = 1,
	EXPP_CONST_BIT = 2
};
/*-------------------STRUCT DEFINITIONS-------------------------*/

/* the actual value types for a constant */
typedef union {
    int i;                     /* integer constants */
    float f;                   /* floating constants */
    long b;                    /* bitfield constants */
} constValue;

/* the name and value for a constant */
typedef struct {
    char *repr;                /* constant's name */
    constValue value;          /* constant's value */
} constIdents;

/* the constant category */
typedef struct {
    unsigned char utype;       /* type indicator this category's constants */
    char *name;                /* constant class name */
    unsigned char tot_members; /* total number of constants */
    constIdents *members;      /* array of constant names/values */
} constDefinition;

typedef struct {
    PyObject_HEAD
    constDefinition *defn;     /* specific constant information */
    unsigned char utype;       /* type indicator (basically defn->utype) */
    constValue value;          /* specific value of this constant */
} BPyconst;

/*-------------------VISIBLE PROTOTYPES-----------------------*/

PyObject * ConstType_Init( void );
PyObject * ConstCategoryType_Init( void );
PyObject *PyConst_NewInt(constDefinition *defn, int value);
PyObject *PyConst_NewFloat(constDefinition *defn, float value);
PyObject *PyConst_NewBit(constDefinition *defn, long value);
int PyConst_Lookup( constDefinition *defn, char *name, constValue *value );
constValue * Const_FromPyObject( constDefinition *defn, PyObject *pyob);
void PyConstCategory_AddObjectToDict( PyObject *dict, constDefinition *defn );

#endif				/* EXPP_const_H */

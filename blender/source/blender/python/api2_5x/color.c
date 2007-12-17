/*
 * $Id: vector.c 11161 2007-07-03 19:13:26Z campbellbarton $
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
 * 
 * Contributor(s): Willian P. Germano & Joseph Gilbert, Ken Hughes
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "color.h"

#include "BLI_blenlib.h"
#include "BKE_utildefines.h"
#include "BLI_arithb.h"

#include "gen_utils.h"
#include "Material.h"
#include "Texture.h"
#include "MTex.h"
#include "Lamp.h"
#include "World.h"
#include "Mesh.h"

#define COL_ERROR_INT EXPP_ReturnIntError( PyExc_RuntimeError, "the data this color references has been removed.")
#define COL_ERROR_PY EXPP_ReturnPyObjError( PyExc_RuntimeError, "the data this color references has been removed.")

/* TODO - ramp colors and MTex colors */


int Color_CheckPyObject(BPyColorObject * self);

/* these functions are used for getting and setting */
int color_from_source__internal(BPyColorObject * self, int check)
{
	if (!self->source)
		return 1;
	
	if (BPyMaterial_Check(self->source)) {
		Material *ma = Material_FromPyObject(self->source);
		switch (self->type) {
		case BPY_COLOR_MAT_DIFF:
			self->color[0] = ma->r; 
			self->color[1] = ma->g; 
			self->color[2] = ma->b;
			break;
		case BPY_COLOR_MAT_SPEC:
			self->color[0] = ma->specr; 
			self->color[1] = ma->specg; 
			self->color[2] = ma->specb;
			break;
		case BPY_COLOR_MAT_MIR:
			self->color[0] = ma->mirr; 
			self->color[1] = ma->mirg; 
			self->color[2] = ma->mirb;
			break;
		case BPY_COLOR_MAT_SSS:
			self->color[0] = ma->sss_col[0]; 
			self->color[1] = ma->sss_col[1]; 
			self->color[2] = ma->sss_col[2];
			break;
		}
	} else if (BPyTexture_Check(self->source)) {
		Tex *tex = Texture_FromPyObject(self->source);
		/*BPY_COLOR_TEX is implicit */
		self->color[0] = tex->rfac; 
		self->color[1] = tex->gfac; 
		self->color[2] = tex->bfac;
	} else if (BPyWorld_Check(self->source)) {
		World *world = World_FromPyObject(self->source);
		switch (self->type) {
		case BPY_COLOR_WORLD_AMB:
			self->color[0] = world->ambr;
			self->color[1] = world->ambg;
			self->color[2] = world->ambb;
			break;
		case BPY_COLOR_WORLD_HOR:
			self->color[0] = world->horr;
			self->color[1] = world->horg;
			self->color[2] = world->horb;
			break;
		case BPY_COLOR_WORLD_ZEN:
			self->color[0] = world->zenr; 
			self->color[1] = world->zeng; 
			self->color[2] = world->zenb;
			break;
		}
	} else if (BPyMTex_Check(self->source)) {
		MTex * mtex = MTex_get_pointer( (BPyMTexObject *)self->source );
		if (!mtex)
			return 0; /* error set */
		self->color[0] = mtex->r; 
		self->color[1] = mtex->g; 
		self->color[2] = mtex->b;
	} else if (BPyLamp_Check(self->source)) {
		/* only one color type for lamp */
		Lamp *la = Lamp_FromPyObject(self->source);
		self->color[0] = la->r; 
		self->color[1] = la->g; 
		self->color[2] = la->b;
	} else if (BPyMesh_Check(self->source)) {
		BPyMeshObject *bpymesh = (BPyMeshObject *)self->source;
		MCol *mcol;
		
		if ( check && ((self->index >= bpymesh->mesh->totface ) || (!bpymesh->mesh->mcol)) )
			return 0;
		
		/* (self->type-BPY_COLOR_MESH_FACE_V1) - is a way we can use teh type to store
		 * which index this color was from
		 * 
		 * At the moment dont check for a tri/quad since it wont crash,
		 * only edit a color we wont see... TODO can fix later */
		mcol = &bpymesh->mesh->mcol[(self->index*4) + (self->type - BPY_COLOR_MESH_FACE_V1)];
		
		self->color[0] = ((float)mcol->b) / 256.0f; /* must swap r and b - wtf? */ 
		self->color[1] = ((float)mcol->g) / 256.0f;
		self->color[2] = ((float)mcol->r) / 256.0f;
		self->color[3] = ((float)mcol->a) / 256.0f;
	}
	
	/* TODO - Other types */
	return 1;
}

int color_to_source__internal(BPyColorObject * self, int check)
{
	if (!self->source)
		return 1;
	
	if (BPyMaterial_Check(self->source)) {
		Material *ma = Material_FromPyObject(self->source);
		switch (self->type) {
		case BPY_COLOR_MAT_DIFF:
			ma->r = self->color[0];
			ma->g = self->color[1];
			ma->b = self->color[2];
			break;
		case BPY_COLOR_MAT_SPEC:
			ma->specr = self->color[0]; 
			ma->specg = self->color[1]; 
			ma->specb = self->color[2]; 
			break;
		case BPY_COLOR_MAT_MIR:
			ma->mirr = self->color[0];
			ma->mirg = self->color[1];
			ma->mirb = self->color[2];
			break;
		case BPY_COLOR_MAT_SSS:
			ma->sss_col[0] = self->color[0];
			ma->sss_col[1] = self->color[1];
			ma->sss_col[2] = self->color[2];
			break;
		}
	} else if (BPyTexture_Check(self->source)) {
		Tex *tex = Texture_FromPyObject(self->source);
		/*BPY_COLOR_TEX is implicit */
		tex->rfac = self->color[0]; 
		tex->gfac = self->color[1]; 
		tex->bfac = self->color[2];
	} else if (BPyWorld_Check(self->source)) {
		World *world = World_FromPyObject(self->source);
		switch (self->type) {
		case BPY_COLOR_WORLD_AMB:
			world->ambr = self->color[0];
			world->ambg = self->color[1];
			world->ambb = self->color[2];
			break;
		case BPY_COLOR_WORLD_HOR:
			world->horr = self->color[0]; 
			world->horg = self->color[1]; 
			world->horb = self->color[2];
			break;
		case BPY_COLOR_WORLD_ZEN:
			world->zenr = self->color[0]; 
			world->zeng = self->color[1]; 
			world->zenb = self->color[2];
			break;
		}
	} else if (BPyMTex_Check(self->source)) {
		MTex * mtex = MTex_get_pointer( (BPyMTexObject *)self->source );
		if (!mtex)
			return 0; /* error set */
		mtex->r = self->color[0]; 
		mtex->g = self->color[1]; 
		mtex->b = self->color[2];
	} else if (BPyLamp_Check(self->source)) {
		/* only one color type for lamp */
		Lamp *la = Lamp_FromPyObject(self->source);
		la->r = self->color[0];
		la->g = self->color[1];
		la->b = self->color[2];
	
	} else if (BPyMesh_Check(self->source)) {
		BPyMeshObject *bpymesh = (BPyMeshObject *)self->source;
		MCol *mcol;
		
		if ( check && ((self->index >= bpymesh->mesh->totface ) || (!bpymesh->mesh->mcol)) )
			return 0;
		
		/* (self->type-BPY_COLOR_MESH_FACE_V1) - is a way we can use teh type to store
		 * which index this color was from
		 * 
		 * At the moment dont check for a tri/quad since it wont crash,
		 * only edit a color we wont see... TODO can fix later */
		mcol = &bpymesh->mesh->mcol[(self->index*4) + (self->type - BPY_COLOR_MESH_FACE_V1)];
		
		mcol->b = (char)(self->color[0]*256.0);
		mcol->g = (char)(self->color[1]*256.0);
		mcol->r = (char)(self->color[2]*256.0);
		mcol->a = (char)(self->color[3]*256.0);
	} /* TODO - Other types */
	
	return 1;
}

/*----------------------------Color.copy() --------------------------------------
  return a copy of the color */
PyObject *Color_copy(BPyColorObject * self)
{
	if (!color_from_source__internal(self, 1))
		return COL_ERROR_PY;
	
	return Color_CreatePyObject(self->color, self->size, BPY_COLOR_IMPLICIT, 0, (PyObject *)NULL);
}

/*----------------------------dealloc()(internal) ----------------
  free the py_object */
static void Color_dealloc(BPyColorObject * self)
{
	/* only free non wrapped */
	if(self->source) {
		
		/* This is like a weakref, the BPyData that we come from links to us
		 * but cannot be a reference because that would make a 2 way reference
		 * loop and neither would be deallocated 
		 * 
		 * So instead, some of the data that made us, has a pointer to us..
		 * but that means we need to invalidate the pointer when we are dec-reffed
		 * else the material for instance will hav a bad pointer
		 * 
		 * This is less hassel then being a weakref however
		 * */
		if (BPyMaterial_Check(self->source)) {
			switch (self->type) {
			case BPY_COLOR_MAT_DIFF:
				((BPyMaterialObject *)self->source)->col = NULL;
				break;
			case BPY_COLOR_MAT_SPEC:
				((BPyMaterialObject *)self->source)->spec = NULL;
				break;
			case BPY_COLOR_MAT_MIR:
				((BPyMaterialObject *)self->source)->mir = NULL;
				break;
			case BPY_COLOR_MAT_SSS:
				((BPyMaterialObject *)self->source)->sss = NULL;
				break;
			case BPY_COLOR_MTEX:
				/*TODO - mtex color */
				break;
			}
		} else if (BPyLamp_Check(self->source)) {
			((BPyLamp *)self->source)->color = NULL;
		} else if (BPyMesh_Check(self->source)) {
			/* do nothing */
		}
		/* need to add more types... */
		
		Py_DECREF(self->source); /* may be None, thats ok */
	}
	PyObject_DEL(self);
}

/*----------------------------print object (internal)-------------
  print the object to screen */
static PyObject *Color_repr(BPyColorObject * self)
{
	if (Color_CheckPyObject(self)) { 
		int i;
		char buffer[48], str[1024];
		BLI_strncpy(str,"[",1024);
		for(i = 0; i < self->size; i++){
			if(i < (self->size - 1)){
				sprintf(buffer, "%.6f, ", self->color[i]);
				strcat(str,buffer);
			}else{
				sprintf(buffer, "%.6f", self->color[i]);
				strcat(str,buffer);
			}
		}
		strcat(str, "](color)");
	
		return PyString_FromString(str);
	} else {
		return PyString_FromString("[<deleted>](color)");
	}
}
/*---------------------SEQUENCE PROTOCOLS------------------------
  ----------------------------len(object)------------------------
  sequence length*/
static int Color_len(BPyColorObject * self)
{
	return self->size;
}
/*----------------------------object[]---------------------------
  sequence accessor (get)*/
static PyObject *Color_item(BPyColorObject * self, int i)
{
	if (!color_from_source__internal(self, 1))
		return COL_ERROR_PY;
	
	if(i < 0 || i >= self->size)
		return EXPP_ReturnPyObjError(PyExc_IndexError,
		"color[index]: out of range\n");

	return PyFloat_FromDouble(self->color[i]);

}
/*----------------------------object[]-------------------------
  sequence accessor (set)*/
static int Color_ass_item(BPyColorObject * self, int i, PyObject * ob)
{
	float f = ( float )PyFloat_AsDouble( ob );
	if (f==-1 && PyErr_Occurred())
		return -1;
	
	/* TODO - a check only would do here */
	if (!color_from_source__internal(self, 1))
		return COL_ERROR_INT;

	if(i < 0 || i >= self->size){
		return EXPP_ReturnIntError(PyExc_IndexError,
			"color[index] = x: assignment index out of range\n");
	}
	self->color[i] = f;
	return 0;
}

/*----------------------------object[z:y]------------------------
  sequence slice (get) */
static PyObject *Color_slice(BPyColorObject * self, int begin, int end)
{
	PyObject *list = NULL;
	int count;
	
	if (!color_from_source__internal(self, 1))
		return COL_ERROR_PY;

	CLAMP(begin, 0, self->size);
	CLAMP(end, 0, self->size);
	begin = MIN2(begin,end);

	list = PyList_New(end - begin);
	for(count = begin; count < end; count++) {
		PyList_SetItem(list, count - begin,
				PyFloat_FromDouble(self->color[count]));
	}

	return list;
}
/*----------------------------object[z:y]------------------------
  sequence slice (set) */
static int Color_ass_slice(BPyColorObject * self, int begin, int end,
			     PyObject * seq)
{
	int i, y, size = 0;
	float col[4];
	PyObject *v;

	/* TODO - Check only */
	if (!color_from_source__internal(self, 1))
		return COL_ERROR_INT;
	
	CLAMP(begin, 0, self->size);
	CLAMP(end, 0, self->size);
	begin = MIN2(begin,end);

	size = PySequence_Length(seq);
	if(size != (end - begin)){
		return EXPP_ReturnIntError(PyExc_TypeError,
			"color[begin:end] = []: size mismatch in slice assignment\n");
	}

	for (i = 0; i < size; i++) {
		v = PySequence_GetItem(seq, i);
		if (v == NULL) { /* Failed to read sequence */
			return EXPP_ReturnIntError(PyExc_RuntimeError, 
				"color[begin:end] = []: unable to read sequence\n");
		}	
		col[i] = ( float )PyFloat_AsDouble( v ); /* catch error below */
		Py_DECREF(v);
	}
	
	if(PyErr_Occurred()) /* parsed item not a number */
		return EXPP_ReturnIntError(PyExc_TypeError, 
				"color[begin:end] = []: sequence argument not a number\n");
	
	/*parsed well - now set in color*/
	for(y = 0; y < size; y++){
		self->color[begin + y] = col[y];
	}
	return 0;
}
/*------------------------NUMERIC PROTOCOLS----------------------
  ------------------------obj + obj------------------------------
  addition*/
static PyObject *Color_add(BPyColorObject * col1, BPyColorObject * col2)
{
	int i;
	float color[4];

	if (!BPyColor_Check(col1) || !BPyColor_Check(col2))
		return EXPP_ReturnPyObjError(PyExc_ValueError,
				"Color addition: arguments not valid for this operation....\n");
	
	if(col1->size != col2->size)
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
		"Color addition: colors must have the same dimensions for this operation\n");
	
	for(i = 0; i < col1->size; i++) {
		color[i] = col1->color[i] +	col2->color[i];
	}
	return Color_CreatePyObject(color, col1->size, 0, 0, (PyObject *)NULL);
}

/*  ------------------------obj += obj------------------------------
  addition in place */
static PyObject *Color_iadd(BPyColorObject * col1, BPyColorObject * col2)
{
	int i;
	
	if (!BPyColor_Check(col1) || !BPyColor_Check(col2)) {
		return EXPP_ReturnPyObjError(PyExc_ValueError,
				"Color addition: arguments not valid for this operation....\n");		
	}
	
	if(col1->size != col2->size) {
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
				"Color addition: colors must have the same dimensions for this operation\n");
	}
	
	if (col1->source) {
		for(i = 0; i < col1->size; i++) {
			col1->color[i] +=	col2->color[i];
			CLAMP(col1->color[i], 0.0, 1.0);
		}
	} else {
		for(i = 0; i < col1->size; i++) {
			col1->color[i] +=	col2->color[i];
		}
	}
	Py_INCREF( (PyObject *)col1 );
	return (PyObject *)col1;
}

/*------------------------obj - obj------------------------------
  subtraction*/
static PyObject *Color_sub(BPyColorObject * col1, BPyColorObject * col2)
{
	int i;
	float col[4];

	if (!BPyColor_Check(col1) || !BPyColor_Check(col2))
		return EXPP_ReturnPyObjError(PyExc_ValueError,
			"Color subtraction: arguments not valid for this operation....\n");
	
	if (!color_from_source__internal(col1, 1) || !color_from_source__internal(col2, 1))
		return COL_ERROR_PY;
	
	if(col1->size != col2->size)
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
		"Color subtraction: colors must have the same dimensions for this operation\n");
	
	if (col1->source) {
		for(i = 0; i < col1->size; i++) {
			col[i] = col1->color[i] - col2->color[i];
			CLAMP(col1->color[i], 0.0, 1.0);
		}
	} else {
		for(i = 0; i < col1->size; i++) {
			col[i] = col1->color[i] - col2->color[i];
		}
	} 

	return Color_CreatePyObject(col, col1->size, 0, 0, (PyObject *)NULL);
}

/*------------------------obj -= obj------------------------------
  subtraction*/
static PyObject *Color_isub(BPyColorObject * col1, BPyColorObject * col2)
{
	int i;

	if (!BPyColor_Check(col1) || !BPyColor_Check(col2))
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Color subtraction: arguments not valid for this operation....\n");

	if (!color_from_source__internal(col1, 1) || !color_from_source__internal(col2, 1))
		return COL_ERROR_PY;
	
	if(col1->size != col2->size)
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
		"Color subtraction: colors must have the same dimensions for this operation\n");

	if (!color_from_source__internal(col1, 1) || !color_from_source__internal(col2, 1))
		return COL_ERROR_PY;
	
	if (col1->source) {
		for(i = 0; i < col1->size; i++) {
			col1->color[i] -= col2->color[i];
			CLAMP(col1->color[i], 0.0, 1.0);
		}
	} else {
		for(i = 0; i < col1->size; i++) {
			col1->color[i] -= col2->color[i];
		}
	}

	Py_INCREF( (PyObject *)col1 );
	return (PyObject *)col1;
}

/*------------------------obj * obj------------------------------
  mulplication*/
static PyObject *Color_mul(PyObject * v1, PyObject * v2)
{
	BPyColorObject *col;
	int i;
	float newcol[4];
	float scalar = (float)PyFloat_AsDouble( v2 );
	
	if (scalar==-1 && PyErr_Occurred())
		return NULL;
	
	if BPyColor_Check(v1) {
		col= (BPyColorObject *)v1;
	} else {
		col= (BPyColorObject *)v2;
		v2 = v1; /* the float is v2 always */
	}
	
	if (!color_from_source__internal(col, 1))
		return COL_ERROR_PY;
	
	
	
	for(i = 0; i < col->size; i++) {
		newcol[i] = col->color[i] * scalar;
	}
	return Color_CreatePyObject(newcol, col->size, 0, 0, (PyObject *)NULL);
}

/*------------------------obj *= obj------------------------------
  in place mulplication */
static PyObject *Color_imul(BPyColorObject *col, PyObject * v2)
{
	int i;
	float scalar = (float)PyFloat_AsDouble( v2 );
	
	if (!color_from_source__internal(col, 1))
		return COL_ERROR_PY;
	
	if (scalar==-1 && PyErr_Occurred())
		return NULL;
	
	for(i = 0; i < col->size; i++) {
		col->color[i] *= scalar;
	}
	
	Py_INCREF( (PyObject *)col );
	return (PyObject *)col;
}

/*------------------------obj / obj------------------------------
  divide*/
static PyObject *Color_div(BPyColorObject * col, PyObject * v2)
{
	int i;
	float newcol[4];
	float scalar = (float)PyFloat_AsDouble( v2 );
	
	if (!BPyColor_Check(col) || (scalar ==-1 && PyErr_Occurred()))
		return EXPP_ReturnPyObjError(PyExc_AttributeError,
			"Color division: a color can only be divided by a float\n");
	
	if (!color_from_source__internal(col, 1))
		return COL_ERROR_PY;
	
	if(scalar==0.0)
		return EXPP_ReturnPyObjError(PyExc_ZeroDivisionError, 
			"Color division: divide by zero error.\n");
	
	for(i = 0; i < col->size; i++) {
		newcol[i] = col->color[i] / scalar;
	}
	return Color_CreatePyObject(newcol, col->size, 0, 0, (PyObject *)NULL);
}

/*------------------------obj /= obj------------------------------
  divide*/
static PyObject *Color_idiv(BPyColorObject *col, PyObject * v2)
{
	int i;
	float scalar = ( float )PyFloat_AsDouble( v2 );
	if (scalar==-1 && PyErr_Occurred())
		return EXPP_ReturnPyObjError(PyExc_TypeError,
				"Color multiplication: arguments not acceptable for this operation\n");
	
	if (!color_from_source__internal(col, 1))
		return COL_ERROR_PY;
	
	if(scalar==0.0)
		return EXPP_ReturnPyObjError(PyExc_ZeroDivisionError, 
			"Color division: divide by zero error.\n");
	
	for(i = 0; i < col->size; i++) {
		col->color[i] /= scalar;
	}
	
	Py_INCREF( (PyObject *)col );
	return (PyObject *)col;
}

/*-------------------------- -obj -------------------------------
  returns the negative of this object*/
static PyObject *Color_neg(BPyColorObject *self)
{
	int i;
	float col[4];
	
	if (!color_from_source__internal(self, 1))
		return COL_ERROR_PY;
	
	for(i = 0; i < self->size; i++){
		col[i] = -self->color[i];
	}
	return Color_CreatePyObject(col, self->size, 0, 0, (PyObject *)NULL);
}

/*------------------------tp_doc*/
static char BPyColor_doc[] = "This is a wrapper for color objects.";
/*------------------------col_magnitude_simple (internal) - for comparing only */
static float col_magnitude_simple(float *data, int size)
{
	float dot = 0.0f;
	int i;

	for(i=0; i<size; i++)
		dot += data[i];
	
	return dot;
}


/*------------------------tp_richcmpr
  returns -1 execption, 0 false, 1 true */
PyObject* Color_richcmpr(BPyColorObject *colA, BPyColorObject *colB, int comparison_type)
{
	int result = 0;
	float epsilon = .000001f;
	double lenA, lenB;
	
	if (!BPyColor_Check(colA) || !BPyColor_Check(colB)){
		if (comparison_type == Py_NE){
			Py_RETURN_TRUE;
		}else{
			Py_RETURN_FALSE;
		}
	}
	
	if (colA->size != colB->size){
		if (comparison_type == Py_NE){
			Py_RETURN_TRUE;
		}else{
			Py_RETURN_FALSE;
		}
	}

	/* invalid types are like NAN - can never == eachother
	 * This also updates the color values internally for colA and colB */
	if (!color_from_source__internal(colA, 1) || !color_from_source__internal(colB, 1)) {
		if (comparison_type == Py_NE){
			Py_RETURN_TRUE;
		}else{
			Py_RETURN_FALSE;
		}
	}
	
	switch (comparison_type){
		case Py_LT:
			if(		col_magnitude_simple(colA->color, colA->size) <
					col_magnitude_simple(colB->color, colB->size)
			) {
				result = 1;
			}
			break;
		case Py_LE:
			lenA = col_magnitude_simple(colA->color, colA->size);
			lenB = col_magnitude_simple(colB->color, colB->size);
			if(		lenA <
					lenB
			) {
				result = 1;
			} else {
				result = (((lenA + epsilon) > lenB) && ((lenA - epsilon) < lenB));
			}
			break;
		case Py_EQ:
			result = EXPP_VectorsAreEqual(colA->color, colB->color, colA->size, 1);
			break;
		case Py_NE:
			result = EXPP_VectorsAreEqual(colA->color, colB->color, colA->size, 1);
			if (result == 0){
				result = 1;
			}else{
				result = 0;
			}
			break;
		case Py_GT:
			lenA = col_magnitude_simple(colA->color, colA->size);
			lenB = col_magnitude_simple(colB->color, colB->size);
			if( lenA > lenB ){
				result = 1;
			}
			break;
		case Py_GE:
			lenA = col_magnitude_simple(colA->color, colA->size);
			lenB = col_magnitude_simple(colB->color, colB->size);
			if( lenA > lenB ){
				result = 1;
			}else{
				result = (((lenA + epsilon) > lenB) && ((lenA - epsilon) < lenB));
			}
			break;
		default:
			printf("The result of the comparison could not be evaluated");
			break;
	}
	if (result == 1){
		Py_RETURN_TRUE;
	}else{
		Py_RETURN_FALSE;
	}
}
/*-----------------PROTCOL DECLARATIONS--------------------------*/
static PySequenceMethods Color_SeqMethods = {
	(inquiry) Color_len,						/* sq_length */
	(binaryfunc) 0,								/* sq_concat */
	(intargfunc) 0,								/* sq_repeat */
	(intargfunc) Color_item,					/* sq_item */
	(intintargfunc) Color_slice,				/* sq_slice */
	(intobjargproc) Color_ass_item,			/* sq_ass_item */
	(intintobjargproc) Color_ass_slice,		/* sq_ass_slice */
};


/* For numbers without flag bit Py_TPFLAGS_CHECKTYPES set, all
   arguments are guaranteed to be of the object's type (modulo
   coercion hacks -- i.e. if the type's coercion function
   returns other types, then these are allowed as well).  Numbers that
   have the Py_TPFLAGS_CHECKTYPES flag bit set should check *both*
   arguments for proper type and implement the necessary conversions
   in the slot functions themselves. */
 
static PyNumberMethods Color_NumMethods = {
	(binaryfunc) Color_add,					/* __add__ */
	(binaryfunc) Color_sub,					/* __sub__ */
	(binaryfunc) Color_mul,					/* __mul__ */
	(binaryfunc) Color_div,					/* __div__ */
	(binaryfunc) NULL,							/* __mod__ */
	(binaryfunc) NULL,							/* __divmod__ */
	(ternaryfunc) NULL,							/* __pow__ */
	(unaryfunc) Color_neg,						/* __neg__ */
	(unaryfunc) NULL,							/* __pos__ */
	(unaryfunc) NULL,							/* __abs__ */
	(inquiry) NULL,								/* __nonzero__ */
	(unaryfunc) NULL,							/* __invert__ */
	(binaryfunc) NULL,							/* __lshift__ */
	(binaryfunc) NULL,							/* __rshift__ */
	(binaryfunc) NULL,							/* __and__ */
	(binaryfunc) NULL,							/* __xor__ */
	(binaryfunc) NULL,							/* __or__ */
	(coercion)  NULL,							/* __coerce__ */
	(unaryfunc) NULL,							/* __int__ */
	(unaryfunc) NULL,							/* __long__ */
	(unaryfunc) NULL,							/* __float__ */
	(unaryfunc) NULL,							/* __oct__ */
	(unaryfunc) NULL,							/* __hex__ */
	
	/* Added in release 2.0 */
	(binaryfunc) Color_iadd,					/*__iadd__*/
	(binaryfunc) Color_isub,					/*__isub__*/
	(binaryfunc) Color_imul,					/*__imul__*/
	(binaryfunc) Color_idiv,					/*__idiv__*/
	(binaryfunc) NULL,							/*__imod__*/
	(ternaryfunc) NULL,							/*__ipow__*/
	(binaryfunc) NULL,							/*__ilshift__*/
	(binaryfunc) NULL,							/*__irshift__*/
	(binaryfunc) NULL,							/*__iand__*/
	(binaryfunc) NULL,							/*__ixor__*/
	(binaryfunc) NULL,							/*__ior__*/
 
	/* Added in release 2.2 */
	/* The following require the Py_TPFLAGS_HAVE_CLASS flag */
	(binaryfunc) NULL,							/*__floordiv__  __rfloordiv__*/
	(binaryfunc) NULL,							/*__truediv__ __rfloordiv__*/
	(binaryfunc) NULL,							/*__ifloordiv__*/
	(binaryfunc) NULL,							/*__itruediv__*/
};
/*------------------PY_OBECT DEFINITION--------------------------*/

static PyObject *Color_invert(BPyColorObject * self )
{	
	int i;
	if (!color_from_source__internal(self, 1))
		return COL_ERROR_PY;
		
	
	for (i=0; i<self->size; i++)
		self->color[i] = 1.0f - self->color[i];
	
	color_to_source__internal(self, 0); /* alredy checked */
	Py_RETURN_NONE;
}

static PyObject *Color_tint(BPyColorObject * self, PyObject * args)
{	
	float tint, tint_inv;
	BPyColorObject * tint_col;
	int i;
	
	if( !PyArg_ParseTuple ( args, "O!f", &BPyColor_Type, &tint_col, &tint ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError, 
			"expected color and a float arguments\n" );
	
	/* make sure the colors are up to date */
	
	if (!color_from_source__internal(self, 1) || !color_from_source__internal(tint_col, 1))
		return COL_ERROR_PY;
	
	/* no tint, so just return */
	if (tint <= 0.0f)
		Py_RETURN_NONE;
	
	/* 1.0 or above, just copy the color */
	if (tint >= 1.0f)
		tint = 1.0f;

	tint_inv = 1.0 - tint;
	
	/* If the tint color is not from blender data it is not always clamped, so clamp it */
	if (self->source && !tint_col->source) {
		for (i=0; i<3; i++) {
			self->color[i] = (tint_col->color[i]*tint) + (self->color[i]*tint_inv);
			CLAMP(self->color[i], 0.0, 1.0);
		}
	} else {
		for (i=0; i<3; i++) {
			self->color[i] = (tint_col->color[i]*tint) + (self->color[i]*tint_inv);
		}
	}
	
	color_to_source__internal(self, 0);
	Py_RETURN_NONE;
}

/*-----------------------METHOD DEFINITIONS ----------------------*/
struct PyMethodDef Color_methods[] = {
	{"invert", (PyCFunction) Color_invert, METH_NOARGS, "Invert this color"},
	{"tint", (PyCFunction) Color_tint, METH_VARARGS, "Tint this color"},
	{"copy", (PyCFunction) Color_copy, METH_NOARGS, "Copy this color"},
	{"__copy__", (PyCFunction) Color_copy, METH_NOARGS, "Copy this color"},
	{NULL, NULL, 0, NULL}
};

static PyObject *Color_getCol(BPyColorObject * self, void * axis)
{
	if ((long)axis > self->size)
			return EXPP_ReturnPyObjError(PyExc_AttributeError,
				"color.a: error, this color has no alpha\n");
	
	if (!color_from_source__internal(self, 1))
		return COL_ERROR_PY;
	
	return PyFloat_FromDouble(self->color[(long)axis]);
}

static int Color_setCol(BPyColorObject * self, PyObject * value, void * axis)
{
	float f = ( float )PyFloat_AsDouble( value );
	if (f==-1 && PyErr_Occurred())
		return -1;
	
	/* TODO - This sucks quite a lot that we do this, should probably add a check only function
	 * since we only need to check teh color and not copy from original data */
	if (!color_from_source__internal(self, 1))
		return COL_ERROR_INT;
	
	
	/* if not wrapped there is no limit to the color value, its a bit like a Color
	 * This is important so you can do stuff like col = (col1+col2) / 2
	 * */
	self->color[(long)axis] = f;
	
	color_to_source__internal(self, 0);
	return 0;
}

static PyObject *Color_getHSV(BPyColorObject * self, void * axis)
{
	float hsv[3];
	if (!color_from_source__internal(self, 1))
		return COL_ERROR_PY;
	
	rgb_to_hsv(self->color[0], self->color[1], self->color[2], &hsv[0], &hsv[1], &hsv[2]);
	return PyFloat_FromDouble(hsv[(long)axis]);
}

static int Color_setHSV(BPyColorObject * self, PyObject * value, void * axis)
{
	float hsv[3];
	float f = ( float )PyFloat_AsDouble( value );
	if (f==-1 && PyErr_Occurred())
		return -1;
	
	if (!color_from_source__internal(self, 1))
		return COL_ERROR_INT;
	
	rgb_to_hsv(self->color[0], self->color[1], self->color[2], &hsv[0], &hsv[1], &hsv[2]);
	
	hsv[(long)axis] = f;
	
	/* now convert back to rgb */
	hsv_to_rgb( hsv[0], hsv[1], hsv[2], &self->color[0], &self->color[1], &self->color[2]);
	
	color_to_source__internal(self, 0);
	return 0;
}

static PyObject *Color_getWrapped(BPyColorObject * self)
{	
	if (self->source)
		Py_RETURN_TRUE;
	
	Py_RETURN_FALSE;
}



static PyObject *Color_getPosition(BPyColorObject * self)
{
	if (self->type != BPY_COLOR_BAND && self->type != BPY_COLOR_BAND_SPEC)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
			"color.position is only valid for colorbands" );
	return PyFloat_FromDouble( 0.0 ); /* TODO - support colorbands */
}

static int Color_setPosition(BPyColorObject * self, PyObject * value)
{
	if (!PyFloat_Check(value) && !PyInt_Check(value))
		return EXPP_ReturnIntError( PyExc_TypeError,
			"expected a number for the Color axis" );
	
	/* TODO - Colorband */
	return 0;
}


/*****************************************************************************/
/* Python attributes get/set structure:                                      */
/*****************************************************************************/
static PyGetSetDef Color_getset[] = {
	{"r",
	 (getter)Color_getCol, (setter)Color_setCol,
	 "color red component",
	 (void *)0},
	{"g",
	 (getter)Color_getCol, (setter)Color_setCol,
	 "color green component",
	 (void *)1},
	{"b",
	 (getter)Color_getCol, (setter)Color_setCol,
	 "color blue component",
	 (void *)2},
	{"a",
	 (getter)Color_getCol, (setter)Color_setCol,
	 "color alpha component",
	 (void *)3},

	{"h",
	 (getter)Color_getHSV, (setter)Color_setHSV,
	 "color hue",
	 (void *)0},
	{"s",
	 (getter)Color_getHSV, (setter)Color_setHSV,
	 "color saturation",
	 (void *)1},
	{"v",
	 (getter)Color_getHSV, (setter)Color_setHSV,
	 "color light value",
	(void *)2},

	/* colorband only */
	{"position",
	 (getter)Color_getPosition, (setter)Color_setPosition,
	 "the position of this color in a colorband",
	 NULL},
	 
	{"wrapped",
	 (getter)Color_getWrapped, (setter)NULL,
	 "Color Length",
	 NULL},
	{NULL}  /* Sentinel */
};



/* this types constructor - apply any fixes to vec, eul, quat and col,  */
static PyObject *Color_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	float fvec[4];
	switch (PyTuple_Size(args)) {
	case 0:
		return Color_CreatePyObject(NULL, 3, 0, 0, (PyObject *)NULL);
	case 1: /* single argument */
		args = PyTuple_GET_ITEM(args, 0);
		if (EXPP_setVec3(args, &fvec[0], &fvec[1], &fvec[2])!=-1) {
			return Color_CreatePyObject(fvec, 3, 0, 0, (PyObject *)NULL);
		} else if (EXPP_setVec4(args, &fvec[0], &fvec[1], &fvec[2], &fvec[3])!=-1) {
			PyErr_Clear(); /* from the failed EXPP_setVec3 */
			return Color_CreatePyObject(fvec, 4, 0, 0, (PyObject *)NULL);
		} else {
			return NULL;
		}
	case 3:
		if (EXPP_setVec3(args, &fvec[0], &fvec[1], &fvec[2])==-1)
			return NULL;
		return Color_CreatePyObject(fvec, 3, 0, 0, (PyObject *)NULL);
	case 4:
		if (EXPP_setVec4(args, &fvec[0], &fvec[1], &fvec[2], &fvec[3])==-1)
			return NULL;
		return Color_CreatePyObject(fvec, 4, 0, 0, (PyObject *)NULL);
	}
	
	return EXPP_ReturnPyObjError( PyExc_TypeError,
			"Error, expected 0, 1, 3 or 4 args for the new color." );
}

PyTypeObject BPyColor_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Color",             /* char *tp_name; */
	sizeof( BPyColorObject ),         /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) Color_dealloc,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,   /* cmpfunc tp_compare; */
	( reprfunc ) Color_repr,     /* reprfunc tp_repr; */

	/* Method suites for standard classes */

	&Color_NumMethods,                       /* PyNumberMethods *tp_as_number; */
	&Color_SeqMethods,                       /* PySequenceMethods *tp_as_sequence; */
	NULL,                       /* PyMappingMethods *tp_as_mapping; */

	/* More standard operations (here for binary compatibility) */

	NULL,                       /* hashfunc tp_hash; */
	NULL,                       /* ternaryfunc tp_call; */
	NULL,                       /* reprfunc tp_str; */
	NULL,                       /* getattrofunc tp_getattro; */
	NULL,                       /* setattrofunc tp_setattro; */

	/* Functions to access object as input/output buffer */
	NULL,                       /* PyBufferProcs *tp_as_buffer; */

  /*** Flags to define presence of optional/expanded features ***/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,         /* long tp_flags; */

	BPyColor_doc,                       /*  char *tp_doc;  Documentation string */
  /*** Assigned meaning in release 2.0 ***/
	/* call function for all accessible objects */
	NULL,                       /* traverseproc tp_traverse; */

	/* delete references to contained objects */
	NULL,                       /* inquiry tp_clear; */

  /***  Assigned meaning in release 2.1 ***/
  /*** rich comparisons ***/
	(richcmpfunc)Color_richcmpr,                       /* richcmpfunc tp_richcompare; */

  /***  weak reference enabler ***/
	0,                          /* long tp_weaklistoffset; */

  /*** Added in release 2.2 ***/
	/*   Iterators */
	NULL,                       /* getiterfunc tp_iter; */
	NULL,                       /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	Color_methods,           /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	Color_getset,				/* struct PyGetSetDef *tp_getset; */
	NULL,                       /* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc )Color_new,		/* newfunc tp_new; */
	/*  Low-level free-memory routine */
	NULL,                       /* freefunc tp_free;  */
	/* For PyObject_IS_GC */
	NULL,                       /* inquiry tp_is_gc;  */
	NULL,                       /* PyObject *tp_bases; */
	/* method resolution order */
	NULL,                       /* PyObject *tp_mro;  */
	NULL,                       /* PyObject *tp_cache; */
	NULL,                       /* PyObject *tp_subclasses; */
	NULL,                       /* PyObject *tp_weaklist; */
	NULL
};


/*------------------------Color_CreatePyObject (internal)-------------
  creates a new Color object
  if source is NULL, this is not a wrapped.
  Py_None - wrapped but its pointer wont be updated  
  Matrix - an item of a pointer, use col->sub_index to set the row
  Vertex - location
  Face - UV, col->sub_index for the point in the face (0-3)
  PVert - ignored, like None
  
  Color_CheckPyObject to see how different data is updated from its source.
  */
//PyObject *Color_CreatePyObject(float *col, int size, PyObject * source)
PyObject *Color_CreatePyObject(float *color, char size, char type, int index, PyObject * source)
{
	int i;
	BPyColorObject *self = PyObject_NEW(BPyColorObject, &BPyColor_Type);
	
	self->size =	size;
	self->index =	index;
	self->type =	type;
	self->source =	source;
	
	if(source) {
		color_from_source__internal(self, 0); /* this updates self->color */
		Py_INCREF(source);
		
	} else { /* New */
		if(!color) { /*new empty*/
			for(i = 0; i < size; i++){
				self->color[i] = 0.0f;
			}
			if(size == 4)  /* do the homogenous thing */
				self->color[3] = 1.0f;
		}else{
			for(i = 0; i < size; i++){
				self->color[i] = color[i];
			}
		}
	}
	return (PyObject *) self;
}

/* type check and validity check, else return the color */
BPyColorObject * Color_GetUpdated(PyObject * obj)
{
	if (!obj || !BPyColor_Check(obj) || !color_from_source__internal((BPyColorObject *)obj, 1))
		return NULL;
	return (BPyColorObject *)obj;
}

//int setColorObject(BPyColorObject * self, PyObject * value) {
//	return Color_ass_slice(self, 0, 4, value);
//}

int Color_CheckPyObject(BPyColorObject * self) {
	return color_from_source__internal(self, 1);
}

PyObject *ColorType_Init(void)
{
	PyType_Ready( &BPyColor_Type );
	return (PyObject *) &BPyColor_Type;	
}

void Color_Init(void)
{
	ColorType_Init();
}

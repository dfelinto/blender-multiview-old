/* 
 * $Id: Geometry.c 12802 2007-12-06 00:01:46Z campbellbarton $
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
 * Inc., 59 Temple Place - Suite 330, Boston, MA	02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * This is a new part of Blender.
 *
 * Contributor(s): Joseph Gilbert, Campbell Barton
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */

#include "Geometry.h"

/*  - Not needed for now though other geometry functions will probably need them
#include "BLI_arithb.h"
#include "BKE_utildefines.h"
*/

/* Used for PolyFill */
#include "BKE_displist.h"
#include "MEM_guardedalloc.h"
#include "BLI_blenlib.h"
#include "BLI_arithb.h"

/* needed for EXPP_ReturnPyObjError and EXPP_check_sequence_consistency */
#include "gen_utils.h"
 
#include "BKE_utildefines.h"
#include "BLI_boxpack2d.h"

#define SWAP_FLOAT(a,b,tmp) tmp=a; a=b; b=tmp
#define eul 0.000001

/*----------------------------------Geometry.PolyFill() -------------------*/
/* PolyFill function, uses Blenders scanfill to fill multiple poly lines */
static PyObject *Geometry_PolyFill( PyObject * self, PyObject * args )
{
	PyObject *tri_list; /*return this list of tri's */
	PyObject *polyLineSeq, *polyLine, *polyVec;
	int i, len_polylines, len_polypoints;
	
	/* display listbase */
	ListBase dispbase={NULL, NULL};
	DispList *dl;
	float *fp; /*pointer to the array of malloced dl->verts to set the points from the vectors */
	int index, *dl_face, totpoints=0;
	
	
	dispbase.first= dispbase.last= NULL;
	
	
	if(!PyArg_ParseTuple ( args, "O", &polyLineSeq) || !PySequence_Check(polyLineSeq)) {
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected a sequence of poly lines" );
	}
	
	len_polylines = PySequence_Size( polyLineSeq );
	
	for( i = 0; i < len_polylines; ++i ) {
		polyLine= PySequence_GetItem( polyLineSeq, i );
		if (!PySequence_Check(polyLine)) {
			freedisplist(&dispbase);
			Py_XDECREF(polyLine); /* may be null so use Py_XDECREF*/
			return EXPP_ReturnPyObjError( PyExc_TypeError,
				  "One or more of the polylines is not a sequence of Mathutils.Vector's" );
		}
		
		len_polypoints= PySequence_Size( polyLine );
		if (len_polypoints>0) { /* dont bother adding edges as polylines */
			if (EXPP_check_sequence_consistency( polyLine, &BPyVector_Type ) != 1) {
				freedisplist(&dispbase);
				Py_DECREF(polyLine);
				return EXPP_ReturnPyObjError( PyExc_TypeError,
					  "A point in one of the polylines is not a Mathutils.Vector type" );
			}
			
			dl= MEM_callocN(sizeof(DispList), "poly disp");
			BLI_addtail(&dispbase, dl);
			dl->type= DL_INDEX3;
			dl->nr= len_polypoints;
			dl->type= DL_POLY;
			dl->parts= 1; /* no faces, 1 edge loop */
			dl->col= 0; /* no material */
			dl->verts= fp= MEM_callocN( sizeof(float)*3*len_polypoints, "dl verts");
			dl->index= MEM_callocN(sizeof(int)*3*len_polypoints, "dl index");
			
			for( index = 0; index<len_polypoints; ++index, fp+=3) {
				polyVec= PySequence_GetItem( polyLine, index );
				
				fp[0] = ((BPyVectorObject *)polyVec)->vec[0];
				fp[1] = ((BPyVectorObject *)polyVec)->vec[1];
				if( ((BPyVectorObject *)polyVec)->size > 2 )
					fp[2] = ((BPyVectorObject *)polyVec)->vec[2];
				else
					fp[2]= 0.0f; /* if its a 2d vector then set the z to be zero */
				
				totpoints++;
				Py_DECREF(polyVec);
			}
		}
		Py_DECREF(polyLine);
	}
	
	if (totpoints) {
		/* now make the list to return */
		filldisplist(&dispbase, &dispbase);
		
		/* The faces are stored in a new DisplayList
		thats added to the head of the listbase */
		dl= dispbase.first; 
		
		tri_list= PyList_New(dl->parts);
		if( !tri_list ) {
			freedisplist(&dispbase);
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					"Geometry.PolyFill failed to make a new list" );
		}
		
		index= 0;
		dl_face= dl->index;
		while(index < dl->parts) {
			PyList_SetItem(tri_list, index, Py_BuildValue("iii", dl_face[0], dl_face[1], dl_face[2]) );
			dl_face+= 3;
			index++;
		}
		freedisplist(&dispbase);
	} else {
		/* no points, do this so scripts dont barf */
		tri_list= PyList_New(0);
	}
	
	return tri_list;
}


//----------------------------------Mathutils.LineIntersect() -------------------
/* Line-Line intersection using algorithm from mathworld.wolfram.com */
PyObject *Geometry_LineIntersect( PyObject * self, PyObject * args )
{
	PyObject * tuple;
	BPyVectorObject *vec1, *vec2, *vec3, *vec4;
	float v1[3], v2[3], v3[3], v4[3], i1[3], i2[3];

	if( !PyArg_ParseTuple
	    ( args, "O!O!O!O!", &BPyVector_Type, &vec1, &BPyVector_Type, &vec2
		, &BPyVector_Type, &vec3, &BPyVector_Type, &vec4 ) )
		return ( EXPP_ReturnPyObjError
			 ( PyExc_TypeError, "expected 4 vector types\n" ) );
	if( vec1->size != vec2->size || vec1->size != vec3->size || vec1->size != vec2->size)
		return ( EXPP_ReturnPyObjError( PyExc_TypeError,
						"vectors must be of the same size\n" ) );

	if( vec1->size == 3 || vec1->size == 2) {
		float a[3], b[3], c[3], ab[3], cb[3], dir1[3], dir2[3];
		float d;
		if (vec1->size == 3) {
			VECCOPY(v1, vec1->vec);
			VECCOPY(v2, vec2->vec);
			VECCOPY(v3, vec3->vec);
			VECCOPY(v4, vec4->vec);
		}
		else {
			v1[0] = vec1->vec[0];
			v1[1] = vec1->vec[1];
			v1[2] = 0.0f;

			v2[0] = vec2->vec[0];
			v2[1] = vec2->vec[1];
			v2[2] = 0.0f;

			v3[0] = vec3->vec[0];
			v3[1] = vec3->vec[1];
			v3[2] = 0.0f;

			v4[0] = vec4->vec[0];
			v4[1] = vec4->vec[1];
			v4[2] = 0.0f;
		}

		VecSubf(c, v3, v1);
		VecSubf(a, v2, v1);
		VecSubf(b, v4, v3);

		VECCOPY(dir1, a);
		Normalize(dir1);
		VECCOPY(dir2, b);
		Normalize(dir2);
		d = Inpf(dir1, dir2);
		if (d == 1.0f || d == -1.0f) {
			/* colinear */
			return EXPP_incr_ret( Py_None );
		}

		Crossf(ab, a, b);
		d = Inpf(c, ab);

		/* test if the two lines are coplanar */
		if (d > -0.000001f && d < 0.000001f) {
			Crossf(cb, c, b);

			VecMulf(a, Inpf(cb, ab) / Inpf(ab, ab));
			VecAddf(i1, v1, a);
			VECCOPY(i2, i1);
		}
		/* if not */
		else {
			float n[3], t[3];
			VecSubf(t, v1, v3);

			/* offset between both plane where the lines lies */
			Crossf(n, a, b);
			Projf(t, t, n);

			/* for the first line, offset the second line until it is coplanar */
			VecAddf(v3, v3, t);
			VecAddf(v4, v4, t);
			
			VecSubf(c, v3, v1);
			VecSubf(a, v2, v1);
			VecSubf(b, v4, v3);

			Crossf(ab, a, b);
			Crossf(cb, c, b);

			VecMulf(a, Inpf(cb, ab) / Inpf(ab, ab));
			VecAddf(i1, v1, a);

			/* for the second line, just substract the offset from the first intersection point */
			VecSubf(i2, i1, t);
		}

		tuple = PyTuple_New( 2 );
		PyTuple_SetItem( tuple, 0, Vector_CreatePyObject(i1, vec1->size, (PyObject *)NULL));
		PyTuple_SetItem( tuple, 1, Vector_CreatePyObject(i2, vec1->size, (PyObject *)NULL));
		return tuple;
	}
	else {
		return ( EXPP_ReturnPyObjError( PyExc_TypeError,
						"2D/3D vectors only\n" ) );
	}
}

static PyObject *Geometry_LineIntersect2D( PyObject * self, PyObject * args )
{
	BPyVectorObject *line_a1, *line_a2, *line_b1, *line_b2;
	float a1x, a1y, a2x, a2y,  b1x, b1y, b2x, b2y, xi, yi, a1,a2,b1,b2, newvec[2];
	if( !PyArg_ParseTuple ( args, "O!O!O!O!",
	  &BPyVector_Type, &line_a1,
	  &BPyVector_Type, &line_a2,
	  &BPyVector_Type, &line_b1,
	  &BPyVector_Type, &line_b2)
	)
		return ( EXPP_ReturnPyObjError
			 ( PyExc_TypeError, "expected 4 vector types\n" ) );
	
	a1x= line_a1->vec[0];
	a1y= line_a1->vec[1];
	a2x= line_a2->vec[0];
	a2y= line_a2->vec[1];

	b1x= line_b1->vec[0];
	b1y= line_b1->vec[1];
	b2x= line_b2->vec[0];
	b2y= line_b2->vec[1];
	
	if((MIN2(a1x, a2x) > MAX2(b1x, b2x)) ||
	   (MAX2(a1x, a2x) < MIN2(b1x, b2x)) ||
	   (MIN2(a1y, a2y) > MAX2(b1y, b2y)) ||
	   (MAX2(a1y, a2y) < MIN2(b1y, b2y))  ) {
		Py_RETURN_NONE;
	}
	/* Make sure the hoz/vert line comes first. */
	if (fabs(b1x - b2x) < eul || fabs(b1y - b2y) < eul) {
		SWAP_FLOAT(a1x, b1x, xi); /*abuse xi*/
		SWAP_FLOAT(a1y, b1y, xi);
		SWAP_FLOAT(a2x, b2x, xi);
		SWAP_FLOAT(a2y, b2y, xi);
	}
	
	if (fabs(a1x-a2x) < eul) { /* verticle line */
		if (fabs(b1x-b2x) < eul){ /*verticle second line */
			Py_RETURN_NONE; /* 2 verticle lines dont intersect. */
		}
		else if (fabs(b1y-b2y) < eul) {
			/*X of vert, Y of hoz. no calculation needed */
			newvec[0]= a1x;
			newvec[1]= b1y;
			return Vector_CreatePyObject(newvec, 2, (PyObject *)NULL);
		}
		
		yi = (float)(((b1y / fabs(b1x - b2x)) * fabs(b2x - a1x)) + ((b2y / fabs(b1x - b2x)) * fabs(b1x - a1x)));
		
		if (yi > MAX2(a1y, a2y)) {/* New point above seg1's vert line */
			Py_RETURN_NONE;
		} else if (yi < MIN2(a1y, a2y)) { /* New point below seg1's vert line */
			Py_RETURN_NONE;
		}
		newvec[0]= a1x;
		newvec[1]= yi;
		return Vector_CreatePyObject(newvec, 2, (PyObject *)NULL);
	} else if (fabs(a2y-a1y) < eul) {  /* hoz line1 */
		if (fabs(b2y-b1y) < eul) { /*hoz line2*/
			Py_RETURN_NONE; /*2 hoz lines dont intersect*/
		}
		
		/* Can skip vert line check for seg 2 since its covered above. */
		xi = (float)(((b1x / fabs(b1y - b2y)) * fabs(b2y - a1y)) + ((b2x / fabs(b1y - b2y)) * fabs(b1y - a1y)));
		if (xi > MAX2(a1x, a2x)) { /* New point right of hoz line1's */
			Py_RETURN_NONE;
		} else if (xi < MIN2(a1x, a2x)) { /*New point left of seg1's hoz line */
			Py_RETURN_NONE;
		}
		newvec[0]= xi;
		newvec[1]= a1y;
		return Vector_CreatePyObject(newvec, 2, (PyObject *)NULL);
	}
	
	b1 = (a2y-a1y)/(a2x-a1x);
	b2 = (b2y-b1y)/(b2x-b1x);
	a1 = a1y-b1*a1x;
	a2 = b1y-b2*b1x;
	
	if (b1 - b2 == 0.0) {
		Py_RETURN_NONE;
	}
	
	xi = - (a1-a2)/(b1-b2);
	yi = a1+b1*xi;
	if ((a1x-xi)*(xi-a2x) >= 0 && (b1x-xi)*(xi-b2x) >= 0 && (a1y-yi)*(yi-a2y) >= 0 && (b1y-yi)*(yi-b2y)>=0) {
		newvec[0]= xi;
		newvec[1]= yi;
		return Vector_CreatePyObject(newvec, 2, (PyObject *)NULL);
	}
	Py_RETURN_NONE;
}

static PyObject *M_Geometry_ClosestPointOnLine( PyObject * self, PyObject * args )
{
	BPyVectorObject *pt, *line_1, *line_2;
	float pt_in[3], pt_out[3], l1[3], l2[3];
	float lambda;
	PyObject *ret;
	
	if( !PyArg_ParseTuple ( args, "O!O!O!",
	&BPyVector_Type, &pt,
	&BPyVector_Type, &line_1,
	&BPyVector_Type, &line_2)
	  )
		return ( EXPP_ReturnPyObjError
				( PyExc_TypeError, "expected 3 vector types\n" ) );
	
	/* accept 2d verts */
	if (pt->size==3) { VECCOPY(pt_in, pt->vec);}
	else { pt_in[2]=0.0;	VECCOPY2D(pt_in, pt->vec) }
	
	if (line_1->size==3) { VECCOPY(l1, line_1->vec);}
	else { l1[2]=0.0;	VECCOPY2D(l1, line_1->vec) }
	
	if (line_2->size==3) { VECCOPY(l2, line_2->vec);}
	else { l2[2]=0.0;	VECCOPY2D(l2, line_2->vec) }
	
	/* do the calculation */
	lambda = lambda_cp_line_ex(pt_in, l1, l2, pt_out);
	
	ret = PyTuple_New(2);
	PyTuple_SET_ITEM( ret, 0, Vector_CreatePyObject(pt_out, 3, (PyObject *)NULL) );
	PyTuple_SET_ITEM( ret, 1, PyFloat_FromDouble(lambda) );
	return ret;
}

#define SIDE_OF_LINE(pa,pb,pp)	((pa[0]-pp[0])*(pb[1]-pp[1]))-((pb[0]-pp[0])*(pa[1]-pp[1]))
#define POINT_IN_TRI(p0,p1,p2,p3)	((SIDE_OF_LINE(p1,p2,p0)>=0) && (SIDE_OF_LINE(p2,p3,p0)>=0) && (SIDE_OF_LINE(p3,p1,p0)>=0))

static PyObject *Geometry_PointInTriangle2D( PyObject * self, PyObject * args )
{
	BPyVectorObject *pt_vec, *tri_p1, *tri_p2, *tri_p3;
	
	if( !PyArg_ParseTuple ( args, "O!O!O!O!",
	  &BPyVector_Type, &pt_vec,
	  &BPyVector_Type, &tri_p1,
	  &BPyVector_Type, &tri_p2,
	  &BPyVector_Type, &tri_p3)
	)
		return ( EXPP_ReturnPyObjError
			 ( PyExc_TypeError, "expected 4 vector types\n" ) );
	
	if POINT_IN_TRI(pt_vec->vec, tri_p1->vec, tri_p2->vec, tri_p3->vec)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

int boxPack_FromPyObject(PyObject * value, boxPack **boxarray )
{
	int len, i;
	PyObject *list_item, *item_1, *item_2;
	boxPack *box;
	
	
	/* Error checking must alredy be done */
	if( !PyList_Check( value ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"can only back a list of [x,y,x,w]" );
	
	len = PyList_Size( value );
	
	(*boxarray) = MEM_mallocN( len*sizeof(boxPack), "boxPack box");
	
	
	for( i = 0; i < len; i++ ) {
		list_item = PyList_GET_ITEM( value, i );
		if( !PyList_Check( list_item ) || PyList_Size( list_item ) < 4 ) {
			MEM_freeN(*boxarray);
			return EXPP_ReturnIntError( PyExc_TypeError,
					"can only back a list of [x,y,x,w]" );
		}
		
		box = (*boxarray)+i;
		
		item_1 = PyList_GET_ITEM(list_item, 2);
		item_2 = PyList_GET_ITEM(list_item, 3);
		
		if (!PyNumber_Check(item_1) || !PyNumber_Check(item_2)) {
			MEM_freeN(*boxarray);
			return EXPP_ReturnIntError( PyExc_TypeError,
					"can only back a list of 2d boxes [x,y,x,w]" );
		}
		
		box->w =  (float)PyFloat_AsDouble( item_1 );
		box->h =  (float)PyFloat_AsDouble( item_2 );
		box->index = i;
		/* verts will be added later */
	}
	return 0;
}

void boxPack_ToPyObject(PyObject * value, boxPack **boxarray)
{
	int len, i;
	PyObject *list_item;
	boxPack *box;
	
	len = PyList_Size( value );
	
	for( i = 0; i < len; i++ ) {
		box = (*boxarray)+i;
		list_item = PyList_GET_ITEM( value, box->index );
		PyList_SET_ITEM( list_item, 0, PyFloat_FromDouble( box->x ));
		PyList_SET_ITEM( list_item, 1, PyFloat_FromDouble( box->y ));
	}
	MEM_freeN(*boxarray);
}


static PyObject *Geometry_BoxPack2D( PyObject * self, PyObject * args )
{
	PyObject *boxlist; /*return this list of tri's */
	boxPack *boxarray;
	float tot_width, tot_height;
	int len;
	int error;
	
	if(!PyArg_ParseTuple ( args, "O", &boxlist) || !PyList_Check(boxlist)) {
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected a sequence of boxes [[x,y,w,h], ... ]" );
	}
	
	len = PyList_Size( boxlist );
	
	if (!len)
		return Py_BuildValue( "ff", 0.0, 0.0);
	
	error = boxPack_FromPyObject(boxlist, &boxarray);
	if (error!=0)	return NULL;
	
	/* Non Python function */
	boxPack2D(boxarray, len, &tot_width, &tot_height);
	
	boxPack_ToPyObject(boxlist, &boxarray);
	
	return Py_BuildValue( "ff", tot_width, tot_height);
}

//---------------------------------INTERSECTION FUNCTIONS--------------------
//----------------------------------Mathutils.Intersect() -------------------
PyObject *Geometry_Intersect( PyObject * self, PyObject * args )
{
	BPyVectorObject *ray, *ray_off, *vec1, *vec2, *vec3;
	float dir[3], orig[3], v1[3], v2[3], v3[3], e1[3], e2[3], pvec[3], tvec[3], qvec[3];
	float det, inv_det, u, v, t;
	int clip = 1;

	if( !PyArg_ParseTuple
	    ( args, "O!O!O!O!O!|i", &BPyVector_Type, &vec1, &BPyVector_Type, &vec2
		, &BPyVector_Type, &vec3, &BPyVector_Type, &ray, &BPyVector_Type, &ray_off , &clip) )
		return ( EXPP_ReturnPyObjError
			 ( PyExc_TypeError, "expected 5 vector types\n" ) );
	if( vec1->size != 3 || vec2->size != 3 || vec3->size != 3 || 
		ray->size != 3 || ray_off->size != 3)
		return ( EXPP_ReturnPyObjError( PyExc_TypeError,
						"only 3D vectors for all parameters\n" ) );

	VECCOPY(v1, vec1->vec);
	VECCOPY(v2, vec2->vec);
	VECCOPY(v3, vec3->vec);

	VECCOPY(dir, ray->vec);
	Normalize(dir);

	VECCOPY(orig, ray_off->vec);

	/* find vectors for two edges sharing v1 */
	VecSubf(e1, v2, v1);
	VecSubf(e2, v3, v1);

	/* begin calculating determinant - also used to calculated U parameter */
	Crossf(pvec, dir, e2);	

	/* if determinant is near zero, ray lies in plane of triangle */
	det = Inpf(e1, pvec);

	if (det > -0.000001 && det < 0.000001) {
		return EXPP_incr_ret( Py_None );
	}

	inv_det = 1.0f / det;

	/* calculate distance from v1 to ray origin */
	VecSubf(tvec, orig, v1);

	/* calculate U parameter and test bounds */
	u = Inpf(tvec, pvec) * inv_det;
	if (clip && (u < 0.0f || u > 1.0f)) {
		return EXPP_incr_ret( Py_None );
	}

	/* prepare to test the V parameter */
	Crossf(qvec, tvec, e1);

	/* calculate V parameter and test bounds */
	v = Inpf(dir, qvec) * inv_det;

	if (clip && (v < 0.0f || u + v > 1.0f)) {
		return EXPP_incr_ret( Py_None );
	}

	/* calculate t, ray intersects triangle */
	t = Inpf(e2, qvec) * inv_det;

	VecMulf(dir, t);
	VecAddf(pvec, orig, dir);

	return Vector_CreatePyObject(pvec, 3, (PyObject *)NULL);
}


//---------------------------------NORMALS FUNCTIONS--------------------
//----------------------------------Mathutils.QuadNormal() -------------------
PyObject *Geometry_QuadNormal( PyObject * self, PyObject * args )
{
	BPyVectorObject *vec1;
	BPyVectorObject *vec2;
	BPyVectorObject *vec3;
	BPyVectorObject *vec4;
	float v1[3], v2[3], v3[3], v4[3], e1[3], e2[3], n1[3], n2[3];

	if( !PyArg_ParseTuple
	    ( args, "O!O!O!O!", &BPyVector_Type, &vec1, &BPyVector_Type, &vec2
		, &BPyVector_Type, &vec3, &BPyVector_Type, &vec4 ) )
		return ( EXPP_ReturnPyObjError
			 ( PyExc_TypeError, "expected 4 vector types\n" ) );
	if( vec1->size != vec2->size || vec1->size != vec3->size || vec1->size != vec4->size)
		return ( EXPP_ReturnPyObjError( PyExc_TypeError,
						"vectors must be of the same size\n" ) );
	if( vec1->size != 3 )
		return ( EXPP_ReturnPyObjError( PyExc_TypeError,
						"only 3D vectors\n" ) );

	VECCOPY(v1, vec1->vec);
	VECCOPY(v2, vec2->vec);
	VECCOPY(v3, vec3->vec);
	VECCOPY(v4, vec4->vec);

	/* find vectors for two edges sharing v2 */
	VecSubf(e1, v1, v2);
	VecSubf(e2, v3, v2);

	Crossf(n1, e2, e1);
	Normalize(n1);

	/* find vectors for two edges sharing v4 */
	VecSubf(e1, v3, v4);
	VecSubf(e2, v1, v4);

	Crossf(n2, e2, e1);
	Normalize(n2);

	/* adding and averaging the normals of both triangles */
	VecAddf(n1, n2, n1);
	Normalize(n1);

	return Vector_CreatePyObject(n1, 3, (PyObject *)NULL);
}

//----------------------------Mathutils.TriangleNormal() -------------------
PyObject *Geometry_TriangleNormal( PyObject * self, PyObject * args )
{
	BPyVectorObject *vec1, *vec2, *vec3;
	float v1[3], v2[3], v3[3], e1[3], e2[3], n[3];

	if( !PyArg_ParseTuple
	    ( args, "O!O!O!", &BPyVector_Type, &vec1, &BPyVector_Type, &vec2
		, &BPyVector_Type, &vec3 ) )
		return ( EXPP_ReturnPyObjError
			 ( PyExc_TypeError, "expected 3 vector types\n" ) );
	if( vec1->size != vec2->size || vec1->size != vec3->size )
		return ( EXPP_ReturnPyObjError( PyExc_TypeError,
						"vectors must be of the same size\n" ) );
	if( vec1->size != 3 )
		return ( EXPP_ReturnPyObjError( PyExc_TypeError,
						"only 3D vectors\n" ) );

	VECCOPY(v1, vec1->vec);
	VECCOPY(v2, vec2->vec);
	VECCOPY(v3, vec3->vec);

	/* find vectors for two edges sharing v2 */
	VecSubf(e1, v1, v2);
	VecSubf(e2, v3, v2);

	Crossf(n, e2, e1);
	Normalize(n);

	return Vector_CreatePyObject(n, 3, (PyObject *)NULL);
}

//--------------------------------- AREA FUNCTIONS--------------------
//----------------------------------Mathutils.TriangleArea() -------------------
PyObject *Geometry_TriangleArea( PyObject * self, PyObject * args )
{
	BPyVectorObject *vec1, *vec2, *vec3;
	float v1[3], v2[3], v3[3];

	if( !PyArg_ParseTuple
	    ( args, "O!O!O!", &BPyVector_Type, &vec1, &BPyVector_Type, &vec2,	&BPyVector_Type, &vec3 ) )
		return ( EXPP_ReturnPyObjError
			 ( PyExc_TypeError, "expected 3 vector types\n" ) );
	if( vec1->size != vec2->size || vec1->size != vec3->size )
		return ( EXPP_ReturnPyObjError( PyExc_TypeError,
						"vectors must be of the same size\n" ) );

	if (vec1->size == 3) {
		VECCOPY(v1, vec1->vec);
		VECCOPY(v2, vec2->vec);
		VECCOPY(v3, vec3->vec);

		return PyFloat_FromDouble( AreaT3Dfl(v1, v2, v3) );
	}
	else if (vec1->size == 2) {
		v1[0] = vec1->vec[0];
		v1[1] = vec1->vec[1];

		v2[0] = vec2->vec[0];
		v2[1] = vec2->vec[1];

		v3[0] = vec3->vec[0];
		v3[1] = vec3->vec[1];

		return PyFloat_FromDouble( AreaF2Dfl(v1, v2, v3) );
	}
	else {
		return ( EXPP_ReturnPyObjError( PyExc_TypeError,
						"only 2D,3D vectors are supported\n" ) );
	}
}

//----------------------------------Mathutils.AngleBetweenVecs() ---------
//calculates the angle between 2 vectors
PyObject *Geometry_AngleBetweenVecs(PyObject * self, PyObject * args)
{
	BPyVectorObject *vec1 = NULL, *vec2 = NULL;
	double dot = 0.0f, angleRads, test_v1 = 0.0f, test_v2 = 0.0f;
	int x, size;

	if(!PyArg_ParseTuple(args, "O!O!", &BPyVector_Type, &vec1, &BPyVector_Type, &vec2))
		goto AttributeError1; //not vectors
	if(vec1->size != vec2->size)
		goto AttributeError1; //bad sizes

	//since size is the same....
	size = vec1->size;

	for(x = 0; x < size; x++) {
		test_v1 += vec1->vec[x] * vec1->vec[x];
		test_v2 += vec2->vec[x] * vec2->vec[x];
	}
	if (!test_v1 || !test_v2){
		goto AttributeError2; //zero-length vector
	}

	//dot product
	for(x = 0; x < size; x++) {
		dot += vec1->vec[x] * vec2->vec[x];
	}
	dot /= (sqrt(test_v1) * sqrt(test_v2));

	if (dot < -1.0f || dot > 1.0f) {
		CLAMP(dot,-1.0f,1.0f);
	}
	angleRads = (double)acos(dot);

	return PyFloat_FromDouble(angleRads * (180/ Py_PI));

AttributeError1:
	return EXPP_ReturnPyObjError(PyExc_AttributeError, 
		"Mathutils.AngleBetweenVecs(): expects (2) VECTOR objects of the same size\n");

AttributeError2:
	return EXPP_ReturnPyObjError(PyExc_AttributeError, 
		"Mathutils.AngleBetweenVecs(): zero length vectors are not acceptable arguments\n");
}

/*-------------------------DOC STRINGS ---------------------------*/
static char Geometry_doc[] = "The Blender Geometry module\n\n";

static char Geometry_PolyFill_doc[] = "(veclist_list) - takes a list of polylines (each point a vector) and returns the point indicies for a polyline filled with triangles";
static char Geometry_LineIntersect_doc[] = "(v1, v2, v3, v4) - returns a tuple with the points on each line respectively closest to the other";
static char Geometry_LineIntersect2D_doc[] = "(lineA_p1, lineA_p2, lineB_p1, lineB_p2) - takes 2 lines (as 4 vectors) and returns a vector for their point of intersection or None";
static char M_Geometry_ClosestPointOnLine_doc[] = "(pt, line_p1, line_p2) - takes a point and a line and returns a (Vector, Bool) for the point on the line, and the bool so you can know if the point was between the 2 points";
static char Geometry_PointInTriangle2D_doc[] = "(pt, tri_p1, tri_p2, tri_p3) - takes 4 vectors, one is the point and the next 3 define the triabgle, only the x and y are used from the vectors";
static char Geometry_BoxPack2D_doc[] = "";
static char Geometry_Intersect_doc[] = "(v1, v2, v3, ray, orig, clip=1) - returns the intersection between a ray and a triangle, if possible, returns None otherwise";
static char Geometry_TriangleArea_doc[] = "(v1, v2, v3) - returns the area size of the 2D or 3D triangle defined";
static char Geometry_TriangleNormal_doc[] = "(v1, v2, v3) - returns the normal of the 3D triangle defined";
static char Geometry_QuadNormal_doc[] = "(v1, v2, v3, v4) - returns the normal of the 3D quad defined";
static char Geometry_AngleBetweenVecs_doc[] = "() - returns the angle between two vectors in degrees";
/*-----------------------METHOD DEFINITIONS ----------------------*/
struct PyMethodDef Geometry_methods[] = {
	{"PolyFill", ( PyCFunction ) Geometry_PolyFill, METH_VARARGS, Geometry_PolyFill_doc},
	{"LineIntersect", ( PyCFunction ) Geometry_LineIntersect, METH_VARARGS, Geometry_LineIntersect_doc},
	{"LineIntersect2D", ( PyCFunction ) Geometry_LineIntersect2D, METH_VARARGS, Geometry_LineIntersect2D_doc},
	{"ClosestPointOnLine", ( PyCFunction ) M_Geometry_ClosestPointOnLine, METH_VARARGS, M_Geometry_ClosestPointOnLine_doc},
	{"PointInTriangle2D", ( PyCFunction ) Geometry_PointInTriangle2D, METH_VARARGS, Geometry_PointInTriangle2D_doc},
	{"BoxPack2D", ( PyCFunction ) Geometry_BoxPack2D, METH_VARARGS, Geometry_BoxPack2D_doc},
	{"Intersect", ( PyCFunction ) Geometry_Intersect, METH_VARARGS, Geometry_Intersect_doc},
	{"TriangleArea", ( PyCFunction ) Geometry_TriangleArea, METH_VARARGS, Geometry_TriangleArea_doc},
	{"TriangleNormal", ( PyCFunction ) Geometry_TriangleNormal, METH_VARARGS, Geometry_TriangleNormal_doc},
	{"QuadNormal", ( PyCFunction ) Geometry_QuadNormal, METH_VARARGS, Geometry_QuadNormal_doc},
	{"AngleBetweenVecs", (PyCFunction) Geometry_AngleBetweenVecs, METH_VARARGS, Geometry_AngleBetweenVecs_doc},
	{NULL, NULL, 0, NULL}
};
/*----------------------------MODULE INIT-------------------------*/
PyObject *Geometry_Init(void)
{
	return Py_InitModule3("bpy.geometry", Geometry_methods, Geometry_doc);
}

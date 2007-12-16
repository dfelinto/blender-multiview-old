/* 
 * $Id: Object.c 12802 2007-12-06 00:01:46Z campbellbarton $
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
 *
 * The Object module provides generic access to Objects of various types via
 * the Python interface.
 *
 *
 * Contributor(s): Michel Selten, Willian Germano, Jacques Guignot,
 * Joseph Gilbert, Stephen Swaney, Bala Gi, Campbell Barton, Johnny Matthews,
 * Ken Hughes, Alex Mole, Jean-Michel Soler
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
*/

struct SpaceIpo;
struct rctf;

#include "Object.h" /*This must come first */

#include "DNA_object_types.h"
#include "DNA_view3d_types.h"
#include "DNA_object_force.h"
#include "DNA_userdef_types.h"
#include "DNA_key_types.h" /* for pinShape and activeShape */

#include "BKE_action.h"
#include "BKE_anim.h" /* used for dupli-objects */
#include "BKE_depsgraph.h"
#include "BKE_effect.h"
#include "BKE_font.h"
#include "BKE_property.h"
#include "BKE_mball.h"
#include "BKE_softbody.h"
#include "BKE_utildefines.h"
#include "BKE_armature.h"
#include "BKE_lattice.h"
#include "BKE_mesh.h"
#include "BKE_library.h"
#include "BKE_object.h"
#include "BKE_curve.h"
#include "BKE_global.h"
#include "BKE_main.h"
#include "BKE_scene.h"
#include "BKE_nla.h"
#include "BKE_material.h"
#include "BKE_modifier.h"
#include "BKE_idprop.h"
#include "BKE_object.h"
#include "BKE_key.h" /* for setting the activeShape */
#include "BKE_displist.h"

#include "BSE_editipo.h"
#include "BSE_edit.h"

#include "BIF_space.h"
#include "BIF_editview.h"
#include "BIF_drawscene.h"
#include "BIF_meshtools.h"
#include "BIF_editarmature.h"
#include "BIF_editaction.h"
#include "BIF_editnla.h"

#include "BLI_arithb.h"
#include "BLI_blenlib.h"

#include "BDR_editobject.h"
#include "BDR_editcurve.h"
#include "BDR_drawobject.h"

#include "MEM_guardedalloc.h"

#include "mydevice.h"
#include "blendef.h"
#include "Scene.h"
#include "Mathutils.h"
#include "Mesh.h"
#include "Curve.h"
#include "CurveText.h"
#include "Ipo.h"
#include "Armature.h"
#include "Pose.h"
#include "Camera.h"
#include "Lamp.h"
#include "Lattice.h"
#include "Text.h"
#include "CurveText.h"
#include "Metaball.h"
#include "Draw.h"
#include "NLA.h"
#include "logic.h"
#include "Effect.h"
#include "Group.h"
#include "SurfNurb.h"
#include "Modifier.h"
#include "Constraint.h"
#include "gen_utils.h"
#include "gen_library.h"
#include "EXPP_interface.h"
#include "BIF_editkey.h"
#include "IDProp.h"
#include "bpy_list.h"
#include "Const.h"
#include "sceneObSeq.h"
#include "object_softbody.h"

/* Defines for insertIpoKey */

#define IPOKEY_LOC              0
#define IPOKEY_ROT              1
#define IPOKEY_SIZE             2
#define IPOKEY_LOCROT           3
#define IPOKEY_LOCROTSIZE       4
#define IPOKEY_PI_STRENGTH      5
#define IPOKEY_PI_FALLOFF       6
#define IPOKEY_PI_MAXDIST       7 /*Not Ready Yet*/
#define IPOKEY_PI_SURFACEDAMP   8
#define IPOKEY_PI_RANDOMDAMP    9
#define IPOKEY_PI_PERM          10

#define PFIELD_FORCE	1
#define PFIELD_VORTEX	2
#define PFIELD_MAGNET	3
#define PFIELD_WIND		4

enum obj_consts {
	EXPP_OBJ_ATTR_LAYERMASK = 0,
	EXPP_OBJ_ATTR_COLBITS,
	EXPP_OBJ_ATTR_DRAWMODE,
	EXPP_OBJ_ATTR_DRAWTYPE,
	EXPP_OBJ_ATTR_DUPON,
	EXPP_OBJ_ATTR_DUPOFF,
	EXPP_OBJ_ATTR_DUPSTA,
	EXPP_OBJ_ATTR_DUPEND,
 	EXPP_OBJ_ATTR_DUPFACESCALEFAC,
	EXPP_OBJ_ATTR_TIMEOFFSET,
	EXPP_OBJ_ATTR_DRAWSIZE,
	EXPP_OBJ_ATTR_PARENT_TYPE,
	EXPP_OBJ_ATTR_PASSINDEX,
	EXPP_OBJ_ATTR_ACT_MATERIAL,
	EXPP_OBJ_ATTR_ACT_SHAPE,
	
	EXPP_OBJ_ATTR_PI_SURFACEDAMP,	/* these need to stay together */
	EXPP_OBJ_ATTR_PI_RANDOMDAMP,	/* and in order */
	EXPP_OBJ_ATTR_PI_PERM,
	EXPP_OBJ_ATTR_PI_STRENGTH,
	EXPP_OBJ_ATTR_PI_FALLOFF,
	EXPP_OBJ_ATTR_PI_MAXDIST,
	EXPP_OBJ_ATTR_PI_SBDAMP,
	EXPP_OBJ_ATTR_PI_SBIFACETHICK,
	EXPP_OBJ_ATTR_PI_SBOFACETHICK,	/* last float paremater */

	EXPP_OBJ_ATTR_PI_TYPE,

	EXPP_OBJ_ATTR_SB_NODEMASS,	/* these need to stay together */
	EXPP_OBJ_ATTR_SB_GRAV,		/* and in order */
	EXPP_OBJ_ATTR_SB_MEDIAFRICT,
	EXPP_OBJ_ATTR_SB_RKLIMIT,
	EXPP_OBJ_ATTR_SB_PHYSICSSPEED,
	EXPP_OBJ_ATTR_SB_GOALSPRING,
	EXPP_OBJ_ATTR_SB_GOALFRICT,
	EXPP_OBJ_ATTR_SB_MINGOAL,
	EXPP_OBJ_ATTR_SB_MAXGOAL,
	EXPP_OBJ_ATTR_SB_DEFGOAL,
	EXPP_OBJ_ATTR_SB_INSPRING,
	EXPP_OBJ_ATTR_SB_INFRICT,	/* last float parameter */

	EXPP_OBJ_ATTR_RB_SHAPEBOUND,
	
	EXPP_OBJ_ATTR_ROT,
	EXPP_OBJ_ATTR_DROT,
};

/*****************************************************************************/
/* Python API function prototypes for the Blender module.		 */
/*****************************************************************************/
/* HELPER FUNCTION FOR PARENTING */
static PyObject *internal_makeParent(Object *parent, PyObject *py_child, int partype, int noninverse, int fast, int v1, int v2, int v3, char *bonename);

/*****************************************************************************/
/* The following string definitions are used for documentation strings.	 */
/* In Python these will be written to the console when doing a		 */
/* Blender.Object.__doc__						 */
/*****************************************************************************/
char M_Object_doc[] = "The Blender Object module\n\n\
This module provides access to **Object Data** in Blender.\n";

/*****************************************************************************/
/* Python BPyObject methods declarations:				   */
/*****************************************************************************/
static int setupPI(Object* ob);

static PyObject *Object_buildParts( BPyObject * self );
static PyObject *Object_clrParent( BPyObject * self, PyObject * args );
static PyObject *Object_clearTrack( BPyObject * self, PyObject * args );

static PyObject *Object_getInverseMatrix( BPyObject * self );
static PyObject *Object_getIpo( BPyObject * self );
static PyObject *Object_getLocation( BPyObject * self, PyObject * args );
static PyObject *Object_getParent( BPyObject * self );
static PyObject *Object_getParentBoneName( BPyObject * self );
static int Object_setParentBoneName( BPyObject * self, PyObject * value );
static PyObject *Object_getTracked( BPyObject * self );
static PyObject *Object_getType( BPyObject * self );
static PyObject *Object_getBoundBox( BPyObject * self );
static PyObject *Object_getAction( BPyObject * self );
static PyObject *Object_getPose( BPyObject * self );
static PyObject *Object_evaluatePose( BPyObject * self, PyObject *value );
static PyObject *Object_getSelected( BPyObject * self );
static PyObject *Object_makeDisplayList( BPyObject * self );
static PyObject *Object_setData( BPyObject * self, PyObject * value );
static PyObject *Object_makeParent( BPyObject * self, PyObject * args );
static PyObject *Object_join( BPyObject * self, PyObject * args );
static PyObject *Object_makeParentDeform( BPyObject * self, PyObject * args );
static PyObject *Object_makeParentVertex( BPyObject * self, PyObject * args );
static PyObject *Object_makeParentBone( BPyObject * self, PyObject * args );
static PyObject *Object_getDupliObjects ( BPyObject * self);
static PyObject *Object_getEffects( BPyObject * self );
static PyObject *Object_insertIpoKey( BPyObject * self, PyObject * value );
static PyObject *Object_insertPoseKey( BPyObject * self, PyObject * args );
static PyObject *Object_insertCurrentPoseKey( BPyObject * self, PyObject * args );
static PyObject *Object_setConstraintInfluenceForBone( BPyObject * self, PyObject * args );
static PyObject *Object_makeTrack( BPyObject * self, PyObject * args );
static PyObject *Object_shareFrom( BPyObject * self, PyObject * args );
static PyObject *Object_getAllProperties( BPyObject * self );
static PyObject *Object_addProperty( BPyObject * self, PyObject * args );
static PyObject *Object_removeProperty( BPyObject * self, PyObject * args );
static PyObject *Object_getProperty( BPyObject * self, PyObject * args );
static PyObject *Object_removeAllProperties( BPyObject * self );
static PyObject *Object_copyAllPropertiesTo( BPyObject * self,
					     PyObject * args );
static PyObject *Object_getPIUseMaxDist( BPyObject * self );
static PyObject *Object_getPIDeflection( BPyObject * self );

static int Object_setRBMass( BPyObject * self, PyObject * args );
static int Object_setRBFlags( BPyObject * self, PyObject * args );

static PyObject *Object_insertShapeKey(BPyObject * self);
static PyObject *Object_copyNLA( BPyObject * self, PyObject * args );
static PyObject *Object_convertActionToStrip( BPyObject * self );
static PyObject *Object_copy(BPyObject * self); /* __copy__ */

/*****************************************************************************/
/* Python BPyObject methods table:					   */
/*****************************************************************************/
static PyMethodDef BPyObject_methods[] = {
	/* name, method, flags, doc */
	{"buildParts", ( PyCFunction ) Object_buildParts, METH_NOARGS,
	 "Recalcs particle system (if any) "},
	{"clrParent", ( PyCFunction ) Object_clrParent, METH_VARARGS,
	 "Clears parent object. Optionally specify:\n\
mode\n\tnonzero: Keep object transform\nfast\n\t>0: Don't update scene \
hierarchy (faster)"},
	{"clearTrack", ( PyCFunction ) Object_clearTrack, METH_VARARGS,
	 "Make this object not track another anymore. Optionally specify:\n\
mode\n\t2: Keep object transform\nfast\n\t>0: Don't update scene \
hierarchy (faster)"},
	{"evaluatePose", ( PyCFunction ) Object_evaluatePose, METH_O,
	"(framenum) - Updates the pose to a certain frame number when the Object is\
	bound to an Action"},
	{"getPose", ( PyCFunction ) Object_getPose, METH_NOARGS,
	"() - returns the pose from an object if it exists, else None"},
	{"getInverseMatrix", ( PyCFunction ) Object_getInverseMatrix,
	 METH_NOARGS,
	 "Returns the object's inverse matrix"},
	{"getLocation", ( PyCFunction ) Object_getLocation, METH_VARARGS,
	 "(space = 'localspace' / 'worldspace') - Returns the object's location (x, y, z)\n\
"},
	{"makeDisplayList", ( PyCFunction ) Object_makeDisplayList, METH_NOARGS,
	 "Update this object's Display List. Some changes like turning\n\
'SubSurf' on for a mesh need this method (followed by a Redraw) to\n\
show the changes on the 3d window."},
	{"makeParent", ( PyCFunction ) Object_makeParent, METH_VARARGS,
	 "Makes the object the parent of the objects provided in the\n\
argument which must be a list of valid Objects. Optional extra arguments:\n\
mode:\n\t0: make parent with inverse\n\t1: without inverse\n\
fast:\n\t0: update scene hierarchy automatically\n\t\
don't update scene hierarchy (faster). In this case, you must\n\t\
explicitly update the Scene hierarchy."},
	{"join", ( PyCFunction ) Object_join, METH_VARARGS,
	 "(object_list) - Joins the objects in object list of the same type, into this object."},
	{"makeParentDeform", ( PyCFunction ) Object_makeParentDeform, METH_VARARGS,
	 "Makes the object the deformation parent of the objects provided in the \n\
argument which must be a list of valid Objects. Optional extra arguments:\n\
mode:\n\t0: make parent with inverse\n\t1: without inverse\n\
fast:\n\t0: update scene hierarchy automatically\n\t\
don't update scene hierarchy (faster). In this case, you must\n\t\
explicitly update the Scene hierarchy."},
	{"makeParentVertex", ( PyCFunction ) Object_makeParentVertex, METH_VARARGS,
	 "Makes the object the vertex parent of the objects provided in the \n\
argument which must be a list of valid Objects. \n\
The second argument is a tuple of 1 or 3 positive integers which corresponds \
to the index of the vertex you are parenting to.\n\
Optional extra arguments:\n\
mode:\n\t0: make parent with inverse\n\t1: without inverse\n\
fast:\n\t0: update scene hierarchy automatically\n\t\
don't update scene hierarchy (faster). In this case, you must\n\t\
explicitly update the Scene hierarchy."},
	{"makeParentBone", ( PyCFunction ) Object_makeParentBone, METH_VARARGS,
	 "Makes this armature objects bone, the parent of the objects provided in the \n\
argument which must be a list of valid Objects. Optional extra arguments:\n\
mode:\n\t0: make parent with inverse\n\t1: without inverse\n\
fast:\n\t0: update scene hierarchy automatically\n\t\
don't update scene hierarchy (faster). In this case, you must\n\t\
explicitely update the Scene hierarchy."},
	{"makeTrack", ( PyCFunction ) Object_makeTrack, METH_VARARGS,
	 "(trackedobj, fast = 0) - Make this object track another.\n\
	 (trackedobj) - the object that will be tracked.\n\
	 (fast = 0) - if 0: update the scene hierarchy automatically.  If you\n\
	 set 'fast' to a nonzero value, don't forget to update the scene yourself\n\
	 (see scene.update())."},
	{"shareFrom", ( PyCFunction ) Object_shareFrom, METH_VARARGS,
	 "Link data of self with object specified in the argument. This\n\
works only if self and the object specified are of the same type."},
	 {"insertIpoKey", ( PyCFunction ) Object_insertIpoKey, METH_O,
	 "( Object IPO type ) - Inserts a key into IPO"},
	 {"insertPoseKey", ( PyCFunction ) Object_insertPoseKey, METH_VARARGS,
	 "( Object Pose type ) - Inserts a key into Action"},
	 {"insertCurrentPoseKey", ( PyCFunction ) Object_insertCurrentPoseKey, METH_VARARGS,
	 "( Object Pose type ) - Inserts a key into Action based on current pose"},
	 {"setConstraintInfluenceForBone", ( PyCFunction ) Object_setConstraintInfluenceForBone, METH_VARARGS,
	  "(  ) - sets a constraint influence for a certain bone in this (armature)object."},
	 {"copyNLA", ( PyCFunction ) Object_copyNLA, METH_VARARGS,
	  "(  ) - copies all NLA strips from another object to this object."},
	{"convertActionToStrip", ( PyCFunction ) Object_convertActionToStrip, METH_NOARGS,
	 "(  ) - copies all NLA strips from another object to this object."},
	{"getAllProperties", ( PyCFunction ) Object_getAllProperties, METH_NOARGS,
	 "() - Get all the properties from this object"},
	{"addProperty", ( PyCFunction ) Object_addProperty, METH_VARARGS,
	 "() - Add a property to this object"},
	{"removeProperty", ( PyCFunction ) Object_removeProperty, METH_VARARGS,
	 "() - Remove a property from  this object"},
	{"getProperty", ( PyCFunction ) Object_getProperty, METH_VARARGS,
	 "() - Get a property from this object by name"},
	{"removeAllProperties", ( PyCFunction ) Object_removeAllProperties,
	 METH_NOARGS,
	 "() - removeAll a properties from this object"},
	{"copyAllPropertiesTo", ( PyCFunction ) Object_copyAllPropertiesTo,
	 METH_VARARGS,
	 "() - copy all properties from this object to another object"},
	{"insertShapeKey", ( PyCFunction ) Object_insertShapeKey, METH_NOARGS,
	 "() - Insert a Shape Key in the current object"},
	{"__copy__", ( PyCFunction ) Object_copy, METH_NOARGS,
	 "() - Return a copy of this object."},
	{"copy", ( PyCFunction ) Object_copy, METH_NOARGS,
	 "() - Return a copy of this object."},
	{NULL, NULL, 0, NULL}
};

/*****************************************************************************/
/* PythonTypeObject constant declarations                                    */
/*****************************************************************************/

/* 
 * structure of "tuples" of constant's string name and int value
 *
 * For example, these two structures will define the constant category
 * "bpy.class.Object.DrawTypes" the constant 
 * "bpy.class.Object.DrawTypes.BOUNDBOX" and others.
 */

static constIdents drawTypesIdents[] = {
	{"BOUNDBOX",	{(int)OB_BOUNDBOX}},
	{"WIRE",		{(int)OB_WIRE}},
	{"SOLID",		{(int)OB_SOLID}},
	{"SHADED",		{(int)OB_SHADED}},
};

/* 
 * structure with constant internal type (int), constant category name,
 * and number of constant values in the category.
 */

static constDefinition drawTypes = {
	EXPP_CONST_INT, "DrawTypes",
		sizeof(drawTypesIdents)/sizeof(constIdents), drawTypesIdents
};

static constIdents parentTypesIdents[] = {
	{"OBJECT",      {(int)PAROBJECT}},
	{"CURVE",       {(int)PARCURVE}},
	{"LATTICE",     {(int)PARSKEL}},
	{"ARMATURE",    {(int)PARSKEL}},
	{"VERT1",       {(int)PARVERT1}},
	{"VERT3",       {(int)PARVERT3}},
	{"BONE",        {(int)PARBONE}},
};

static constDefinition parentTypes = {
	EXPP_CONST_INT, "ParentTypes",
		sizeof(parentTypesIdents)/sizeof(constIdents), parentTypesIdents
};

static constIdents PITypesIdents[] = {
	{"NONE",        {(int)0}},
	{"FORCE",       {(int)PFIELD_FORCE}},
	{"VORTEX",      {(int)PFIELD_VORTEX}},
	{"WIND",        {(int)PFIELD_WIND}},
	{"GUIDE",       {(int)PFIELD_GUIDE}},
};

static constDefinition PITypes = {
	EXPP_CONST_INT, "PITypes",
		sizeof(PITypesIdents)/sizeof(constIdents), PITypesIdents
};

static constIdents RBShapesIdents[] = {
	{"BOX",         {(int)OB_BOUND_BOX}},
	{"SPHERE",      {(int)OB_BOUND_SPHERE}},
	{"CYLINDER",    {(int)OB_BOUND_CYLINDER}},
	{"CONE",        {(int)OB_BOUND_CONE}},
	{"POLYHEDERON", {(int)OB_BOUND_POLYH}},
};

static constDefinition RBShapes = {
	EXPP_CONST_INT, "RBShapes",
		sizeof(RBShapesIdents)/sizeof(constIdents), RBShapesIdents
};

static constIdents IpoKeyTypesIdents[] = {
	{"LOC",           {(int)IPOKEY_LOC}},
	{"ROT",           {(int)IPOKEY_ROT}},
	{"SCALE",          {(int)IPOKEY_SIZE}},
	{"LOCROT",        {(int)IPOKEY_LOCROT}},
	{"LOCROTSCALE",    {(int)IPOKEY_LOCROTSIZE}},
	{"PI_STRENGTH",   {(int)IPOKEY_PI_STRENGTH}},
	{"PI_FALLOFF",    {(int)IPOKEY_PI_FALLOFF}},
	{"PI_SURFACEDAMP",{(int)IPOKEY_PI_SURFACEDAMP}},
	{"PI_RANDOMDAMP", {(int)IPOKEY_PI_RANDOMDAMP}},
	{"PI_PERM",       {(int)IPOKEY_PI_PERM}},
};

static constDefinition IpoKeyTypes = {
	EXPP_CONST_INT, "IpoKeyTypes",
		sizeof(IpoKeyTypesIdents)/sizeof(constIdents), IpoKeyTypesIdents
};

/*****************************************************************************/
/* PythonTypeObject callback function prototypes			 */
/*****************************************************************************/
static void Object_dealloc( BPyObject * obj );

/*****************************************************************************/
/* Function:			  M_Object_New				 */
/* Python equivalent:	  Blender.Object.New				 */
/*****************************************************************************/

/*
 * Note: if this method is called without later linking object data to it, 
 * errors can be caused elsewhere in Blender.  Future versions of the API
 * will designate obdata as a parameter to this method to prevent this, and
 * eventually this method will be deprecated.
 *
 * When we can guarantee that objects will always have valid obdata, 
 * unlink_object() should be edited to remove checks for NULL pointers and
 * debugging messages.
 */
/* todo - bring back the functionality of Object.Duplicate() */

/*****************************************************************************/
/* Python BPyObject methods:					*/
/*****************************************************************************/

static PyObject *Object_buildParts( BPyObject * self )
{
	build_particle_system( self->object );
	Py_RETURN_NONE;
}

static PyObject *Object_clrParent( BPyObject * self, PyObject * args )
{
	int mode = 0;
	int fast = 0;

	if( !PyArg_ParseTuple( args, "|ii", &mode, &fast ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected one or two optional integers as arguments" );

	/* Remove the link only, the object is still in the scene. */
	self->object->parent = NULL;

	if( mode == 2 ) {
		/* Keep transform */
		apply_obmat( self->object );
	}

	if( !fast )
		DAG_scene_sort( G.scene );

	Py_RETURN_NONE;
}

static PyObject *Object_clearTrack( BPyObject * self, PyObject * args )
{
	int mode = 0;
	int fast = 0;

	if( !PyArg_ParseTuple( args, "|ii", &mode, &fast ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected one or two optional integers as arguments" );

	/* Remove the link only, the object is still in the scene. */
	self->object->track = NULL;

	if( mode ) {
		/* Keep transform */
		apply_obmat( self->object );
	}

	if( !fast )
		DAG_scene_sort( G.scene );

	Py_RETURN_NONE;
}

static PyObject *Object_getAction( BPyObject * self )
{	/* action can be NULL -> None */
	return Action_CreatePyObject( self->object->action );
}

static int Object_setAction( BPyObject * self, PyObject * value )
{
	return GenericLib_assignData(value, (void **) &self->object->action, 0, 1, ID_AC, 0);
}

static PyObject *Object_evaluatePose(BPyObject *self, PyObject *value)
{
	int frame = PyInt_AsLong(value);
	if (frame == -1 && PyErr_Occurred())
		return NULL;

	frame = EXPP_ClampInt(frame, MINFRAME, MAXFRAME);
	G.scene->r.cfra = frame;
	do_all_pose_actions(self->object);
	where_is_pose (self->object);

	Py_RETURN_NONE;
}

static PyObject * Object_getPose(BPyObject *self)
{
	/*if there is no pose will return PyNone*/
	return Pose_CreatePyObject(self);
}

static PyObject *Object_getSelected( BPyObject * self )
{
	Base *base;
	
	for (base = FIRSTBASE; base; base = base->next) {
		if( base->object == self->object ) {
			if( base->flag & SELECT ) {
				Py_RETURN_TRUE;
			} else {
				Py_RETURN_FALSE;
			}
		}
	}
	return EXPP_ReturnPyObjError( PyExc_RuntimeError,
			"could not find object's selection state" );
}

static int Object_setSelect( BPyObject * self, PyObject * value )
{
	Base *base;
	int setting = PyObject_IsTrue( value );

	if( setting == -1 )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected true/false argument" );

	base = FIRSTBASE;
	while( base ) {
		if( base->object == self->object ) {
			if( setting == 1 ) {
				base->flag |= SELECT;
				self->object->flag = (short)base->flag;
				set_active_base( base );
			} else {
				base->flag &= ~SELECT;
				self->object->flag = (short)base->flag;
			}
			break;
		}
		base = base->next;
	}
	countall(  );
	return 0;
}

static PyObject *Object_getInverseMatrix( BPyObject * self )
{
	BPyMatrixObject *inverse =
		( BPyMatrixObject * ) Matrix_CreatePyObject( NULL, 4, 4, (PyObject *)NULL );
	Mat4Invert( (float ( * )[4])*inverse->matrix, self->object->obmat );

	return ( ( PyObject * ) inverse );
}

static PyObject *Object_getIpo( BPyObject * self )
{	/* ipo NULL -> None is ok */
	return Ipo_CreatePyObject( self->object->ipo );
}

static PyObject *Object_getLocation( BPyObject * self, PyObject * args )
{
	char *space = "localspace";	/* default to local */
	PyObject *attr;
	if( !PyArg_ParseTuple( args, "|s", &space ) ) 
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a string or nothing" );

	if( BLI_streq( space, "worldspace" ) ) {	/* Worldspace matrix */
		disable_where_script( 1 );
		where_is_object( self->object );
		
		attr = Py_BuildValue( "fff",
					self->object->obmat[3][0],
					self->object->obmat[3][1],
					self->object->obmat[3][2] );
		
		disable_where_script( 0 );
	} else if( BLI_streq( space, "localspace" ) ) {	/* Localspace matrix */
		attr = Py_BuildValue( "fff",
					self->object->loc[0],
					self->object->loc[1],
					self->object->loc[2] );
	} else {
		return EXPP_ReturnPyObjError( PyExc_ValueError,
				"expected either nothing, 'localspace' (default) or 'worldspace'" );
	}

	return attr;
}

static PyObject *Object_getParent( BPyObject * self )
{
	return Object_CreatePyObject( self->object->parent );
}

static PyObject *Object_getParentBoneName( BPyObject * self )
{
	if( self->object->parent && self->object->parsubstr[0] != '\0' )
		return PyString_FromString( self->object->parsubstr );
	Py_RETURN_NONE;
}

static int Object_setParentBoneName( BPyObject * self, PyObject *value )
{
	char *bonename;
	
	if (!PyString_Check(value))
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected an int or nothing" );
	
	if (
		self->object->parent &&
		self->object->parent->type == OB_ARMATURE &&
		self->object->partype == PARBONE
	) {/* its all good */} else
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"can only set the parent bone name for objects that already have a bone parent" );
	
	bonename = PyString_AsString(value);

	if (!get_named_bone(self->object->parent->data, bonename))
		return EXPP_ReturnIntError( PyExc_ValueError,
				"cannot parent to this bone: invalid bone name" );
	
	strcpy(self->object->parsubstr, bonename);
	DAG_scene_sort( G.scene );
	return 0;
}

static PyObject *Object_getTracked( BPyObject * self )
{
	return Object_CreatePyObject( self->object->track );
}

static PyObject *Object_getType( BPyObject * self )
{
	int type = self->object->type;
	/* if object not yet linked to data, return the stored type */
	if( self->realtype != OB_EMPTY )
		type = self->realtype;
	
	switch ( type ) {
	case OB_ARMATURE:
		return EXPP_incr_ret((PyObject *)&BPyArmature_Type);
	case OB_CAMERA:
		return EXPP_incr_ret((PyObject *)&BPyCamera_Type);
	case OB_CURVE:
		return EXPP_incr_ret((PyObject *)&BPyCurve_Type);
	case OB_FONT:
		return EXPP_incr_ret((PyObject *)&BPyCurveText_Type);
	case OB_LAMP:
		return EXPP_incr_ret((PyObject *)&BPyLamp_Type);
	case OB_LATTICE:
		return EXPP_incr_ret((PyObject *)&BPyLattice_Type);
	case OB_MBALL:
		return EXPP_incr_ret((PyObject *)&BPyMetaball_Type);
	case OB_MESH:
		return EXPP_incr_ret((PyObject *)&BPyMesh_Type);
	case OB_SURF:
		return EXPP_incr_ret((PyObject *)&BPySurfNurb_Type);
	/*case OB_EMPTY:*/
	}
	Py_RETURN_NONE;
}

static PyObject *Object_getBoundBox( BPyObject * self )
{
	int i;
	float *vec = NULL;
	PyObject *vector, *bbox;

	if( !self->object->data )
		return EXPP_ReturnPyObjError( PyExc_AttributeError,
				"This object isn't linked to any object data (mesh, curve, etc) yet" );

	if( !self->object->bb ) {	/* if no ob bbox, we look in obdata */
		Mesh *me;
		Curve *curve;
		switch ( self->object->type ) {
		case OB_MESH:
			me = self->object->data;
			vec = (float*) mesh_get_bb(me)->vec;
			break;
		case OB_CURVE:
		case OB_FONT:
		case OB_SURF:
			curve = self->object->data;
			if( !curve->bb )
				tex_space_curve( curve );
			vec = ( float * ) curve->bb->vec;
			break;
		default:
			Py_RETURN_NONE;
		}

		{		/* transform our obdata bbox by the obmat.
				   the obmat is 4x4 homogeneous coords matrix.
				   each bbox coord is xyz, so we make it homogenous
				   by padding it with w=1.0 and doing the matrix mult.
				   afterwards we divide by w to get back to xyz.
				 */
			/* printmatrix4( "obmat", self->object->obmat); */

			float tmpvec[4];	/* tmp vector for homogenous coords math */
			int i;
			float *from;

			bbox = PyList_New( 8 );
			if( !bbox )
				return EXPP_ReturnPyObjError
					( PyExc_MemoryError,
					  "couldn't create pylist" );
			for( i = 0, from = vec; i < 8; i++, from += 3 ) {
				memcpy( tmpvec, from, 3 * sizeof( float ) );
				tmpvec[3] = 1.0f;	/* set w coord */
				Mat4MulVec4fl( self->object->obmat, tmpvec );
				/* divide x,y,z by w */
				tmpvec[0] /= tmpvec[3];
				tmpvec[1] /= tmpvec[3];
				tmpvec[2] /= tmpvec[3];

#if 0
				{	/* debug print stuff */
					int i;

					printf( "\nobj bbox transformed\n" );
					for( i = 0; i < 4; ++i )
						printf( "%f ", tmpvec[i] );

					printf( "\n" );
				}
#endif

				/* because our bounding box is calculated and
				   does not have its own memory,
				   we must create vectors that allocate space */

				vector = Vector_CreatePyObject( NULL, 3, (PyObject *)NULL);
				memcpy( ( ( BPyVectorObject * ) vector )->vec,
					tmpvec, 3 * sizeof( float ) );
				PyList_SET_ITEM( bbox, i, vector );
			}
		}
	} else {		/* the ob bbox exists */
		vec = ( float * ) self->object->bb->vec;

		if( !vec )
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
						      "couldn't retrieve bounding box data" );

		bbox = PyList_New( 8 );

		if( !bbox )
			return EXPP_ReturnPyObjError( PyExc_MemoryError,
						      "couldn't create pylist" );

		/* create vectors referencing object bounding box coords */
		for( i = 0; i < 8; i++ ) {
			vector = Vector_CreatePyObject( vec, 3, (PyObject *)self );
			PyList_SET_ITEM( bbox, i, vector );
			vec += 3;
		}
	}

	return bbox;
}

static PyObject *Object_makeDisplayList( BPyObject * self )
{
	Object *ob = self->object;

	if( ob->type == OB_FONT ) {
		Curve *cu = ob->data;
		freedisplist( &cu->disp );
		text_to_curve( ob, 0 );
	}

	DAG_object_flush_update(G.scene, ob, OB_RECALC_DATA);

	Py_RETURN_NONE;
}

static PyObject *Object_setData( BPyObject * self, PyObject * value )
{
	ID *id;
	ID *oldid;
	int obj_id;
	void *data = NULL;
	int ok;

	if( BPyArmature_Check( value ) )
		data = ( void * ) Armature_FromPyObject( value );
	else if( BPyCamera_Check( value ) )
		data = ( void * ) Camera_FromPyObject( value );
	else if( BPyLamp_Check( value ) )
		data = ( void * ) Lamp_FromPyObject( value );
	else if( BPyCurveBase_Check( value ) ) /* curve, surf, text3d */
		data = ( void * ) CurveBase_FromPyObject( value );
	else if( BPyMesh_Check( value ) )
		data = ( void * ) Mesh_FromPyObject( value, self->object );
	else if( BPyLattice_Check( value ) )
		data = ( void * ) Lattice_FromPyObject( value );
	else if( BPyMetaball_Check( value ) )
		data = ( void * ) Metaball_FromPyObject( value );

	/* have we set data to something good? */
	if( !data )
		return EXPP_ReturnPyObjError( PyExc_AttributeError,
				"link argument type is not supported " );

	oldid = ( ID * ) self->object->data;
	id = ( ID * ) data;
	obj_id = MAKE_ID2( id->name[0], id->name[1] );

	/* if the object object has not been linked to real data before, we
	 * can now let it assume its real type */
	if( self->realtype != OB_EMPTY ) {
		self->object->type = self->realtype;
		self->realtype = OB_EMPTY;
	}

	ok = 1;
	switch ( obj_id ) {
	case ID_AR:
		if( self->object->type != OB_ARMATURE ) {
			ok = 0;
		}
		break;
	case ID_CA:
		if( self->object->type != OB_CAMERA ) {
			ok = 0;
		}
		break;
	case ID_LA:
		if( self->object->type != OB_LAMP ) {
			ok = 0;
		}
		break;
	case ID_ME:
		if( self->object->type != OB_MESH ) {
			ok = 0;
		}
		break;
	case ID_CU:
		if( self->object->type != OB_CURVE && self->object->type != OB_FONT ) {
			ok = 0;
		}
		break;
	case ID_LT:
		if( self->object->type != OB_LATTICE ) {
			ok = 0;
		}
		break;
	case ID_MB:
		if( self->object->type != OB_MBALL ) {
			ok = 0;
		}
		break;
	default:
		return EXPP_ReturnPyObjError( PyExc_AttributeError,
				"Linking this object type is not supported" );
	}

	if( !ok )
		return EXPP_ReturnPyObjError( PyExc_AttributeError,
				"The 'link' object is incompatible with the base object" );
	self->object->data = data;

	/* creates the curve for the text object */
	if (self->object->type == OB_FONT) 
		text_to_curve(self->object, 0);

	id_us_plus( id );
	if( oldid ) {
		if( oldid->us > 0 ) {
			oldid->us--;
		} else {
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					"old object reference count below 0" );
		}
	}

	/* make sure data and object materials are consistent */
	test_object_materials( id );

	Py_RETURN_NONE;
}

static PyObject *Object_makeParentVertex( BPyObject * self, PyObject * args )
{
	PyObject *list;
	PyObject *vlist;
	PyObject *py_child;
	PyObject *ret_val;
	Object *parent;
	int noninverse = 0;
	int fast = 0;
	int partype;
	int v1, v2=0, v3=0;
	int i;

	/* Check if the arguments passed to makeParent are valid. */
	if( !PyArg_ParseTuple( args, "OO|ii", &list, &vlist, &noninverse, &fast ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a list of objects, a tuple of integers and one or two integers as arguments" );

	if( !PySequence_Check( list ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected a list of objects" );

	if (!PyTuple_Check( vlist ))
		return EXPP_ReturnPyObjError( PyExc_TypeError,
						"expected a tuple of integers" );

	switch( PyTuple_Size( vlist ) ) {
	case 1:
		if( !PyArg_ParseTuple( vlist, "i", &v1 ) )
			return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a tuple of 1 or 3 integers" );

		if ( v1 < 0 )
			return EXPP_ReturnPyObjError( PyExc_ValueError,
				"indices must be strictly positive" );

		partype = PARVERT1;
		break;
	case 3:
		if( !PyArg_ParseTuple( vlist, "iii", &v1, &v2, &v3 ) )
			return EXPP_ReturnPyObjError( PyExc_TypeError,
					"expected a tuple of 1 or 3 integers" );

		if ( v1 < 0 || v2 < 0 || v3 < 0)
			return EXPP_ReturnPyObjError( PyExc_ValueError,
					   	"indices must be strictly positive" );
		partype = PARVERT3;
		break;
	default:
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a tuple of 1 or 3 integers" );
	}

	parent = ( Object * ) self->object;

	if (!ELEM3(parent->type, OB_MESH, OB_CURVE, OB_SURF))
		return EXPP_ReturnPyObjError( PyExc_ValueError,
				"Parent Vertex only applies to curve, mesh or surface objects" );

	if (parent->id.us == 0)
		return EXPP_ReturnPyObjError (PyExc_RuntimeError,
			"object must be linked to a scene before it can become a parent");

	/* Check if the PyObject passed in list is a Blender object. */
	for( i = 0; i < PySequence_Length( list ); i++ ) {
		py_child = PySequence_GetItem( list, i );

		ret_val = internal_makeParent(parent, py_child, partype, noninverse, fast, v1, v2, v3, NULL);
		Py_DECREF (py_child);

		if (ret_val)
			Py_DECREF(ret_val);
		else {
			if (!fast)	/* need to sort when interrupting in the middle of the list */
				DAG_scene_sort( G.scene );
			return NULL; /* error has been set already */
		}
	}

	if (!fast) /* otherwise, only sort at the end */
		DAG_scene_sort( G.scene );

	Py_RETURN_NONE;
}

static PyObject *Object_makeParentDeform( BPyObject * self, PyObject * args )
{
	PyObject *list;
	PyObject *py_child;
	PyObject *ret_val;
	Object *parent;
	int noninverse = 0;
	int fast = 0;
	int i;

	/* Check if the arguments passed to makeParent are valid. */
	if( !PyArg_ParseTuple( args, "O|ii", &list, &noninverse, &fast ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a list of objects and one or two integers as arguments" );

	if( !PySequence_Check( list ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a list of objects" );

	parent = ( Object * ) self->object;

	if (parent->type != OB_CURVE && parent->type != OB_ARMATURE)
		return EXPP_ReturnPyObjError( PyExc_ValueError,
				"Parent Deform only applies to curve or armature objects" );

	if (parent->id.us == 0)
		return EXPP_ReturnPyObjError (PyExc_RuntimeError,
			"object must be linked to a scene before it can become a parent");

	/* Check if the PyObject passed in list is a Blender object. */
	for( i = 0; i < PySequence_Length( list ); i++ ) {
		py_child = PySequence_GetItem( list, i );

		ret_val = internal_makeParent(parent, py_child, PARSKEL, noninverse, fast, 0, 0, 0, NULL);
		Py_DECREF (py_child);

		if (ret_val)
			Py_DECREF(ret_val);
		else {
			if (!fast)	/* need to sort when interupting in the middle of the list */
				DAG_scene_sort( G.scene );
			return NULL; /* error has been set already */
		}
	}

	if (!fast) /* otherwise, only sort at the end */
		DAG_scene_sort( G.scene );

	Py_RETURN_NONE;
}


static PyObject *Object_makeParentBone( BPyObject * self, PyObject * args )
{
	char *bonename;
	PyObject *list;
	PyObject *py_child;
	PyObject *ret_val;
	Object *parent;
	int noninverse = 0;
	int fast = 0;
	int i;
	
	/* Check if the arguments passed to makeParent are valid. */
	if( !PyArg_ParseTuple( args, "Os|ii", &list, &bonename, &noninverse, &fast ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a list of objects, bonename and optionally two integers as arguments" );
	
	parent = ( Object * ) self->object;
	
	if (parent->type != OB_ARMATURE)
		return EXPP_ReturnPyObjError( PyExc_ValueError,
				"Parent Bone only applies to armature objects" );

	if (parent->id.us == 0)
		return EXPP_ReturnPyObjError (PyExc_RuntimeError,
			"object must be linked to a scene before it can become a parent");
	
	if (!parent->data)
		return EXPP_ReturnPyObjError (PyExc_RuntimeError,
			"object must be linked to armature data");
	
	if (!get_named_bone(parent->data, bonename))
		return EXPP_ReturnPyObjError( PyExc_ValueError,
				"Parent Bone Name is not in the armature" );
	
	/* Check if the PyObject passed in list is a Blender object. */
	for( i = 0; i < PySequence_Length( list ); i++ ) {
		py_child = PySequence_GetItem( list, i );

		ret_val = internal_makeParent(parent, py_child, PARBONE, noninverse, fast, 0, 0, 0, bonename);
		Py_DECREF (py_child);

		if (ret_val)
			Py_DECREF(ret_val);
		else {
			if (!fast)	/* need to sort when interupting in the middle of the list */
				DAG_scene_sort( G.scene );
			return NULL; /* error has been set already */
		}
	}

	if (!fast) /* otherwise, only sort at the end */
		DAG_scene_sort( G.scene );

	Py_RETURN_NONE;
}


static PyObject *Object_makeParent( BPyObject * self, PyObject * args )
{
	PyObject *list;
	PyObject *py_child;
	PyObject *ret_val;
	Object *parent;
	int noninverse = 0;
	int fast = 0;
	int i;

	/* Check if the arguments passed to makeParent are valid. */
	if( !PyArg_ParseTuple( args, "O|ii", &list, &noninverse, &fast ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					"expected a list of objects and one or two integers as arguments" );

	if( !PySequence_Check( list ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a list of objects" );

	parent = ( Object * ) self->object;

	if (parent->id.us == 0)
		return EXPP_ReturnPyObjError (PyExc_RuntimeError,
			"object must be linked to a scene before it can become a parent");

	/* Check if the PyObject passed in list is a Blender object. */
	for( i = 0; i < PySequence_Length( list ); i++ ) {
		py_child = PySequence_GetItem( list, i );

		ret_val = internal_makeParent(parent, py_child, PAROBJECT, noninverse, fast, 0, 0, 0, NULL);
		Py_DECREF (py_child);

		if (ret_val)
			Py_DECREF(ret_val);
		else {
			if (!fast)	/* need to sort when interupting in the middle of the list */
				DAG_scene_sort( G.scene );
			return NULL; /* error has been set already */
		}
	}

	if (!fast) /* otherwise, only sort at the end */
		DAG_scene_sort( G.scene );

	Py_RETURN_NONE;
}

static PyObject *Object_join( BPyObject * self, PyObject * args )
{
	PyObject *list;
	PyObject *py_child;
	Object *parent;
	Object *child;
	Scene *temp_scene;
	Scene *orig_scene;
	Base *temp_base;
	short type;
	int i, ok=0, ret_value=0, list_length=0;

	/* joining in background causes segfaults */
	if( G.background == 1 )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					"cannot join objects in background mode" );

	/* Check if the arguments passed to makeParent are valid. */
	if( !PyArg_ParseTuple( args, "O", &list ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a list of objects" );
	
	if( !PySequence_Check( list ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					"expected a list of objects" );
	
	list_length = PySequence_Length( list ); /* if there are no objects to join then exit silently */
	
	if( !list_length ) {
		Py_RETURN_NONE;
	}
	
	parent = ( Object * ) self->object;
	type = parent->type;
	
	/* Only these object types are sypported */
	if( type!=OB_MESH && type!=OB_CURVE && type!=OB_SURF && type!=OB_ARMATURE )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
						"Base object is not a type Blender can join" );
	
	if( !object_in_scene( parent, G.scene ) )
		return EXPP_ReturnPyObjError( PyExc_AttributeError,
				"object must be in the current scene" );

	/* exit editmode so join can be done */
	if( G.obedit )
		exit_editmode( EM_FREEDATA );
	
	temp_scene = add_scene( "Scene" ); /* make the new scene */
	temp_scene->lay= 1; /* first layer on */
	
	/* TODO: use EXPP_check_sequence_consistency here */

	/* Check if the PyObject passed in list is a Blender object. */
	for( i = 0; i < list_length; i++ ) {
		py_child = PySequence_GetItem( list, i );
		if( !BPyObject_Check( py_child ) ) {
			/* Cleanup */
			free_libblock( &G.main->scene, temp_scene );
			Py_DECREF( py_child );
			return EXPP_ReturnPyObjError( PyExc_TypeError,
					"expected a list of objects, one or more of the list items is not a Blender Object." );
		} else {
			/* List item is an object, is it the same type? */
			child = Object_FromPyObject( py_child );
			Py_DECREF( py_child );
			if( parent->type == child->type ) {
				if( !object_in_scene( child, G.scene ) ) {
					free_libblock( &G.main->scene, temp_scene );
					return EXPP_ReturnPyObjError( PyExc_AttributeError,
							"object must be in the current scene" );
				}

				ok =1;
				/* Add a new base, then link the base to the temp_scene */
				temp_base = MEM_callocN( sizeof( Base ), "pynewbase" );
				/* we know these types are the same, link to the temp scene
				 * for joining */
				temp_base->object = child;	/* link object to the new base */
				temp_base->flag |= SELECT;
				temp_base->lay = 1; /*1 layer on */
				
				BLI_addhead( &temp_scene->base, temp_base );	/* finally, link new base to scene */
				child->id.us += 1; /*Would usually increase user count but in this case it's ok not to */
				
				/*DAG_object_flush_update(temp_scene, temp_base->object, OB_RECALC_DATA);*/
			}
		}
	}
	
	orig_scene = G.scene; /* backup our scene */
	
	/* Add the main object into the temp_scene */
	temp_base = MEM_callocN( sizeof( Base ), "pynewbase" );
	temp_base->object = parent;	/* link object to the new base */
	temp_base->flag |= SELECT;
	temp_base->lay = 1; /*1 layer on */
	BLI_addhead( &temp_scene->base, temp_base );	/* finally, link new base to scene */
	parent->id.us += 1;
	
	/* all objects in the scene, set it active and the active object */
	set_scene( temp_scene );
	set_active_base( temp_base );
	
	/* Do the joining now we know everythings OK. */
	if(type == OB_MESH)
		ret_value = join_mesh();
	else if(type == OB_CURVE)
		ret_value = join_curve(OB_CURVE);
	else if(type == OB_SURF)
		ret_value = join_curve(OB_SURF);
	else if(type == OB_ARMATURE)
		ret_value = join_armature();
	
	/* May use this for correcting object user counts later on */
	/*
	if (!ret_value) {
		temp_base = temp_scene->base.first;
		while( base ) {
			object = base->object;
			object->id.us +=1
			base = base->next;
		}
	}*/

	/* remove old scene */
	set_scene( orig_scene );
	free_libblock( &G.main->scene, temp_scene );

	/* no objects were of the correct type, return None */
	if (!ok) {
		Py_RETURN_NONE;
	}

	/* If the join failed then raise an error */
	if (!ret_value)
		return ( EXPP_ReturnPyObjError( PyExc_RuntimeError,
"Blender failed to join the objects, this is not a script error.\n\
Please add exception handling to your script with a RuntimeError exception\n\
letting the user know that their data could not be joined." ) );

	Py_RETURN_NONE;
}

static PyObject *internal_makeParent(Object *parent, PyObject *py_child,
		int partype,                /* parenting type */
		int noninverse, int fast,   /* parenting arguments */
		int v1, int v2, int v3,     /* for vertex parent */
		char *bonename)             /* for bone parents - assume the name is already checked to be a valid bone name*/
{
	Object *child = NULL;

	if( BPyObject_Check( py_child ) )
		child = Object_FromPyObject( py_child );

	if( child == NULL )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					"Object Type expected" );

	if( test_parent_loop( parent, child ) )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"parenting loop detected - parenting failed" );

	if (partype == PARSKEL && child->type != OB_MESH)
		child->partype = PAROBJECT;
	else
		child->partype = (short)partype;

	if (partype == PARVERT3) {
		child->par1 = v1;
		child->par2 = v2;
		child->par3 = v3;
	}
	else if (partype == PARVERT1) {
		child->par1 = v1;
	} else if (partype == PARBONE) {
		strcpy( child->parsubstr, bonename );
	}
	
	

	child->parent = parent;
	/* py_obj_child = (BPyObject *) py_child; */
	if( noninverse == 1 ) {
		Mat4One(child->parentinv);
		/* Parent inverse = unity */
		child->loc[0] = 0.0;
		child->loc[1] = 0.0;
		child->loc[2] = 0.0;
	} else {
		what_does_parent( child );
		Mat4Invert( child->parentinv, workob.obmat );
		clear_workob();
	}

	if( !fast )
		child->recalc |= OB_RECALC_OB;

	Py_RETURN_NONE;
}

#define DTX_MASK ( OB_AXIS | OB_TEXSPACE | OB_DRAWNAME | \
		OB_DRAWIMAGE | OB_DRAWWIRE | OB_DRAWXRAY | OB_DRAWTRANSP )

static int Object_setDrawMode( BPyObject * self, PyObject * args )
{
	PyObject* integer = PyNumber_Int( args );
	int value;

	if( !integer )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected integer argument" );

	value = ( int )PyInt_AS_LONG( integer );
	Py_DECREF( integer );
	if( value & ~DTX_MASK )
		return EXPP_ReturnIntError( PyExc_ValueError,
				"undefined bit(s) set in bitfield" );

	self->object->dtx = value;
	self->object->recalc |= OB_RECALC_OB;  

	return 0;
}

static int Object_setMatrix( BPyObject * self, BPyMatrixObject * mat )
#if 0
{
	int x, y;

	if( !BPyMatrix_Check( mat ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected matrix object as argument" );

	if( mat->rowSize == 4 && mat->colSize == 4 ) {
		for( x = 0; x < 4; x++ ) {
			for( y = 0; y < 4; y++ ) {
				self->object->obmat[x][y] = mat->matrix[x][y];
			}
		}
	} else if( mat->rowSize == 3 && mat->colSize == 3 ) {
		for( x = 0; x < 3; x++ ) {
			for( y = 0; y < 3; y++ ) {
				self->object->obmat[x][y] = mat->matrix[x][y];
			}
		}
		/* if a 3x3 matrix, clear the fourth row/column */
		for( x = 0; x < 3; x++ )
			self->object->obmat[x][3] = self->object->obmat[3][x] = 0.0;
		self->object->obmat[3][3] = 1.0;
	} else 
		return EXPP_ReturnIntError( PyExc_ValueError,
				"expected 3x3 or 4x4 matrix" );

	apply_obmat( self->object );

	/* since we have messed with object, we need to flag for DAG recalc */
	self->object->recalc |= OB_RECALC_OB;  

	return 0;
}
#endif
{
	int x, y;
	float matrix[4][4]; /* for the result */
	float invmat[4][4]; /* for the result */

	if( !BPyMatrix_Check( mat ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected matrix object as argument" );

	if( mat->rowSize == 4 && mat->colSize == 4 ) {
		for( x = 0; x < 4; x++ ) {
			for( y = 0; y < 4; y++ ) {
				matrix[x][y] = mat->matrix[x][y];
			}
		}
	} else if( mat->rowSize == 3 && mat->colSize == 3 ) {
		for( x = 0; x < 3; x++ ) {
			for( y = 0; y < 3; y++ ) {
				matrix[x][y] = mat->matrix[x][y];
			}
		}
		/* if a 3x3 matrix, clear the fourth row/column */
		for( x = 0; x < 3; x++ )
			matrix[x][3] = matrix[3][x] = 0.0;
		matrix[3][3] = 1.0;
	} else 
		return EXPP_ReturnIntError( PyExc_ValueError,
				"expected 3x3 or 4x4 matrix" );

	/* localspace matrix is truly relative to the parent, but parameters
	 * stored in object are relative to parentinv matrix.  Undo the parent
	 * inverse part before updating obmat and calling apply_obmat() */
	if( self->object->parent ) {
		Mat4Invert( invmat, self->object->parentinv );
		Mat4MulMat4( self->object->obmat, matrix, invmat );
	} else
		Mat4CpyMat4( self->object->obmat, matrix );

	apply_obmat( self->object );

	/* since we have messed with object, we need to flag for DAG recalc */
	self->object->recalc |= OB_RECALC_OB;  

	return 0;
}


/*
 * Object_insertIpoKey()
 *  inserts Object IPO key for LOC, ROT, SIZE, LOCROT, or LOCROTSIZE
 *  Note it also inserts actions! 
 */

static PyObject *Object_insertIpoKey( BPyObject * self, PyObject * value )
{
	Object *ob= self->object;
	int key= 0;
	char *actname= NULL;
	constValue *c;

	/* check that we have a valid constant */
	c = Const_FromPyObject( &IpoKeyTypes, value);
	if( !c )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected IpoKey Constant argument" );

	key= c->i;

	if(ob->ipoflag & OB_ACTION_OB)
		actname= "Object";

	/* this could be cleaned up with switch/case statements */

	if (key == IPOKEY_LOC || key == IPOKEY_LOCROT || key == IPOKEY_LOCROTSIZE){
		insertkey((ID *)ob, ID_OB, actname, NULL,OB_LOC_X, 0);
		insertkey((ID *)ob, ID_OB, actname, NULL,OB_LOC_Y, 0);
		insertkey((ID *)ob, ID_OB, actname, NULL,OB_LOC_Z, 0);      
	}
	if (key == IPOKEY_ROT || key == IPOKEY_LOCROT || key == IPOKEY_LOCROTSIZE){
		insertkey((ID *)ob, ID_OB, actname, NULL,OB_ROT_X, 0);
		insertkey((ID *)ob, ID_OB, actname, NULL,OB_ROT_Y, 0);
		insertkey((ID *)ob, ID_OB, actname, NULL,OB_ROT_Z, 0);      
	}
	if (key == IPOKEY_SIZE || key == IPOKEY_LOCROTSIZE ){
		insertkey((ID *)ob, ID_OB, actname, NULL,OB_SIZE_X, 0);
		insertkey((ID *)ob, ID_OB, actname, NULL,OB_SIZE_Y, 0);
		insertkey((ID *)ob, ID_OB, actname, NULL,OB_SIZE_Z, 0);      
	}

	if (key == IPOKEY_PI_STRENGTH ){
		insertkey((ID *)ob, ID_OB, actname, NULL, OB_PD_FSTR, 0);   
	} else if (key == IPOKEY_PI_FALLOFF ){
		insertkey((ID *)ob, ID_OB, actname, NULL, OB_PD_FFALL, 0);   
	} else if (key == IPOKEY_PI_SURFACEDAMP ){
		insertkey((ID *)ob, ID_OB, actname, NULL, OB_PD_SDAMP, 0);   
	} else if (key == IPOKEY_PI_RANDOMDAMP ){
		insertkey((ID *)ob, ID_OB, actname, NULL, OB_PD_RDAMP, 0);   
	} else if (key == IPOKEY_PI_PERM ){
		insertkey((ID *)ob, ID_OB, actname, NULL, OB_PD_PERM, 0);   
	}

	allspace(REMAKEIPO, 0);
	EXPP_allqueue(REDRAWIPO, 0);
	EXPP_allqueue(REDRAWVIEW3D, 0);
	EXPP_allqueue(REDRAWACTION, 0);
	EXPP_allqueue(REDRAWNLA, 0);

	Py_RETURN_NONE;
}

/*
 * Object_insertPoseKey()
 * inserts a Action Pose key from a given pose (sourceaction, frame) to the
 * active action to a given framenum
 */

static PyObject *Object_insertPoseKey( BPyObject * self, PyObject * args )
{
	Object *ob= self->object;
	BPyActionObject *sourceact;
	char *chanName;
	int actframe;


	/* for doing the time trick, similar to editaction bake_action_with_client() */
	int oldframe;
	int curframe;

	if( !PyArg_ParseTuple( args, "O!sii", &BPyAction_Type, &sourceact,
				&chanName, &actframe, &curframe ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expects an action to copy poses from, a string for chan/bone name, an int argument for frame to extract from the action and finally another int for the frame where to put the new key in the active object.action" );

	extract_pose_from_action(ob->pose, sourceact->action, (float)actframe);

	oldframe = G.scene->r.cfra;
	G.scene->r.cfra = curframe;

	/* XXX: must check chanName actually exists, otherwise segfaults! */
	//achan = get_action_channel(sourceact->action, chanName);

	insertkey(&ob->id, ID_PO, chanName, NULL, AC_LOC_X, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_LOC_Y, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_LOC_Z, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_QUAT_X, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_QUAT_Y, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_QUAT_Z, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_QUAT_W, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_SIZE_X, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_SIZE_Y, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_SIZE_Z, 0);
	
	G.scene->r.cfra = oldframe;

	allspace(REMAKEIPO, 0);
	EXPP_allqueue(REDRAWIPO, 0);
	EXPP_allqueue(REDRAWVIEW3D, 0);
	EXPP_allqueue(REDRAWACTION, 0);
	EXPP_allqueue(REDRAWNLA, 0);

	/* restore, but now with the new action in place */
	/*extract_pose_from_action(ob->pose, ob->action, G.scene->r.cfra);
	where_is_pose(ob);*/
	
	EXPP_allqueue(REDRAWACTION, 1);

	Py_RETURN_NONE;
}

static PyObject *Object_insertCurrentPoseKey( BPyObject * self, PyObject * args )
{
	Object *ob= self->object;
	char *chanName;

	/* for doing the time trick, similar to editaction bake_action_with_client() */
	int oldframe;
	int curframe;

	if( !PyArg_ParseTuple( args, "si", &chanName, &curframe ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected chan/bone name, and a time (int) argument" );

	oldframe = G.scene->r.cfra;
	G.scene->r.cfra = curframe;

	/* XXX: must check chanName actually exists, otherwise segfaults! */

	insertkey(&ob->id, ID_PO, chanName, NULL, AC_LOC_X, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_LOC_Y, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_LOC_Z, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_QUAT_X, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_QUAT_Y, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_QUAT_Z, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_QUAT_W, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_SIZE_X, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_SIZE_Y, 0);
	insertkey(&ob->id, ID_PO, chanName, NULL, AC_SIZE_Z, 0);

	G.scene->r.cfra = oldframe;

	allspace(REMAKEIPO, 0);
	EXPP_allqueue(REDRAWIPO, 0);
	EXPP_allqueue(REDRAWVIEW3D, 0);
	EXPP_allqueue(REDRAWACTION, 0);
	EXPP_allqueue(REDRAWNLA, 0);

	/* restore */
	extract_pose_from_action(ob->pose, ob->action, (float)G.scene->r.cfra);
	where_is_pose(ob);

	EXPP_allqueue(REDRAWACTION, 1);

	Py_RETURN_NONE;
}  

static PyObject *Object_setConstraintInfluenceForBone( BPyObject * self,
		PyObject * args )
{
	char *boneName, *constName;
	float influence;
	IpoCurve *icu;

	if( !PyArg_ParseTuple( args, "ssf", &boneName, &constName, &influence ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expects bonename, constraintname, influenceval" );
	
	icu = verify_ipocurve((ID *)self->object, ID_CO, boneName, constName, NULL,
			CO_ENFORCE);
	
	if (!icu)
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"cannot get a curve from this IPO, may be using libdata" );		
	
	insert_vert_icu(icu, (float)CFRA, influence, 0);
	self->object->recalc |= OB_RECALC_OB;  

	Py_RETURN_NONE;
}

static PyObject *Object_copyNLA( BPyObject * self, PyObject * args ) {
	BPyObject *bpy_fromob;

	if( !PyArg_ParseTuple( args, "O", &bpy_fromob ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					"requires a Blender Object to copy NLA strips from." );
	copy_nlastrips(&self->object->nlastrips, &bpy_fromob->object->nlastrips);
	self->object->recalc |= OB_RECALC_OB;  

	Py_RETURN_NONE;
}

/*Now that  BPY has a Strip type, return the created strip.*/
static PyObject *Object_convertActionToStrip( BPyObject * self )
{
	return ActionStrip_CreatePyObject( convert_action_to_strip( self->object ) );
}

static PyObject *Object_makeTrack( BPyObject * self, PyObject * args )
{
	BPyObject *tracked = NULL;
	Object *ob = self->object;
	int fast = 0;

	if( !PyArg_ParseTuple( args, "O!|i", &BPyObject_Type, &tracked, &fast ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected an object and optionally also an int as arguments." );

	ob->track = tracked->object;

	if( !fast )
		DAG_scene_sort( G.scene );

	Py_RETURN_NONE;
}

static PyObject *Object_shareFrom( BPyObject * self, PyObject * args )
{
	BPyObject *object;
	ID *id;
	ID *oldid;

	if( !PyArg_ParseTuple( args, "O!", &BPyObject_Type, &object ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "expected an object argument" );

	if( !object->object->data )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "Object argument has no data linked yet or is an empty" );
	
	if( self->object->type != object->object->type &&
		self->realtype != object->object->type)
		return EXPP_ReturnPyObjError( PyExc_TypeError,
					      "objects are not of same data type" );

	switch ( object->object->type ) {
	case OB_MESH:
	case OB_LAMP:
	case OB_CAMERA:	/* we can probably add the other types, too */
	case OB_ARMATURE:
	case OB_CURVE:
	case OB_SURF:
	case OB_LATTICE:
		
		/* if this object had no data, we need to enable the realtype */
		if (self->object->type == OB_EMPTY) {
			self->object->type= self->realtype;
			self->realtype = OB_EMPTY;
		}
	
		oldid = ( ID * ) self->object->data;
		id = ( ID * ) object->object->data;
		self->object->data = object->object->data;

		if( self->object->type == OB_MESH && id ) {
			self->object->totcol = 0;
			EXPP_synchronizeMaterialLists( self->object );
		}

		id_us_plus( id );
		if( oldid ) {
			if( oldid->us > 0 ) {
				oldid->us--;
			} else {
				return EXPP_ReturnPyObjError ( PyExc_RuntimeError,
					   "old object reference count below 0" );
			}
		}
		
		Py_RETURN_NONE;
	default:
		return EXPP_ReturnPyObjError( PyExc_ValueError,
				"object type not supported" );
	}
}

static PyObject *Object_getAllProperties( BPyObject * self )
{
	PyObject *prop_list, *pyval;
	bProperty *prop = NULL;

	prop_list = PyList_New( 0 );

	prop = self->object->prop.first;
	while( prop ) {
		pyval = Property_CreatePyObject( prop );
		PyList_Append( prop_list, pyval );
		Py_DECREF(pyval);
		prop = prop->next;
	}
	return prop_list;
}

static PyObject *Object_getProperty( BPyObject * self, PyObject * args )
{
	char *prop_name = NULL;
	bProperty *prop = NULL;

	if( !PyArg_ParseTuple( args, "s", &prop_name ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a string" );

	prop = get_property( self->object, prop_name );
	if( prop )
		return Property_CreatePyObject( prop );

	return EXPP_ReturnPyObjError( PyExc_RuntimeError,
			"couldn't find the property" );
}

static PyObject *Object_addProperty( BPyObject * self, PyObject * args )
{
	bProperty *prop = NULL;
	char *prop_name = NULL;
	PyObject *prop_data = Py_None;
	char *prop_type = NULL;
	short type = -1;
	BPyGamePropObject *py_prop = NULL;
	int argslen = PyObject_Length( args );

	if( argslen == 3 || argslen == 2 ) {
		if( !PyArg_ParseTuple( args, "sO|s", &prop_name, &prop_data,
					&prop_type ) ) {
			return EXPP_ReturnPyObjError( PyExc_TypeError,
					"expecting string, data, and optional string" );
		}
	} else if( argslen == 1 ) {
		if( !PyArg_ParseTuple( args, "O!", &BPyGameProperty_Type, &py_prop ) )
			return EXPP_ReturnPyObjError( PyExc_TypeError,
					"expecting a Property" );

		if( py_prop->property != NULL )
			return EXPP_ReturnPyObjError( PyExc_ValueError,
					"Property is already added to an object" );
	} else {
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected 1,2 or 3 arguments" );
	}

	/*parse property type*/
	if( !py_prop ) {
		if( prop_type ) {
			if( BLI_streq( prop_type, "BOOL" ) )
				type = PROP_BOOL;
			else if( BLI_streq( prop_type, "INT" ) )
				type = PROP_INT;
			else if( BLI_streq( prop_type, "FLOAT" ) )
				type = PROP_FLOAT;
			else if( BLI_streq( prop_type, "TIME" ) )
				type = PROP_TIME;
			else if( BLI_streq( prop_type, "STRING" ) )
				type = PROP_STRING;
			else
				return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					   "BOOL, INT, FLOAT, TIME or STRING expected" );
		} else {
			/*use the default*/
			if( PyInt_Check( prop_data ) )
				type = PROP_INT;
			else if( PyFloat_Check( prop_data ) )
				type = PROP_FLOAT;
			else if( PyString_Check( prop_data ) )
				type = PROP_STRING;
		}
	} else {
		type = py_prop->type;
	}

	/*initialize a new bProperty of the specified type*/
	prop = new_property( type );

	/*parse data*/
	if( !py_prop ) {
		BLI_strncpy( prop->name, prop_name, 32 );
		if( PyInt_Check( prop_data ) ) {
			*( ( int * ) &prop->data ) =
				( int ) PyInt_AsLong( prop_data );
		} else if( PyFloat_Check( prop_data ) ) {
			*( ( float * ) &prop->data ) =
				( float ) PyFloat_AsDouble( prop_data );
		} else if( PyString_Check( prop_data ) ) {
			BLI_strncpy( prop->poin,
				     PyString_AsString( prop_data ),
				     MAX_PROPSTRING );
		}
	} else {
		py_prop->property = prop;

		/* this should never be able to happen is we just assigned a valid
		 * proper to py_prop->property */

		if( !updateProperyData( py_prop ) ) {
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
							"Could not update property data" );
		}
	}

	/*add to property listbase for the object*/
	BLI_addtail( &self->object->prop, prop );

	Py_RETURN_NONE;
}

static PyObject *Object_removeProperty( BPyObject * self, PyObject * args )
{
	char *prop_name = NULL;
	BPyGamePropObject *py_prop = NULL;
	bProperty *prop = NULL;

	/* we accept either a property stringname or actual object */
	if( PyTuple_Size( args ) == 1 ) {
		PyObject *prop = PyTuple_GET_ITEM( args, 0 );
		if( BPyProperty_Check( prop ) )
			py_prop = (BPyGamePropObject *)prop;
		else
			prop_name = PyString_AsString( prop );
	}
	if( !py_prop && !prop_name )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected a Property or a string" );

	/*remove the link, free the data, and update the py struct*/
	if( py_prop ) {
		BLI_remlink( &self->object->prop, py_prop->property );
		if( updatePyProperty( py_prop ) ) {
			free_property( py_prop->property );
			py_prop->property = NULL;
		}
	} else {
		prop = get_property( self->object, prop_name );
		if( prop ) {
			BLI_remlink( &self->object->prop, prop );
			free_property( prop );
		}
	}
	Py_RETURN_NONE;
}

static PyObject *Object_removeAllProperties( BPyObject * self )
{
	free_properties( &self->object->prop );
	Py_RETURN_NONE;
}

static PyObject *Object_copyAllPropertiesTo( BPyObject * self,
					     PyObject * args )
{
	PyObject *dest;
	bProperty *prop = NULL;
	bProperty *propn = NULL;

	if( !PyArg_ParseTuple( args, "O!", &BPyObject_Type, &dest ) )
		return EXPP_ReturnPyObjError( PyExc_TypeError,
				"expected an Object" );

	/*make a copy of all its properties*/
	prop = self->object->prop.first;
	while( prop ) {
		propn = copy_property( prop );
		BLI_addtail( &( ( BPyObject * ) dest )->object->prop, propn );
		prop = prop->next;
	}

	Py_RETURN_NONE;
}

static PyObject *Object_getNLAflagBits ( BPyObject * self ) 
{
	if (self->object->nlaflag & OB_NLA_OVERRIDE)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static int Object_setNLAflagBits ( BPyObject * self, PyObject * args ) 
{
	int value;

	value = PyObject_IsTrue( args );
	if( value == -1 )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected 1/0 or true/false" );

	if (value==1)
		self->object->nlaflag |= OB_NLA_OVERRIDE;
	else 
		self->object->nlaflag &= ~OB_NLA_OVERRIDE;
		
	self->object->recalc |= OB_RECALC_OB;  

	return 0;
}

static PyObject *Object_getDupliObjects( BPyObject * self )
{
	Object *ob= self->object;
	PyObject *pair;
	
	if(ob->transflag & OB_DUPLI) {
		/* before make duplis, update particle for current frame */
		if(ob->transflag & OB_DUPLIVERTS) {
			PartEff *paf= give_parteff(ob);
			if(paf) {
				if(paf->flag & PAF_ANIMATED) build_particle_system(ob);
			}
		}
		if(ob->type!=OB_MBALL) {
			PyObject *list;
			DupliObject *dupob;
			int index;
			ListBase *duplilist = object_duplilist(G.scene, ob);

			list = PyList_New( BLI_countlist(duplilist) );
			if( !list )
				return EXPP_ReturnPyObjError( PyExc_RuntimeError,
						"PyList_New() failed" );

			for(dupob= duplilist->first, index=0; dupob; dupob= dupob->next, index++) {
				pair = PyTuple_New( 2 );
				
				PyTuple_SET_ITEM( pair, 0, Object_CreatePyObject(dupob->ob) );
				PyTuple_SET_ITEM( pair, 1, Matrix_CreatePyObject((float*)dupob->mat,4,4, (PyObject *)NULL) );
				PyList_SET_ITEM( list, index, pair);
			}
			free_object_duplilist(duplilist);
			return list;
		}
	}
	return PyList_New( 0 );
}

static PyObject *Object_getDupliGroup( BPyObject * self )
{	/* dup_group == NULL is ok -> None */
	return Group_CreatePyObject( self->object->dup_group );
}

static int Object_setDupliGroup( BPyObject * self, PyObject * value )
{
	return GenericLib_assignData(value, (void **) &self->object->dup_group, 0, 1, ID_GR, 0);
}

static PyObject *Object_getEffects( BPyObject * self )
{
	PyObject *effect_list, *pyval;
	Effect *eff;

	effect_list = PyList_New( 0 );
	if( !effect_list )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"PyList_New() failed" );

	eff = self->object->effect.first;

	while( eff ) {
		pyval = Effect_CreatePyObject( eff, self->object );
		PyList_Append( effect_list, pyval );
		Py_DECREF(pyval);
		eff = eff->next;
	}
	return effect_list;
}

static PyObject *Object_getActionStrips( BPyObject * self )
{
	return ActionStripSeq_CreatePyObject( self->object );
}

static PyObject *Object_getConstraints( BPyObject * self )
{
	return ObConstraintSeq_CreatePyObject( self->object );
}

static PyObject *Object_getModifiers( BPyObject * self )
{
	return ModSeq_CreatePyObject( self->object, NULL );
}

static int Object_setModifiers( BPyObject * self, PyObject * value )
{
	BPyModSeqObject *pymodseq;
	ModifierData *md;
	
	if (!BPyModSeq_Check(value))
		return EXPP_ReturnIntError( PyExc_TypeError,
				"can only assign another objects modifiers" );
	
	pymodseq = ( BPyModSeqObject * ) value;
	
	if (self->object->type != pymodseq->object->type)
		return EXPP_ReturnIntError( PyExc_TypeError,
				"can only assign modifiers between objects of the same type" );
	
	if (self->object == pymodseq->object)
		return 0;
	
	object_free_modifiers(self->object);
	for (md=pymodseq->object->modifiers.first; md; md=md->next) {
		if (md->type!=eModifierType_Hook) {
			ModifierData *nmd = modifier_new(md->type);
			modifier_copyData(md, nmd);
			BLI_addtail(&self->object->modifiers, nmd);
		}
	}
	
	DAG_object_flush_update(G.scene, self->object, OB_RECALC_DATA);
	return 0;
}

static PyObject *Object_insertShapeKey(BPyObject * self)
{
	insert_shapekey(self->object);
	Py_RETURN_NONE;
}

/* __copy__() */
static  PyObject *Object_copy(BPyObject * self)
{
	/* copy_object never returns NULL */
	struct Object *object= copy_object( self->object );
	object->id.us= 0; /*is 1 by default, not sure why */
	
	/* Create a Python object from it. */
	return Object_CreatePyObject( object );
}

/*****************************************************************************/
/* Function:	Object_CreatePyObject					 */
/* Description: This function will create a new BlenObject from an existing  */
/*		Object structure.					 */
/*****************************************************************************/
PyObject *Object_CreatePyObject( struct Object * obj )
{
	BPyObject *blen_object;

	if( !obj )
		Py_RETURN_NONE;
	
	blen_object =
		( BPyObject * ) PyObject_NEW( BPyObject, &BPyObject_Type );

	if( blen_object == NULL ) {
		return ( NULL );
	}
	blen_object->object = obj;
	blen_object->realtype = OB_EMPTY;
	obj->id.us++;
	return ( ( PyObject * ) blen_object );
}

/*****************************************************************************/
/* Function:    Object_dealloc                                               */
/* Description: This is a callback function for the BlenObject type. It is   */
/*      the destructor function.                                             */
/*****************************************************************************/
static void Object_dealloc( BPyObject * self )
{
	if( self->realtype != OB_EMPTY ) 
		free_libblock_us( &G.main->object, self->object );
	else 
		self->object->id.us--;

#if 0	/* this will adjust the ID and if zero delete the object */
	free_libblock_us( &G.main->object, self->object );
#endif
	PyObject_DEL( self );
}

/* Particle Deflection functions */

static PyObject *Object_getPIDeflection( BPyObject * self )
{  
    if( !self->object->pd && !setupPI(self->object) )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"particle deflection could not be accessed" );

	return PyBool_FromLong( ( long ) self->object->pd->deflect );
}

static int Object_setPIDeflection( BPyObject * self, PyObject * args )
{
	int value;

    if( !self->object->pd && !setupPI(self->object) )
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"particle deflection could not be accessed" );

	value = PyObject_IsTrue( args );
	if( value == -1 )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected true/false argument" );

	self->object->pd->deflect = (short)value;
	self->object->recalc |= OB_RECALC_OB;  

	return 0;
}

/*
 * Setter for "piType" constant attribute.  
 */

static int Object_setPIType( BPyObject * self, PyObject * value )
{
	constValue *c;

    if( !self->object->pd && !setupPI(self->object) )
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"particle deflection could not be accessed" );

	/* check explicitly for a PITypes constant */
    c = Const_FromPyObject( &PITypes, value );
    if (!c)
    	return EXPP_ReturnIntError( PyExc_TypeError,
    			"expected PITypes constant" );
	
	if ( c->i == PFIELD_MAGNET )
		return EXPP_ReturnIntError( PyExc_ValueError,
				"PFIELD_MAGNET not supported" );

	self->object->pd->forcefield = c->i;
	self->object->recalc |= OB_RECALC_OB;
	
	return 0;	
}

static PyObject *Object_getPIUseMaxDist( BPyObject * self )
{  
    if( !self->object->pd && !setupPI(self->object) )
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"particle deflection could not be accessed" );

	return PyBool_FromLong( ( long )self->object->pd->flag );
}

static int Object_setPIUseMaxDist( BPyObject * self, PyObject * args )
{
	int value;

    if( !self->object->pd && !setupPI(self->object) )
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"particle deflection could not be accessed" );

	value = PyObject_IsTrue( args );
	if( value == -1 )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected true/false argument" );

	self->object->pd->flag = (short)value;
	self->object->recalc |= OB_RECALC_OB;  

	return 0;
}

/* RIGIDBODY FUNCTIONS */

static PyObject *Object_getRBMass( BPyObject * self )
{
    return PyFloat_FromDouble( (double)self->object->mass );
}

static int Object_setRBMass( BPyObject * self, PyObject * args )
{
    float value;
	PyObject* flt = PyNumber_Float( args );

	if( !flt )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected float argument" );
	value = (float)PyFloat_AS_DOUBLE( flt );
	Py_DECREF( flt );

	if( value < 0.0f )
		return EXPP_ReturnIntError( PyExc_ValueError,
			"acceptable values are non-negative, 0.0 or more" );

	self->object->mass = value;
	self->object->recalc |= OB_RECALC_OB;  

	return 0;
}

/* this is too low level, possible to add helper methods */

#define GAMEFLAG_MASK ( OB_DYNAMIC | OB_CHILD | OB_ACTOR | OB_DO_FH | \
		OB_ROT_FH | OB_ANISOTROPIC_FRICTION | OB_GHOST | OB_RIGID_BODY | \
		OB_BOUNDS | OB_COLLISION_RESPONSE | OB_SECTOR | OB_PROP | \
		OB_MAINACTOR )

static PyObject *Object_getRBFlags( BPyObject * self )
{
    return PyInt_FromLong( (long)( self->object->gameflag & GAMEFLAG_MASK ) );
}

static int Object_setRBFlags( BPyObject * self, PyObject * args )
{
	PyObject* integer = PyNumber_Int( args );
	int value;

	if( !integer )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected integer argument" );

	value = ( int )PyInt_AS_LONG( integer );
	Py_DECREF( integer );
	if( value & ~GAMEFLAG_MASK )
		return EXPP_ReturnIntError( PyExc_ValueError,
				"undefined bit(s) set in bitfield" );

	self->object->gameflag = value;
	self->object->recalc |= OB_RECALC_OB;  

	return 0;
}

static int setupPI( Object* ob )
{
	if( ob->pd==NULL ) {
		ob->pd= MEM_callocN(sizeof(PartDeflect), "PartDeflect");
		/* and if needed, init here */
	}

	if( !ob->pd )
		return 0;

	ob->pd->deflect      =0;		
	ob->pd->forcefield   =0;	
	ob->pd->flag         =0;	
	ob->pd->pdef_damp    =0;		
	ob->pd->pdef_rdamp   =0;		
	ob->pd->pdef_perm    =0;	
	ob->pd->f_strength   =0;	
	ob->pd->f_power      =0;	
	ob->pd->maxdist      =0;	       
	return 1;
}

/*
 * scan list of Objects looking for matching obdata.
 * if found, set OB_RECALC_DATA flag.
 * call this from a bpy type update() method.
 */

void Object_updateDag( void *data )
{
	Object *ob;

	if( !data )
		return;

	for( ob = G.main->object.first; ob; ob= ob->id.next ){
		if( ob->data == data ) {
			ob->recalc |= OB_RECALC_DATA;
		}
	}
}

/*
 * utilities routines for handling generic getters and setters
 */

/*
 * get integer attributes
 */

static PyObject *getIntAttr( BPyObject *self, void *type )
{
	PyObject *attr = NULL;
	int param;
	struct Object *object = self->object;

	switch( (int)type ) {
	case EXPP_OBJ_ATTR_LAYERMASK:
		param = object->lay;
		break;
	case EXPP_OBJ_ATTR_COLBITS:
		param = object->colbits;
		if( param < 0 ) param += 65536;
		break;
	case EXPP_OBJ_ATTR_DRAWMODE:
		param = object->dtx;
		break;
	case EXPP_OBJ_ATTR_DUPON:
		param = object->dupon;
		break;
	case EXPP_OBJ_ATTR_DUPOFF:
		param = object->dupoff;
		break;
	case EXPP_OBJ_ATTR_DUPSTA:
		param = object->dupsta;
		break;
	case EXPP_OBJ_ATTR_DUPEND:
		param = object->dupend;
		break;
	case EXPP_OBJ_ATTR_PASSINDEX:
		param = object->index;
		break;
	case EXPP_OBJ_ATTR_ACT_MATERIAL:
		param = object->actcol;
		break;
	case EXPP_OBJ_ATTR_ACT_SHAPE:
		param = object->shapenr;
		break;		
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"undefined type in getIntAttr" );
	}

	attr = PyInt_FromLong( param );
	
	if( attr )
		return attr;

	return EXPP_ReturnPyObjError( PyExc_MemoryError,
				"PyInt_FromLong() failed!" );
}

/*
 * get constant attributes
 */

static PyObject *getConstAttr( BPyObject *self, void *type )
{
	Object *object = self->object;
	constDefinition *constType;
	constValue param;
	int ctype = EXPP_CONST_INT;

	switch( (int)type ) {
	case EXPP_OBJ_ATTR_DRAWTYPE:
		param.i = object->dt;
		constType = &drawTypes;
		break;
	case EXPP_OBJ_ATTR_PARENT_TYPE:
		param.i = object->partype;
		constType = &parentTypes;
		break;
	case EXPP_OBJ_ATTR_RB_SHAPEBOUND:
		param.i = object->boundtype;
		constType = &RBShapes;
		break;
	case EXPP_OBJ_ATTR_PI_TYPE:
		if( !object->pd && !setupPI(object) )
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
					"particle deflection could not be accessed" );
		param.i = object->pd->forcefield;
		constType = &PITypes;
		break;
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"undefined type in getConstAttr" );
	}

	switch( ctype ) {
	case EXPP_CONST_INT:
    	return PyConst_NewInt( constType, param.i );
	case EXPP_CONST_FLOAT:
	default:
    	return PyConst_NewFloat( constType, param.f );
	}
}

/*
 * set integer attributes which require clamping
 */

static int setIntAttrClamp( BPyObject *self, PyObject *value, void *type )
{
	void *param;
	struct Object *object = self->object;
	int min, max, size;

	switch( (int)type ) {
	case EXPP_OBJ_ATTR_DUPON:
		min = 1;
		max = 1500;
		size = 'H';			/* in case max is later made > 32767 */
		param = (void *)&object->dupon;
		break;
	case EXPP_OBJ_ATTR_DUPOFF:
		min = 0;
		max = 1500;
		size = 'H';			/* in case max is later made > 32767 */
		param = (void *)&object->dupoff;
		break;
	case EXPP_OBJ_ATTR_DUPSTA:
		min = 1;
		max = 32767;
		size = 'H';			/* in case max is later made > 32767 */
		param = (void *)&object->dupsta;
		break;
	case EXPP_OBJ_ATTR_DUPEND:
		min = 1;
		max = 32767;
		size = 'H';			/* in case max is later made > 32767 */
		param = (void *)&object->dupend;
		break;
	case EXPP_OBJ_ATTR_PASSINDEX:
		min = 0;
		max = 1000;
		size = 'H';			/* in case max is later made > 32767 */
		param = (void *)&object->index;
		break;
	case EXPP_OBJ_ATTR_ACT_MATERIAL:
		min = 1;
		max = object->totcol;
		size = 'b';			/* in case max is later made > 128 */
		param = (void *)&object->actcol;
		break;
	case EXPP_OBJ_ATTR_ACT_SHAPE:
	{
		Key *key= ob_get_key(object);
		KeyBlock *kb;
		min = 1;
		max = 0;
		if (key) {
			max= 1;
			for (kb = key->block.first; kb; kb=kb->next, max++);
		}
		size = 'h';			/* in case max is later made > 128 */
		param = (void *)&object->shapenr;
		break;
	}
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setIntAttrClamp");
	}

	self->object->recalc |= OB_RECALC_OB;  
	return EXPP_setIValueClamped( value, param, min, max, size );
}

/*
 * set integer attributes which require range checking
 */

static int setIntAttrRange( BPyObject *self, PyObject *value, void *type )
{
	void *param;
	struct Object *object = self->object;
	int min, max, size;

	if( !PyInt_Check( value ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
					"expected integer argument" );

	/* these parameters require clamping */

	switch( (int)type ) {
	case EXPP_OBJ_ATTR_COLBITS:
		min = 0;
		max = 0xffff;
		size = 'H';
		param = (void *)&object->colbits;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setIntAttrRange" );
	}

	self->object->recalc |= OB_RECALC_OB;  
	return EXPP_setIValueRange( value, param, min, max, size );
}

/*
 * set constant attributes
 */

static int setConstAttr( BPyObject *self, PyObject *value, void *type )
{
	void *param;
	struct Object *object = self->object;
	constDefinition *constType;
	constValue *c;
	
	/* for each constant, assign pointer to its storage and set the type
 	 * of constant class we should match */

	switch( (int)type ) {
	case EXPP_OBJ_ATTR_DRAWTYPE:
		param = (void *)&object->dt;
		constType = &drawTypes;
		break;
	case EXPP_OBJ_ATTR_RB_SHAPEBOUND:
		param = (void *)&object->boundtype;
		constType = &RBShapes;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setConstAttr" );
	}
	
	/*
	 * this checks for a constant of the correct type, and if so use the
	 * value
	 */
	/* check that we have a valid constant */
	c = Const_FromPyObject( constType, value);
	if( !c )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected DrawType constant or string" );
	
	/* if we mess with object, we need to flag for DAG recalc */
	*(int *)param = c->i;
	self->object->recalc |= OB_RECALC_OB;  
	return 0;
}

/*
 * get floating point attributes
 */

static PyObject *getFloatAttr( BPyObject *self, void *type )
{
	float param;
	struct Object *object = self->object;

	if( (int)type >= EXPP_OBJ_ATTR_PI_SURFACEDAMP &&
			(int)type <= EXPP_OBJ_ATTR_PI_SBOFACETHICK ) {
    	if( !self->object->pd && !setupPI(self->object) )
			return EXPP_ReturnPyObjError( PyExc_RuntimeError,
				"particle deflection could not be accessed" );
	}

	switch( (int)type ) {
	case EXPP_OBJ_ATTR_TIMEOFFSET:
		param = object->sf;
		break;
	case EXPP_OBJ_ATTR_DRAWSIZE:
		param = object->empty_drawsize;
		break;
	case EXPP_OBJ_ATTR_PI_SURFACEDAMP:
		param = object->pd->pdef_perm;
		break;
	case EXPP_OBJ_ATTR_PI_RANDOMDAMP:
		param = object->pd->pdef_rdamp;
		break;
	case EXPP_OBJ_ATTR_PI_PERM:
		param = object->pd->pdef_perm;
		break;
	case EXPP_OBJ_ATTR_PI_STRENGTH:
		param = object->pd->f_strength;
		break;
	case EXPP_OBJ_ATTR_PI_FALLOFF:
		param = object->pd->f_power;
		break;
	case EXPP_OBJ_ATTR_PI_MAXDIST:
		param = object->pd->maxdist;
		break;
	case EXPP_OBJ_ATTR_PI_SBDAMP:
		param = object->pd->pdef_sbdamp;
		break;
	case EXPP_OBJ_ATTR_PI_SBIFACETHICK:
		param = object->pd->pdef_sbift;
		break;
	case EXPP_OBJ_ATTR_PI_SBOFACETHICK:
		param = object->pd->pdef_sboft;
		break;
	case EXPP_OBJ_ATTR_DUPFACESCALEFAC:
		param = object->dupfacesca;
		break;
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError, 
				"undefined type in getFloatAttr" );
	}

	return PyFloat_FromDouble( param );
}

/*
 * set floating point attributes which require clamping
 */

static int setFloatAttrClamp( BPyObject *self, PyObject *value, void *type )
{
	float *param;
	struct Object *object = self->object;
	float min, max;

	if( (int)type >= EXPP_OBJ_ATTR_PI_SURFACEDAMP &&
			(int)type <= EXPP_OBJ_ATTR_PI_SBOFACETHICK ) {
    	if( !self->object->pd && !setupPI(self->object) )
			return EXPP_ReturnIntError( PyExc_RuntimeError,
				"particle deflection could not be accessed" );
	}

	switch( (int)type ) {
	case EXPP_OBJ_ATTR_DRAWSIZE:
		min= 0.01; max= 10.0;
		param = &object->empty_drawsize;
		break;
	case EXPP_OBJ_ATTR_TIMEOFFSET:
		min = -MAXFRAMEF;
		max = MAXFRAMEF;
		param = &object->sf;
		break;
	case EXPP_OBJ_ATTR_PI_SURFACEDAMP:
		min= 0.0f; max= 1.0f;
		param = &object->pd->pdef_perm;
		break;
	case EXPP_OBJ_ATTR_PI_RANDOMDAMP:
		min= 0.0f; max= 1.0f;
		param = &object->pd->pdef_rdamp;
		break;
	case EXPP_OBJ_ATTR_PI_PERM:
		min= 0.0f; max= 1.0f;
		param = &object->pd->pdef_perm;
		break;
	case EXPP_OBJ_ATTR_PI_STRENGTH:
		min= 0.0f; max= 1000.0f;
		param = &object->pd->f_strength;
		break;
	case EXPP_OBJ_ATTR_PI_FALLOFF:
		min= 0.0f; max= 1.0f;
		param = &object->pd->f_power;
		break;
	case EXPP_OBJ_ATTR_PI_MAXDIST:
		min= 0.0f; max= 1000.0f;
		param = &object->pd->maxdist;
		break;
	case EXPP_OBJ_ATTR_PI_SBDAMP:
		min= 0.0f; max= 1.0f;
		param = &object->pd->pdef_sbdamp;
		break;
	case EXPP_OBJ_ATTR_PI_SBIFACETHICK:
		min= 0.001f; max= 1.0f;
		param = &object->pd->pdef_sbift;
		break;
	case EXPP_OBJ_ATTR_PI_SBOFACETHICK:
		min= 0.001f; max= 1.0f;
		param = &object->pd->pdef_sboft;
		break;
	case EXPP_OBJ_ATTR_DUPFACESCALEFAC:
		min = 0.001; max = 10000;
		param = &self->object->dupfacesca;
		break;
	default:
		return EXPP_ReturnIntError( PyExc_RuntimeError,
				"undefined type in setFloatAttrClamp" );
	}

	self->object->recalc |= OB_RECALC_OB;  
	return EXPP_setFloatClamped( value, param, min, max );
}

/*
 * set floating point attributes
 */


static PyObject *getVecAttr( BPyObject *self, void *type )
{
	float *pt;
	switch((long)type) {
	case VEC_SUBTYPE_OB_LOC:
		pt = self->object->loc;
		break;
	case VEC_SUBTYPE_OB_DLOC:
		pt = self->object->dloc;
		break;
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
			"undefined type in getVecAttr" );
	}
	return Vector_CreatePyObject(pt, 3, (PyObject *)self);
}

static int setVecAttr( BPyObject *self, PyObject *value, void *type )
{
	/* TODO */
	return 0;
}

static PyObject *getEulerAttr( BPyObject * self, void *type )
{
	float *pt;
	switch((long)type) {
	case EXPP_OBJ_ATTR_ROT:
		pt = self->object->rot;
		break;
	case EXPP_OBJ_ATTR_DROT:
		pt = self->object->drot;
		break;
	default:
		return EXPP_ReturnPyObjError( PyExc_RuntimeError,
			"undefined type in getVecAttr" );
	}
	return Euler_CreatePyObject( pt, (PyObject *)self );
}


static int setEulerAttr( BPyObject * self, PyObject * value )
{
	/* TODO */
	return 0;
}

/*****************************************************************************/
/* BPyObject methods and attribute handlers                                 */
/*****************************************************************************/

static PyObject *Object_getShapeFlag( BPyObject *self, void *type )
{
	if (self->object->shapeflag & (int)type)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static int Object_setShapeFlag( BPyObject *self, PyObject *value,
		void *type )
{
	if (PyObject_IsTrue(value) )
		self->object->shapeflag |= (int)type;
	else
		self->object->shapeflag &= ~(int)type;
	
	self->object->recalc |= OB_RECALC_OB;
	return 0;
}

static PyObject *Object_getRestricted( BPyObject *self, void *type )
{
	if (self->object->restrictflag & (int)type)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

static int Object_setRestricted( BPyObject *self, PyObject *value,
		void *type )
{
	if (PyObject_IsTrue(value) )
		self->object->restrictflag |= (int)type;
	else
		self->object->restrictflag &= ~(int)type;
	
	return 0;
}

static PyObject *Object_getDrawModeBits( BPyObject *self, void *type )
{
	return EXPP_getBitfield( (void *)&self->object->dtx, (int)type, 'b' );
}

static int Object_setDrawModeBits( BPyObject *self, PyObject *value,
		void *type )
{
	self->object->recalc |= OB_RECALC_OB;  
	return EXPP_setBitfield( value, (void *)&self->object->dtx,
			(int)type, 'b' );
}

static PyObject *Object_getTransflagBits( BPyObject *self, void *type )
{
	return EXPP_getBitfield( (void *)&self->object->transflag,
			(int)type, 'h' );
}

static int Object_setTransflagBits( BPyObject *self, PyObject *value,
		void *type )
{
	self->object->recalc |= OB_RECALC_OB;  
	return EXPP_setBitfield( value, (void *)&self->object->transflag,
			(int)type, 'h' );
}

static int Object_setLayersMask( BPyObject *self, PyObject *value )
{
	int layers = 0, local;
	Base *base;

	if( !PyInt_Check( value ) )
		return EXPP_ReturnIntError( PyExc_TypeError,
			"expected an integer (bitmask) as argument" );

	layers = PyInt_AS_LONG( value );

	/* make sure some bits are set, and only those bits are set */

	if( !( layers & 0xFFFFF ) || ( layers & 0xFFF00000 ) )
		return EXPP_ReturnIntError( PyExc_ValueError,
			"bitmask must have between 1 and 20 bits set" );

	/* update any bases pointing to our object */

	base = FIRSTBASE;  /* first base in current scene */
	while( base ) {
		if( base->object == self->object ) {
			base->lay &= 0xFFF00000;
			local = base->lay;
			base->lay = local | layers;
			self->object->lay = base->lay;
		}
		base = base->next;
	}
	countall();
	DAG_scene_sort( G.scene );
	return 0;
}

static int Object_setIpo( BPyObject * self, PyObject * value )
{
	return GenericLib_assignData(value, (void **) &self->object->ipo, 0, 1, ID_IP, ID_OB);
}

static int Object_setTracked( BPyObject * self, PyObject * value )
{
	int ret;
	ret = GenericLib_assignData(value, (void **) &self->object->track, 0, 0, ID_OB, 0);
	if (ret==0) {
		self->object->recalc |= OB_RECALC_OB;  
		DAG_scene_sort( G.scene );
	}
	return ret;
}

/* Localspace matrix */

static PyObject *Object_getMatrixLocal( BPyObject * self )
{
	/* TODO - This is silly, its returnes wrapped if their is no parent */
	if( self->object->parent ) {
		float matrix[4][4]; /* for the result */
		float invmat[4][4]; /* for inverse of parent's matrix */
  	 
		Mat4Invert(invmat, self->object->parent->obmat );
		Mat4MulMat4(matrix, self->object->obmat, invmat);
		return Matrix_CreatePyObject((float*)matrix,4,4, (PyObject *)NULL);
	} else { /* no parent, so return world space matrix */
		disable_where_script( 1 );
		where_is_object( self->object );
		disable_where_script( 0 );
		return Matrix_CreatePyObject((float*)self->object->obmat,4,4, (PyObject *)self);
	}
}

/* Worldspace matrix */

static PyObject *Object_getMatrixWorld( BPyObject * self )
{
	disable_where_script( 1 );
	where_is_object( self->object );
	disable_where_script( 0 );
	return Matrix_CreatePyObject((float*)self->object->obmat,4,4, (PyObject *)self);
}

/* Parent Inverse matrix */

static PyObject *Object_getMatrixParentInverse( BPyObject * self )
{
	return Matrix_CreatePyObject((float*)self->object->parentinv,4,4, (PyObject *)self);
}

static PyObject *Object_getData( BPyObject *self )
{
	Object *object = self->object;
	PyObject *data_object = NULL;

	switch ( object->type ) {
	case OB_ARMATURE:
		data_object = Armature_CreatePyObject( object->data );
		break;
	case OB_CAMERA:
		data_object = Camera_CreatePyObject( object->data );
		break;
	case OB_CURVE:
	case OB_SURF:
		data_object = Curve_CreatePyObject( object->data );
		break;
	case ID_IM:
		data_object = Image_CreatePyObject( object->data );
		break;
	case ID_IP:
		data_object = Ipo_CreatePyObject( object->data );
		break;
	case OB_LAMP:
		data_object = Lamp_CreatePyObject( object->data );
		break;
	case OB_LATTICE:
		data_object = Lattice_CreatePyObject( object->data );
		break;
	case ID_MA:
		break;
	case OB_MESH:
		data_object = Mesh_CreatePyObject( object->data, object );
		break;
	case OB_MBALL:
		data_object = Metaball_CreatePyObject( object->data );
		break;
	case ID_OB:
		data_object = Object_CreatePyObject( object->data );
		break;
	case ID_SCE:
		break;
	case OB_FONT:
		data_object = CurveText_CreatePyObject( object->data );
		break;		
	case ID_WO:
		break;
	default:
		break;
	}

	if( data_object )
		return data_object;

	Py_RETURN_NONE;
}

static PyObject *get_obj_dataname( BPyObject *self )
{
	Object *object = self->object;
	if (object->data)
		return PyString_FromString(((ID *)object->data)->name+2);
	Py_RETURN_NONE;
}

#define PROTFLAGS_MASK ( OB_LOCK_LOCX | OB_LOCK_LOCY | OB_LOCK_LOCZ | \
		OB_LOCK_ROTX | OB_LOCK_ROTY | OB_LOCK_ROTZ | \
		OB_LOCK_SCALEX | OB_LOCK_SCALEY | OB_LOCK_SCALEZ )

static PyObject *Object_getProtectFlags( BPyObject * self )
{
	return PyInt_FromLong( (long)(self->object->protectflag & PROTFLAGS_MASK) );
}

static int Object_setProtectFlags( BPyObject * self, PyObject * value )
{
	PyObject* integer = PyNumber_Int( value );
	short param;

	if( !integer )
		return EXPP_ReturnIntError( PyExc_TypeError,
				"expected integer argument" );

	param = ( short )PyInt_AS_LONG( integer );
	Py_DECREF( integer );
	if( param & ~PROTFLAGS_MASK )
		return EXPP_ReturnIntError( PyExc_ValueError,
				"undefined bit(s) set in bitfield" );

	self->object->protectflag = param;
	self->object->recalc |= OB_RECALC_OB;  
	return 0;
}

static PyObject *Object_getRBRadius( BPyObject * self )
{
    return PyFloat_FromDouble( (double) self->object->inertia );
}

static int Object_setRBRadius( BPyObject * self, PyObject * value )
{
    float param;
    PyObject* flt = PyNumber_Float( value );

    if( !flt )
        return EXPP_ReturnIntError( PyExc_TypeError,
                "expected float argument" );
    param = (float)PyFloat_AS_DOUBLE( flt );
	Py_DECREF( flt );

    if( param < 0.0f )
        return EXPP_ReturnIntError( PyExc_ValueError,
            "acceptable values are non-negative, 0.0 or more" );

    self->object->inertia = param;
	self->object->recalc |= OB_RECALC_OB;  

    return 0;
}

static PyObject *Object_getRBHalfExtents( BPyObject * self )
{
	float center[3], extents[3];

	get_local_bounds( self->object, center, extents );
	return Py_BuildValue( "[fff]", extents[0], extents[1], extents[2] );
}

static PyGetSetDef BPyObject_getset[] = {
	GENERIC_LIB_GETSETATTR_SCRIPTLINK,
	GENERIC_LIB_GETSETATTR_MATERIAL,
	GENERIC_LIB_GETSETATTR_LAYER,
	
	{"pinShape",
	 (getter)Object_getShapeFlag, (setter)Object_setShapeFlag, 
	 "Set the state for pinning this object",
	 (void *)OB_SHAPE_LOCK},
	{"activeShape",
	 (getter)getIntAttr, (setter)setIntAttrClamp, 
	 "set the index for the active shape key",
	 (void *)EXPP_OBJ_ATTR_ACT_SHAPE},
	 
	{"softBody",
	 (getter)SoftBody_CreatePyObject, (setter)NULL, 
	 "Object softbody data",
	 NULL},
	
	{"loc",
	 (getter)getVecAttr, (setter)setVecAttr,
	 "The (X,Y,Z) location coordinates of the object",
	 (void *)VEC_SUBTYPE_OB_LOC},
	{"dloc",
	 (getter)getVecAttr, (setter)setVecAttr,
	 "The delta (X,Y,Z) location coordinates of the object",
	 (void *)VEC_SUBTYPE_OB_DLOC},
	{"scale",
	 (getter)getVecAttr, (setter)setVecAttr,
	 "The (X,Y,Z) scale of the object",
	 (void *)VEC_SUBTYPE_OB_SCALE},
	{"dscale",
	 (getter)getVecAttr, (setter)setVecAttr,
	 "The delta (X,Y,Z) scale of the object",
	 (void *)VEC_SUBTYPE_OB_DSCALE},
	 
	{"rot",
	 (getter)getEulerAttr, (setter)setEulerAttr,
	 "The (X,Y,Z) rotation angles (in degrees) of the object",
	 (void *)EXPP_OBJ_ATTR_ROT},
	{"drot",
	 (getter)getEulerAttr, (setter)setEulerAttr,
	 "The delta (X,Y,Z) rotation angles (in radians) of the object",
	 (void *)EXPP_OBJ_ATTR_DROT},

	{"layerMask",
	 (getter)getIntAttr, (setter)Object_setLayersMask,
	 "The object layers (bitfield)",
	 (void *)EXPP_OBJ_ATTR_LAYERMASK},
	{"ipo",
	 (getter)Object_getIpo, (setter)Object_setIpo,
	 "Object's Ipo data",
	 NULL},
	{"colbits",
	 (getter)getIntAttr, (setter)setIntAttrRange,
	 "The Material usage bitfield",
	 (void *)EXPP_OBJ_ATTR_COLBITS},
	{"drawMode",
	 (getter)getIntAttr, (setter)Object_setDrawMode,
	 "The object's drawing mode bitfield",
	 (void *)EXPP_OBJ_ATTR_DRAWMODE},
	{"drawType",
	 (getter)getConstAttr, (setter)setConstAttr,
	 "The object's drawing type",
	 (void *)EXPP_OBJ_ATTR_DRAWTYPE},
	{"parentType",
	 (getter)getConstAttr, (setter)NULL,
	 "The object's parent type",
	 (void *)EXPP_OBJ_ATTR_PARENT_TYPE},
	{"DupOn",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "DupOn setting (for DupliFrames)",
	 (void *)EXPP_OBJ_ATTR_DUPON},
	{"DupOff",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "DupOff setting (for DupliFrames)",
	 (void *)EXPP_OBJ_ATTR_DUPOFF},
	{"DupSta",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Starting frame (for DupliFrames)",
	 (void *)EXPP_OBJ_ATTR_DUPSTA},
	{"DupEnd",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Ending frame (for DupliFrames)",
	 (void *)EXPP_OBJ_ATTR_DUPEND},
	{"passIndex",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Index for object masks in the compositor",
	 (void *)EXPP_OBJ_ATTR_PASSINDEX},
	{"activeMaterial",
	 (getter)getIntAttr, (setter)setIntAttrClamp,
	 "Index for the active material (displayed in the material panel)",
	 (void *)EXPP_OBJ_ATTR_ACT_MATERIAL},
	{"matrixWorld",
	 (getter)Object_getMatrixWorld, (setter)NULL,
	 "worldspace matrix: absolute, takes vertex parents, tracking and Ipos into account",
	 NULL},
	{"matrixLocal",
	 (getter)Object_getMatrixLocal, (setter)Object_setMatrix,
	 "localspace matrix: relative to the object's parent",
	 NULL},
	{"matrixParentInverse",
	 (getter)Object_getMatrixParentInverse, (setter)NULL,
	 "parents inverse matrix: parents localspace inverted matrix",
	 NULL},
	{"data",
	 (getter)Object_getData, (setter)Object_setData,
	 "The Datablock object linked to this object",
	 NULL},
	{"dataname",
	 (getter)get_obj_dataname, (setter)NULL,
	 "The name of the datablock object linked to this object",
	 NULL},
	{"sel",
	 (getter)Object_getSelected, (setter)Object_setSelect,
	 "The object's selection state",
	 NULL},
	{"parent",
	 (getter)Object_getParent, (setter)NULL,
	 "The object's parent object (if parented)",
	 NULL},
	{"parentbonename",
	 (getter)Object_getParentBoneName, (setter)Object_setParentBoneName,
	 "The object's parent object's sub name",
	 NULL},
	{"track",
	 (getter)Object_getTracked, (setter)Object_setTracked,
	 "The object's tracked object",
	 NULL},
	{"timeOffset",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "The time offset of the object's animation",
	 (void *)EXPP_OBJ_ATTR_TIMEOFFSET},
	{"type",
	 (getter)Object_getType, (setter)NULL,
	 "The object's type",
	 NULL},
	{"boundingBox",
	 (getter)Object_getBoundBox, (setter)NULL,
	 "The bounding box of this object",
	 NULL},
	{"action",
	 (getter)Object_getAction, (setter)Object_setAction,
	 "The action associated with this object (if defined)",
	 NULL},
	{"game_properties",
	 (getter)Object_getAllProperties, (setter)NULL,
	 "The object's properties",
	 NULL},
	 
	{"piFalloff",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "The particle interaction falloff power",
	 (void *)EXPP_OBJ_ATTR_PI_FALLOFF},
	{"piMaxDist",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Max distance for the particle interaction field to work",
	 (void *)EXPP_OBJ_ATTR_PI_MAXDIST},
	{"piPermeability",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Probability that a particle will pass through the mesh",
	 (void *)EXPP_OBJ_ATTR_PI_PERM},
	{"piRandomDamp",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Random variation of particle interaction damping",
	 (void *)EXPP_OBJ_ATTR_PI_RANDOMDAMP},
	{"piStrength",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Particle interaction force field strength",
	 (void *)EXPP_OBJ_ATTR_PI_STRENGTH},
	{"piSurfaceDamp",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Amount of damping during particle collision",
	 (void *)EXPP_OBJ_ATTR_PI_SURFACEDAMP},
	{"piSoftbodyDamp",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Damping factor for softbody deflection",
	 (void *)EXPP_OBJ_ATTR_PI_SBDAMP},
	{"piSoftbodyIThick",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Inner face thickness for softbody deflection",
	 (void *)EXPP_OBJ_ATTR_PI_SBIFACETHICK},
	{"piSoftbodyOThick",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	 "Outer face thickness for softbody deflection",
	 (void *)EXPP_OBJ_ATTR_PI_SBOFACETHICK},

	{"piDeflection",
	 (getter)Object_getPIDeflection, (setter)Object_setPIDeflection,
	 "Deflects particles based on collision",
	 NULL},
	{"piType",
	 (getter)getConstAttr, (setter)Object_setPIType,
	 "Type of particle interaction (force field, wind, etc)",
	 (void *)EXPP_OBJ_ATTR_PI_TYPE},
	{"piUseMaxDist",
	 (getter)Object_getPIUseMaxDist, (setter)Object_setPIUseMaxDist,
	 "Use a maximum distance for the field to work",
	 NULL},

	{"axis",
	 (getter)Object_getDrawModeBits, (setter)Object_setDrawModeBits,
	 "Display of active object's center and axis enabled",
	 (void *)OB_AXIS},
	{"texSpace",
	 (getter)Object_getDrawModeBits, (setter)Object_setDrawModeBits,
	 "Display of active object's texture space enabled",
	 (void *)OB_TEXSPACE},
	{"nameMode",
	 (getter)Object_getDrawModeBits, (setter)Object_setDrawModeBits,
	 "Display of active object's name enabled",
	 (void *)OB_DRAWNAME},
	{"wireMode",
	 (getter)Object_getDrawModeBits, (setter)Object_setDrawModeBits,
	 "Add the active object's wireframe over solid drawing enabled",
	 (void *)OB_DRAWWIRE},
	{"xRay",
	 (getter)Object_getDrawModeBits, (setter)Object_setDrawModeBits,
	 "Draw the active object in front of others enabled",
	 (void *)OB_DRAWXRAY},
	{"transp",
	 (getter)Object_getDrawModeBits, (setter)Object_setDrawModeBits,
	 "Transparent materials for the active object (mesh only) enabled",
	 (void *)OB_DRAWTRANSP},

	{"enableNLAOverride",
	 (getter)Object_getNLAflagBits, (setter)Object_setNLAflagBits,
	 "Toggles Action-NLA based animation",
	 (void *)OB_NLA_OVERRIDE},

	{"enableDupVerts",
	 (getter)Object_getTransflagBits, (setter)Object_setTransflagBits,
	 "Duplicate child objects on all vertices",
	 (void *)OB_DUPLIVERTS},
	{"enableDupFaces",
	 (getter)Object_getTransflagBits, (setter)Object_setTransflagBits,
	 "Duplicate child objects on all faces",
	 (void *)OB_DUPLIFACES},
	{"enableDupFacesScale",
	 (getter)Object_getTransflagBits, (setter)Object_setTransflagBits,
	 "Use face scale to scale all dupliFaces",
	 (void *)OB_DUPLIFACES_SCALE},
	{"dupFacesScaleFac",
 	 (getter)getFloatAttr, (setter)setFloatAttrClamp,
	"Use face scale to scale all dupliFaces",
	 (void *)EXPP_OBJ_ATTR_DUPFACESCALEFAC},
	{"enableDupFrames",
	 (getter)Object_getTransflagBits, (setter)Object_setTransflagBits,
	 "Make copy of object for every frame",
	 (void *)OB_DUPLIFRAMES},
	{"enableDupGroup",
	 (getter)Object_getTransflagBits, (setter)Object_setTransflagBits,
	 "Enable group instancing",
	 (void *)OB_DUPLIGROUP},
	{"enableDupRot",
	 (getter)Object_getTransflagBits, (setter)Object_setTransflagBits,
	 "Rotate dupli according to vertex normal",
	 (void *)OB_DUPLIROT},
	{"enableDupNoSpeed",
	 (getter)Object_getTransflagBits, (setter)Object_setTransflagBits,
	 "Set dupliframes to still, regardless of frame",
	 (void *)OB_DUPLINOSPEED},
	{"DupObjects",
	 (getter)Object_getDupliObjects, (setter)NULL,
	 "Get a list of tuple pairs (object, matrix), for getting dupli objects",
	 NULL},
	{"DupGroup",
	 (getter)Object_getDupliGroup, (setter)Object_setDupliGroup,
	 "Get a list of tuples for object duplicated by dupliframe",
	 NULL},

	{"effects",
	 (getter)Object_getEffects, (setter)NULL, 
	 "The list of particle effects associated with the object",
	 NULL},
	{"actionStrips",
	 (getter)Object_getActionStrips, (setter)NULL, 
	 "The action strips associated with the object",
	 NULL},
	{"constraints",
	 (getter)Object_getConstraints, (setter)NULL, 
	 "The constraints associated with the object",
	 NULL},
	{"modifiers",
	 (getter)Object_getModifiers, (setter)Object_setModifiers, 
	 "The modifiers associated with the object",
	 NULL},
	{"protectFlags",
	 (getter)Object_getProtectFlags, (setter)Object_setProtectFlags, 
	 "The \"transform locking\" bitfield for the object",
	 NULL},
	{"drawSize",
	 (getter)getFloatAttr, (setter)setFloatAttrClamp, 
	 "The size to display the Empty",
	 (void *)EXPP_OBJ_ATTR_DRAWSIZE},

	{"rbFlags",
	 (getter)Object_getRBFlags, (setter)Object_setRBFlags, 
	 "Rigid body flags",
	 NULL},
	{"rbMass",
	 (getter)Object_getRBMass, (setter)Object_setRBMass, 
	 "Rigid body object mass",
	 NULL},
	{"rbRadius",
	 (getter)Object_getRBRadius, (setter)Object_setRBRadius, 
	 "Rigid body bounding sphere size",
	 NULL},
	{"rbShapeBoundType",
	 (getter)getConstAttr, (setter)setConstAttr, 
	 "Rigid body physics bounds object type",
	 (void *)EXPP_OBJ_ATTR_RB_SHAPEBOUND},
	{"rbHalfExtents",
	 (getter)Object_getRBHalfExtents, (setter)NULL, 
	 "Rigid body physics bounds object type",
	 NULL},

	{"restrictDisplay",
	 (getter)Object_getRestricted, (setter)Object_setRestricted, 
	 "Toggle object restrictions",
	 (void *)OB_RESTRICT_VIEW},
	{"restrictSelect",
	 (getter)Object_getRestricted, (setter)Object_setRestricted, 
	 "Toggle object restrictions",
	 (void *)OB_RESTRICT_SELECT},
	{"restrictRender",
	 (getter)Object_getRestricted, (setter)Object_setRestricted, 
	 "Toggle object restrictions",
	 (void *)OB_RESTRICT_RENDER},
	 
	{NULL}  /* Sentinel */
};


/* this types constructor */
static PyObject *Object_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	return Scene_object_new(G.scene, args);
}

/*****************************************************************************/
/* Python BPyObject_Type structure definition:                                  */
/*****************************************************************************/
PyTypeObject BPyObject_Type = {
	PyObject_HEAD_INIT( NULL )  /* required py macro */
	0,                          /* ob_size */
	/*  For printing, in format "<module>.<name>" */
	"Blender Object",           /* char *tp_name; */
	sizeof( BPyObject ),       /* int tp_basicsize; */
	0,                          /* tp_itemsize;  For allocation */

	/* Methods to implement standard operations */

	( destructor ) Object_dealloc,/* destructor tp_dealloc; */
	NULL,                       /* printfunc tp_print; */
	NULL,                       /* getattrfunc tp_getattr; */
	NULL,                       /* setattrfunc tp_setattr; */
	NULL,						/* cmpfunc tp_compare; */
	NULL,						/* reprfunc tp_repr; */

	/* Method suites for standard classes */

	NULL,                       /* PyNumberMethods *tp_as_number; */
	NULL,                       /* PySequenceMethods *tp_as_sequence; */
	NULL,                       /* PyMappingMethods *tp_as_mapping; */

	/* More standard operations (here for binary compatibility) */

	NULL,						/* hashfunc tp_hash; */
	NULL,                       /* ternaryfunc tp_call; */
	NULL,                       /* reprfunc tp_str; */
	NULL,                       /* getattrofunc tp_getattro; */
	NULL,                       /* setattrofunc tp_setattro; */

	/* Functions to access object as input/output buffer */
	NULL,                       /* PyBufferProcs *tp_as_buffer; */

  /*** Flags to define presence of optional/expanded features ***/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,	/* long tp_flags; */

	NULL,                       /*  char *tp_doc;  Documentation string */
  /*** Assigned meaning in release 2.0 ***/
	/* call function for all accessible objects */
	NULL,                       /* traverseproc tp_traverse; */

	/* delete references to contained objects */
	NULL,                       /* inquiry tp_clear; */

  /***  Assigned meaning in release 2.1 ***/
  /*** rich comparisons ***/
	NULL,                       /* richcmpfunc tp_richcompare; */

  /***  weak reference enabler ***/
	0,                          /* long tp_weaklistoffset; */

  /*** Added in release 2.2 ***/
	/*   Iterators */
	NULL,                       /* getiterfunc tp_iter; */
	NULL,                       /* iternextfunc tp_iternext; */

  /*** Attribute descriptor and subclassing stuff ***/
	BPyObject_methods,         /* struct PyMethodDef *tp_methods; */
	NULL,                       /* struct PyMemberDef *tp_members; */
	BPyObject_getset,       /* struct PyGetSetDef *tp_getset; */
	&BPyGenericLib_Type,                       /* struct _typeobject *tp_base; */
	NULL,                       /* PyObject *tp_dict; */
	NULL,                       /* descrgetfunc tp_descr_get; */
	NULL,                       /* descrsetfunc tp_descr_set; */
	0,                          /* long tp_dictoffset; */
	NULL,                       /* initproc tp_init; */
	NULL,                       /* allocfunc tp_alloc; */
	( newfunc ) Object_new,		/* newfunc tp_new; */
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

/*****************************************************************************/
/* ObjectType_Init(): add constants to the type at run-rime and initialize   */
/*****************************************************************************/
PyObject *ObjectType_Init( void )
{
	/* only allocate new dictionary once */
	if( BPyObject_Type.tp_dict == NULL ) {
		BPyObject_Type.tp_dict = PyDict_New();
		PyConstCategory_AddObjectToDict( BPyObject_Type.tp_dict, &drawTypes );
		PyConstCategory_AddObjectToDict( BPyObject_Type.tp_dict, &parentTypes );
		PyConstCategory_AddObjectToDict( BPyObject_Type.tp_dict, &PITypes );
		PyConstCategory_AddObjectToDict( BPyObject_Type.tp_dict, &RBShapes );
		PyConstCategory_AddObjectToDict( BPyObject_Type.tp_dict, &IpoKeyTypes );
		PyType_Ready( &BPyObject_Type ) ;
	}
	return (PyObject *) &BPyObject_Type ;
}

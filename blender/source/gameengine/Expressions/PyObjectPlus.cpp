/**
 * $Id$
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

#ifndef NO_EXP_PYTHON_EMBEDDING

/*------------------------------
 * PyObjectPlus cpp
 *
 * C++ library routines for Crawl 3.2
 *
 * Derived from work by
 * David Redish
 * graduate student
 * Computer Science Department 
 * Carnegie Mellon University (CMU)
 * Center for the Neural Basis of Cognition (CNBC) 
 * http://www.python.org/doc/PyCPP.html
 *
------------------------------*/
#include <assert.h>
#include "stdlib.h"
#include "PyObjectPlus.h"
#include "STR_String.h"
/*------------------------------
 * PyObjectPlus Type		-- Every class, even the abstract one should have a Type
------------------------------*/

PyTypeObject PyObjectPlus::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,				/*ob_size*/
	"PyObjectPlus",			/*tp_name*/
	sizeof(PyObjectPlus),		/*tp_basicsize*/
	0,				/*tp_itemsize*/
	/* methods */
	PyDestructor,	  		/*tp_dealloc*/
	0,			 	/*tp_print*/
	__getattr, 			/*tp_getattr*/
	__setattr, 			/*tp_setattr*/
	0,			        /*tp_compare*/
	__repr,			        /*tp_repr*/
	0,			        /*tp_as_number*/
	0,		 	        /*tp_as_sequence*/
	0,			        /*tp_as_mapping*/
	0,			        /*tp_hash*/
	0,				/*tp_call */
};

PyObjectPlus::PyObjectPlus(PyTypeObject *T) 				// constructor
{
	assert(T != NULL);
	this->ob_type = T; 
	_Py_NewReference(this);
};
  
/*------------------------------
 * PyObjectPlus Methods 	-- Every class, even the abstract one should have a Methods
------------------------------*/
PyMethodDef PyObjectPlus::Methods[] = {
  {"isA",		 (PyCFunction) sPy_isA,			Py_NEWARGS},
  {NULL, NULL}		/* Sentinel */
};

/*------------------------------
 * PyObjectPlus Parents		-- Every class, even the abstract one should have parents
------------------------------*/
PyParentObject PyObjectPlus::Parents[] = {&PyObjectPlus::Type, NULL};

/*------------------------------
 * PyObjectPlus attributes	-- attributes
------------------------------*/
PyObject *PyObjectPlus::_getattr(char *attr)
{
  //if (streq(attr, "type"))
  //  return Py_BuildValue("s", (*(GetParents()))->tp_name);

  return Py_FindMethod(Methods, this, attr);    
}

int PyObjectPlus::_setattr(char *attr, PyObject *value)
{
	//return PyObject::_setattr(attr,value);
	//cerr << "Unknown attribute" << endl;
  return 1;
}

/*------------------------------
 * PyObjectPlus repr		-- representations
------------------------------*/
PyObject *PyObjectPlus::_repr(void)
{
  Py_Error(PyExc_SystemError, "Representation not overridden by object.");  
}

/*------------------------------
 * PyObjectPlus isA		-- the isA functions
------------------------------*/
bool PyObjectPlus::isA(PyTypeObject *T)		// if called with a Type, use "typename"
{
  return isA(T->tp_name);
}


bool PyObjectPlus::isA(const char *mytypename)		// check typename of each parent
{
  int i;
  PyParentObject  P;
  PyParentObject *Ps = GetParents();
  
  for (P = Ps[i=0]; P != NULL; P = Ps[i++])
  {
      if (STR_String(P->tp_name) == STR_String(mytypename)	)
		  return true;
  }
	
  return false;
}

PyObject *PyObjectPlus::Py_isA(PyObject *args)		// Python wrapper for isA
{
  char *mytypename;
  Py_Try(PyArg_ParseTuple(args, "s", &mytypename));
  if(isA(mytypename))
    {Py_INCREF(Py_True); return Py_True;}
  else
    {Py_INCREF(Py_False); return Py_False;};
}

#endif //NO_EXP_PYTHON_EMBEDDING


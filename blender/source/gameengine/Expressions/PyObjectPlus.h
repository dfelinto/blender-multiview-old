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
#ifndef NO_EXP_PYTHON_EMBEDDING

#ifndef _adr_py_lib_h_				// only process once,
#define _adr_py_lib_h_				// even if multiply included

#ifndef __cplusplus				// c++ only
#error Must be compiled with C++
#endif

#include "KX_Python.h"

/*------------------------------
 * Python defines
------------------------------*/

								// some basic python macros
#define Py_NEWARGS 1			
#define Py_Return Py_INCREF(Py_None); return Py_None;	

#define Py_Error(E, M)   {PyErr_SetString(E, M); return NULL;}
#define Py_Try(F) {if (!(F)) return NULL;}
#define Py_Assert(A,E,M) {if (!(A)) {PyErr_SetString(E, M); return NULL;}}

inline void Py_Fatal(char *M) {
	//cout << M << endl; 
	exit(-1);
};

								// This must be the first line of each 
								// PyC++ class
#define Py_Header \
 public: \
  static PyTypeObject   Type; \
  static PyMethodDef    Methods[]; \
  static PyParentObject Parents[]; \
  virtual PyTypeObject *GetType(void) {return &Type;}; \
  virtual PyParentObject *GetParents(void) {return Parents;}

								// This defines the _getattr_up macro
								// which allows attribute and method calls
								// to be properly passed up the hierarchy.
#define _getattr_up(Parent) \
  PyObject *rvalue = Py_FindMethod(Methods, this, attr); \
  if (rvalue == NULL) \
    { \
      PyErr_Clear(); \
      return Parent::_getattr(attr); \
    } \
  else \
    return rvalue 


/*------------------------------
 * PyObjectPlus
------------------------------*/
typedef PyTypeObject * PyParentObject;				// Define the PyParent Object

class PyObjectPlus : public PyObject {				// The PyObjectPlus abstract class

  Py_Header;							// Always start with Py_Header

 public:  
  PyObjectPlus(PyTypeObject *T);
  
  virtual ~PyObjectPlus() {};					// destructor
  static void PyDestructor(PyObject *P)				// python wrapper
  {  
	  delete ((PyObjectPlus *) P);  
  };

  //void INCREF(void) {
//	  Py_INCREF(this);
//  };				// incref method
  //void DECREF(void) {
//	  Py_DECREF(this);
//  };				// decref method

  virtual PyObject *_getattr(char *attr);			// _getattr method
  static  PyObject *__getattr(PyObject * PyObj, char *attr) 	// This should be the entry in Type. 
    { return ((PyObjectPlus*) PyObj)->_getattr(attr); };
   
  virtual int _setattr(char *attr, PyObject *value);		// _setattr method
  static  int __setattr(PyObject *PyObj, 			// This should be the entry in Type. 
			char *attr, 
			PyObject *value)
    { return ((PyObjectPlus*) PyObj)->_setattr(attr, value);  };

  virtual PyObject *_repr(void);				// _repr method
  static  PyObject *__repr(PyObject *PyObj)			// This should be the entry in Type.
    {  return ((PyObjectPlus*) PyObj)->_repr();  };


								// isA methods
  bool isA(PyTypeObject *T);
  bool isA(const char *mytypename);
  PyObject *Py_isA(PyObject *args);
  static PyObject *sPy_isA(PyObject *self, PyObject *args, PyObject *kwd)
    {return ((PyObjectPlus*)self)->Py_isA(args);};
};

#endif //  _adr_py_lib_h_

#endif //NO_EXP_PYTHON_EMBEDDING


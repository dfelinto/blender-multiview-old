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

#include "KX_MeshProxy.h"

#include "RAS_IPolygonMaterial.h"
#include "RAS_MeshObject.h"
#include "KX_VertexProxy.h"


PyTypeObject KX_MeshProxy::Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"KX_MeshProxy",
	sizeof(KX_MeshProxy),
	0,
	PyDestructor,
	0,
	__getattr,
	__setattr,
	0, //&MyPyCompare,
	__repr,
	0, //&cvalue_as_number,
	0,
	0,
	0,
	0
};

PyParentObject KX_MeshProxy::Parents[] = {
	&KX_MeshProxy::Type,
	&SCA_IObject::Type,
	&CValue::Type,
	NULL
};

PyMethodDef KX_MeshProxy::Methods[] = {
{"getNumMaterials", (PyCFunction)KX_MeshProxy::sPyGetNumMaterials,METH_VARARGS},
{"getMaterialName", (PyCFunction)KX_MeshProxy::sPyGetMaterialName,METH_VARARGS},
{"getTextureName", (PyCFunction)KX_MeshProxy::sPyGetTextureName,METH_VARARGS},
{"getVertexArrayLength", (PyCFunction)KX_MeshProxy::sPyGetVertexArrayLength,METH_VARARGS},
{"getVertex", (PyCFunction)KX_MeshProxy::sPyGetVertex,METH_VARARGS},
//{"getIndexArrayLength", (PyCFunction)KX_MeshProxy::sPyGetIndexArrayLength,METH_VARARGS},

  {NULL,NULL} //Sentinel
};

PyObject*
KX_MeshProxy::_getattr(char* attr)
{
  _getattr_up(SCA_IObject);
}



KX_MeshProxy::KX_MeshProxy(RAS_MeshObject* mesh)
	: m_meshobj(mesh)
{
	
}

KX_MeshProxy::~KX_MeshProxy()
{
	
}



// stuff for cvalue related things
CValue*		KX_MeshProxy::Calc(VALUE_OPERATOR op, CValue *val) { return NULL;}
CValue*		KX_MeshProxy::CalcFinal(VALUE_DATA_TYPE dtype, VALUE_OPERATOR op, CValue *val) { return NULL;}	

const STR_String &	KX_MeshProxy::GetText() {return m_meshobj->GetName();};
float		KX_MeshProxy::GetNumber() { return -1;}
STR_String	KX_MeshProxy::GetName() { return m_meshobj->GetName();}
void		KX_MeshProxy::SetName(STR_String name) { };
CValue*		KX_MeshProxy::GetReplica() { return NULL;}
void		KX_MeshProxy::ReplicaSetName(STR_String name) {};


// stuff for python integration
	
PyObject* KX_MeshProxy::PyGetNumMaterials(PyObject* self, 
			       PyObject* args, 
			       PyObject* kwds)
{
	int num = m_meshobj->NumMaterials();
	return PyInt_FromLong(num);
}

PyObject* KX_MeshProxy::PyGetMaterialName(PyObject* self, 
			       PyObject* args, 
			       PyObject* kwds)
{
    int matid= 1;
	STR_String matname;

	if (PyArg_ParseTuple(args,"i",&matid))
	{
		matname = m_meshobj->GetMaterialName(matid);
	}

	return PyString_FromString(matname.Ptr());
		
}
	

PyObject* KX_MeshProxy::PyGetTextureName(PyObject* self, 
			       PyObject* args, 
			       PyObject* kwds)
{
    int matid= 1;
	STR_String matname;

	if (PyArg_ParseTuple(args,"i",&matid))
	{
		matname = m_meshobj->GetTextureName(matid);
	}

	return PyString_FromString(matname.Ptr());
		
}

PyObject* KX_MeshProxy::PyGetVertexArrayLength(PyObject* self, 
			       PyObject* args, 
			       PyObject* kwds)
{
    int matid= -1;
	int length = -1;

	
	if (PyArg_ParseTuple(args,"i",&matid))
	{
		RAS_IPolyMaterial* mat = m_meshobj->GetMaterialBucket(matid)->GetPolyMaterial();
		if (mat)
		{
			length = m_meshobj->GetVertexArrayLength(mat);
		}
	}

	return PyInt_FromLong(length);
		
}


PyObject* KX_MeshProxy::PyGetVertex(PyObject* self, 
			       PyObject* args, 
			       PyObject* kwds)
{
    int vertexindex= 1;
	int matindex= 1;
	PyObject* vertexob = NULL;

	if (PyArg_ParseTuple(args,"ii",&matindex,&vertexindex))
	{
		RAS_TexVert* vertex = m_meshobj->GetVertex(matindex,vertexindex);
		if (vertex)
		{
			vertexob = new KX_VertexProxy(vertex);
		}
	}

	return vertexob;
		
}

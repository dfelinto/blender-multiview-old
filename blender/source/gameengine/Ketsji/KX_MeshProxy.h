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
#ifndef __KX_MESHPROXY
#define __KX_MESHPROXY

#include "SCA_IObject.h"

class KX_MeshProxy	: public SCA_IObject
{
	Py_Header;

	class RAS_MeshObject*	m_meshobj;
public:
	KX_MeshProxy(class RAS_MeshObject* mesh);
	virtual ~KX_MeshProxy();

	// stuff for cvalue related things
	virtual CValue*		Calc(VALUE_OPERATOR op, CValue *val) ;
	virtual CValue*		CalcFinal(VALUE_DATA_TYPE dtype, VALUE_OPERATOR op, CValue *val);
	virtual const STR_String &	GetText();
	virtual float		GetNumber();
	virtual STR_String	GetName();
	virtual void		SetName(STR_String name);								// Set the name of the value
	virtual void		ReplicaSetName(STR_String name);
	virtual CValue*		GetReplica();

// stuff for python integration
	virtual PyObject*  _getattr(char *attr);
	KX_PYMETHOD(KX_MeshProxy,GetNumMaterials);
	KX_PYMETHOD(KX_MeshProxy,GetMaterialName);
	KX_PYMETHOD(KX_MeshProxy,GetTextureName);
	
	// both take materialid (int)
	KX_PYMETHOD(KX_MeshProxy,GetVertexArrayLength);
	KX_PYMETHOD(KX_MeshProxy,GetVertex);
};
#endif //__KX_MESHPROXY

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
#ifndef __KX_VERTEXPROXY
#define __KX_VERTEXPROXY

#include "SCA_IObject.h"

class KX_VertexProxy	: public SCA_IObject
{
	Py_Header;

	class RAS_TexVert*	m_vertex;
public:
	KX_VertexProxy(class RAS_TexVert* vertex);
	virtual ~KX_VertexProxy();

	// stuff for cvalue related things
	CValue*		Calc(VALUE_OPERATOR op, CValue *val) ;
	CValue*		CalcFinal(VALUE_DATA_TYPE dtype, VALUE_OPERATOR op, CValue *val);
	const STR_String &	GetText();
	float		GetNumber();
	STR_String	GetName();
	void		SetName(STR_String name);								// Set the name of the value
	void		ReplicaSetName(STR_String name);
	CValue*		GetReplica();


// stuff for python integration
	virtual PyObject*  _getattr(char *attr);

	KX_PYMETHOD(KX_VertexProxy,GetXYZ);
	KX_PYMETHOD(KX_VertexProxy,SetXYZ);
	KX_PYMETHOD(KX_VertexProxy,GetUV);
	KX_PYMETHOD(KX_VertexProxy,SetUV);
	KX_PYMETHOD(KX_VertexProxy,GetRGBA);
	KX_PYMETHOD(KX_VertexProxy,SetRGBA);
	KX_PYMETHOD(KX_VertexProxy,GetNormal);
	KX_PYMETHOD(KX_VertexProxy,SetNormal);

};
#endif //__KX_VERTEXPROXY

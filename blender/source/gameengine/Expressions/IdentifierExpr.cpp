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

#include "IdentifierExpr.h"


CIdentifierExpr::CIdentifierExpr(const STR_String& identifier,CValue* id_context)
:m_identifier(identifier)
{
	if (id_context)
		m_idContext = id_context->AddRef();
	else
		m_idContext=NULL;
}


CIdentifierExpr::~CIdentifierExpr()
{
	if (m_idContext)
		m_idContext->Release();
}



CValue*	CIdentifierExpr::Calculate()
{
	CValue* result = NULL;
	if (m_idContext)
		result = m_idContext->FindIdentifier(m_identifier);

	return result;
}



bool CIdentifierExpr::MergeExpression(CExpression* otherexpr)
{
	return false;
}



unsigned char CIdentifierExpr::GetExpressionID()
{
	return CIDENTIFIEREXPRESSIONID;
}



bool CIdentifierExpr::NeedsRecalculated()
{
	return true;
}



CExpression* CIdentifierExpr::CheckLink(std::vector<CBrokenLinkInfo*>& brokenlinks)
{
	assertd(false); // not implemented yet
	return NULL;
}



void CIdentifierExpr::ClearModified() 
{
	assertd(false); // not implemented yet
}



void CIdentifierExpr::BroadcastOperators(VALUE_OPERATOR op)
{
	assertd(false); // not implemented yet
}
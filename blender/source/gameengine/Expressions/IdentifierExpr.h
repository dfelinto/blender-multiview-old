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
#ifndef __IDENTIFIER_EXPR
#define __IDENTIFIER_EXPR

#include "Expression.h"

class CIdentifierExpr : public CExpression
{
	CValue*		m_idContext;
	STR_String	m_identifier;
public:
	CIdentifierExpr(const STR_String& identifier,CValue* id_context);
	virtual ~CIdentifierExpr();

	virtual CValue*			Calculate();
	virtual bool			MergeExpression(CExpression* otherexpr);
	virtual unsigned char	GetExpressionID();
	virtual bool			NeedsRecalculated();
	virtual CExpression*	CheckLink(std::vector<CBrokenLinkInfo*>& brokenlinks);
	virtual void			ClearModified();
	virtual void			BroadcastOperators(VALUE_OPERATOR op);
};

#endif //__IDENTIFIER_EXPR


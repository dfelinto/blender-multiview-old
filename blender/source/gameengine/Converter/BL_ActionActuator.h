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

#ifndef BL_ACTIONACTUATOR
#define BL_ACTIONACTUATOR

#include "SCA_IActuator.h"
#include "MT_Point3.h"

class BL_ActionActuator : public SCA_IActuator  
{
public:
	Py_Header;
	BL_ActionActuator(SCA_IObject* gameobj,
						const STR_String& propname,
						float starttime,
						float endtime,
						struct bAction *action,
						short	playtype,
						short	blendin,
						short	priority,
						float	stride,
						PyTypeObject* T=&Type) 
		: SCA_IActuator(gameobj,T),
		m_starttime (starttime),
		m_endtime(endtime) ,
		m_localtime(starttime),
		m_lastUpdate(-1),
		m_propname(propname), 
		m_action(action),
		m_playtype(playtype),
		m_flag(0),
		m_blendin(blendin),
		m_blendframe(0),
		m_pose(NULL),
		m_userpose(NULL),
		m_blendpose(NULL),
		m_priority(priority),
		m_stridelength(stride),
		m_lastpos(0, 0, 0)
	{
	};
	virtual ~BL_ActionActuator();
	virtual	bool Update(double curtime,double deltatime);
	CValue* GetReplica();
	void ProcessReplica();

	KX_PYMETHOD_DOC(BL_ActionActuator,SetAction);
	KX_PYMETHOD_DOC(BL_ActionActuator,SetBlendin);
	KX_PYMETHOD_DOC(BL_ActionActuator,SetPriority);
	KX_PYMETHOD_DOC(BL_ActionActuator,SetStart);
	KX_PYMETHOD_DOC(BL_ActionActuator,SetEnd);
	KX_PYMETHOD_DOC(BL_ActionActuator,SetFrame);
	KX_PYMETHOD_DOC(BL_ActionActuator,SetProperty);
	KX_PYMETHOD_DOC(BL_ActionActuator,SetBlendtime);
	KX_PYMETHOD_DOC(BL_ActionActuator,SetChannel);

	KX_PYMETHOD_DOC(BL_ActionActuator,GetAction);
	KX_PYMETHOD_DOC(BL_ActionActuator,GetBlendin);
	KX_PYMETHOD_DOC(BL_ActionActuator,GetPriority);
	KX_PYMETHOD_DOC(BL_ActionActuator,GetStart);
	KX_PYMETHOD_DOC(BL_ActionActuator,GetEnd);
	KX_PYMETHOD_DOC(BL_ActionActuator,GetFrame);
	KX_PYMETHOD_DOC(BL_ActionActuator,GetProperty);
//	KX_PYMETHOD(BL_ActionActuator,GetChannel);


	virtual PyObject* _getattr(char* attr);
	void SetBlendTime (float newtime);

protected:
	float	m_blendframe;
	MT_Point3	m_lastpos;
	int		m_flag;
	float	m_starttime;
	float	m_endtime;
	float	m_localtime;
	float	m_lastUpdate;
	short	m_playtype;
	float	m_blendin;
	short	m_priority;
	float	m_stridelength;
	struct bPose* m_pose;
	struct bPose* m_blendpose;
	struct bPose* m_userpose;
	STR_String	m_propname;
	struct bAction *m_action;
	
};

enum {
	ACT_FLAG_REVERSE	= 0x00000001,
	ACT_FLAG_LOCKINPUT	= 0x00000002,
	ACT_FLAG_KEYUP		= 0x00000004
};
#endif


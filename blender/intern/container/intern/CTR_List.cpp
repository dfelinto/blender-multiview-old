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

#include "CTR_List.h"


CTR_Link::
CTR_Link(
) : 
	m_next(0), 
	m_prev(0) 
{
}

CTR_Link::
CTR_Link(
	CTR_Link *next,
	CTR_Link *prev
) : 
	m_next(next), 
	m_prev(prev) 
{
}

	CTR_Link *
CTR_Link::
getNext(
) const {
	return m_next; 
}

	CTR_Link *
CTR_Link::
getPrev(
) const { 
	return m_prev; 
}  

	bool 
CTR_Link::
isHead(
) const { 
	return m_prev == 0; 
}

	bool 
CTR_Link::
isTail(
) const { 
	return m_next == 0; 
}

	void 
CTR_Link::
insertBefore(
	CTR_Link *link
) {
    m_next         = link;
    m_prev         = link->m_prev;
    m_next->m_prev = this;
    m_prev->m_next = this;
} 

	void 
CTR_Link::
insertAfter(
	CTR_Link *link
) {
    m_next         = link->m_next;
    m_prev         = link;
    m_next->m_prev = this;
    m_prev->m_next = this;
} 

	void 
CTR_Link::
remove(
) { 
    m_next->m_prev = m_prev; 
    m_prev->m_next = m_next;
}


CTR_List::
CTR_List(
) : 
	m_head(&m_tail, 0), 
	m_tail(0, &m_head) 
{
}

	CTR_Link *
CTR_List::
getHead(
) const { 
	return m_head.getNext(); 
}

	CTR_Link *
CTR_List::
getTail(
) const { 
	return m_tail.getPrev();
} 

	void 
CTR_List::
addHead(
	CTR_Link *link
) { 
	link->insertAfter(&m_head); 
}

	void 
CTR_List::
addTail(
	CTR_Link *link
) { 
	link->insertBefore(&m_tail); 
}


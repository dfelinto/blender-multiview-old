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


#ifndef CTR_LIST_H
#define CTR_LIST_H

class CTR_Link {
public:
    CTR_Link(
	) ;

    CTR_Link(
		CTR_Link *next,
		CTR_Link *prev
	) ;
    
		CTR_Link *
	getNext(
	) const ;
  
		CTR_Link *
	getPrev(
	) const ;

		bool 
	isHead(
	) const ;

		bool 
	isTail(
	) const ;

		void 
	insertBefore(
		CTR_Link *link
	) ;

		void 
	insertAfter(
		CTR_Link *link
	) ;

		void 
	remove(
	) ;

private:  
    CTR_Link  *m_next;
    CTR_Link  *m_prev;
};

class CTR_List {
public:

    CTR_List(
	) ;

		CTR_Link *
	getHead(
	) const ;
 
		CTR_Link *
	getTail(
	) const ;

		void 
	addHead(
		CTR_Link *link
	) ;

		void 
	addTail(
		CTR_Link *link
	) ;
    
private:
    CTR_Link m_head;
    CTR_Link m_tail;
};

#endif




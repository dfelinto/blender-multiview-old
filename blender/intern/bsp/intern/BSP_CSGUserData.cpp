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

#include "BSP_CSGUserData.h"

 

BSP_CSGUserData::
BSP_CSGUserData(
	const int width
):
	m_width (width)
{
}

/**
 * Add a new uninitialized record to the end of the 
 * array
 */

	void
BSP_CSGUserData::
IncSize(
){
	m_data.insert(m_data.end(),m_width,char(0));
}

	int
BSP_CSGUserData::
Duplicate(
	void *record
){
	if (m_width) {
		int output = Size();
		IncSize();

		memcpy(m_data.end() - m_width,record,m_width);
	
		return output;
	}
	return 0;
}	

	void
BSP_CSGUserData::
Duplicate(
	int record_index
){
	if (m_width) {
		int output = Size();
		IncSize();
		memcpy(m_data.end() - m_width,m_data.begin() + record_index * m_width,m_width);
	}
}	


	void
BSP_CSGUserData::
Copy(
	void *output,
	int pos
){
	if (m_width) {
		memcpy(output,m_data.begin() + m_width*pos,m_width);
	}
}
	void
BSP_CSGUserData::
Reserve(
	int size
){
	m_data.reserve(size * m_width);
}


/// Return the width of an individual record

	int
BSP_CSGUserData::
Width(
) const{
	return m_width;
}


/// return the current number of records stored in the array.
	int
BSP_CSGUserData::
Size(
) const {
	if (m_width == 0) return 0;
	return m_data.size() / m_width;
} 


/// return a pointer to the start of the nth record in the array.

	void *
BSP_CSGUserData::
operator [] (
	const int pos
){
	return m_data.begin() + m_width*pos;
}


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

#ifndef BSP_CSGUserData_h

#define BSP_CSGUserData_h

#include <vector>

/**
 * This data represents a continuous block of 
 * data of unknown type. This holds the user
 * data during a BSP operation.
 */

class BSP_CSGUserData 
{
public :

	/**
	 * width defines the size in bytes of a
	 * single element (record) of user data
	 */

	BSP_CSGUserData(
		const int width
	);

	/**
	 * Reserve some space in the array
	 */
		void
	Reserve(
		int size
	);

	/**
	 * Add a new uninitialized record to the end of the 
	 * array
	 */

		void
	IncSize(
	);

	/**
	 * duplicate a recod and insert it into the end of the array
	 * returns the index of the new record. Make sure that the
	 * record does not belong to this buffer as this can cause errors.
	 */

		int
	Duplicate(
		void *
	);

		void
	Duplicate(
		int record_index
	);

	/**
	 * Copies the record at position pos in the array to the
	 * memory pointed to by output
	 */

		void
	Copy(
		void *output,
		int pos
	);	

	/// Return the width of an individual record

		int
	Width(
	) const;


	/// return the current number of records stored in the array.
		int
	Size(
	) const;


	/// return a pointer to the start of the nth record in the array.

		void *
	operator [] (
		const int pos
	);

private :

	/// Private - force use of public constructor only.

	BSP_CSGUserData(
	);


	/// The block of data.
	std::vector<char> m_data;
	/// The width of a record in this array.
	int m_width;
};


#endif

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
 * @author	Maarten Gribnau
 * @date	March 8, 2001
 */

#ifndef _H_IMG_MemPtr
#define _H_IMG_MemPtr

#include <stddef.h>

/**
 * A memory pointer for memory of any type.
 * It can be used to avoid memory leaks when allocating memory in constructors.
 * @author	Maarten Gribnau
 * @date	March 8, 2001
 */

template <class T> class IMG_MemPtr {
public:
	/** Pointer to the memory */
	T* m_p;
	bool m_owned;

	/**
	 * Size exception.
	 * A size exception is thrown when an invalid width and/or height is passed.
	 */
	class Size {};
	/**
	 * Memory exception.
	 * A size exception is thrown when a there is not enough memory to allocate the image.
	 */
	class Memory {};

	/**
	 * Constructs a memory pointer.
	 * @param	s	requested size of the pointer
	 * @throw <Size>	when an invalid width and/or height is passed.
	 * @throw <Memory>	when a there is not enough memory to allocate the image.
	 */
	IMG_MemPtr(size_t s)
		: m_p(0), m_owned(false)
	{
		if (s > 0) {
			m_p = new T[s];
			if (!m_p) {
				throw Memory();
			}
			m_owned = true;
		}
		else {
			throw Size();
		}
	}

	/**
	 * Constructs a memory pointer from a pointer.
	 * @param	p	the pointer
	 * @param	s	requested size of the pointer
	 * @throw <Size>	when an invalid width and/or height is passed.
	 */
	IMG_MemPtr(void* p, size_t s)
		: m_p(0), m_owned(false)
	{
		if (p && (s > 0)) {
			m_p = (T*)p;
		}
		else {
			throw Size();
		}
	}

	/**
	 * Destructor.
	 */
	~IMG_MemPtr() { if (m_p && m_owned) { delete [] m_p; m_p = 0; } }

	/**
	 * Access to the memory.
	 * @return	pointer to the memory
	 */
	operator T*() { return m_p; }
};

#endif // _H_IMG_MemPtr

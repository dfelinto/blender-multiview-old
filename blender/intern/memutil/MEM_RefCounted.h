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
 * Copyright (C) 2001 NaN Technologies B.V.
 * @author	Maarten Gribnau
 * @date	March 31, 2001
 */

#ifndef _H_MEM_REF_COUNTED
#define _H_MEM_REF_COUNTED


/**
 * An object with reference counting.
 * Base class for objects with reference counting.
 * When a shared object is ceated, it has reference count == 1.
 * If the the reference count of a shared object reaches zero, the object self-destructs.
 * The default destructor of this object has been made protected on purpose.
 * This disables the creation of shared objects on the stack.
 *
 * @author	Maarten Gribnau
 * @date	March 31, 2001
 */

class MEM_RefCounted {
public:
	/**
	 * Constructs a a shared object.
	 */
	MEM_RefCounted() : m_refCount(1)
	{
	}

	/** 
	 * Returns the reference count of this object.
	 * @return the reference count.
	 */
	inline virtual int getRef() const;

	/** 
	 * Increases the reference count of this object.
	 * @return the new reference count.
	 */
	inline virtual int incRef();

	/** 
	 * Decreases the reference count of this object.
	 * If the the reference count reaches zero, the object self-destructs.
	 * @return the new reference count.
	 */
	inline virtual int decRef();

protected:
	/**
	 * Destructs a shared object.
	 * The destructor is protected to force the use of incRef and decRef.
	 */
	virtual ~MEM_RefCounted()
	{
	}

protected:
	/// The reference count.
	int m_refCount;
};


inline int MEM_RefCounted::getRef() const
{
	return m_refCount;
}

inline int MEM_RefCounted::incRef()
{
	return ++m_refCount;
}

inline int MEM_RefCounted::decRef()
{
	m_refCount--;
	if (m_refCount == 0) {
		delete this;
		return 0;
	}
	return m_refCount;
}


#endif // _H_MEM_REF_COUNTED
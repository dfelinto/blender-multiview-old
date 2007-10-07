//-----------------------------------------------------------------------------
// simple bool array, boolean values set as individual bits in int array.
//-----------------------------------------------------------------------------

#ifndef _BOOLARRAY_H
#define _BOOLARRAY_H

//#include <cassert>

#include "QDRender.h"
__BEGIN_QDRENDER

class boolarray_t
{
private:
	boolarray_t(const boolarray_t&);
public:
	boolarray_t() : ba(NULL), _size(0), _numbits(0) {}
	boolarray_t(unsigned int numbits) : ba(NULL), _size(0), _numbits(0) { resize(numbits); }
	~boolarray_t() { if (ba) delete[] ba;  ba = NULL; }
	boolarray_t& operator=(const boolarray_t& ba2)
	{
		if (ba2._size != _size) {
			if (ba) delete[] ba;
			_size = ba2._size;
			_numbits = ba2._numbits;
			ba = new unsigned int[_size];
		}
		memcpy(ba, ba2.ba, _size*sizeof(unsigned int));
		return *this;
	}
	void resize(unsigned int numbits)
	{
		_numbits = numbits;
		_size = ((numbits ? numbits : 1) + 31) >> 5;
		if (ba) delete[] ba;
		ba = new unsigned int[_size];
		memset(ba, 0, _size*sizeof(unsigned int));
	}
	bool operator[](unsigned int i)
	{
		const unsigned int b = 1 << (i & 31);
		i >>= 5;
		//assert(i < _size);
		return (ba[i] & b);
	}
	void operator()(unsigned int i, bool bl)
	{
		const unsigned int b = 1 << (i & 31);
		i >>= 5;
		//assert(i < _size);
		if (bl)
			ba[i] |= b;
		else
			ba[i] &= ~b;
	}
	boolarray_t& operator|=(const boolarray_t& ba2)
	{
		//assert(ba2._size == _size);
		for (size_t i=0; i<_size; ++i)
			ba[i] |= ba2.ba[i];
		return *this;
	}
	boolarray_t& operator&=(const boolarray_t& ba2)
	{
		//assert(ba2._size == _size);
		for (size_t i=0; i<_size; ++i)
			ba[i] &= ba2.ba[i];
		return *this;
	}
	size_t size() const { return _numbits; }
protected:
	unsigned int *ba;
	size_t _size, _numbits;
};

__END_QDRENDER

#endif	// _BOOLARRAY_H

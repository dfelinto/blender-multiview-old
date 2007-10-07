#include "qdtl.h"

__BEGIN_QDRENDER

//------------------------------------------------------------------------------
// the actual hash function
// by Paul Hsieh, see http://www.azillionmonkeys.com/qed/hash.html

#define get16bits(d) (*((const unsigned short *)(d)))
unsigned int hashfunc(const unsigned char* data, unsigned int len)
{
	unsigned int hash = len;

	if (len == 0 || data == NULL) return 0;

	int rem = len & 3;
	len >>= 2;

	// Main loop
	for (;len > 0; len--) {
		hash  += get16bits (data);
		const unsigned int tmp    = (get16bits (data+2) << 11) ^ hash;
		hash   = (hash << 16) ^ tmp;
		data  += 2*sizeof (unsigned short);
		hash  += hash >> 11;
	}

	// Handle end cases
	switch (rem) {
		case 3: hash += get16bits (data);
		        hash ^= hash << 16;
		        hash ^= data[sizeof (unsigned short)] << 18;
		        hash += hash >> 11;
		        break;
		case 2: hash += get16bits (data);
		        hash ^= hash << 11;
		        hash += hash >> 17;
		        break;
		case 1: hash += *data;
		        hash ^= hash << 10;
		        hash += hash >> 1;
	}

	// Force "avalanching" of final 127 bits
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;
	return hash;
}
#undef get16bits

__END_QDRENDER

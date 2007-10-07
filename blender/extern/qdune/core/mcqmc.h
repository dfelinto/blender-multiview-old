//---------------------------------------------------------------------
// Monte Carlo & Quasi Monte Carlo stuff
//---------------------------------------------------------------------

#ifndef __MCQMC_H
#define __MCQMC_H

class Halton
{
public:
	// default base 2
	Halton() { setBase(2); }
	Halton(int b) { setBase(b); }
	void setBase(unsigned int base)
	{
		_base = base;
		invBase = 1.0/double(base);
		value = 0;
		// skip first, apparently better
		getNext();
	}
	~Halton() {}
	void reset() { value=0; }
	void setStart(unsigned int i)
	{
		value = 0.0;
		double f, factor;
		f = factor = invBase;
		while (i>0) {
			value += double(i % _base) * factor;
			i /= _base;
			factor *= f;
		}
	}
	float getNext()
	{
		// have to use doubles here, not enough precision otherwise...
		double r = 1.0 - value - 0.0000000001;
		if (invBase < r)
			value += invBase;
		else {
			double h = invBase, hh;
			do {
				hh = h;
				h *= invBase;
			} while (h >= r);
			value += hh + h - 1.0;
		}
		// unfortunately also need extra check to make sure it stays in bound
		// can cause exceptions otherwise when used in math functions like sqrt...
		return float((value < 0) ? 0 : ((value>1) ? 1 : value));
	}
private:
	unsigned int _base;
	double invBase, value;
};


// from 'Efficient Multidimensional Sampling' by Alexander Keller
// base 2 sequences

// van der Corput
inline float RI_vdC(unsigned int bits, unsigned int r=0)
{
		bits = ( bits << 16) | ( bits >> 16);
		bits = ((bits & 0x00ff00ff) << 8) | ((bits & 0xff00ff00) >> 8);
		bits = ((bits & 0x0f0f0f0f) << 4) | ((bits & 0xf0f0f0f0) >> 4);
		bits = ((bits & 0x33333333) << 2) | ((bits & 0xcccccccc) >> 2);
		bits = ((bits & 0x55555555) << 1) | ((bits & 0xaaaaaaaa) >> 1);
		bits ^= r;
		return (float)((double)bits / 4294967296.0);
		//double v = (double)bits / 4294967296.0;
		//return (float)((v<0)?0:((v>1)?1:v));
}

// Sobol, here renamed to RI_Sb to prevent clash with RI_S from ri.h
inline float RI_Sb(unsigned int i, unsigned int r=0)
{
	for (unsigned int v = 1<<31; i; i >>= 1, v ^= v>>1)
		if (i & 1) r ^= v;
	return (float)((double)r / (double)4294967296.0);
}

// Larcher & Pillichshammer
inline float RI_LP(unsigned int i, unsigned int r=0)
{
	for (unsigned int v = 1<<31; i; i >>= 1, v |= v>>1)
		if (i & 1) r ^= v;
	return (float)((double)r / (double)4294967296.0);
}

inline int nextPrime(int lastPrime)
{
	int newPrime = lastPrime + (lastPrime & 1) + 1;
	for (;;) {
		int dv=3;  bool ispr=true;
		while ((ispr) && (dv*dv<=newPrime)) {
			ispr = ((newPrime % dv)!=0);
			dv += 2;
		}
		if (ispr) break;
		newPrime += 2;
	}
	return newPrime;
}

#endif	//__MCQMC_H

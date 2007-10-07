#ifndef _MICROPOLYGON_H
#define _MICROPOLYGON_H

#include "qdVector.h"
#include "Color.h"

#include "QDRender.h"
__BEGIN_QDRENDER

// somewhat simplified FreelistHeap version from the paper by Berger/Zorn/McKinley:
// "Composing High-Performance Memory Allocators"
// See also the HeapLayers library.
// Does not seem to gain anything unfortunately...
// There is a slight speedup in linux and even seems to use a tiny bit less memory...
// (which is surprising, since if I understand correctly, the main linux allocator apparently does something like this already?)
// In any case, it probably means currently the bottleneck is elsewhere,
// probably the frequent moving around of data which is really the critical factor at this point.
// Also probably should alloc as block instead of per micropoly/vertex, but cannot currently
// do that since mp vertices and mp's themselves can be deleted at any time.
// Another problem with this freelist is the loss of locality, but not sure yet how much impact it has on anything really.
class FreelistHeap
{
public:
	FreelistHeap(): myFreeList(NULL) {}
	~FreelistHeap()
	{
		freeObject* ptr = myFreeList;
		while (ptr) {
			freeObject* oldptr = ptr;
			ptr = ptr->next;
			delete[] reinterpret_cast<char*>(oldptr);
		}
	}
	inline void* Alloc(size_t sz)
	{
		freeObject* ptr = myFreeList;
		if (ptr) myFreeList = myFreeList->next; else ptr = reinterpret_cast<freeObject*>(new char[sz]);
		return ptr;
	}
	inline void Free(void* ptr)
	{
		freeObject* fo = reinterpret_cast<freeObject*>(ptr);
		fo->next = myFreeList;
		myFreeList = fo;
	}
private:
	struct freeObject { freeObject* next; };
	freeObject* myFreeList;
};

class MicroPolygonVertex
{
	friend class MicroPolygon;
private:
	MicroPolygonVertex(const MicroPolygonVertex &mpv);
	MicroPolygonVertex& operator=(const MicroPolygonVertex &mpv);
public:
	// ctor
	MicroPolygonVertex() : crad(-1.f), refc(0) {}
	// dtor
	virtual ~MicroPolygonVertex() {}
	inline void* operator new(size_t sz) { return getHeap().Alloc(sz); }
	inline void operator delete(void* ptr) { getHeap().Free(ptr); }
	// data
	//Point P; // not really needed
	Point3 pp; // pp -> projected point coords (2D xy + z depth)
	float crad;   // CoC radius, initialized in hider
protected:
	int refc;	// reference count

	static FreelistHeap& getHeap()
	{
		static FreelistHeap flist;
		return flist;
	}

};

class ShadedVertex : public MicroPolygonVertex
{
public:
	ShadedVertex() {}
	virtual ~ShadedVertex() {}
	Point3 ppc;	// ppc -> pp of mpv at shutter close time (only valid when mblur used)
	Color Ci, Oi;
	// TODO some container here for userdefined outputs
};

// UnshadedVertex used for raytracing (unused TODO)
/*
class UnshadedVertex : public MicroPolygonVertex
{
public:
	UnshadedVertex() {}
	virtual ~UnshadedVertex() {}
	Vector N, dPdu, dPdv;
	//....
};
*/

class MicroPolygon
{
private:
	MicroPolygon(const MicroPolygon &mp);
	MicroPolygon& operator=(const MicroPolygon &mp);
public:
	// ctors
	MicroPolygon(MicroPolygonVertex* _v0, MicroPolygonVertex* _v1,
	             MicroPolygonVertex* _v2, MicroPolygonVertex* _v3)
	            : v0(_v0), v1(_v1), v2(_v2), v3(_v3), refc(0)
	{
		v0->refc++, v1->refc++, v2->refc++, v3->refc++;
	}
	// for RiPoints (particles) (v2, v3 don't need init, see union below, shared by radsq)
	MicroPolygon(MicroPolygonVertex* _v0, float _radsq)
	            : v0(_v0), v1(NULL), radsq(_radsq), refc(0)
	{
		v0->refc++;
	}
	//dtor
	~MicroPolygon()
	{
		if (v1 == NULL)
			delete v0;	// point vertex, never shared
		else {
			// delete vertices when no longer shared
			if (--v0->refc == 0) delete v0;
			if (--v1->refc == 0) delete v1;
			if (--v2->refc == 0) delete v2;
			if (--v3->refc == 0) delete v3;
		}
	}

	// mtds
	inline void* operator new(size_t sz) { return getHeap().Alloc(sz); }
	inline void operator delete(void* ptr) { getHeap().Free(ptr); }

	void incRefCount() { refc++; }
	int decRefCount() { return --refc; }
	int getRefCount() const { return refc; }

	// this probably looks odd, but this way a bit of memory is saved,
	// in the case of points, v1/2/3 are not used, so may as well use space to store particle squared radius.
	// v1 however is used as flag, if NULL it indicates mp is a point.
	// (there is still room to store at least another float if needed)
	MicroPolygonVertex *v0, *v1;
	union {
		struct { MicroPolygonVertex *v2, *v3; };
		float radsq;
	};
	int xmin, xmax, ymin, ymax;   // 2D bound
	float zmin, zmax;

protected:
	// reference count
	int refc;

	static FreelistHeap& getHeap()
	{
		static FreelistHeap flist;
		return flist;
	}

};

__END_QDRENDER

#endif //_MICROPOLYGON_H

//--------------------------------------------------------------------------------
// sort of the 'QuietDune Template Library' ;)
// Various implementations of arrays, linked lists, hashtables, tree's, etc..
// Originally contained even more variations of each, but now only what is
// actually used in the program. Should really each be moved to its own file.
// Was mostly created for possibly easier specialized optimization issues vs. stl.
// But didn't really get around to that, so all of these could be thrown out
// and use the stl instead. But it was useful as a learning experience anyway...
//--------------------------------------------------------------------------------

#ifndef __QDTL_H
#define __QDTL_H

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif
#include <cassert>
#include <cstring>

#include "QDRender.h"
__BEGIN_QDRENDER

// fixed size array
template<typename T>
class fsArray_t
{
public:
	fsArray_t():array(NULL), _size(0) {}
	fsArray_t(unsigned int Size):array(NULL), _size(0) { resize(Size); }
	fsArray_t(const fsArray_t &fa):array(NULL), _size(0) { copy(fa); }
	~fsArray_t() { _free(); }
	fsArray_t& operator=(const fsArray_t& fa) { copy(fa);  return *this; }
	void resize(unsigned int Size) { _free();  _size = Size;   array = new T [_size]; }
	T& operator[](unsigned int i) { return array[i]; }
	T& operator[](unsigned int i) const { return array[i]; }
	unsigned int size() const { return _size; }
	bool empty() const { return (_size == 0); }
	typedef T* iterator;
	typedef const T* const_iterator;
	iterator begin() { return array; }
	iterator end() { return array + _size; }
	const_iterator begin() const { return array; }
	const_iterator end() const { return array + _size; }
protected:
	void _free() { if (array) delete[] array;  array = NULL;  _size = _size = 0; }
	void copy(const fsArray_t& fa)
	{
		_free();  resize(fa._size);
		for (unsigned int i=0; i<_size; ++i) array[i] = fa.array[i];
	}
	T* array;
	unsigned int _size;
};

// generic growable array class, similar to STL vector, can be used as stack/list as well,
template <typename T>
class array_t
{
private:
	//array_t(const array_t& a);
	//array_t& operator=(const array_t& a);
public:
	array_t():array(NULL), curidx(0), maxidx(0) {}
	array_t(const array_t& a)
	{
		array = new T [a.maxidx];
		for (unsigned int i=0; i<a.maxidx; ++i) array[i] = a.array[i];
		curidx = a.curidx;
		maxidx = a.maxidx;
	}
	array_t(unsigned int initsize)
	{
		curidx = maxidx = initsize;
		array = new T [maxidx];
		memset(array, 0, maxidx*sizeof(T));
	}
	~array_t()
	{
		if (array) {
			delete[] array;
			array = NULL;
		}
	}
	array_t& operator=(const array_t& a)
	{
		// don't delete if enough space already
		if (array && (a.maxidx <= maxidx)) {
			for (unsigned int i=0; i<a.curidx; ++i) array[i] = a.array[i];
			curidx = a.curidx;
			return *this;
		}
		if (array) delete[] array;
		array = new T [a.maxidx];
		for (unsigned int i=0; i<a.maxidx; ++i) array[i] = a.array[i];
		curidx = a.curidx;
		maxidx = a.maxidx;
		return *this;
	}
	void clear() { curidx = 0; } // no delete, just set index back to 0
	void resize(unsigned int newsize)
	{
		// do nothing if newsize less or equal to already allocated space
		if (array && (newsize <= maxidx)) return;
		maxidx = newsize;
		T* na = new T [maxidx];
		memset(na, 0, maxidx*sizeof(T));
		if (array) {
			for (unsigned int i=0; i<curidx; ++i) na[i] = array[i];
			delete[] array;
		}
		curidx = newsize;
		array = na;
	}
	// as above, but does not copy old contents
	void resize_clear(unsigned int newsize)
	{
		// only clear if newsize less or equal to already allocated space (if any)
		if (array && (newsize <= maxidx)) { curidx = 0;  return; }
		maxidx = newsize;
		if (array) delete[] array;
		array = new T [maxidx];
		curidx = 0;
	}
	// add item to array
	void push_back(const T& item) { checkCapacity();  array[curidx++] = item; }
	/*
	// returns reference to first item
	T& front() const { return array[0]; }
	// pops first item from the array, does not do anything if array empty
	void pop_front() { if (curidx) { curidx--;  memmove(array, array+1, curidx*sizeof(T)); } }
	// returns reference to last item
	T& back() const { return array[curidx-1]; }
	// pops last item from the array, does not do anything if array empty
	void pop_back() { if (curidx) curidx--; }
	// pops and returns last item from the array, aborts if array empty
	T& pop() { assert(curidx!=0);  return array[--curidx]; }
	*/
	// array access, DOES NO BOUNDS CHECKING!!!!
	T& operator[](unsigned int i) { return array[i]; }
	T& operator[](unsigned int i) const { return array[i]; }
	// same, but does check bounds, aborts if trying to access array beyond bounds
	T& at(unsigned int i) { assert(i<curidx);  return array[i]; }
	bool empty() const { return (curidx==0); }
	unsigned int size() const { return curidx; }
	unsigned int capacity() const { return maxidx; }
	// returns true if item is contained in array
	bool contains(const T& item) const
	{
		for (unsigned int i=0; i<curidx; ++i)
			if (array[i] == item) return true;
		return false;
	}
	// returns index number of item in array, if not found, returns -1
	int index(const T& item) const
	{
		for (unsigned int i = 0; i<curidx; ++i)
			if (array[i] == item) return i;
		return -1;
	}
	// for iterator emulation
	typedef T* iterator;
	typedef const T* const_iterator;
	iterator begin() { return &array[0]; }
	iterator end() { return &array[curidx]; }
	const_iterator begin() const { return &array[0]; }
	const_iterator end() const { return &array[curidx]; }
protected:
	// check if more memory needed
	void checkCapacity()
	{
		if (curidx >= maxidx) {
			maxidx = (maxidx==0) ? 1 : (maxidx << 1);
			T* na = new T [maxidx];
			if (array) {
				for (unsigned int i=0; i<curidx; ++i) na[i] = array[i];
				delete[] array;
			}
			array = na;
		}
	}
	T* array;
	unsigned int curidx, maxidx;
};
//------------------------------------------------------------------------------

// associative linked list
template<typename keytype, typename datatype>
class alist_t
{
private:
	alist_t(const alist_t& L);
	alist_t& operator=(const alist_t& L);
public:
	alist_t():list(NULL), maxitems(0) {}
	~alist_t() { clear(); }
	void insert(const keytype& k, const datatype& item)
	{
		list = new link_t(k, item, list);
		++maxitems;
	}
	// remove item, search by item key,
	// returns false if item was not found, does not delete data!
	bool remove(const keytype& k)
	{
		for (link_t *L=list, *pL=NULL; L!=NULL; pL=L, L=L->next) {
			if (L->k == k) {
				if (pL)
					pL->next = L->next;
				else
					list = L->next;
				delete L;
				--maxitems;
				return true;
			}
		}
		return false;
	}
	datatype* find(const keytype& k) const
	{
		for (link_t* L=list; L!=NULL; L=L->next)
			if (L->k == k) return &L->d;
		return NULL;
	}
	unsigned int size() const { return maxitems; }
	void clear()
	{
		link_t* L = list;
		while (L) {
			link_t* tmp = L->next;
			delete L;
			L = tmp;
		}
		list = NULL;
		maxitems = 0;
	}
	// if list has allocated data, use this to delete it properly.
	// just for convenience, probably should really leave it to user though...
	void clear_delete()
	{
		link_t* it = list;
		while (it) {
			link_t* tmp = it->next;
			delete it->d;
			delete it;
			it = tmp;
		}
		list = NULL;
		maxitems = 0;
	}
	// as above, array version
	void clear_delete_array()
	{
		link_t* it = list;
		while (it) {
			link_t* tmp = it->next;
			delete[] it->d;
			delete it;
			it = tmp;
		}
		list = NULL;
		maxitems = 0;
	}
protected:
	struct link_t
	{
		link_t(const keytype& _k, const datatype& _d, link_t* n):k(_k), d(_d), next(n) {}
		keytype k;
		datatype d;
		link_t* next;
	};
	link_t *list;
	unsigned int maxitems;
};

// as above, for string keys
template<typename datatype>
class sklist_t
{
private:
	sklist_t(const sklist_t& L);
	sklist_t& operator=(const sklist_t& L);
public:
	sklist_t():list(NULL), curitem(NULL), maxitems(0) {}
	~sklist_t() { clear(); }
	void clear()
	{
		link_t* L = list;
		while (L) {
			link_t* pL = L->next;
			delete[] L->name;
			delete L;
			L = pL;
		}
		list = curitem = NULL;
		maxitems = 0;
	}
	void insert(const char* name, const datatype& item)
	{
		link_t* L = new link_t;
		L->name = new char[strlen(name) + 1];
		strcpy(L->name, name);
		L->data = item;
		L->next = list;
		list = L;
		maxitems++;
	}
	datatype* find(const char* name) const
	{
		for (link_t* L=list; L!=NULL; L=L->next)
			if (!strcmp(name, L->name)) return &L->data;
		return NULL;
	}
	void remove(const char* name, datatype& dt)
	{
		for (link_t *L=list, *pL=NULL; L!=NULL; pL=L, L=L->next)
			if (!strcmp(name, L->name)) {
				if (pL)
					pL->next = L->next;
				else
					list = L->next;
				dt = L->data;
				delete[] L->name;
				delete L;
				--maxitems;
				return;
			}
	}
	inline bool empty() const { return (maxitems == 0); }
	inline unsigned int size() const { return maxitems; }
	inline datatype* first() { return ((curitem = list)==NULL) ? NULL : &curitem->data; }
	inline datatype* next()
	{
		return (curitem==NULL) ? NULL : (((curitem = curitem->next)==NULL) ? NULL : &curitem->data);
	}
	// returns key string, use with first/next
	inline char* getName() { return (curitem==NULL) ? NULL : curitem->name; }
protected:
	struct link_t
	{
		char* name;
		datatype data;
		link_t* next;
	};
	link_t *list, *curitem;
	unsigned int maxitems;
};

//------------------------------------------------------------------------------

// more experiments, Andersson tree, C++ implementation of Jullienne Walker's jsw_atree library
// see http://eternallyconfuzzled.com/jsw_home.aspx
// Works quite well, could possibly be used as replacement for map<>
template<typename keytype, typename datatype>
class aatree_t
{
public:
	aatree_t():numitems(0) { nil.link[0] = nil.link[1] = root = &nil; }
	~aatree_t() { clear(); }
	// mtds
	void clear()
	{
		node_t *save, *it = root;
		while (it != &nil) {
			if (it->link[0] == &nil) {
				save = it->link[1];
				delete it;
			}
			else {
				save = it->link[0];
				it->link[0] = save->link[1];
				save->link[1] = it;
			}
			it = save;
		}
		root = &nil;
		numitems = 0;
	}
	void clear_delete()
	{
		node_t *save, *it = root;
		while (it != &nil) {
			if (it->link[0] == &nil) {
				save = it->link[1];
				delete it->data;
				delete it;
			}
			else {
				save = it->link[0];
				it->link[0] = save->link[1];
				save->link[1] = it;
			}
			it = save;
		}
		root = &nil;
		numitems = 0;
	}
	datatype* find(const keytype& key) const
	{
		node_t* it = root;
		while (it != &nil) {
			if (it->key == key) return &it->data;
			it = it->link[it->key < key];
		}
		return NULL;
	}
	void insert(const keytype& key, const datatype& data)
	{
		if (root == &nil) // Empty tree case
			root = new node_t(1, key, data, &nil);
		else {
			// if key already exists, replace item
			node_t *it = root, *path[64];
			int dir, top = 0;
			// Find a spot and save the path
			for (;;) {
				assert(top < 64);
				path[top++] = it;
				dir = (it->key < key);
				if (it->link[dir] == &nil) break;
				it = it->link[dir];
			}
			// Create a new item
			it->link[dir] = new node_t(1, key, data, &nil);
			// Walk back and rebalance
			while (--top >=  0) {
				// Which child?
				if (top != 0) dir = ((path[top - 1]->link[1] == path[top]));
				skew(path[top]);
				split(path[top]);
				// Fix the parent
				if (top != 0)
					path[top - 1]->link[dir] = path[top];
				else
					root = path[top];
			}
		}
		++numitems;
	}
	// remove item, does not delete data!
	bool remove(const keytype& key)
	{
		if (root == &nil) return false;

		node_t *it = root, *path[64];
		int dir=0, top = 0;

		/* Find node to remove and save path */
		for (;;) {
			assert(top < 64);
			path[top++] = it;
			if (it == &nil) return false;
			if (it->key == key) break;
			dir = (it->key < key);
			it = it->link[dir];
		}

		// Remove the found node
		if ((it->link[0] == &nil) || (it->link[1] == &nil))
		{
			// Single child case
			int dir2 = (it->link[0] == &nil);

			// Unlink the item
			if (--top != 0)
				path[top - 1]->link[dir] = it->link[dir2];
			else
				root = it->link[1];

			delete it;
		}
		else {
			// Two child case
			node_t *heir = it->link[1], *prev = it;

			while (heir->link[0] != &nil) {
				assert(top < 64);
				path[top++] = prev = heir;
				heir = heir->link[0];
			}

			//	Order is important!
			//	(free item, replace item, free heir)
			it->key = heir->key;
			it->data = heir->data;
			prev->link[prev == it] = heir->link[1];
			delete heir;
		}

		/* Walk back up and rebalance */
		while (--top >= 0) {
			node_t *up = path[top];

			if (top != 0)
				dir = (path[top - 1]->link[1] == up);

			// Rebalance (aka. black magic)
			if ((up->link[0]->level < up->level - 1) || (up->link[1]->level < up->level - 1))
			{
				if (up->link[1]->level > --up->level)
					up->link[1]->level = up->level;

				// Order is important!
				skew(up);
				skew(up->link[1]);
				skew(up->link[1]->link[1]);
				split(up);
				split(up->link[1]);
			}

			// Fix the parent
			if ( top != 0 )
				path[top - 1]->link[dir] = up;
			else
				root = up;
		}
	
		--numitems;
		return true;
	}
	bool empty() const { return (numitems == 0); }
	size_t size() const { return numitems; }
	size_t nodesize() const { return sizeof(node_t); }
protected:
	struct node_t
	{
		node_t():level(0)  { link[0] = link[1] = NULL; }
		node_t(int L, const keytype &K, const datatype &D, node_t* il):level(L), key(K), data(D) { link[0] = link[1] = il; }
		int level;
		keytype key;
		datatype data;
		node_t* link[2];
	};

	// Remove left horizontal links
	inline void skew(node_t* &t)
	{
		if ((t->link[0]->level == t->level) && (t->level != 0)) {
			node_t *save = t->link[0];
			t->link[0] = save->link[1];
			save->link[1] = t;
			t = save;
		}
	}
	// Remove consecutive horizontal links
	inline void split(node_t* &t)
	{
		if ((t->link[1]->link[1]->level == t->level) && (t->level != 0)) {
			node_t *save = t->link[1];
			t->link[1] = save->link[0];
			save->link[0] = t;
			t = save;
			++t->level;
		}
	}

	node_t nil, *root;
	size_t numitems;
};

//------------------------------------------------------------------------------
// hashtable_t/Map

unsigned int hashfunc(const unsigned char* data, unsigned int len);

// simple generic hashtable
template<typename hashtype, typename datatype, unsigned int log2_size=10>	// default of 1024 buckets
class hashtable_t
{
public:
	hashtable_t():numitems(0) { }
	~hashtable_t() {}
	void insert(const hashtype& key, const datatype& data)
	{
		++numitems;
		values[hashfunc(reinterpret_cast<const unsigned char*>(&key), sizeof(key)) & ((1 << log2_size)-1)].insert(key, data);
	}
	datatype* find(const hashtype& key) const
	{
		return values[hashfunc(reinterpret_cast<const unsigned char*>(&key), sizeof(key)) & ((1 << log2_size)-1)].find(key);
	}
	bool remove(const hashtype& key)
	{
		if (values[hashfunc(reinterpret_cast<const unsigned char*>(&key), sizeof(key)) & ((1 << log2_size)-1)].remove(key)) {
			--numitems;
			return true;
		}
		return false;
	}	
	void clear()
	{
		if (numitems) {
			for (unsigned int n=0; n<(1 << log2_size); n++)
				values[n].clear();
			numitems = 0;
		}
	}
	// used for case where items are allocated when added
	void clear_delete()
	{
		for (unsigned int n=0; n<(1 << log2_size); n++)
			values[n].clear_delete();
		numitems = 0;
	}
	size_t size() const { return numitems; }
	bool empty() const { return (numitems == 0); }
	void stats() const
	{
		int minbk=0x7fffffff, maxbk=-1;
		int minit=0x7fffffff, maxit=-1;
		for (unsigned int n=0; n<(1 << log2_size); n++) {
			const int sz = (int)values[n].size();
			if (sz < minit) { minbk = n;  minit = sz; }
			if (sz > maxit) { maxbk = n;  maxit = sz; }
		}
		printf("hashtable_t() bucket %d has least items %d\n", minbk, minit);
		printf("hashtable_t() bucket %d has most items %d\n", maxbk, maxit);
	}
private:
	size_t numitems;
	alist_t<hashtype, datatype> values[1 << log2_size];
	//aatree_t<hashtype, datatype> values[1 << log2_size];
};


// as above, strings as key
template<typename datatype, unsigned int log2_size=10>	// default of 1024 buckets
class hashmap_t
{
public:
	hashmap_t():maxitems(0), curbk(0) {}
	~hashmap_t() {}
	void insert(const char* name, const datatype &data)
	{
		values[hashfunc(reinterpret_cast<const unsigned char*>(name),
		                (unsigned int)strlen(name)) & ((1 << log2_size)-1)].insert(name, data);
		maxitems++;
	}
	datatype* find(const char* name) const
	{
		return values[hashfunc(reinterpret_cast<const unsigned char*>(name),
		                       (unsigned int)strlen(name)) & ((1 << log2_size)-1)].find(name);
	}
	void remove(const char* name, datatype& dt)
	{
		values[hashfunc(reinterpret_cast<const unsigned char*>(name),
		                (unsigned int)strlen(name)) & ((1 << log2_size)-1)].remove(name, dt);
		if (dt) --maxitems;
	}
	size_t size() const { return maxitems; }
	void clear()
	{
		for (unsigned int i=0; i<(1 << log2_size); ++i)
			values[i].clear();
	}
	datatype* first()
	{
		// first non-empty bucket
		curbk = 0;
		if (maxitems == 0) return NULL;
		while (values[curbk].empty())
			if (++curbk == (1 << log2_size)) return NULL;
		return values[curbk].first();
	}
	datatype* next()
	{
		if (curbk == (1 << log2_size)) return NULL;
		datatype* dt = values[curbk].next();
		if (dt == NULL) {	// next non-empty bucket
			curbk++;
			while (values[curbk].empty())
				if (++curbk == (1 << log2_size)) return NULL;
			dt = values[curbk].first();
		}
		return dt;
	}
	// returns key name string, use with first/next
	inline char* getName()
	{
		if (curbk == (1 << log2_size)) return NULL;
		return values[curbk].getName();
	}
private:
	sklist_t<datatype> values[1 << log2_size];
	size_t maxitems;
	unsigned int curbk;
};

__END_QDRENDER

#endif // __QDTL_H

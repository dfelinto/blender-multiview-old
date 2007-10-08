//-----------------------------------------------------------
// Least Recently Used (LRU) ordered list
//-----------------------------------------------------------

#ifndef _LRULIST_H
#define _LRULIST_H

#include "qdtl.h"

#include "QDRender.h"
__BEGIN_QDRENDER

#if 0
#include <string>
#include <list>

template<typename key, typename data>
class stl_lrulist_t
{
	typedef typename list<pair<key, data> >::iterator ItemIterator;
	typedef typename list<pair<key, data> >::const_iterator ItemConstIterator;
private:
	stl_lrulist_t(const stl_lrulist_t&);
	stl_lrulist_t& operator=(const stl_lrulist_t&);
public:
	stl_lrulist_t(size_t num_items):max_items(num_items) {}
	~stl_lrulist_t() {}
	void insert(const key& k, const data& d)
	{
		if (ca.size() == max_items) ca.pop_front();
		ca.push_back(make_pair(k, d));
	}
	data* find(const key& k)
	{
		for (ItemIterator ii=ca.begin(); ii!=ca.end(); ++ii)
			if (ii->first == k) {
				pair<key, data> MRU = *ii;
				ca.erase(ii);
				ca.push_back(MRU);
				return &ii->second;
			}
		return NULL;
	}
protected:
	list<pair<key, data> > ca;
	size_t max_items;
};
#endif

// update: now uses hashtable for lookup,
// though not very much faster than linear search starting from mru item.
// it also seems kind of a waste of space, but can't think of a way
// to somehow combine the two yet, but I'm not sure that's even possible...
template<typename keytype, typename datatype>
class lrulist_t
{
private:
	lrulist_t(const lrulist_t&);
	lrulist_t& operator=(const lrulist_t&);
public:
	lrulist_t(size_t _max_items) : lru(NULL), mru(NULL), curitem(NULL), max_items(_max_items), num_items(0) {}
	~lrulist_t()
	{
		//printf("Items in hash: %lu\n", htab.size());
		//htab.stats();
		link_t* it = lru;
		while (it) {
			link_t* next = it->next;
			delete it;
			it = next;
		}
	}
	// insert new item, discarding lru if list is full.
	// returns pointer to possibly discarded data in dd
	// (cannot be pointer reference to data as return value,
	//  since when link_t deleted, pointer reference to data is no longer valid either)
	void insert(const keytype& k, const datatype& d, datatype& dd)
	{
		if (lru && (num_items == max_items)) { // list full, throw out lru item
			htab.remove(lru->key);
			link_t* it = lru;
			dd = it->data;
			lru = lru->next;
			num_items--;
			delete it;
		}
		// new item, becomes mru
		if (lru == NULL)	// empty list
			lru = mru = new link_t(k, d);
		else {
			lru->prev = NULL;
			link_t* old_mru = mru;
			mru = old_mru->next = new link_t(k, d, old_mru);
		}
		htab.insert(k, mru);
		num_items++;
	}
	datatype* find(const keytype& k)
	{
		if (num_items == 0) return NULL;
		if (mru->key != k) {
			// not mru, lookup by hashtable
			link_t** dt = htab.find(k);
			if (dt == NULL) return NULL;
			link_t* it = *dt;	// found, promote to mru
			if (it->prev) {
				it->prev->next = it->next;
				it->next->prev = it->prev;
			}
			else  { // was lru item
				lru = it->next;
				lru->prev = NULL;
			}
			it->next = NULL;
			it->prev = mru;
			mru = mru->next = it;
		}
		return &mru->data;
	}
	size_t size() const { return num_items; }
	// for traversal
	inline datatype first() { return ((curitem = lru)==NULL) ? NULL : curitem->data; }
	inline datatype next()
	{
		return (curitem==NULL) ? NULL : (((curitem = curitem->next)==NULL) ? NULL : curitem->data);
	}
protected:
	struct link_t
	{
		link_t(const keytype& k, const datatype& d, link_t* p=NULL):key(k), data(d), prev(p), next(NULL) {}
		keytype key;
		datatype data;
		link_t *prev, *next;
	};
	link_t *lru, *mru, *curitem;
	size_t max_items, num_items;
	hashtable_t<keytype, link_t*> htab;
};

__END_QDRENDER

#endif // _LRULIST_H

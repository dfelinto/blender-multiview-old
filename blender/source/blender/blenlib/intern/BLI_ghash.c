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
 * A general (pointer -> pointer) hash table ADT
 */

#include <stdlib.h>
#include <string.h>

#include "MEM_guardedalloc.h"

#include "BLI_ghash.h"

/***/

static unsigned int hashsizes[]= {
	1, 3, 5, 11, 17, 37, 67, 131, 257, 521, 1031, 2053, 4099, 8209, 
	16411, 32771, 65537, 131101, 262147, 524309, 1048583, 2097169, 
	4194319, 8388617, 16777259, 33554467, 67108879, 134217757, 
	268435459
};

/***/

typedef struct Entry Entry;
struct Entry {
	Entry *next;
	
	void *key, *val;
};

struct GHash {
	GHashHashFP	hashfp;
	GHashCmpFP	cmpfp;
	
	Entry **buckets;
	int nbuckets, nentries, cursize;
};

/***/

GHash *BLI_ghash_new(GHashHashFP hashfp, GHashCmpFP cmpfp) {
	GHash *gh= MEM_mallocN(sizeof(*gh), "GHash");
	gh->hashfp= hashfp;
	gh->cmpfp= cmpfp;
	
	gh->cursize= 0;
	gh->nentries= 0;
	gh->nbuckets= hashsizes[gh->cursize];
	
	gh->buckets= malloc(gh->nbuckets*sizeof(*gh->buckets));
	memset(gh->buckets, 0, gh->nbuckets*sizeof(*gh->buckets));
	
	return gh;
}

void BLI_ghash_insert(GHash *gh, void *key, void *val) {
	unsigned int hash= gh->hashfp(key)%gh->nbuckets;
	Entry *e= malloc(sizeof(*e));

	e->key= key;
	e->val= val;
	e->next= gh->buckets[hash];
	gh->buckets[hash]= e;
	
	if (++gh->nentries>gh->nbuckets*3) {
		Entry *e, **old= gh->buckets;
		int i, nold= gh->nbuckets;
		
		gh->nbuckets= hashsizes[++gh->cursize];
		gh->buckets= malloc(gh->nbuckets*sizeof(*gh->buckets));
		memset(gh->buckets, 0, gh->nbuckets*sizeof(*gh->buckets));
		
		for (i=0; i<nold; i++) {
			for (e= old[i]; e;) {
				Entry *n= e->next;
				
				hash= gh->hashfp(e->key)%gh->nbuckets;
				e->next= gh->buckets[hash];
				gh->buckets[hash]= e;
				
				e= n;
			}
		}
		
		free(old);
	}
}

void* BLI_ghash_lookup(GHash *gh, void *key) {
	unsigned int hash= gh->hashfp(key)%gh->nbuckets;
	Entry *e;
	
	for (e= gh->buckets[hash]; e; e= e->next)
		if (gh->cmpfp(key, e->key)==0)
			return e->val;
	
	return NULL;
}

int BLI_ghash_haskey(GHash *gh, void *key) {
	unsigned int hash= gh->hashfp(key)%gh->nbuckets;
	Entry *e;
	
	for (e= gh->buckets[hash]; e; e= e->next)
		if (gh->cmpfp(key, e->key)==0)
			return 1;
	
	return 0;
}

void BLI_ghash_free(GHash *gh, GHashKeyFreeFP keyfreefp, GHashValFreeFP valfreefp) {
	int i;
	
	for (i=0; i<gh->nbuckets; i++) {
		Entry *e;
		
		for (e= gh->buckets[i]; e; ) {
			Entry *n= e->next;
			
			if (keyfreefp) keyfreefp(e->key);
			if (valfreefp) valfreefp(e->val);
			free(e);
			
			e= n;
		}
	}
	
	free(gh->buckets);
	MEM_freeN(gh);
}

/***/

unsigned int BLI_ghashutil_ptrhash(void *key) {
	return (unsigned int) key;
}
int BLI_ghashutil_ptrcmp(void *a, void *b) {
	if (a==b)
		return 0;
	else
		return (a<b)?-1:1;
}

unsigned int BLI_ghashutil_strhash(void *ptr) {
	char *s= ptr;
	unsigned int i= 0;
	unsigned char c;
	
	while (c= *s++)
		i= i*37 + c;
		
	return i;
}
int BLI_ghashutil_strcmp(void *a, void *b) {
	return strcmp(a, b);
}

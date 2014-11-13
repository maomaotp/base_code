#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "key.h"


static void _dictReset(dictht *ht)
{
    ht->table = NULL;
    ht->size = 0;
    ht->sizemask = 0;
    ht->used = 0;
}

unsigned int dictHashKey(const void* key, int len) 
{
	uint32_t seed = 5381;
	const uint32_t m = 0x5bd1e995;
	const int r = 24;
	uint32_t h = seed ^ len;

	const unsigned char *data = (const unsigned char *)key;

	while(len >=4) {
		uint32_t k = *(uint32_t *)data;

		k *=m;
		k ^= k>>r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -=4;
	}

	/*Handle the last few bytes of the input array*/
	switch(len) {
		case 3: h ^= data[2] << 16;
		case 2: h ^= data[1] << 8;
		case 1: h ^= data[0]; h *= m;
	}
	
	/*Do a few final mixes of hash to ensure the last few bytes are well-incorporated*/
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return (unsigned int)h;
}

static int dictExpand(dict *d, unsigned long size)
{
	dictht n;
	n.size = size;
	n.sizemask = size-1;
	n.table = calloc(size*sizeof(dictEntry*));
	if(n.table == NULL) {
		perror("calloc error");
	}
	n.used = 0;

	d->ht = n;
	return 0;
}

static int dictKeyIndex(dict *d, const char *key)
{
	unsigned int h, idx;

	dictExpand(d, 40);
	h = dictHashKey(key, strlen(key));
	idx = h & d->d->ht.sizemask;

	return idx;
}

dictEntry *dictAddRaw(dict *d, void *key)
{
	int index;
	dictEntry *entry;
	dictht *ht;

	index = dictKeyIndex(d, key);

	entry = (dictEntry *)malloc(sizeof(dictEntry));
	entry->next = d->table[index];
	d->table[index] = entry;
	d->used++;

	dictSetKey(d, entry ,key);
	
	return entry;
}

int dictAdd(dict *d, void *key, void *val)
{
	dictEntry *entry = dictAddRaw(d, key);
	if(entry == NULL) {
		printf("entry == NULL\n");
		return -1;
	}
	dictSetVal(d, entry, val);
}


/* Initialize the hash table */
int _dictInit(dict *d, void *privDataPtr)
{
    _dictReset(&d->ht);
    return 0;
}

/* Create a new hash table */
dict *dictCreate(void *privDataPtr)
{
    dict *d = malloc(sizeof(*d));

    _dictInit(d,privDataPtr);
    return d;
}

int main()
{
	dict *d;
	d = dictCreate("private");
	dictAdd(d, "apple", "www.apple.com");

	return 0;
}





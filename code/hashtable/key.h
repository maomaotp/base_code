#ifndef _HEAD_DICT_H
#define _HEAD_DICT_H

#define dictSetKey(d, entry, _key_) do { \
	entry->key = (_key_); \
} while(0)


#define dictSetVal(d, entry, _val_) do { \
	entry->v.val = (_val_); \
} while(0)

typedef struct dictEntry {
	char key[24];
	char val[1024];
	struct dictEntry *next;
}dictEntry;


typedef struct dictht {
	dictEntry **table;
	unsigned long size;
	unsigned long sizemask;
	unsigned long used;
}dictht;

typedef struct dict {
	void *privdata;
	dictht ht;
	int rehashidx;
	int iterators;
} dict;

#endif

#include <stdio.h>
#include "dict.h"
#include <string.h>


/*****************字符串处理***********************/
typedef char *sds;

struct sdshdr {
	int len;
	int free;
	char buf[];
};

sds sdsnewlen(const void *init, size_t initlen) {
	struct sdshdr *sh;
	if (init) {
		sh = zmalloc(sizeof(struct sdshdr)+initlen+1);
	} else {
		sh = zcalloc(sizeof(struct sdshdr)+initlen+1);
	}
	if (sh == NULL){
		return NULL;
	}

	sh->len = initlen;
	sh->free = 0;

	if (initlen && init)
		memcpy(sh->buf, init, initlen);
	sh->buf[initlen] = '\0';
	
	return (char*)sh->buf;
}

sds sdsnew(const char *init) {
	size_t initlen = (init == NULL) ? 0 : strlen(init);
	return sdsnewlen(init, initlen);
}

void sdsfree(sds s) {
	if (s == NULL) return;
	zfree(s-sizeof(struct sdshdr));
}

static inline size_t sdslen(const sds s) {
	struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
	return sh->len;
}


/*******************hash函数*************************/
/*
unsigned int dictGenHashFunction(const void *key, int len) {
	uint32_t seed = 5381;
	const uint32_t m = 0x5bd1e995;
	const int r = 24;

	uint32_t h = seed ^ len;
	const unsigned char *data = (const unsigned char *)key;
	while(len >= 4) {
		uint32_t k = *(uint32_t*)data;
		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;
		data += 4;
		len -= 4;
	}

	switch(len) {
		case 3: h ^= data[2] << 16;
		case 2: h ^= data[1] << 8;
		case 1: h ^= data[0]; h *= m;
	};

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return (unsigned int)h;
}
*/

unsigned int dictSdsHash(const void *key) {
	return dictGenHashFunction((unsigned char*)key, sdslen((char*)key));
}

int dictSdsKeyCompare(void *privdata, const void *key1, const void *key2){
	int l1,l2;
	DICT_NOTUSED(privdata);

	l1 = sdslen((sds)key1);
	l2 = sdslen((sds)key2);
	if (l1 != l2) return 0;
	return memcmp(key1, key2, l1) == 0;
}

void dictSdsDestructor(void *privdata, void *val) {
	DICT_NOTUSED(privdata);
	sdsfree(val);
}

void dictRedisObjectDestructor(void *privdata, void *val){
	DICT_NOTUSED(privdata);
	if (val == NULL) return; /* Values of swapped out keys as set to NULL */
	//decrRefCount(val);
}

dictType dbDictType = {
	dictSdsHash,                /* hash function */
	NULL,                       /* key dup */
	NULL,                       /* val dup */
	dictSdsKeyCompare,          /* key compare */
	dictSdsDestructor,          /* key destructor */
	dictRedisObjectDestructor   /* val destructor */
};

int main(int argc, char **argv)
{
	dict *d;
	char val[100];
	char key[10];
	char value[100];
	int i;

	d = dictCreate(&dbDictType, NULL);

	if (dictAdd(d, sdsnew("key"), sdsnew("hello,world")) != 0) {
		printf("dict add error\n");
	}
	for(i=0; i<20; i++){
		sprintf(key, "key%d", i);
		sprintf(value, "value%d", i);
		dictAdd(d, sdsnew(key), sdsnew(value));
	}

//	strcpy(val, dictFetchValue( d, sdsnew("key1") ));

	printf("dict used = %d\n", d->ht[0].used);
	printf("dict size = %d\n", d->ht[0].size);

	while(1){
	}

	return 0;
}

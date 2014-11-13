#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "redis_hash.h"


struct sharedObjectsStruct shared;
static uint32_t dict_hash_function_seed = 5381;

dictType commandTableDictType = {
	dictSdsCaseHash,           /* hash function */
	NULL,                      /* key dup */
	NULL,                      /* val dup */
	dictSdsKeyCaseCompare,     /* key compare */
	dictSdsDestructor,         /* key destructor */
	NULL                       /* val destructor */
}

dictEntry *dictFind(dict *d, const void *key)
{
	dictEntry *he;
	unsigned int h, idx, table;
	if (d->ht.size == 0) return NULL;
	h = dictHashKey(d, key);

	idx = h & d->ht.sizemask;
	he = d->ht.table[idx];
	while(he) {
		if(dictCompareKeys(d, key, he->key))
			return he;
		he = he->next;
	}
	return NULL;
}

/*C-level DB API*/
robj *lookupKey(redisDb *db, robj *key) {
	dictEntry *de = dictFind(db->dict, key->ptr);
	if(de) {
		robj *val = de->v.val;
		return val;
	}
	else {
		return NULL;
	}
}

robj *lookupKeyReadOrReply(redisClient *c, robj *key, robj *reply) {
	robj *o = lookupKey(c->db, key);
	if(!o) {
		//addReply(c, reply);
	}
	return o;
}

int getCommand(redisClient *c) {
	robj *o;

	if( (o = lookupKeyReadOrReply(c, c->argv[1], shared, nullbulk)) == NULL ) {
		return REDIS_OK;
	}
	addReplyBulk(c,o);
	return REDIS_OK;
}

struct redisCommand redisCommandTable[] = {
    {"get",getCommand,2,"rF",0,NULL,1,1,1,0,0}
   // {"set",setCommand,-3,"wm",0,NULL,1,1,1,0,0},
   // {"del",delCommand,-2,"w",0,NULL,1,-1,1,0,0},
   // {"exists",existsCommand,2,"rF",0,NULL,1,1,1,0,0},
};

unsigned int dictHashKey(const unsigned char* buf, int len) 
{
	unsigned int hash = (unsigned int )dict_hash_function_seed;

	while(len--) {
		hash = ((hash << 5) + hash) + (tolower(*buf++));
		printf("hash=%u, tolower=%d\n", hash, tolower(*buf++));
	}
	return hash;
}

static void _dictInit(dict *d, dictType *type, void *privDataPtr)
{
	&d->ht->table = NULL;
	&d->ht->size = 0;
	&d->ht->sizemask = 0;
	&d->ht->used = 0;
	
	d->type = type;
	d->privdata = privataPtr;
	d->iterators = 0;

	return DICT_OK;
}

dict *dictCreate(dictType *type, void *privDataPtr)
{
	dict *d = malloc(sizeof(*d));

	_dictInit(d, type, privDataPtr);
	return d;
}

static int _dictKeyIndex(dict *d, const void *key)
{
	unsigned int h, idx;
	dictEntry *he;

	h = dictHashKey(key, strlen(key));
	idx = h & d->ht.sizemask;
	he = d->ht.table[idx];

	while(he) {
		if (dictCompareKeys(d, key, he->key)) {
			return -1;
		}
		he = he->next;
	}

	return idx;
}

dictEntry *dictAddRaw(dict *d, void *key)
{
	int index;
	dictEntry *entry;
	dictht *ht;
	
	if( (index = _dictKeyIndex(d, key)) == -1 ) {
		return NULL;
	}
	ht = &d->ht;

	entry = malloc(sizeof(*entry));
	entry->next = ht->table[index];
	ht->used++;

	dictSetKey(d, entry, key);
	
	return entry;
}

int dictAdd(dict *d, void *key, void *val)
{
	dictEntry *entry = dictAddRaw(d, key);

	if (!entry) return 1;

	dictSetVal(d, entry, val);
	return 0;
}


void populateCommandTable(void) 
{
	int j;
	int numcommands = sizeof(redisCommandTable)/sizeof(struct redisCommand);
	dict *d = dictCreate(&commandTableDictType, NULL);

	for(j=0; j<numcommands; j++) {
		struct redisCommand *c = redisCommandTable+j;
		char *f = c->sflags;
		int retval1;
		
		while(*f != '\0') {
			switch(*f) {
			case 'w': c->flags |= REDIS_CMD_WRITE; break;
			case 'r': c->flags |= REDIS_CMD_READONLY; break;
			case 'm': c->flags |= REDIS_CMD_DENYOOM; break;
			case 'a': c->flags |= REDIS_CMD_ADMIN; break;
			case 'p': c->flags |= REDIS_CMD_PUBSUB; break;
			case 'S': c->flags |= REDIS_CMD_SORT_FOR_SCRIPT; break;
			case 'l': c->flags |= REDIS_CMD_LOADING; break;
			case 't': c->flags |= REDIS_CMD_STALE; break;
			case 'M': c->flags |= REDIS_CMD_SKIP_MONITOR; break;
			case 'F': c->flags |= REDIS_CMD_FAST; break;
			}
			f++;
		}

		retval1 = dictAdd(d, c->name, c)
	}

}

/*********/
int dictAdd(dict *d, void *key, void *val)
{
	dictEntry *entry = dictAddRaw(d, key);
	if(entry ==NULL) {
		return -1;
	}
	dictSetVal(d, entry, val);

	return 0;
}

void dbAdd(redisDb *db, robj *key, robj *val)
{
	dictAdd(db->dict, copy, val);
}

void setkey(redisDb *db, robj *key, robj *val)
{
	dbAdd(db, key, val);
}

int main(int argc, char* argv[])
{
	populateCommandTable();
	return 0;
}

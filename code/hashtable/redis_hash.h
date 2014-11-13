#ifndef _REDIS_HASH_H
#define _REDIS_HASH_H

#define REDIS_OK     0
#define REDIS_ERR    -1

#define REDIS_CMD_WRITE 1                   /* "w" flag */
#define REDIS_CMD_READONLY 2                /* "r" flag */
#define REDIS_CMD_DENYOOM 4                 /* "m" flag */
#define REDIS_CMD_NOT_USED_1 8              /* no longer used flag */
#define REDIS_CMD_ADMIN 16                  /* "a" flag */
#define REDIS_CMD_PUBSUB 32                 /* "p" flag */
#define REDIS_CMD_NOSCRIPT  64              /* "s" flag */
#define REDIS_CMD_RANDOM 128                /* "R" flag */
#define REDIS_CMD_SORT_FOR_SCRIPT 256       /* "S" flag */
#define REDIS_CMD_LOADING 512               /* "l" flag */
#define REDIS_CMD_STALE 1024                /* "t" flag */
#define REDIS_CMD_SKIP_MONITOR 2048         /* "M" flag */
#define REDIS_CMD_ASKING 4096               /* "k" flag */
#define REDIS_CMD_FAST 8192                 /* "F" flag */

#define dictCompareKeys(d, key1, key2) \
	((d)->type->keyCompare) ? \
		(d)->type->keyCompare((d)->privdata, key1, key2) : \
			(key1) == (key2)

#define dictSetKey(d, entry, _key_) do { \
	if ((d)->type->keyDup) \
	        entry->key = (d)->type->keyDup((d)->privdata, _key_); \
	    else \
	         entry->key = (_key_); \
} while(0)

#define dictSetVal(d, entry, _val_) do { \
     if ((d)->type->valDup) \
         entry->v.val = (d)->type->valDup((d)->privdata, _val_); \
     else \
         entry->v.val = (_val_); \
} while(0)

typedef struct dictEntry {
	void *key;
	union {
		void *val;
		uint64_t u64;
		int64_t s64;
	} v;
	struct dictEntry *next;
} dictEntry;

typedef struct dictht {
	dictEntry ** table;
	unsigned long size;
	unsigned long sizemask;
	unsigned long used;
} dictht;

typedef struct dictType {
	unsigned int (*hashFunction)(const void *key);
	void *(*keyDup)(void *privdata, const void *key);
	void *(*valDup)(void *privdata, const void *obj);
	void *(*keyCompare)(void *privdata, const void *key1, const void *key2);
	void *(*keyDestructor)(void *privdata, void *key);
	void *(*valDestructor)(void *privdata, void *obj);
} dictType;

typedef struct dict {
	dictType *type;
	void *privdata;
	dictht ht;
	int iterators;
} dict;

/****************dict***********/

//typedef void redisCommandProc(redisClient *c);
//typedef int *redisGetkeysProc(struct redisCommand *cmd, robj **argv, int argc, int *numkeys, int flags);

struct redisCommand {
	char name[20];
	redisCommandProc *proc;
	int arity;
	char *sflags;
	int flags;
	redisGetKeysProc *getkeys_proc;
	int firstkey;
	int lastkey;
	int keystep;
	long long microseconds, calls;
};


typedef struct listNode {
    struct listNode *prev;
    struct listNode *next;
    void *value;
} listNode;

typedef struct list {
    listNode *head;
    listNode *tail;
    void *(*dup)(void *ptr);
    void (*free)(void *ptr);
    int (*match)(void *ptr, void *key);
    unsigned long len;
} list;

typedef struct redisObject {
    unsigned type:4;
    unsigned encoding:4;
    unsigned lru:24; /* lru time (relative to server.lruclock) */
    int refcount;
    void *ptr;
} robj;

typedef struct redisDb {
    dict *dict;                 /* The keyspace for this DB */
    dict *expires;              /* Timeout of keys with a timeout set */
    dict *blocking_keys;        /* Keys with clients waiting for data (BLPOP) */
    dict *ready_keys;           /* Blocked keys that received a PUSH */
    dict *watched_keys;         /* WATCHED keys for MULTI/EXEC CAS */
    int id;
    long long avg_ttl;          /* Average TTL, just for stats */
} redisDb;

typedef struct redisClient {
    uint64_t id;            /* Client incremental unique ID. */
    int fd;
    redisDb *db;
    int dictid;
    robj *name;             /* As set by CLIENT SETNAME */
    sds querybuf;
    size_t querybuf_peak;   /* Recent (100ms or more) peak of querybuf size */
    int argc;
    robj **argv;
    struct redisCommand *cmd, *lastcmd;
    int reqtype;
    int multibulklen;       /* number of multi bulk arguments left to read */
    long bulklen;           /* length of bulk argument in multi bulk request */
    list *reply;
    unsigned long reply_bytes; /* Tot bytes of objects in reply list */
    int sentlen;            /* Amount of bytes already sent in the current
                               buffer or object being sent. */
    time_t ctime;           /* Client creation time */
    time_t lastinteraction; /* time of the last interaction, used for timeout */
    time_t obuf_soft_limit_reached_time;
    int flags;              /* REDIS_SLAVE | REDIS_MONITOR | REDIS_MULTI ... */
    int authenticated;      /* when requirepass is non-NULL */
    int replstate;          /* replication state if this is a slave */
    int repldbfd;           /* replication DB file descriptor */
    off_t repldboff;        /* replication DB file offset */
    off_t repldbsize;       /* replication DB file size */
    long long reploff;      /* replication offset if this is our master */
    long long repl_ack_off; /* replication ack offset, if this is a slave */
    long long repl_ack_time;/* replication ack time, if this is a slave */
    char replrunid[41]; /* master run id if this is a master */
    int slave_listening_port; /* As configured with: SLAVECONF listening-port */
    sds peerid;             /* Cached peer ID. */

    /* Response buffer */
    int bufpos;
    char buf[16*1024];
}

struct sharedObjectsStruct {
    robj *crlf, *ok, *err, *emptybulk, *czero, *cone, *cnegone, *pong, *space,
    *colon, *nullbulk, *nullmultibulk, *queued,
    *emptymultibulk, *wrongtypeerr, *nokeyerr, *syntaxerr, *sameobjecterr,
    *outofrangeerr, *noscripterr, *loadingerr, *slowscripterr, *bgsaveerr,
    *masterdownerr, *roslaveerr, *execaborterr, *noautherr, *noreplicaserr,
    *oomerr, *plus, *messagebulk, *pmessagebulk, *subscribebulk,
    *unsubscribebulk, *psubscribebulk, *punsubscribebulk, *del, *rpop, *lpop,
    *lpush, *emptyscan, *minstring, *maxstring,
    *select[10],
    *integers[10000],
    *mbulkhdr[32], /* "*<value>\r\n" */
    *bulkhdr[32];  /* "$<value>\r\n" */
};

#endif

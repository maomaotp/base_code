#ifndef TIPS_SQLITE_SYNC_H
#define TIPS_SQLITE_SYNC_H

#include <iostream>
#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "sqlite3.h"

using namespace std;
/*
typedef struct sqlite_sync_stut{
	pthread_mutex_t sqlite_mutex;
	int manage_op;
	int ui_op;
	key_t IPCkey;
}sqlite_sync_stut_t;
*/
#define TIPS_DIR "/root/lirongxun/manage_20130324/tips.db"

void *sqlite_sync(void *thread_init);

typedef struct Tips_sqlite_thread{
	int level;
//	int firstrun;
	sqlite3 *db;
}Tips_sqlite_thread_t;

class CTips_sqlite_sync{
public:
	int sqlite_sync_serv_init(char *filename);
	int sqlite_sync_serv_destory();
	int sqlite_sync_cli_init(char *filename);
	int sqlite_sync_cli_destory();
	int sqlite_sync_interface(sqlite3 *db, int num);
	int init_sync_mutex();
	int tips_lock(int level);
	int tips_unlock(int level);
private:
	//sqlite_sync_stut* tips_sync;
	int semid;
	int level2;
	int level3;
};

#endif

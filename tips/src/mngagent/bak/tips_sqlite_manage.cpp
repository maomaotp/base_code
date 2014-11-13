#include "tips_sqlite_manage.h"

pthread_mutex_t sqlite_sync_mutex;
int startrun = 0;
CTips_sqlite_sync tips_sqlite_sync;


void *sqlite_sync(void *thread_init)
{
	Tips_sqlite_thread_t *tips_thread;
	tips_thread = (Tips_sqlite_thread_t *)thread_init;
	tips_sqlite_sync.sqlite_sync_interface(tips_thread->db, tips_thread->level);
	return NULL;
}


int CTips_sqlite_sync::init_sync_mutex()
{
	 int err = pthread_mutex_init(&sqlite_sync_mutex, NULL);
         if(err != 0){
	         printf("mutex init:%s", strerror(err));
                 exit(0);
         }
	 return 0;
}


int CTips_sqlite_sync::sqlite_sync_serv_init(char *filename)
{
	int id = 1;
	int err = 0;
/*初始化映射文件*/
/*
	sqlite_sync_stut synctemp;
	int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
	if(-1 == fd){
		printf("open %s:%s\n", filename, strerror(errno));
		exit(0);
	}
	write(fd, &synctemp, sizeof(synctemp));
*/
/*开始映射*/
/*
	tips_sync = (sqlite_sync_stut*)mmap(NULL, sizeof(synctemp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(tips_sync == (sqlite_sync_stut_t *)-1){
                 printf("mmap error:%s", strerror(errno));
                 exit(0);
         }
*/
/*初始化线程互斥量*/
/*
	 err = pthread_mutex_init(&(tips_sync->sqlite_mutex), NULL);
         if(err != 0){
	         printf("mutex init:%s", strerror(err));
                 exit(0);
         }
*/
/*初始化IPC键*/
	key_t IPCkey = ftok(filename, 1);
	if((key_t)-1 == IPCkey){
	         printf("IPCkey init:%s", strerror(errno));
                 exit(0);
	}
	semid = semget(IPCkey, 6, 0);
	printf("semid=%d\n",semid);
	if(semid != -1){
		semctl(semid, 0, IPC_RMID);
	}
	semid = semget(IPCkey, 6, IPC_CREAT | IPC_EXCL);
	if(semid == -1){
		printf("create sem:%s\n",strerror(errno));
		exit(0);
	}
	printf("%d\n",semid);
	semctl(semid, 0, SETVAL, 0);
	semctl(semid, 1, SETVAL, 0);
	semctl(semid, 2, SETVAL, 0);
	semctl(semid, 3, SETVAL, 0);
	semctl(semid, 4, SETVAL, 0);
	semctl(semid, 5, SETVAL, 0);
	int i = semctl(semid, 4, GETVAL);
	printf("i==%d\n",i);

/*初始化UI和manage操作标志*/
/*
	tips_sync->manage_op = 0;
	tips_sync->ui_op = 0;
*/
	return 0;
}

int CTips_sqlite_sync::sqlite_sync_serv_destory()
{
	semctl(semid, 0, IPC_RMID);
//	munmap(tips_sync, sizeof(sqlite_sync_stut));
	return 0;
}

int CTips_sqlite_sync::sqlite_sync_cli_init(char *filename)
{

/*开始映射*/
/*
	tips_sync = (sqlite_sync_stut*)mmap(NULL, sizeof(synctemp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(tips_sync == (sqlite_sync_stut_t *)-1){
                 printf("mmap error:%s", strerror(errno));
                 exit(0);
         }
*/
/*初始化IPC键*/
	key_t IPCkey = ftok(filename, 1);
	if((key_t)-1 == IPCkey){
	         printf("IPCkey init:%s", strerror(errno));
                 exit(0);
	}
	semid = semget(IPCkey, 6, 0);
	int num = semctl(IPCkey, 0, GETVAL);
        printf("num:%d\n", num);

	printf("%d\n",semid);
	return 0;
}

int CTips_sqlite_sync::sqlite_sync_cli_destory()
{
	//munmap(tips_sync, sizeof(sqlite_sync_stut));
	return 0;
}

int CTips_sqlite_sync::tips_lock(int level)
{
	int i = 0;
	pthread_mutex_lock(&sqlite_sync_mutex);
	struct sembuf sembuf2[1]= {0,1,0};
	struct sembuf sembuf2wait[1]= {1,-1,0};
	struct sembuf sembuf3[1]= {3,1,0};
	struct sembuf sembuf3wait[1]= {4,-1,0};
	switch(level){
		case 1:{
			if(semctl(semid, 1, GETVAL) < 1){
/*加密level 2*/
				level2 = 1;
        			while(-1 == semop(semid, sembuf2, 1)){
	        			printf("semop sembuf2:%s\n",strerror(errno));
        			}
        			while(-1 == semop(semid, sembuf2wait, 1)){
	        			printf("semop sembuf2:%s\n",strerror(errno));
        			}

/*加密level 3*/
				if(semctl(semid, 4, GETVAL) < 1){
					level3 = 1;
        				while(-1 == semop(semid, sembuf3, 1)){
	        				printf("semop sembuf3:%s\n",strerror(errno));
        				}
        				while(-1 == semop(semid, sembuf3wait, 1)){
	        				printf("semop sembuf2:%s\n",strerror(errno));
        				}
				}
			}
			break;
		}
		case 2:{
			printf("yyyyyyyyyyyyyyyyyyyyyyyy\n");
			if((i = semctl(semid, 4, GETVAL)) < 1){
				level3 = 1;
				printf("zzzzzzzzzzzzzzzzzzzzzzzzzzz\n");
				//exit(0);
        			while(-1 == semop(semid, sembuf3, 1)){
	        			printf("semop sembuf3:%s\n",strerror(errno));
        			}
					fprintf (stdout, "mimmamammamamam\n");
        			while(-1 == semop(semid, sembuf3wait, 1)){
	        			printf("semop sembuf2:%s\n",strerror(errno));
        			}
					fprintf (stdout, "mimmamammamamam11111111111111111\n");
			}
			break;
		}
		default:{
			 pthread_mutex_unlock(&sqlite_sync_mutex);
			 return -1;
			}
	}
	printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxx:%d\n",i);
	pthread_mutex_unlock(&sqlite_sync_mutex);
	return 0;
}

int CTips_sqlite_sync::tips_unlock(int level)
{
	pthread_mutex_lock(&sqlite_sync_mutex);
	struct sembuf sembuf2[1]= {1,-1,0};
	struct sembuf sembuf2sig[1]= {2,1,0};
	struct sembuf sembuf3[1]= {4,-1,0};
	struct sembuf sembuf3sig[1]= {5,1,0};
	switch(level){
		case 1:{
			printf("1unlock\n");
			if(level3 == 1){
        			while(-1 == semop(semid, sembuf3, 1)){
	        			printf("semop sembuf:%s\n",strerror(errno));
        			}
        			while(-1 == semop(semid, sembuf3sig, 1)){
	        			printf("semop sembuf:%s\n",strerror(errno));
        			}
				level3 = 0;
			}
			if(level2 == 1){
        			while(-1 == semop(semid, sembuf2, 1)){
	        			printf("semop sembuf:%s\n",strerror(errno));
        			}
        			while(-1 == semop(semid, sembuf2sig, 1)){
	        			printf("semop sembuf:%s\n",strerror(errno));
        			}
				level2 = 0;
			}
			printf("1unlockfinish\n");
			break;
		}
		case 2:{
			printf("jinlaiqian\n");
			if(level3 == 1){
				printf("level3=======\n");
        			while(-1 == semop(semid, sembuf3, 1)){
	        			printf("semop sembuf:%s\n",strerror(errno));
        			}
				printf("level3=======after\n");
				
        			while(-1 == semop(semid, sembuf3sig, 1)){
	        			printf("semop sembuf:%s\n",strerror(errno));
        			}
				level3 = 0;
			}
			printf("2unlockfinish\n");
			break;
		}
		default:{
			 pthread_mutex_unlock(&sqlite_sync_mutex);
			 return -1;
			}
	}

	pthread_mutex_unlock(&sqlite_sync_mutex);
	return 0;
}


int CTips_sqlite_sync::sqlite_sync_interface(sqlite3 *db, int num)
{
	char *err = NULL;
	int ret = 0;
	struct sembuf sembufbefore[1]= {num*3 + 0, -1, 0};
	struct sembuf sembufafter[1]= {num*3 + 1, 2, 0};
	struct sembuf sembuflast[1]= {num*3 + 2, -1, 0};
	while(1){
			fprintf (stdout, "111111111111111111333333333333333111111111111\n");
        	while(-1 == semop(semid, sembufbefore, 1)){
	       		printf("semop sem before:%s\n",strerror(errno));
        	}
			fprintf (stdout, "11111111111111111133222222223333333333333111111111111\n");
		pthread_mutex_lock(&sqlite_sync_mutex);
		printf("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc\n");
		if((db != NULL) && (startrun != 0)){
			ret = sqlite3_exec(db, "commit", NULL, NULL, &err);
			printf("shujugfjiejfiejfei\n");
			if(ret != 0){
				printf("commit err:%s\n", err);
				exit(0);
			}
		}
        	while(-1 == semop(semid, sembufafter, 1)){
	        	printf("semop sem after:%s\n",strerror(errno));
        	}
		printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\n");
        	while(-1 == semop(semid, sembuflast, 1)){
	        	printf("semop sem after:%s\n",strerror(errno));
        	}
		if((db != NULL) && (startrun != 0)){
			ret = sqlite3_exec(db, "begin", NULL, NULL, &err);
			if(ret != 0){
				printf("commit err:%s\n", err);
				exit(0);
			}
		}
		printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
		pthread_mutex_unlock(&sqlite_sync_mutex);
	}
	return 0;
}


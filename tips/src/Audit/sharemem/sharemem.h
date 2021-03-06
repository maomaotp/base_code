#ifndef SHAREMEM_H
#define	SHAREMEM_H
#include "shm_struct_t.h"

#define DAEMONPID 		0
#define ALARMPID 		1
#define AUDITPID 		2
#define BLOCKPID 		3
#define SENSTPID 		4
#define CACHEPID 		5
#define MNGAGNTPID 	6
#define UIPID				7
#define NETSTATPID 	8
#define PREALLOC1PID 9
#define PREALLOC2PID 10

int Read_config(shm_struct_t * shm);
int Set_var_type(char * str, var_t * var_ptr);
int Sizeof(int type);
int Init_shm(shm_struct_t ** shm);
int Unlink_shm(shm_struct_t * shm);
int Set_status_shm(shm_struct_t * shm, int status_id, void * param, int len);
int Get_status_shm(shm_struct_t * shm, int status_id, void * param, int len);
void Set_pid_shm(shm_struct_t * shm, int process);
int Get_pid_shm(shm_struct_t * shm, int process);
#endif


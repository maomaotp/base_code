#ifndef SHM_STRUCT_H
#define SHM_STRUCT_H

#include <stdio.h>
#include <fcntl.h>         
#include <sys/stat.h>      
#include <semaphore.h>

#define MAX_VAR_NUM 1024
#define MAX_SHM_SIZE (1024*1024)

typedef	struct var_t
{
	int offset;
	int type;
	int num;
	int valid;
}var_t;

typedef struct shm_struct_t
{
	int			next_var_offset;
	sem_t		mutex;
	var_t 		var[MAX_VAR_NUM];
	char 		mem[MAX_SHM_SIZE];
}shm_struct_t;

#endif


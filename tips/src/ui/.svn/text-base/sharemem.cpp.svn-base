#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "sharemem.h"
#define IPC_SHM_NAME "tips_shm_name"
#define	SHMCONF "shm.conf"
#define UNDEFINE		0
#define	CHAR			1
#define	INT				2
#define LONG			3
#define UNSIGNEDINT 	4
#define	FLOAT			5
#define	DOUBLE			6


int ShareMem::Init_shm()
{
	int fd = -1;
	shm_struct_t * ptr = NULL;
	fd = shm_open(IPC_SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	if(fd >0 )
	{
		printf("create shm\n");
		ftruncate(fd, sizeof(shm_struct_t));
//		printf("shm_struct_t size = %d\n", sizeof(shm_struct_t));
		ptr = (shm_struct_t *) mmap(NULL, sizeof(shm_struct_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if(NULL == ptr)
		{
			printf("mmap failed\n");
			return -1;
		}
		memset(ptr, 0, sizeof(shm_struct_t));
		sem_init(&ptr->mutex, 1, 1);
		shm = ptr;
		int ret = 0;
		ret = Read_config();
		if(-1 == ret)
		{
			printf("shm read config failed\n");
			return -1;
		}
	}
	else if (fd < 0) 
	{	
		printf("open exist shm\n");
		fd = shm_open(IPC_SHM_NAME, O_RDWR , S_IRUSR | S_IWUSR);
		ptr =(shm_struct_t *) mmap(NULL, sizeof(shm_struct_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if(NULL == ptr)
		{
			printf("mmap failed\n");
			return -1;
		}
		shm = ptr;
	}
	close(fd);
	if(NULL == ptr) 
	{
		printf("mmap failed\n");
		return -1;
	}
	shm = ptr;
	return 0;
}

int ShareMem::Read_config()
{
//	printf("Read_config start\n");
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	const char* delim  = "#";
	FILE * fp = fopen(SHMCONF,"r");
	int i = 0, id = 0, num = 0, size = 0;
	if(NULL == fp)
	{
		printf("shm open configure file failed\n");
		return -1;
	}
//	printf("read configure file start\n");
	shm->next_var_offset = 0;
	for(i=0; NULL != fgets(buf, sizeof(buf), fp); i++)
	{
#if 1
		char str[3][256];
		memset(str[0], 0, sizeof(str[0]));
		strcpy(str[0],strtok(buf, delim));
		memset(str[1], 0, sizeof(str[1]));
		strcpy(str[1],strtok(NULL, delim));
		memset(str[2], 0, sizeof(str[2]));
		strcpy(str[2],strtok(NULL, delim));
		id = atoi(str[0]);
		memset(&(shm->var[id]), 0, sizeof(shm->var[id]));
//		printf("id=%2d ",id);
		if(id >= MAX_VAR_NUM || id == 0)
				continue;
		shm->var[id].offset = shm->next_var_offset;
//		printf("var[%d].offset=%d ", id, shm->var[id].offset);
		Set_var_type(str[1],&(shm->var[id]));
//		printf("var[%d].type=%d ", id, shm->var[id].type);
//		printf("numstr=%s ", str[2]);
		num = atoi(str[2]);
		shm->var[id].num = num;
//		printf("num=%4d ", num);
//		printf("var[%d].num=%d ", id,shm->var[id].num);
		size = Sizeof(shm->var[id].type);
//		printf("typesize=%d\n", size);
		shm->next_var_offset += (size * num);
		if((unsigned int)shm->next_var_offset >= sizeof(shm->mem))			//update by hyj
		{
			printf("allocated shm not enough\n");
			return -1;
		}
//		printf("next_var_offset=%d\n\n", shm->next_var_offset);
#endif 
	}
	printf("read configure finished\n");
	return 0;
}


int ShareMem::Sizeof(int type)
{
	int size = 0;
	switch(type)
	{
		case CHAR:
			size = sizeof(char);
			break;
		case INT:
			size = sizeof(int);
			break;
		case UNSIGNEDINT:
			size = sizeof(unsigned int);
			break;
		case LONG:
			size = sizeof(long);
			break;
		case FLOAT:
			size = sizeof(float);
			break;
		case DOUBLE:
			size = sizeof(double);
			break;
		case UNDEFINE:
			size = 0;
			break;
		default:
			printf("could't count type size in shm\n");
			size = 0;
			break;
	}
	return size;
}

int ShareMem::Set_var_type(char* str, var_t * var_ptr)
{
//	printf("str=%s strsize=%d ", str, sizeof(str));
	if(0 == strncmp(str, "char",sizeof("char")))
	{
		var_ptr->type = CHAR;
	}
	else if(0 == strncmp(str, "int",sizeof("int")))
	{
		var_ptr->type = INT;
	}
	else if(0 == strncmp(str, "unsigned int",sizeof("unsigned int")))
	{
		var_ptr->type = UNSIGNEDINT;
	}
	else if(0 == strncmp(str, "long", sizeof("long")))
	{
		var_ptr->type = LONG;
	}
	else if(0 == strncmp(str, "float",sizeof("float")))
	{
		var_ptr->type = FLOAT;
	}
	else if(0 == strncmp(str, "double",sizeof("double")))
	{
		var_ptr->type = DOUBLE;
	}
	else 
	{
		printf("\n%s type undefined in shm\n", str);
		var_ptr->type = UNDEFINE;
	}
//	printf("type=%d ", var_ptr->type);
	return 0;
}

int ShareMem::Unlink_shm()
{
	shm_unlink(IPC_SHM_NAME);
	shm = NULL;
	printf("unlink shm\n");
/*返回值？*/return 0;
}


int ShareMem::Set_status_shm(int id, void * param, int len)
{
//	printf("var[%d].type=%d\n", id, shm->var[id].type);
//	printf("var[%d].num=%d\n", id, shm->var[id].num);
	int size = (Sizeof(shm->var[id].type) * shm->var[id].num);
//	printf("size=%d\n", size);
	if(len != size)
	{
		printf("param length not right,so set status failed\n");
		return -1;
	}
	char* ptr = shm->mem;
	int offset = shm->var[id].offset;
	int mutex_value;
	sem_getvalue(&shm->mutex, &mutex_value);
	if(!mutex_value)
	{
		printf("shm is busy,so set status failed\n");
		return -1;
	}
	if(-1 == sem_trywait(&shm->mutex))
	{
		printf("shm is busy,so set status failed\n");
		return -1;
	}
	memset(&ptr[offset],0,size);
	memcpy(&ptr[offset],param,len);
	sem_post(&shm->mutex);
	shm->var[id].valid = 1;
	//printf("set id:%d status successfull\n", id);
	return 0;
}


int ShareMem::Get_status_shm(int id, void * param, int len)
{
	if( 0 == shm->var[id].valid)
	{
		printf("id:%d status had not write in shm\n", id);
		return -1;
	}
	int size = Sizeof(shm->var[id].type) * shm->var[id].num;
	if(len < size)
	{
		printf("your buffer not enough, so get status failed\n");
		return -1;
	}
	char* ptr = (char*) shm->mem;
	int offset = shm->var[id].offset;
	int mutex_value;
	sem_getvalue(&shm->mutex, &mutex_value);
	if(!mutex_value)
	{
		printf("shm is busy,so set status failed\n");
		return -1;
	}
	if(-1 == sem_trywait(&shm->mutex))
	{
		printf("shm is busy,so get status failed\n");
		return -1;
	}
	memcpy(param,&ptr[offset],size);
	sem_post(&shm->mutex);
	//printf("get id:%d status successfull\n", id);
	return 0;
}




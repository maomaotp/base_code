#include "sharemem.h"
#include <unistd.h>
#define NUM 21
int main()
{
	int i = 0;
	struct shm_struct_t * shm = NULL;
	int ret = Init_shm(&shm);
	if(-1 == ret){
		printf("Init_shm failed\n");
		return -1;
	}

//	printf("pid=%d\n", getpid());
//	Set_pid_shm(shm,ALARMPID);
//	int pid = Get_pid_shm(shm,ALARMPID);
//	printf("current pid = %d\n", pid);


	char status_input[NUM], status_output[NUM];
	for(i=0;i<NUM;i++)
	{
		status_input[i] = 65 + i;
	}
	ret = Set_status_shm(shm,14,status_input,sizeof(status_input));
	if(-1 == ret)
	{
		printf("set failed\n");
	}
	ret = Get_status_shm(shm,14,status_output,sizeof(status_output));
	if(-1 == ret)
	{
		printf("get failed\n");
		return -1;
	}
	for(i=0;i<NUM;i++)
	{
		printf("status_output[%d]=%c\n", i, status_output[i]);
	}
//	obj.Unlink_shm();
	
	return 0;
}

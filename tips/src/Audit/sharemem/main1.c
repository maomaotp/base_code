#include "sharemem.h"
#define NUM 16
int main()
{
	shm_struct_t * shm = NULL;
	Init_shm(&shm);
	int i = 0;
	char status_input[NUM], status_output[NUM];
//	for(i=0;i<20;i++)
//	{
//		status_input[i] = i+0.001;
//	}
//	int ret = obj.Set_status_shm(3,&status_input,sizeof(status_input));
//	if(-1 == ret){
//		printf("set failed\n");
//	}
	int ret = Get_status_shm(shm, 5,&status_output,sizeof(status_output));
	if(-1 == ret)
	{
		printf("get failed\n");
		Unlink_shm(shm);
		return -1;
	}
	for(i=0;i<NUM;i++)
	{
		printf("status_output[%d]=%c\n", i, status_output[i]);
	}
	Unlink_shm(shm);
	return 0;
}

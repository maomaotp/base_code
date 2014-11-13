#ifndef SHAREMEM_H
#define	SHAREMEM_H
#include "shm_struct_t.h"

class ShareMem
{
	private:
	shm_struct_t * shm;
	int Create_or_open_shm();
	int Read_config();
	int Set_var_type(char * str, var_t * var_ptr);
	int Sizeof(int type);
	public:
	int Init_shm();
	int Unlink_shm();
	int Set_status_shm(int status_id, void * param, int len);
	int Get_status_shm(int status_id, void * param, int len);
};

#endif


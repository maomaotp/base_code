#ifndef CMEMCACHE_H
#define CMEMCACHE_H
#include <pthread.h>
#include <sys/types.h>
struct Mem_Unit{
	struct Mem_Unit* next;
	int free_len;
	int data_len;
};
struct Mem_Head{
	struct Mem_Unit *read_sign,*write_sign;
	int cache_size;
	int current_count;
};
class CMemcache{
	private:
	struct Mem_Head *read_head,*write_head,*head_one,*head_two;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	pthread_mutex_t cond_mutex;
	int size,change_flag;
	bool write_flag,read_finish;
	private:
	int Wait_Cond(int sec);
	int Cast_Cond();
	int Init_Head(struct Mem_Head* memhead);
	public:
	int Mem_Init(int cache_size,void* param);
	int Mem_Read(void* buff,int* len);
	int Mem_Read_All(int (*fun)(void*,void*),void* param);
	int Mem_Write(void* buff,int* len);
	int Mem_Change(int sec);
	int Mem_Exit();	

};
#endif

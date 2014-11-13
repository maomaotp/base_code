#ifndef CMEMPOOL_H
#define CMEMPOOL_H
#include <pthread.h>
#include <sys/types.h>
#define UNIT_DYNAMIC 1 //当写入内容长度比unitsize大时使用多个unit
#define UNIT_STATIC 0 //当写入内容长度比unitsize大时不写入直接返回
struct Unit{
		u_int8_t flag;//标志位，最后一位为写入标志
		u_int32_t free_size;//剩余空间大小
		int employ_size;//已经使用大小
		struct Unit* next_unit;//下一个内存单元地址(当head->type==1时有效)
};
struct Head{
		int block_size;
		int unit_size;
		int unit_count;
		int already;//已经使用的快数
		int type;//是否存在连续单元
		struct Head* next_block;//下一个内存快指针
		struct Unit* last_unit;//最后一个单元地址
		struct Unit* first_unit;//第一个地址单元
};
class CMempool{
		private:
				pthread_mutex_t mutex;
				pthread_cond_t cond;
				struct Head* head;
				struct Unit* PRead,*PWrite;//读写指针
				struct Head* PBRead,*PBWrite;//当前读写block
				int unit_free;
				bool writeflag;//当前write指针是否可写
		private:
				struct Head* Expand_Mempool();//扩容函数
				void Destroy_Mempool();//清理扩容出来的空间
				bool Deal_PWrite();
				bool Deal_PRead();
		public:
				bool Mempool_Init(int unit_size, int unit_count, int type);
				void Mempool_Destroy();
				int Mem_Write(void* buff, unsigned int len);
				int Mem_Write2(char* name, void* buff,int len);
				void* Mem_Read();
				void Readed_Set();
};
#endif

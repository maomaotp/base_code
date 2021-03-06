#ifndef CCACHEHANDLE_H
#define CCACHEHANDLE_H
#define SERVERPORT 8224
#define DATAUNITLEN 2048
#define CLIENTNUM 100
#define EPOLLMAXFD CLIENTNUM
#include <sys/types.h>
#include "cmemcache.h"
#include "initsql.h"
#include "insertdata.h"
#include "auditinfo.h"
#include <list>
#include <pthread.h>
#include "data.h"
#include "rulesdata.h"
#include "cachestruct.h"
#include "accept_policy.h"
using namespace std;
struct Data_Cache{
	void* data;
	int len;
};

class CCachehandler{
private:
		Insertdb insertdb;
		int listenfd,epfd;
		CMemcache memcache;
		list<Data_Cache> cache_list;
		pthread_mutex_t list_mutex;
		bool cache_flag;
		int second;//定时处理缓存数据定时时间(秒)
		pthread_t proid,csmid,rulesid;
		bool Set_Non_Block(int sockfd);
		int Epoll_Connect();
		int Epoll_In(struct epoll_event* event);
		int Epoll_Out(struct epoll_event* event);
		int Read_Data(int fd, void* buff, int len);
		int Read_Data_Len(int fd);
		int Deal_Data(void* data, int len);
		int Deal_Client_Data(int sockfd);
		int Init_Socket(int listenqueuenum);
		int Pop_Data();
		int Start_Rules();
		int Start_Consumer(int sec);
		int Start_Productor();
		int Insert_DB(void* data);
public:
		CCachehandler();
		void Wait_Exit();
		void Rules_Do();
		void Consumer_Do();
		void Productor_Do();
		int Init_Server(int listenqnum, int cache_size);
		int Start_Server(int sec);
		int Exit_Server();
};
#endif

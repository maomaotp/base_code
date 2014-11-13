#ifndef ITIPSCHECKER_H
#define ITIPSCHECKER_H
#include <netinet/ip.h>
#include <iostream>
#include "chkmngdata.h"
#include "cmempool.h"
#include <pthread.h>
#include <string.h>
#define CLASSCOUNT 40
using namespace std;
class ITipsChecker{
	protected:
	int type;
	bool exist;
	bool exit;
	CMempool* filemem;
	int CHECKIP;
	struct mirror_struct mirror;
	pthread_mutex_t map_mutex;
	pthread_mutex_t mirror_mutex;
	public:
	ITipsChecker();
	virtual void Get_Mirror(struct mirror_struct* m);
	virtual int Load_Rules(char* rule,int len)=0;
	virtual int ReLoad_Rules(char* rule,int len)=0;
	virtual void Clear();
	virtual int Start_Check(struct tcp_stream* a_tcp,void** tcpobj);
	virtual int Start_Check(struct ip* packet,bool type,struct net_element* e);
	virtual int Start_Check(struct tuple4* addr,char* buf,int len,struct ip* iph);
	int Set_Type(int t);
	int Get_Type();
	bool Get_Exist();
	void Set_Exist(bool e);
	void Set_CheckIP(int ip);
	void Set_FileMem(CMempool* mem);
};
#endif

#include "itipschecker.h"
int ITipsChecker::Set_Type(int t)
{
    type=t;
    return 0;
}
int ITipsChecker::Get_Type()
{
	return type;
}
bool ITipsChecker::Get_Exist()
{
	return exist;
}
void ITipsChecker::Set_Exist(bool e)
{
	exist=e;
}
void ITipsChecker::Clear()
{

}
ITipsChecker::ITipsChecker()
{
	pthread_mutex_init(&map_mutex,NULL);
	exist=false;
	exit=false;
	CHECKIP=0;
	//dbhandler=NULL;
	type=0;
}

#if 1
int ITipsChecker::Start_Check(struct tcp_stream* a_tcp,void** tcpobj)
{
    /*liuqiang code resolve warning*/
    if(NULL == a_tcp) return 0;
    if(NULL == tcpobj) return 0;

	cout<<"itipschecker tcp checker"<<endl;
    return 0;
}
int ITipsChecker::Start_Check(struct ip* packet,bool type,struct net_element* e)
{
    /*liuqiang code resolve warning*/
    if(NULL == packet || 0 == type || NULL == e) return 0;
    cout<<"itipschecker ip checker"<<endl;
	return 0;
}
int ITipsChecker::Start_Check(struct tuple4* addr,char* buf,int len,struct ip* iph)
{
    /*liuqiang code resolve waring*/
    if(NULL == addr || 0 == len || NULL == iph || NULL == buf) return 0;
	cout<<"itipschecker udp checker"<<endl;
	return 0;
}
#endif

void ITipsChecker::Set_CheckIP(int ip)
{
	CHECKIP=ip;
}
void ITipsChecker::Get_Mirror(struct mirror_struct* m)
{
	mirror.mirror = '1';
	mirror.flow = 12.8;
	mirror.live = 5;
	strcpy(mirror.module, "11330000");
	memcpy(m,&mirror,sizeof(mirror));
}
void ITipsChecker::Set_FileMem(CMempool* mem)
{
	filemem=mem;
}

#include <iostream>
#include "cachehandle.h"
int main()
{
	CCachehandler cachehandler;
	cachehandler.Init_Server(100, 1024);//初始化服务器程序
	//第一个参数为监听队列(100),第二个参数为缓存池缓存的数据个数;
	cout<<"init_server sucess!"<<endl;
	cachehandler.Start_Server(60);//开启服务，60为定时读取数据时间
	cout<<"start_server sucess!"<<endl;
	cachehandler.Wait_Exit();
	cachehandler.Exit_Server();
}

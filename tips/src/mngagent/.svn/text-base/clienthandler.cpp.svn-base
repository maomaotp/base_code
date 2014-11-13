#include "clienthandler.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
using namespace std;
int CCacheclient::Init_Client(char* serverip)
{
	pthread_mutex_init(&mutex,NULL);
	serverfd=socket(PF_INET,SOCK_STREAM,0);
	if(serverfd<=0)
	{
		cout<<"创建客户端套接字失败!"<<endl;
		return -1;
	}
	struct sockaddr_in serveraddr;
	serveraddr.sin_family=PF_INET;
	serveraddr.sin_port=htons(SERVERPORT);
	inet_aton(serverip,&(serveraddr.sin_addr));
	int ret=connect(serverfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	if(ret<0)
	{
		perror("connect");
		cout<<"链接数据缓存服务器失败!"<<endl;
		return -2;
	}
	cout<<"链接成功，等待发送数据!"<<serverfd<<endl;
	return serverfd;
}
int CCacheclient::Send_Msg(char* buff,int len)
{
	pthread_mutex_lock(&mutex);
	char send_buff[len+sizeof(len)];
	memcpy(send_buff,&len,sizeof(len));
	memcpy(send_buff+sizeof(len),buff,len);
	cout<<"即将发送!"<<endl;
	int w_len=write(serverfd,send_buff,len+sizeof(len));
	cout<<"发送字节数为::"<<w_len<<endl;
	pthread_mutex_unlock(&mutex);
	return w_len;
}
void CCacheclient::Exit_Client()
{
	close(serverfd);
}

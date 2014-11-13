#include "client++.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include "data.h"

using namespace std;
bool CCacheclient::Init_Client(char* serverip)
{
    serverfd=socket(PF_INET,SOCK_STREAM,0);
	if(serverfd<=0)
	{
		dzlog_info("create client socket failed!!!");
		close(serverfd);
		return false;
	}
	struct sockaddr_in serveraddr;
	serveraddr.sin_family=PF_INET;
	serveraddr.sin_port=htons(SERVERPORT);
	inet_aton(serverip,&(serveraddr.sin_addr));
	int ret=connect(serverfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	if(ret<0)
	{
		dzlog_info("connect to cache server failed!!!");
		close(serverfd);
		return false;
	}
	dzlog_info("connect cache is success:%d", serverfd);
	return true;
}
int CCacheclient::Send_Msg(char* buff,int len)
{
	char send_buff[len+sizeof(len)];
	memcpy(send_buff,&len,sizeof(len));
	memcpy(send_buff+sizeof(len),buff,len);
	
	int w_len=write(serverfd,send_buff,len+sizeof(len));
#if 0
    char command[30] = "lpush send_Msg ";
    strcat(command, send_buff);
    redis_command(command);
#endif
	dzlog_info("send the number of bytes:%d", w_len);
	return w_len;
}
void CCacheclient::Exit_Client()
{
	close(serverfd);
}

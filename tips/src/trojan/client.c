#include "client.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "init.h"

int serverfd;
int Init_Client(char* serverip)
{
	serverfd = socket(PF_INET, SOCK_STREAM, 0);
	if (serverfd <= 0){
        dzlog_info("create client socket failed!!!");
		return -1;
	}
	struct sockaddr_in serveraddr;
	serveraddr.sin_family = PF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	inet_aton(serverip, &(serveraddr.sin_addr));
	int ret = connect(serverfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (ret < 0){
		dzlog_info("connect to cache failed!!!");
		return -2;
	}
//	cout<<"链接成功，等待发送数据!"<<serverfd<<endl;
	return serverfd;
}

int Send_Msg(char* buff, int len)
{	
	char send_buff[len + sizeof (len)];
	memcpy(send_buff, &len, sizeof (len));
	memcpy(send_buff + sizeof (len), buff, len);
	
	dzlog_debug("send to cache alarm start>>>");
	int lenth = sizeof (len) + len;
	int w_len = 0;
	while (w_len <= 0){
		w_len = write(serverfd, send_buff, lenth);
		lenth -= w_len;
	}
	dzlog_debug("alarm send complete");
	
	return w_len;
}

void Exit_Client()
{
	close(serverfd);
}

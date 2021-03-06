#include "client.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "zlog.h"

int serverfd;
int Init_Client()
{
	serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverfd <= 0)
	{
	    printf("socket fail\n");
		return -1;
	}
	struct sockaddr_in serveraddr;

    memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	//serveraddr.sin_port = htons(SERVERPORT);
	//inet_aton(serverip, &(serveraddr.sin_addr));
	serveraddr.sin_port = htons(8224);
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int ret = connect(serverfd, (struct sockaddr*)&serveraddr, sizeof (serveraddr));
	if (ret < 0)
	{
		//dzlog_info("connect");
		printf("connect to server failed!!!\n");
		return -2;
	}
	printf("链接成功，等待发送数据!\n");
	return serverfd;
}

int Send_Msg(char* buff, int len)
{
	
	char send_buff[len + sizeof (len)];
	memcpy(send_buff, &len, sizeof (len));
	memcpy(send_buff + sizeof (len), buff, len);
//	cout<<"即将发送!"<<endl;
	int lenth = sizeof (len) + len;
	int w_len = 0;
	while (w_len <= 0)
	{
		w_len = write(serverfd, send_buff, lenth);
		lenth -= w_len;
	}
//	cout<<"发送字节数为::"<<w_len<<endl;
	return w_len;
}

void Exit_Client()
{
	close(serverfd);
}

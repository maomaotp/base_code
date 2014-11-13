#include "client.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
int serverfd;
int Init_Client(char* serverip)
{
	serverfd = socket(PF_INET, SOCK_STREAM, 0);
	if (serverfd <= 0)
	{
		printf("创建客户端套接字失败!\n");
		return -1;
	}
	struct sockaddr_in serveraddr;
	serveraddr.sin_family = PF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	inet_aton(serverip, &(serveraddr.sin_addr));
#if 1
	int ret = connect(serverfd, (struct sockaddr*)&serveraddr, sizeof (serveraddr));
	if (ret < 0)
	{
		perror("connect");
		printf("connect server failed!\n");
		return -2;
	}
	printf("connect server sucess!%d\n",serverfd);
	return serverfd;
#endif
}

int Send_Msg(char* buff, int len)
{
	
	char send_buff[len + sizeof (len)];
	memcpy(send_buff, &len, sizeof (len));
	memcpy(send_buff + sizeof (len), buff, len);
//	printf("即将发送!\n");
	int w_len = write(serverfd, send_buff, len + sizeof (len));
//	printf("发送字节数为::%d\n",w_len);
	return w_len;
}

void Exit_Client()
{
	close(serverfd);
}

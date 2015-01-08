#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>


//从网络上接收数据写入一个文件中
int main()
{
	struct sockaddr_in addr,caddr;
	int sockfd, fps, maxfdp, connectfd;
	int len;
	FILE *fp;
	fd_set rfds,wfds;
	struct timeval timeout={3,0}; // select 等到3秒，3秒就轮训，要求非阻塞就0
	char buffer[256] = {0};   //256字节的数据缓冲区
	
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		perror("socket error");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(7092);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if ( bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1){
		perror("bind error");
		return -1;
	}
	
	listen(sockfd, 10);
	fp = fopen("/home/lingban/base_code/code/IO/select/test.log","a");
	fps = fileno(fp);  //把文件流指针转换为文件描述符

	while(1) {
		FD_ZERO(&wfds); //每次循环清空集合，否则不能检测描述符变化
		FD_ZERO(&rfds);
		memset(buffer, 0, sizeof(buffer));
		FD_SET(sockfd, &rfds);  //添加描述符
		FD_SET(fps, &wfds);
		int res;
		maxfdp = sockfd>fps?sockfd+1:fps+1;
		switch( res=select(maxfdp, &rfds, &wfds, NULL, &timeout) ){
		//switch( res=select(maxfdp, &rfds, NULL, NULL, &timeout) ){
			case -1:
				exit(-1); break;
			case 0: 
				break;
			default:
				printf("result=%d\n", res);
				if(FD_ISSET(sockfd, &rfds)){   //测试sockfd 是否可读
					if ( (connectfd = accept(sockfd, (struct sockaddr *)&caddr, (socklen_t *)&len)) < 0){
							return -1;
					}
					FD_SET(connectfd, &rfds);
					if (connectfd > maxfdp){
						maxfdp = connectfd;
					}
					
					if (FD_ISSET(connectfd, &rfds)){
						recv(connectfd, buffer, sizeof(buffer), 0);
						printf("buff=%s\n", buffer);
		
						if(FD_ISSET(fps,&wfds)){  //测试文件是否可写
							fwrite(buffer, 1, sizeof(buffer), fp);
						}
					}
					else {
						printf("wait the send function\n");
					}
				}
		}
	sleep(1);
	}
	
	return 0;
}

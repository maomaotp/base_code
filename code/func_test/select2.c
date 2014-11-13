#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

int main()
{
	int sockfd;
	FILE *fp;
	struct fd_set fds;
    struct sockaddr_in servaddr;

	struct timeval timeout={3,0}; //select等待3秒，3秒轮询，要非阻塞就置0 
	char buffer[256]={0};

	if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket");
		exit(1);
	}
	memset(servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8224);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");


    if(bind(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        exit(1);
    }
	fp = fopen("udp.log", "w+");

	while(1) {
		FD_ZERO(&fds);  //每次循环都要清空集合，否则不能检测描述符变化
		FD_SET(sockfd, &fds); //添加描述符 
		FD_SET(fp, &fds);

		maxfdp = sockfd>fp ? sock+1:fp+1;
		switch(select(maxfd, &fds, &fds, NULL, &timeout)) {
			case -1:
				exit(-1);
				break;
			case 0:
				break;
			default:
				if(FD_ISSET(sockfd, &fds)) {
					recvfrom(sockfd, buffer, 256, ...);
				}
				if(FD_ISSET(fp, &fds))
}

    

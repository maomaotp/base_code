#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <netinet/in.h>
#include <string.h>

int main()
{
	int sfd;
	int length;
	sfd = socket(AF_INET,SOCK_STREAM,0);
	assert(sfd > 0);

	struct sockaddr_in myaddr;
	memset(&myaddr, 0, sizeof(myaddr));

	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(26666);
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sfd,(struct sockaddr*)&myaddr,sizeof(struct sockaddr)) < 0){
		printf("bind failed\n");
		return 0;
	}
	if (listen(sfd,10)<0 ){
		printf("listen failed\n");
		return 0;
	}
	printf("listen waiting...\n");

	struct sockaddr_in otheraddr;
	int cfd, result;

	fd_set testfds;
	int maxfds, fd, readflag;
	fd_set rwfds;
	FD_ZERO(&rwfds);
	maxfds = sfd;
	FD_SET(sfd, &rwfds);

	while(1){
		testfds = rwfds;
		result = select(maxfds+1,&testfds,NULL,NULL,NULL);
		if(result < 1){
			printf("Select unsuccessfully :%d\n", result);
			break;
		}
		for(fd=0; fd<FD_SETSIZE; fd++) {
			if(FD_ISSET(fd, &testfds)){
				if(fd == sfd){
					cfd = accept(sfd, (struct sockaddr*)&otheraddr, &length);
					if (cfd > 0){
						if(cfd > maxfds){
							maxfds = cfd;
						}
						FD_SET(cfd,&rwfds);
						printf("The incoming socket address is %s:%d \n", inet_ntoa(otheraddr.sin_addr),ntohs(otheraddr.sin_port));
					}
				}
				else {
					ioctl(fd,FIONREAD,&readflag);
					if(readflag == 0){
						close(fd);
						FD_CLR(fd,&rwfds);
					}
					else{
						char buffer[254]={0};
						read(fd, buffer, 254);
						printf("The receive buffer is %s\n",buffer);
					}
				}
			}
		}
	}
	for(fd = 0; fd < FD_SETSIZE; fd++){
		close(fd);
		FD_CLR(fd,&rwfds); 
	}
	return 1;
}

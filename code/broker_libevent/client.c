#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int socketfd;
    int result;
    struct sockaddr_in servaddr;
    char buf[200];
	char cmd[20];
/*
	if( argc < 2 ) {
		printf("error argc: ./client send 100 or ./client get\n");
		exit(1);
	}
  */  
    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = inet_addr("192.168.88.129");
   
    if(connect(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        exit(1);
    }
    memset(buf, 0, sizeof(buf));
    //fgets(cmd, sizeof(cmd), stdin);
	if(argc < 2 || strncmp(argv[1], "send", 4)) {
		//strcpy(buf, argv[1]);
		send(socketfd, "get", sizeof(buf), 0);

		memset(buf, 0, sizeof(buf));
		recv(socketfd, buf, sizeof(buf), 0);
		printf("the response:\n%s\n", buf);
		getchar();
	}
	else if( !strncmp(argv[1], "send", 4) ) {
		int i = 0;
		int cirNum=0;

		printf("input circu->number:");
		scanf("%d", &cirNum);

		for( i=0; i<cirNum; i++) {
			snprintf(buf, sizeof(buf),"<?xml version=\"1.0\"?><load_balance IP=\"192.168.1.%d\" ID=\"client%d\"><channels current=\"%d\" max=\"100\"/></load_balance>", i, i, i);
			send(socketfd, buf, sizeof(buf), 0);
			sleep(1);
		}
		return 0;
	}

    return 0;
}

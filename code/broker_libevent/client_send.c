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
	int i;

    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
   
    if(connect(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        exit(1);
    }
    memset(buf, 0, sizeof(buf));

	for( i=0; i<30; i++) {
		snprintf(buf, sizeof(buf),"<?xml version=\"1.0\"?><load_balance IP=\"192.168.1.%d\" ID=\"client%d\"><channels current=\"%d\" max=\"100\"/></load_balance>", 100-i, 100-i, 100-i);
		send(socketfd, buf, sizeof(buf), 0);
		printf("send buf:%s\nsuccessed\n", buf);
		sleep(1);
	}
    return 0;
}

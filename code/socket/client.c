#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>

int main()
{
    int socketfd;
    struct sockaddr_in servaddr;
    char buf[124];
    int result;
    
    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8224);
    servaddr.sin_addr.s_addr = inet_addr("172.16.0.130");
   
    if(connect(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        exit(1);
    }
    while(1) {
        memset(buf, 0, sizeof(buf));
        printf("please input message:");
        fgets(buf, sizeof(buf), stdin);
        send(socketfd, buf, sizeof(buf), 0);

        if((strncmp(buf, "quit", 4) == 0) || (strncmp(buf, "exit", 4) == 0)) {
            printf("quit now");
            break;
        }

        memset(buf, 0, sizeof(buf));
        recv(socketfd, buf, sizeof(buf), 0);
        printf("server:%s", buf);
    }
    return 0;
}

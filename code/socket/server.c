#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>

int main()
{
    int socketfd, clientfd, result;
    struct sockaddr_in servaddr, clientaddr;
    char buf[124];
	char buf_ip[24];
    socklen_t len;
    
    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8224);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(bind(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        exit(1);
    }

    memset(&clientaddr, 0, sizeof(servaddr));
    memset(buf, 0, sizeof(buf));
    memset(buf_ip, 0, sizeof(buf_ip));

    listen(socketfd, 5);
    printf(">>now listen and wait connected\n");

    len = sizeof(clientaddr);
    if((clientfd = accept(socketfd, (struct sockaddr *)&clientaddr, &len)) < 0) {
        perror("accept");
        exit(1);
    }
	inet_ntop(AF_INET,&(clientaddr.sin_addr.s_addr),buf_ip,24);
    //printf("client ip=%s,port=%d\n", inet_ntoa(clientaddr.sin_addr.s_addr), ntohs(clientaddr.sin_port));
    printf("client ip=%s,port=%d\n", buf_ip, ntohs(clientaddr.sin_port));
    while(1) {
        memset(buf, 0, sizeof(buf));
        recv(clientfd, buf, sizeof(buf), 0);
        printf("client:%s", buf);

        if((strncmp(buf, "quit", 4) == 0) || (strncmp(buf, "exit", 4) == 0)) {
            printf("quit chat\n");
            break;
        }

        memset(buf, 0, sizeof(buf));
        printf("server:%s", buf);
        fgets(buf, sizeof(buf), stdin);
        send(clientfd, buf, sizeof(buf), 0);
    }
    return 0;
}

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/un.h>

int main()
{
    int serverfd, clientfd;
    struct sockaddr_un saddr, clientaddr;
    socklen_t len;
    char buf[128];
    int length;

    unlink("/tmp/domain.block");
    serverfd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(serverfd < 0) {
        perror("socket");
    }

    saddr.sun_family=AF_UNIX;
    strcpy(saddr.sun_path, "/tmp/domain.block");

    if(bind(serverfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        perror("bind");
        return 0;
    }
    
    listen(serverfd, 5);
    printf("listen now and wait the connect\n");

    while(1) {
    len = sizeof(clientaddr);
    if((clientfd = accept(serverfd, (struct sockaddr *)&clientaddr, &len)) < 0) {
        perror("accept");
        return 0;
    }

        memset(buf, 0, sizeof(buf));
        printf("wait the message\n");

        if((length = read(clientfd, buf, sizeof(buf))) < 0) {
            printf("length ==%d<<<<<0, break\n", length);
            //break;
        }
        printf("length == %d buf==%s\n", length, buf);
    }
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#define DOMAIN_CACHE "/tmp/domain.cache"
int Init_accept_server();
int accept_policy(int server_sock,char * cmdbuff, int* len);

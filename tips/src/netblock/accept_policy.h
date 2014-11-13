#ifndef ACCEPT_POLICT_H
#define ACCEPT_POLICT_H
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int Init_accept_server();
int accept_policy(int server_sock,char * cmdbuff, int* len);
#endif


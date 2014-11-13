#ifndef COMMUNICATE_H
#define COMMUNICATE_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./net_guard_final/client.h"

#define   NET_OK    1
#define   NET_DOW   2
#define   NET_UNGATE   3
#define   NET_UNLIK  4

typedef struct commucate_t
{
   char connectIP[32];
   char masterIP[32];
   char masterPORT[16];
   int masterstatus;
   char slaveIP[32];
   char slavePORT[16];
   int slavestatus;
}commucate_t;


void getCommunicatePortIp(commucate_t &connect_struct);
void getCommunicatemasterInfo(commucate_t &connect_struct);
char testNetwork(int servertype, const char *centerip,char *order);
void getCenterConf(char * order, int size, commucate_t & connect_struct);
void getcominfo(commucate_t &connect_struct);
#endif

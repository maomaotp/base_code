#ifndef CATCHPACKET_H
#define CATCHPACKET_H
#include <pcap.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>

#include "manageCommand.h"
#include "reportBlockInfo.h"

bool isPacket(const u_char *packet);
void getOrderParameter(const char *parameter);
void processOrder(int *temp,const char *str,char *p[]);
void createBlockInfo(const int *counter,char * const pointer[],BlockOrder *blockinfo);
void initInsideAddress(char *sip,BlockOrder *blockinfo);
void getIpAndMask(const char *str,char *ip,char *mask);
void transferMask(const char *trains,char *mask);
void initHost(const char *ip,BlockOrder *blockinfo);
void initNetWorkPam(const char *ip,const char *mask,BlockOrder *blockinfo);
void initOutSideAddress(char *dip,BlockOrder *blockinfo);
void initNetAddr(const char *ip,BlockOrder *blockinfo);
void initNetWorkOutPam(const char *ip,const char *mask,BlockOrder *blockinfo);
void initBlockTime(const char *time,BlockOrder *blockinfo);
void initInsidePort(const char *sport,BlockOrder *blockinfo);
void initOutsidePort(const char *dport,BlockOrder *blockinfo);
void initBlock(BlockOrder &p);
void findBlockOrder(BlockOrder &blockinfo);
bool searchThisOrder(BlockOrder &blockinfo,BlockOrder &order);
bool compare(PORT &src,PORT &dest);
#endif

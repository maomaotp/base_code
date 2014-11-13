#ifndef REPORTBLOCKINFO_H
#define REORTBLOCKINFO_H
#include <pthread.h>
#include <pcap.h>
#include <libnet.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "client.h"
#include "InitProcess.h"
#include "juageIP.h"

void   initNetWork();
void   getopt_reset();
void*  pthreadFunc(void *arg);
void   createPthread(pcap_t *handle);
bool   juageIp(struct iphdr *ip,BlockOrder *order);
void   block_tcp_stream(struct iphdr *ip,struct tcphdr *tcp);
void   mycallback(u_char *user,const struct pcap_pkthdr *h,const u_char *bytes);
bool   compareIP(const in_addr_t *addr,const BlockAddr *listElement);
bool   juagePort(struct iphdr *ip,struct tcphdr *tcp,BlockOrder *order);
bool   comparePort(const in_port_t *port,const PORT *listElement);
void   initAche_block_t(struct cache_block_t *info,const time_t *now,const struct iphdr *ip,const struct tcphdr *tcp);
void   TurnTime(const time_t clock,char *buffer,int size);
 bool searchTimeout(std::list<BlockOrder>::iterator &p,const time_t *now);
#endif

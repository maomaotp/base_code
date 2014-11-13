#ifndef UNKOWN_H
#define UNKOWN_H

#include <time.h>
#include <pcap.h>
#include <stdbool.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <string.h>
#include <time.h>
#include <openssl/md5.h>
#include "cachestruct.h"

#define UNSIZE 300000
#define TIMEE 180
#define MESSAGE_SIZE 32

typedef struct unkown_trojan
{
	int ishttp;
	bool report;
	char plugin_id[4];
	u_char* message;
	time_t last_time;
	size_t index;
    size_t download;
	size_t upload;
	struct unkown_trojan *next;
	struct unkown_trojan *prev;
}unkown_trojan;

void TurnTime(const time_t clock,char *buffer,int size);
void alarmutrojan(const struct iphdr *ip,const struct tcphdr *tcp);
void processtcppacket(const struct tcphdr *tcp,const struct iphdr *ip,const u_char *packet);
void enterunkprocess(const struct pcap_pkthdr *head, const u_char *packet);
void processippacket(const struct iphdr *ip,const u_char *data);
void createunkown();
bool finduntrojan(const struct tcphdr *tcp,const struct iphdr *ip,bool *isnull,uint32_t *positiontemp,struct unkown_trojan **pt);
void packmd5c(char *data,unsigned char *value);
void createmd5c(const struct iphdr *ip,uint32_t hashtemp,unsigned char *digest,uint32_t hashtep,unsigned char *summary);
bool searchunstream(uint32_t position,uint32_t location, const struct iphdr *ip,uint32_t hashtemp,int *result,unkown_trojan **pos,uint32_t *index);
void addnode(uint32_t pos,const struct iphdr *ip,const struct tcphdr *tcp);
struct unkown_trojan* processnode(uint32_t pos,const struct iphdr *ip,const struct tcphdr *tcp);
void deleteun(struct unkown_trojan *it);
void processpacket(const struct tcphdr *tcp,const struct iphdr *ip,const u_char *packet);
void clearunhash();
void clearun(struct unkown_trojan *it);
bool isovertime(time_t now,struct unkown_trojan *begin,struct unkown_trojan **pos);
uint32_t jhash_3words(uint32_t srcip,uint32_t destip,uint32_t port,uint32_t initval);
void createmd5(const struct iphdr *ip,uint32_t hashtemp,unsigned char *digest,uint32_t hashtep,unsigned char *summary);

#endif

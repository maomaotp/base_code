#ifndef HASHDATA_H
#define HASHDATA_H

#include <stdbool.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include "loadrule.h"

struct stream
{
  bool isTrojanHorse;
  bool content;
  int filefd;
};

struct horserule
{
   struct  filerule *regulation;
   struct  stream *flow;
   struct horserule *prev;
   struct horserule *next;
   time_t last;
   char dmac[20];
   unsigned char *message;
};

void getlocalether(in_addr_t *src,in_addr_t *dest,u_char **temp,struct ether_header *ether);
char convertbyt(char t);
void clearhash(int signo);
bool stre(time_t now,struct horserule *begin,struct horserule **pos);
void createHashTable();
void initpacphead();
void savestream(struct horserule *save,const struct pcap_pkthdr *head,const u_char *packet);
void enterprocess(const struct pcap_pkthdr *head,const u_char *packet);
void processstream(struct horserule *cur,struct iphdr *ip,const u_char *packet);
void designtre(struct horserule *tr,const u_char *packet);
void alar(struct horserule *save,const u_char *packet);
void writefile(struct horserule *save);
void freethisstream(struct horserule *freestr,uint32_t index);
void clearstream(struct horserule *freestr);
void addrulehorse(struct horserule *end,struct iphdr *ip,struct tcphdr *tcp,const u_char *packet);
void addhorse(struct horserule *end,struct iphdr *ip,struct tcphdr *tcp,uint32_t datalength,struct filerule *pos);
void createhashvalue(uint32_t position,struct iphdr *ip,const u_char *packet);
void create(uint32_t hashpos,const u_char *packet,uint32_t datalength,struct filerule *pos);
void packmd5(unsigned char *data,unsigned char *value);
void createmd5(struct iphdr *ip,uint32_t hashtemp,unsigned char *digest,uint32_t hashtep,unsigned char *summary);
bool searchstream(uint32_t position,uint32_t location,struct iphdr *ip,uint32_t hashtemp,int *result,struct horserule **pos,uint32_t *index);
#endif

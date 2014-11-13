#ifndef CHECK_H
#define CHECK_H
#include <set>
#include <pcap.h>
#include <signal.h>
#include "juageIP.h"
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <openssl/md5.h>
#include "mirrorNetwork.h"
#include "monitorNetwork.h"
#include "communiteNetwork.h"
#include "./sharemem/sharemem.h"
#include <netinet/if_ether.h>


#define GETTIME 3
typedef struct _info
{
   commucate_t   commu_info;
   Monitor_t     monitor_t;
   workmode_t    mirror_info;
}info_t;

typedef struct _mess
{
   unsigned char mess[16];
}message;

struct compare
{
   bool operator()(const message &it,const message &ie) const
   {
      return memcmp(it.mess,ie.mess,16);
   }
};

void init_flag(int *flag);
bool find(const message &it);
bool search(const message &it);
void packmd5(unsigned char *data,unsigned char *value);
void geteth2dataflow(const struct pcap_pkthdr *hdr);
void geteth3dataflow(const struct pcap_pkthdr *hdr);
void process(const int *flag);
void process_time(const int *flag,time_t remained);
#endif

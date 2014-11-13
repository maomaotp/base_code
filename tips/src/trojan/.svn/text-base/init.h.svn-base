#ifndef INIT_H
#define INIT_H

#include "loadrule.h"
#include "hashdata.h"
#include "client.h"
#include "geoip.h"
#include "zlog.h"

#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


void init();
void trojan_process(const struct pcap_pkthdr *hdr,const u_char *bytes);
void readrule();
void createunkown();
void getAddr_ergodic(const unsigned int ip, char* geography, char* scc_flag);

#endif

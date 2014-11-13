#ifndef IPBLACKCHECKER_H
#define IPBLACKCHECKER_H
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <map>

#include "itipschecker.h"
#include "rulesdata.h"
#include "cachestruct.h"
#include "wtpcap.h"
#include "wtEnd.h"
#include "tools.h"
#include "client++.h"

extern "C"
{
#include "init.h"
#include "client.h"
}

#define NameSize 256
//#define TopLimit 4294967295

#define IPLinkPlugin_id 1002

#define DataPacketType 1000
#define TorjanID 0
#define AlertType 80

#define IP_HL(ip) (((ip->ip_hl) & 0x0f) * 4)
#define IP_VR(ip) ((ip->ip_v) & 0x0f) 

#define IPFileDIR ((char *)"IPBLACK")
#define ID_NUM_FILE ((char *)"/root/lirongxun/checklrx/ID.txt")

class IPBlackChecker:public ITipsChecker{
	private:
		Pcap * wtpcap;
		//int Black_Check(struct net_element* e,struct ip *addr);
		map<u_short,struct ip_rules> whitelist;
		map<u_short,struct ip_rules> blacklist;
		map<u_int8_t,u_int8_t>Dgrous;
	public:
		int Load_Rules(char* rule,int len);
		int ReLoad_Rules(char* rule,int len);
		int Start_Check(struct ip* packet,bool type,struct net_element* e);
		bool CmpIP(map<u_short,struct ip_rules>::iterator it,map<u_short,struct ip_rules> *Xmap,uint32_t *dstip,uint32_t *srcip);
		//void WtStructMem(struct net_element *e,u_int8_t port);
		//int writemem(uint32_t sip, uint32_t dip, uint16_t sport, uint16_t dport, char risk);
int writemem(uint32_t sip, uint32_t dip, uint16_t sport, uint16_t dport, u_char srcmac[6],u_int8_t protocol,char risk);
		//void WtFileMem(struct ip* addr);
		bool ID(u_int32_t *id);
		void Print_Error(int fd,char *Api);
		//void StopLink(struct ip *addr,struct net_element *e);
		//bool LibnetSend(struct ip *addr,struct net_element *e,libnet_t *ppacket);
		//bool LibnetTcpSend(u_int32_t SRCIP,u_int32_t DSTIP,u_int16_t SRCPORT,u_int16_t DSTPORT,struct ip *addr,libnet_t * ppacket);
		//bool LibnetIcmpSend(u_int32_t SRCIP,u_int32_t DSTIP,u_int8_t *addr,libnet_t *ppacket);
		void DgrousLevel(char *srclevel,char *newlevel);
};
#endif

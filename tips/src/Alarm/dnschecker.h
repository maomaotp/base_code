#ifndef DNSCHECKER_H
#define DNSCHECKER_H
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <map>
#include <string>
#include <list>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <openssl/md5.h>

#include "itipschecker.h"
#include "rulesdata.h"
#include "multikeymatch.h"
#include "client++.h"
#include "wtpcap.h"
#include "wtEnd.h"
#include "cachestruct.h"
#include "tools.h"

extern "C"
{
#include "init.h"
#include "client.h"
}

#define DNSPORT 53
#define DNSHL 12
#define DNLEN 1024
#define BUFFSIZE 256
#define FileNameSize 256
#define IPSize 16

#define DnsDealPlugin_id 1001
#define DnsLinkPlugin_id 1002

#define DataPacketType 1000
#define TorjanID 0
#define AlertType 80
//#define TopLimit 4294967295
//FIXME by -- yong 2013.07.10
#define AlertTimeOut 10

#define DNSFileDIR ((char *)"DNS")

#define IP_HL(ip) (((ip->ip_hl) & 0x0f) * 4)
#define IP_VR(ip) ((ip->ip_v) & 0x0f) 
#define DNSCLASSID 4
#define DNS_NUM_FILE ((char *)"/root/lirongxun/checklrx/ID.txt")



extern CCacheclient cacheclient;


extern int DEV_ID;

struct dnshdr
{
	u_short ident;//标识
	u_short mark;//标志
	u_short nproblem;//问题数
	u_short nresource;//资源记录数
	u_short grant;//授权资源记录数
	u_short aditional;//额外资源记录数
	u_int8_t *name;//查询的问题
};
struct dnshead
{
	u_short ident;//标识
	u_short mark;//标志
	u_short nproblem;//问题数
	u_short nresource;//资源记录数
	u_short grant;//授权资源记录数
	u_short aditional;//额外资源记录数
};

struct dns_response //DNS响应数据报：
{
	unsigned short type; //查询类型

	unsigned short classes; //类型码

	unsigned int ttl; //生存时间

	unsigned short length; //资源数据长度

};

class DNSChecker:public ITipsChecker{
	private:
		map<u_int32_t,string> DnameRigour;
		MultiKeyMatch keymatch;
		list<struct result_struct> last;
		map<u_int32_t,u_int8_t> Dgrous;
	public:
		Pcap * wtpcap;
		void GreateHash(const char *str,u_int32_t *ip,u_int32_t *hash);
		void DgrousLevel(u_int8_t *srclevel,char *newlevel);
		int Load_Rules(char* rule,int len);
		int ReLoad_Rules(char* rule,int len);
		int Start_Check(struct ip* ip, bool type, struct net_element *e);
		//int UdpParse(struct udphdr *udp_addr,struct net_element *e);
		int UdpParse(struct ip* addr,struct net_element *e);
		void Clear();
		void Print_Error(int fd,char *Api);
		bool DNS_ID(u_int32_t *id);
		bool Fill_alert(char *DomainName,int len,cache_alert_t *dns_alert,struct net_element *e);
		//void JudgeDNS(char *DomainName,int len,struct net_element *e);
		void JudgeDNS(char *DomainName,struct ip* addr,int len,struct net_element *e);
		void WtMemPool(cache_alert_t * dns_alert,int len);
		void WtMem2(struct ip *addr);
		void WtFileMem(char *Name,char *PacketContent,int ContentLen);

		//void UpdateAlertList(char *DomainBuf);
		void UpdateAlertList(char *DomainBuf,unsigned int addr);
};
#endif
